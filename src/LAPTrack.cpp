/** @file LAPTrack.cpp
 *  @author Mark J. Olah (mjo at cs.unm.edu)
 *  @date 2015-2019
 *  @brief The member definitions for LAPTrack
 */
#include "Tracker/LAPTrack.h"
#include "Tracker/LAP_JVSparse.h"

namespace tracker {

LAPTrack::LAPTrack(const VecParamT &param) : Tracker(param)
{
//     for(auto &stat: param)
//         std::cout<<stat.first<<":"<<stat.second<<std::endl;
    //Read in parameters
    if (param.find("D") != param.end())
        D = static_cast<FloatT>(param.at("D")(0));
    if (param.find("kon") != param.end())
        kon = static_cast<FloatT>(param.at("kon")(0));
    if (param.find("koff") != param.end())
        koff = static_cast<FloatT>(param.at("koff")(0));
    if (param.find("rho") != param.end())
        rho = static_cast<FloatT>(param.at("rho")(0));
    if (param.find("maxSpeed") != param.end())
        maxSpeed = static_cast<FloatT>(param.at("maxSpeed")(0));
    if (param.find("maxPositionDisplacementSigma") != param.end())
        maxPositionDisplacementSigma = static_cast<FloatT>(param.at("maxPositionDisplacementSigma")(0));
    if (param.find("maxFeatureDisplacementSigma") != param.end())
        maxFeatureDisplacementSigma =  static_cast<FloatT>(param.at("maxFeatureDisplacementSigma")(0));
    if (param.find("maxGapCloseFrames") != param.end())
        maxGapCloseFrames =  static_cast<IdxT>(param.at("maxGapCloseFrames")(0));
    if (param.find("minGapCloseTrackLength") != param.end())
        minGapCloseTrackLength =  static_cast<IdxT>(param.at("minGapCloseTrackLength")(0));
    if (param.find("minFinalTrackLength") != param.end())
        minFinalTrackLength =  static_cast<IdxT>(param.at("minFinalTrackLength")(0));
    if (param.find("featureVar") != param.end())
        featureVar = param.at("featureVar");
    //Pre-compute logarithms of commonly used values
    logkon = log(kon);
    log1mkon = log(1-kon);
    logkoff = log(koff);
    log1mkoff = log(1-koff);
    logrho = log(rho);
}

LAPTrack::VecParamT LAPTrack::getStats() const
{
    auto stats = Tracker::getStats();
    stats["D"] = D;
    stats["kon"] =kon;
    stats["koff"] = koff;
    stats["rho"] = rho;
    stats["maxSpeed"] = maxSpeed;
    stats["maxPositionDisplacementSigma"] = maxPositionDisplacementSigma;
    stats["maxFeatureDisplacementSigma"] = maxFeatureDisplacementSigma;
    stats["maxGapCloseFrames"] = maxGapCloseFrames;
    stats["minGapCloseTrackLength"] = minGapCloseTrackLength;
    stats["minFinalTrackLength"] = minFinalTrackLength;
    stats["featureVar"] = featureVar;
    return stats;
}

void LAPTrack::initializeTracks(const IVecT &frameIdx_, const MatT &position_, const MatT &SE_position_)
{
    MatT feature_, SE_feature_;
    initializeTracks(frameIdx_, position_, SE_position_,feature_, SE_feature_);
}

void LAPTrack::initializeTracks(const IVecT &frameIdx_, const MatT &position_, const MatT &SE_position_, const MatT &feature_, const MatT &SE_feature_)
{
    Tracker::initializeTracks(frameIdx_, position_, SE_position_, feature_,SE_feature_);
    frameBirthStartIdx.clear();
    birthFrameIdx.clear();
    state = UNTRACKED;
}

void LAPTrack::generateTracks()
{
    //Do whatever is still needed to produce the tracks
    switch(state){
        case UNTRACKED:
            linkF2F();
            /* fall through */
        case F2F_LINKED:
            closeGaps();
            /* fall through */
        case GAPS_CLOSED:
            break;
    }
}

void LAPTrack::debugF2F(IdxT curFrame, IVecT &cur_locs, IVecT &next_locs, SpMatT &cost, IMatT &connections, VecT &conn_costs) const
{
    if(curFrame>=lastFrame || curFrame<0){
        std::ostringstream msg;
        msg<<"got bad curFrame: "<<curFrame;
        throw LogicalError(msg.str());
    }
    IdxT nextFrame = curFrame+1;
    while(frameLocIdx(nextFrame-firstFrame).is_empty()) nextFrame++; //Find next frame with localizations
    cur_locs = frameLocIdx(curFrame-firstFrame);
    next_locs = frameLocIdx(nextFrame-firstFrame);
    cost = computeF2FCostMat(curFrame, nextFrame);
    IVecT frame_assignment = LAP_JVSparse<FloatT>::solve(cost);
    IdxT nCurLocs = nFrameLocs(curFrame-firstFrame);
    IdxT nNextLocs = nFrameLocs(nextFrame-firstFrame);
    //Comput nCons - number of connections.  This includes track->track, births, and deaths, but not phantom connections.
    IdxT nCons = nCurLocs;
    for(IdxT n=nCurLocs; n<nCurLocs+nNextLocs; n++) {
        if(frame_assignment(n)<nNextLocs) {
            nCons++;
        }//otherwise is a phantom connection so don't report it.
    }
    connections.set_size(nCons,2);
    IdxT conIdx=0;
    for(IdxT n=0;n<nCurLocs+nNextLocs;n++){
        if (n>=nCurLocs) {
            if (frame_assignment(n)>=nNextLocs) continue; //phantom
            connections(conIdx,0) = -1; //birth
        } else {
            connections(conIdx,0) = cur_locs(n); //connection
        }
        connections(conIdx,1) = (frame_assignment(n)>=nNextLocs) ? -1 : next_locs(frame_assignment(n)); //deaths
        conIdx++;
    }
    conn_costs = LAP_JVSparse<FloatT>::computeCost(cost, frame_assignment);
    conn_costs = conn_costs.elem(arma::find(conn_costs>cost_epsilon));
}

void LAPTrack::linkF2F()
{
    if(state!=UNTRACKED) throw LogicalError("linkF2F: frame is not UNTRACKED");
    //firstFrame and lastFrame are guaranteed to have the localizations others may not
    //we choose to connect with the next non-empty frame
    IdxT curFrame = firstFrame;
    //Initialize first frame of tracks
    IVecT &initLocs = frameLocIdx(0);
    frameBirthStartIdx.set_size(nFrames);
    for(IdxT i=0; i< nFrameLocs(0); i++){
        IdxT locIdx = initLocs(i);
        tracks.push_back(TrackT());
        tracks[i].push_back(locIdx);
        trackAssignment[locIdx]=i;
        frameBirthStartIdx(curFrame-firstFrame) = 0; // record births
        birthFrameIdx.push_back(curFrame); // record birth frame time
    }

    while(curFrame < lastFrame){  //When curFrame==lastFrame we have linked all frames
        IdxT nextFrame = curFrame+1;
//         std::cout<<"------------F2F------------"<<"\n";
        while(frameLocIdx(nextFrame-firstFrame).is_empty()){
            frameBirthStartIdx(nextFrame-firstFrame) = static_cast<IdxT>(tracks.size());//Record absence of new births for frame nextFrame
            nextFrame++;
        }
//         std::cout<<"curFrame:"<<curFrame<<" nextFrame:"<<nextFrame<<"\n";
//         std::cout<<"trackAssignment: "<<trackAssignment.t()<<"\n";
//         std::cout<<"NTracks:"<<tracks.size()<<"\n";
        IdxT nCur = nFrameLocs(curFrame-firstFrame);
        if(nCur<=0) throw LogicalError("linkF2F: nCur non positive");

        IdxT nNext=nFrameLocs(nextFrame-firstFrame);
//         std::cout<<"Ncur:"<<nCur<<" Nnext:"<<nNext<<"\n";
        
        SpMatT cost = computeF2FCostMat(curFrame, nextFrame); //Make the cost sparse matrix
        arma::mat dC(cost);
//         std::cout<<"Cost: ("<<cost.n_rows<<","<<cost.n_cols<<"):\n"<<dC<<"\n";
        IVecT frame_assignment = LAP_JVSparse<FloatT>::solve(cost); //Solve for the assignments.
//         std::cout<<"frameAssignment: "<<frame_assignment.t()<<"\n";
        IVecT &curFrameIdxs = frameLocIdx(curFrame-firstFrame);
        IVecT &nextFrameIdxs = frameLocIdx(nextFrame-firstFrame);
//         std::cout<<"curFrameIdxs: "<<curFrameIdxs.t()<<"\n";
//         std::cout<<"nextFrameIdxs: "<<nextFrameIdxs.t()<<"\n";

        
        
        for(IdxT i=0; i<nCur; i++){
            //process frame_assignment for each of the current frame localizations
            IdxT asgn = frame_assignment(i); //In terms of the possible next frames connections or deaths.
            IdxT cur_id = curFrameIdxs(i);
            IdxT track_id = trackAssignment(cur_id);
            if (asgn < nNext) { //connection - extend track corresponding to current frame localization
                if(track_id<0) throw LogicalError("linkF2F: connection: bad track_id");

                IdxT next_loc_idx = nextFrameIdxs(asgn);
                if(trackAssignment(next_loc_idx) != -1) throw LogicalError("linkF2F: connection: bad next_loc_idx. Expected trackAssignment is unassigned.");

                trackAssignment(next_loc_idx) = track_id;
                tracks[track_id].push_back(next_loc_idx);
//                 std::cout<<"Connect: Track:"<<track_id<<" "<<cur_id<<"->"<<next_loc_idx<<"\n";
            }
        }
        //Process births
        frameBirthStartIdx(nextFrame-firstFrame) = tracks.size(); //Births for next frame start with next track added.
//         std::cout<<"Frame Birth Start Idx: "<<frameBirthStartIdx.t()<<"\n";
        for(IdxT i=nCur; i<nCur+nNext; i++) {
            IdxT birth_id = i-nCur; //index into nextFrameIdx of this localization
            IdxT asgn = frame_assignment(i);
            if (asgn < nNext) { //birth - make a track of size 1.
                IdxT track_id = tracks.size(); //new track_id
//                 std::cout<<"Birthing NewTrackID: "<<track_id<<std::endl;
                
//                 assert(track_id>=1);
                IdxT birth_loc_idx = nextFrameIdxs(birth_id); //Actual localization index of the birth localization
                if(trackAssignment(birth_loc_idx) != -1) throw LogicalError("linkF2F: birth: bad birth_loc_idx. Expected trackAssignment is unassigned.");
                trackAssignment(birth_loc_idx) = track_id;
                tracks.push_back(TrackT());
                tracks[track_id].push_back(birth_loc_idx);
                birthFrameIdx.push_back(nextFrame); // record birth frame time as happening in next frame
//                 std::cout<<"birthId:"<<birth_id<<" birthLocIdx:"<<birth_loc_idx<<"\n";
//                 std::cout<<"trackAssignment:"<<trackAssignment.t()<<"\n";
//                 std::cout<<"recorded birthFrameIdx: "<<birthFrameIdx.back()<<"\n";
            }
        }
        curFrame=nextFrame;
    }
//     for(unsigned i=0; i<deathLocIdx.size();i++){
//         if(deathLocIdx[i]==-1) {
//             IdxT lastIdx = tracks[i].back();
//             std::cout<<"Finalize Tracks: TrackID: "<<i<<" lastIdx:"<<lastIdx<<" frameIdx:"<<frameIdx(lastIdx)<<" deathLocIdx:"<<deathLocIdx[i]<<"\n";
//             assert(frameIdx(lastIdx)==lastFrame);
//         }
//     }
//     std::cout<<"NTracks: "<<tracks.size()<<"\n";
//     std::cout<<"TrackAssignment: "<<trackAssignment.t()<<"\n";
//     std::cout<<"frameBirthStartIdx: "<<IVecT(frameBirthStartIdx).t()<<"\n";
//     std::cout<<"BirthFrameIdx: "<<IVecT(birthFrameIdx).t()<<"\n";
    state = F2F_LINKED;
}

LAPTrack::SpMatT
LAPTrack::computeF2FCostMat(IdxT curFrame, IdxT nextFrame) const
{
    IdxT nCur = nFrameLocs(curFrame-firstFrame);
    IdxT nNext = nFrameLocs(nextFrame-firstFrame);
    IdxT nTot = nCur+nNext;
    //These will be our sparse matrix format vectors
    std::vector<arma::uword> row_index;
    std::vector<arma::uword> col_index;
    std::vector<FloatT> values;
    IdxT reserve_size = nTot+2*std::min(nCur*nNext, std::max(nCur,nNext)*10); //Guesstimate amount of entries used
    row_index.reserve(reserve_size);
    col_index.reserve(reserve_size);
    values.reserve(reserve_size);

    IdxT deltaT = nextFrame - curFrame; //The number of frames spanned in the link
    FloatT DdT = 2*D*deltaT;
    FloatT position_gaussian_exponent_cuttoff = (maxPositionDisplacementSigma*maxPositionDisplacementSigma)/2.; //Only allow connections within 5 sigma
    VecT feature_gaussian_exponent_cuttoff = (maxFeatureDisplacementSigma%maxFeatureDisplacementSigma)/2.; //Only allow connections within 5 sigma
    FloatT norm_const = (nDims+nFeatures)*log2pi; //Pre-compute this

    //Fill in connection costs
    const IVecT &curFrameLocs = frameLocIdx(curFrame-firstFrame);
    const IVecT &nextFrameLocs = frameLocIdx(nextFrame-firstFrame);
//     std::cout<<"nCur:"<<nCur<<" nNext:"<<nNext<<"\n";
//     std::cout<<"nFrameLocs:"<<nFrameLocs.t()<<"\n";
    //connecting locI in current frame to locJ in next frame
    for(IdxT j=0; j<nNext; j++) {
        IdxT next_idx = nextFrameLocs(j);
        for(IdxT i=0; i<nCur; i++){
            IdxT cur_idx = curFrameLocs(i);
//             std::cout<<"i:"<<i<<" j:"<<j<<" curIdx:"<<cur_idx<<" nextIdx:"<<next_idx<<" DdT:"<<DdT<<"\n";
            FloatT C=0;
            bool feasible = true;
            FloatT total_dist_sq=0;
            for(IdxT d=0; d<nDims; d++){
                FloatT dist_var = DdT + SE_position(cur_idx,d) + SE_position(next_idx,d);
                FloatT dist = position(cur_idx,d) - position(next_idx,d);
                FloatT dist_sq = dist*dist;
                total_dist_sq += dist_sq;
                FloatT cost_exponent = dist_sq/dist_var;
//                 std::cout<<"Dim:"<<d<<" dist:"<<dist<<" dist_var:"<<dist_var<<" costExp:"<<cost_exponent<<" ExpCuttoff:"<<position_gaussian_exponent_cuttoff<<"\n";
//                 std::cout<<"SE_position(cur_idx,d):"<<SE_position(cur_idx,d)<<"SE_position(next_idx,d):"<<SE_position(next_idx,d)<<"\n";
                if(cost_exponent > position_gaussian_exponent_cuttoff) { //Too far away to be connected
                    feasible=false;
                    break;
                }
                C+= cost_exponent + log(dist_var);
//                 std::cout<<"cost: "<<C<<"\n";
            }
            if(!feasible) continue; //gaussian sigma constraint violated: move to next pair.
            if(maxSpeed>0 && sqrt(total_dist_sq)/deltaT > maxSpeed) continue; //maxSpeed constraint violated
            for(IdxT f=0; f<nFeatures; f++){
                FloatT feat_var = featureVar(f) + SE_feature(cur_idx,f)+ SE_feature(next_idx,f);
                FloatT feat_dist = feature(cur_idx,f) - feature(next_idx,f);
                FloatT cost_exponent = feat_dist*feat_dist/feat_var;
                if(cost_exponent > feature_gaussian_exponent_cuttoff(f)) { //Too far away to be connected
                    feasible=false;
                    break;
                }
                C+= cost_exponent + log(feat_var);
            }
            if(!feasible) continue; //move to next pair.
            //Otherwise we have a valid cost so normalize and record it
            C+= norm_const;
            C*= 0.5;
            C-= log1mkoff;
//             std::cout<<"C:"<<C<<" log1mkoff:"<<log1mkoff<<"\n";
            //Record cost
            row_index.push_back(i);
            col_index.push_back(j);
            values.push_back(C);
            //Record lower right block dummy cost
            row_index.push_back(nCur+j);
            col_index.push_back(nNext+i);
            values.push_back(cost_epsilon);
        }
    }
    //Fill in death costs
    FloatT deathC= -logkoff;
    for(IdxT i=0; i<nCur; i++){
        row_index.push_back(i);
        col_index.push_back(nNext+i);
        values.push_back(deathC);
    }
    //Fill in birth costs
    FloatT birthC = -logrho-logkon;
    for(IdxT j=0; j<nNext; j++){
        row_index.push_back(nCur+j);
        col_index.push_back(j);
        values.push_back(birthC);
    }
    //Assemble sparse matrix
    IdxT nnz = values.size();
    UMatT locations(2,nnz);
    for(IdxT n=0; n<nnz;n++){
        locations(0,n) = row_index[n];
        locations(1,n) = col_index[n];
    }
    VecT values_vec(values.data(), nnz, false);//Re-use the vector's memory directly.
    bool sort_them = true; //Make sure armadillo sorts the locations
    bool check_for_zeros = false; //Don't bother checking for zeros
    return {locations, values_vec, static_cast<arma::uword>(nTot), static_cast<arma::uword>(nTot), sort_them, check_for_zeros};
}

void LAPTrack::checkFrameIdxs()
{
    if(state!=F2F_LINKED) throw std::runtime_error("state != F2F_LINKED");
    IdxT trackIdx=0;
    for(IdxT n=firstFrame; n<=lastFrame; n++){
        IdxT stidx = frameBirthStartIdx(n-firstFrame);
//         std::cout<<"Frame:"<<n<<" TrackIdx:"<<trackIdx<<" StartIdx:"<<stidx<<"\n";
        if(stidx!=trackIdx) throw std::runtime_error("startidx != trackidx");
        if(!(trackIdx == static_cast<IdxT>(tracks.size()) ||  frameIdx(tracks[trackIdx].front()) >=n)) throw std::runtime_error("Track indexing error");
        while(trackIdx < static_cast<IdxT>(tracks.size()) && frameIdx(tracks[trackIdx].front())==n) {
//             std::cout<<"TrackIdx:"<<trackIdx<<" Correctly begins at frame:"<<n<<"\n";
            trackIdx++;
        }
    }
}

void LAPTrack::debugCloseGaps(SpMatT &cost, IMatT &connections, VecT &conn_costs) const
{
    if(state!=F2F_LINKED) throw std::runtime_error("state != F2F_LINKED");
    cost = computeGapCloseMatrix();
    IVecT track_assignment = LAP_JVSparse<FloatT>::solve(cost);
    IdxT nTracks = static_cast<IdxT>(tracks.size());
    IdxT nCons = nTracks;
    for(IdxT n=nTracks; n<2*nTracks; n++)
        if(track_assignment(n) < nTracks) nCons++;
    connections.set_size(nCons,2);
    IdxT conIdx=0;
    for(IdxT n=0;n<2*nTracks;n++){
        if (n>=nTracks) {
            if (track_assignment(n)>=nTracks) continue; //phantom
            connections(conIdx,0) = -1; //birth
        } else {
            connections(conIdx,0) = n; //connection
        }
        connections(conIdx,1) = (n>=nTracks) ? -1 : track_assignment(n); //deaths
        conIdx++;
    }
    conn_costs = LAP_JVSparse<FloatT>::computeCost(cost, track_assignment);
    conn_costs = conn_costs.elem(arma::find(conn_costs>cost_epsilon));
}

void LAPTrack::closeGaps()
{
    //Invariant: tracks are in birth order.  So when connecting trackM->trackN we have M<N;
    if(state!=F2F_LINKED) throw std::runtime_error("state != F2F_LINKED");
    auto cost = computeGapCloseMatrix();
//     arma::mat dC(cost);
//     std::cout<<"Cost: ("<<cost.n_rows<<","<<cost.n_cols<<"):\n"<<dC<<"\n";
    IVecT track_assignment = LAP_JVSparse<FloatT>::solve(cost);
    IdxT nTracks = tracks.size();
    IdxT nNewTracks = nTracks;
    for(IdxT m=nTracks-1; m>=0; m--){ //start at the end.  Last track cannot connect so skip it.
        //we are considering the connection for trackM -> trackN
        IdxT n = track_assignment(m);
        if(!(m<n)){ //either we don't connect or we connect to an N born after M. so M<N.
            throw LogicalError("Gap close ordering problem.");
        }
        if(n < nTracks) {
            //Valid track join operation.
            if(tracks[m].empty()) throw LogicalError("Gap close. empty track");
//             std::cout<<"Tracks(m="<<m<<"):"<<tracks[m].size()<<" Tracks(n="<<n<<"):"<<tracks[n].size()<<std::endl;
            tracks[m].splice(tracks[m].end(),tracks[n]);
            nNewTracks--;
//             std::cout<<"Joined "<<m<<"->"<<n<<" New num tracks:"<<nNewTracks<<"\n";
        }
    }
    //Compress down tracks.
    TrackVecT new_tracks(nNewTracks);
    //Remove empty tracks and any not meeting minFinalTrackLength
    std::copy_if(tracks.cbegin(), tracks.cend(), new_tracks.begin(), 
                    [this](const TrackT &t) {return t.size()>0 && (minFinalTrackLength<=1 || static_cast<IdxT>(t.size())>minFinalTrackLength);});
    tracks = new_tracks;
    //These are all now invalid since tracks variable is changed
    trackAssignment.clear();
    birthFrameIdx.clear();
    frameBirthStartIdx.clear();
    state = GAPS_CLOSED;
}

LAPTrack::SpMatT 
LAPTrack::computeGapCloseMatrix() const
{
    IdxT nTracks = static_cast<IdxT>(tracks.size());
    
    //These will be our sparse matrix format vectors
    std::vector<arma::uword> row_index;
    std::vector<arma::uword> col_index;
    std::vector<FloatT> values;
    IdxT reserve_size = nTracks*10; //Guesstimate amount of entries used
    row_index.reserve(reserve_size);
    col_index.reserve(reserve_size);
    values.reserve(reserve_size);

    FloatT position_gaussian_exponent_cuttoff = (maxPositionDisplacementSigma*maxPositionDisplacementSigma)/2.; //Only allow connections within 5 sigma
    VecT feature_gaussian_exponent_cuttoff = (maxFeatureDisplacementSigma%maxFeatureDisplacementSigma)/2.; //Only allow connections within 5 sigma
    
    FloatT norm_const = (nDims+nFeatures)*log2pi; //Pre-compute this
    FloatT birthC = -logrho-logkon;
    FloatT deathC= -logkoff;
//     std::cout<<"frameBirthStartIdx: "<<frameBirthStartIdx.t()<<"\n";
    
    //connect trackI to trackJ so trackJ must start after trackI ends.
    for(IdxT i=0; i<nTracks; i++){
        if(static_cast<IdxT>(tracks[i].size()) < minGapCloseTrackLength) continue; //Don't connect tracks shorter than minGapCloseTrackLength
        IdxT locI = tracks[i].back(); //last localization for track I.
        IdxT trackIend = frameIdx(locI); //frame death
        if (trackIend >= lastFrame-1) continue; //Tracks ending on last 2 frames can be a "start" point since this would be connected by F2F
        for(IdxT j=frameBirthStartIdx(trackIend+2-firstFrame); j<nTracks; j++){
            if(static_cast<IdxT>(tracks[j].size()) < minGapCloseTrackLength) continue; //Don't connect tracks shorter than minGapCloseTrackLength
            IdxT trackJstart = birthFrameIdx[j];
//             std::cout<<"Track:"<<j<<" Birth Loc:"<<tracks[j].front()<<std::endl;
//             std::cout<<" Birth Frame Idx:"<<frameIdx(tracks[j].front())<<std::endl;
//             std::cout<<" Recorded: "<<birthFrameIdx[j]<<std::endl;
            IdxT deltaT = trackJstart - trackIend;
//             std::cout<<"i("<<i<<") -> j("<<j<<"): endI:"<<trackIend<<" startJ:"<<trackJstart<<" deltaT:"<<deltaT<<"\n";
            if(deltaT<1) throw LogicalError("DeltaT should be positive.");
            if(deltaT>=maxGapCloseFrames) continue; //Gap must be at most maxGapCloseFrames
            IdxT locJ = tracks[j].front();
            FloatT DdT = 2*D*deltaT;
            FloatT total_dist_sq=0;
            FloatT C=0;
            bool feasible = true;
            for(IdxT d=0; d<nDims; d++){
                FloatT dist_var = DdT + SE_position(locI,d) + SE_position(locJ,d);
                FloatT dist = position(locI,d) - position(locJ,d);
                FloatT dist_sq = dist*dist;
                total_dist_sq += dist_sq;
                FloatT cost_exponent = dist_sq/dist_var;
//                 std::cout<<"Dim:"<<d<<" dist:"<<dist<<" dist_var:"<<dist_var<<" costExp:"<<cost_exponent<<" ExpCuttoff:"<<gaussian_exponent_cuttoff<<"\n";
//                 std::cout<<"SE_position(cur_idx,d):"<<SE_position(cur_idx,d)<<"SE_position(next_idx,d):"<<SE_position(next_idx,d)<<"\n";
                if(cost_exponent > position_gaussian_exponent_cuttoff) { //Too far away to be connected
                    feasible=false;
                    break;
                }
                C+= cost_exponent + log(dist_var);
            }
            if(!feasible) continue; //gaussian sigma constraint violated: move to next pair.
            if(maxSpeed>0 && sqrt(total_dist_sq)/deltaT > maxSpeed) continue; //maxSpeed constraint violated
            for(IdxT f=0; f<nFeatures; f++){
                FloatT feat_var = featureVar(f) + SE_feature(locI,f)+ SE_feature(locJ,f);
                FloatT feat_dist = feature(locI,f) - feature(locJ,f);
                FloatT cost_exponent = feat_dist*feat_dist/feat_var;
                if(cost_exponent > feature_gaussian_exponent_cuttoff(f)) { //Too far away to be connected
                    feasible=false;
                    break;
                }
                C+= cost_exponent + log(feat_var);
            }
            if(!feasible) continue; //move to next pair.
            //Otherwise we have a valid cost so normalize and record it
            C+= norm_const;
            C*= 0.5;
            C-= logkon +logkoff*deltaT;
            //Record cost
            row_index.push_back(i);
            col_index.push_back(j);
            values.push_back(C);
            //Record lower right block dummy cost
            row_index.push_back(nTracks+j);
            col_index.push_back(nTracks+i);
            values.push_back(cost_epsilon);
        }
    }
    
    for(IdxT i=0; i<nTracks; i++){
        //Fill in death costs
        row_index.push_back(i);
        col_index.push_back(nTracks+i);
        values.push_back(deathC);
        //Fill in birth costs
        row_index.push_back(nTracks+i);
        col_index.push_back(i);
        values.push_back(birthC);
    }
    IdxT nnz = values.size();
    UMatT locations(2,nnz);
    for(IdxT n=0; n<nnz;n++){
        locations(0,n) = row_index[n];
        locations(1,n) = col_index[n];
    }
    bool sort_them = true; //Make sure armadillo sorts the locations
    bool check_for_zeros = false; //Don't bother checking for zeros
    return {locations, VecT(values), 2*static_cast<arma::uword>(nTracks), 2*static_cast<arma::uword>(nTracks), sort_them, check_for_zeros};
}

} /* namespace tracker */

