/** @file Tracker.cpp
 *  @author Mark J. Olah (mjo at cs.unm.edu)
 *  @date 04-2015
 *  @brief The member definitions for Tracker
 */

#include <cmath>

#include "Tracker/Tracker.h"

namespace tracker {

const Tracker::FloatT Tracker::log2pi = log(2*arma::Datum<Tracker::FloatT>::pi);

Tracker::Tracker(const VecParamT &)
{
}

Tracker::VecParamT Tracker::getStats() const
{
    VecParamT stats;
    stats["nLocalizations"] = N;
    stats["nDims"] = nDims;
    stats["nFeatures"] = nFeatures;
    stats["firstFrame"] =firstFrame;
    stats["lastFrame"] = lastFrame;
    stats["nFrames"] = nFrames;
    stats["nTracks"] = tracks.size();
    stats["nLocalizationsAssigned"] = arma::sum(trackAssignment>=0);
    return stats;
}

void Tracker::initializeTracks(const IVecT &frameIdx_, const MatT &position_, const MatT &SE_position_)
{
    MatT feature_, SE_feature_;
    initializeTracks(frameIdx_, position_, SE_position_,feature_, SE_feature_); //Call with empty features
}

void Tracker::initializeTracks(const IVecT &frameIdx_, const MatT &position_, const MatT &SE_position_, const MatT &feature_, const MatT &SE_feature_)
{
    if(frameIdx_.n_elem != position_.n_rows){
        std::ostringstream msg;
        msg<<"Bad tracks sizing. Expected frameIdx.n_elem="<<frameIdx_.n_elem<<" == position.n_rows="<<position_.n_rows;
        throw ParameterValueError(msg.str());
    }
    if(frameIdx_.n_elem != SE_position_.n_rows){
        std::ostringstream msg;
        msg<<"Bad tracks sizing. Expected frameIdx.n_elem="<<frameIdx_.n_elem<<" == SE_position.n_rows="<<SE_position_.n_rows;
        throw ParameterValueError(msg.str());
    }
    if(!feature.is_empty() && frameIdx_.n_elem != feature_.n_rows){
        std::ostringstream msg;
        msg<<"Bad tracks sizing. Expected frameIdx.n_elem="<<frameIdx_.n_elem<<" == feature.n_rows="<<feature_.n_rows;
        throw ParameterValueError(msg.str());
    }
    if(!feature.is_empty() && frameIdx_.n_elem != SE_feature_.n_rows){
        std::ostringstream msg;
        msg<<"Bad tracks sizing. Expected frameIdx.n_elem="<<frameIdx_.n_elem<<" == SE_feature.n_rows="<<SE_feature_.n_rows;
        throw ParameterValueError(msg.str());
    }
    if(position_.n_cols != SE_position_.n_cols){
        std::ostringstream msg;
        msg<<"Bad tracks sizing. Expected position.n_cols="<<position_.n_cols<<" == SE_position.n_cols="<<SE_position_.n_cols;
        throw ParameterValueError(msg.str());
    }
    if(!feature.is_empty() && feature_.n_cols != SE_feature_.n_cols){
        std::ostringstream msg;
        msg<<"Bad tracks sizing. Expected feature.n_cols="<<feature_.n_cols<<" == SE_feature.n_cols="<<SE_feature_.n_cols;
        throw ParameterValueError(msg.str());
    }

    N = static_cast<IdxT>(frameIdx_.n_elem);
    nDims = static_cast<IdxT>(position_.n_cols)/2;
    nFeatures = static_cast<IdxT>(feature_.n_cols)/2;
    frameIdx = frameIdx_;
    position = position_;
    SE_position = SE_position_;
    feature = feature_;
    SE_feature = SE_feature_;

    //Clear track data structures
    tracks.clear();
    tracks.reserve(static_cast<IdxT>(ceil(sqrt(N))));
    trackAssignment.set_size(N);
    trackAssignment.fill(-1);

    //Initialize number of frames and range
    arma::uvec sFrameIdx = arma::stable_sort_index(frameIdx);//Ensure sort is stable.
    firstFrame = frameIdx(sFrameIdx(0));
    lastFrame = frameIdx(sFrameIdx(N-1));
    nFrames = lastFrame-firstFrame+1;
    //Initialize frameLocIdx - the list of localizations for each frame
    IndexVectorT buf;//buffer to store the current frames indexs
    buf.reserve(std::max(N,10*(N/nFrames)));//Pre-allocate approximate ammount of needed space for efficiency
    nFrameLocs.set_size(nFrames);
    frameLocIdx.set_size(nFrames);
    IdxT cur_frame=firstFrame; // The current frame we are processing
    for(IdxT n=0; n<N; n++){
        IdxT next_loc_idx = sFrameIdx(n);
        IdxT next_loc_frame = frameIdx(next_loc_idx);
        if (next_loc_frame>cur_frame) { //Next loc is from a future frame.
            frameLocIdx(cur_frame-firstFrame) = IVecT(buf);
            buf.clear(); //Reset for new cur_frame;
            while(++cur_frame<next_loc_frame) frameLocIdx(cur_frame-firstFrame).reset(); //zero-out blank frames
        }
        buf.push_back(next_loc_idx);//Add to loc to cur_frame
    }
    frameLocIdx(cur_frame-firstFrame) = IVecT(buf);
    if(cur_frame!=lastFrame) {
        std::ostringstream msg;
        msg<<"initializeTracks: Initialize frameLocIdx. Expected cur_frame="<<cur_frame<<" == lastFrame="<<lastFrame;
        throw LogicalError(msg.str());
    }
    for(IdxT n=0; n<nFrames; n++) nFrameLocs(n)= frameLocIdx(n).n_elem;
    
//     IdxT sum=0;
//     for(IdxT n=0; n<nFrames; n++) sum+=frameLocIdx(n).n_elem;
//     std::cout<<"frameLocIdx(0): "<<frameLocIdx(0).t()<<std::endl;
//     std::cout<<"frameLocIdx(1): "<<frameLocIdx(1).t()<<std::endl;
//     std::cout<<"nFrameLocs: "<<nFrameLocs.t()<<std::endl;
//     
//     std::cout<<"Sum: "<<sum<<std::endl;
//     assert(sum==N);
}

void Tracker::printTracks() const
{
    IdxT nTracks = static_cast<IdxT>(tracks.size());
    std::cout<<"Number of tracks: "<<nTracks<<"\n";
    for(IdxT n=0; n<nTracks; n++){
        IdxT start = frameIdx(tracks[n].front());
        IdxT end = frameIdx(tracks[n].back());
        IdxT len = static_cast<IdxT>(tracks[n].size());
        std::cout<<"Track["<<n<<"]: StartFrame: "<<start<<" EndFrame: "<<end<<" #Locs:"<<len<<"\n";
        std::cout<<"  Locs: ";
        for(auto k=tracks[n].cbegin(); k!=tracks[n].cend(); ++k) std::cout<<*k<<", ";
        std::cout<<std::endl;
    }
}

} /* namespace tracker */
