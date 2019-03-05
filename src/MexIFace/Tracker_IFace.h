/** @file Tracker_IFace.h
 * @author Mark J. Olah (mjo\@cs.unm.edu)
 * @date 2015-2019
 * @brief The class declaration and inline and templated functions for Tracker_IFace.
 */

#ifndef TRACKER_TRACKER_IFACE_H
#define TRACKER_TRACKER_IFACE_H

#include<functional>

#include "MexIFace/MexIFace.h"
#include "Tracker/Tracker.h"

template<class TrackerT>
class Tracker_IFace : public mexiface::MexIFace, public mexiface::MexIFaceHandler<TrackerT>
{
public:
    Tracker_IFace();

protected:
    using mexiface::MexIFaceHandler<TrackerT>::obj;
    using FloatT = typename TrackerT::FloatT;
    using IdxT = typename TrackerT::IdxT;
    //Constructor
    void objConstruct() override;

    //Non-static method calls
    void objInitializeTracks();
    void objGetTracks();
    void objDebugF2F();
    void objLinkF2F();
    void objCloseGaps();
    void objDebugCloseGaps();
    void objGenerateTracks();
    void objGetStats();
};

template<class TrackerT>
void Tracker_IFace<TrackerT>::objConstruct()
{
    // args:
    // [params] - struct of named double vectors.
    checkNumArgs(1,1);
    this->outputHandle(new TrackerT(getVecDict()));
}

template<class TrackerT>
Tracker_IFace<TrackerT>::Tracker_IFace()
{
    methodmap["initializeTracks"] = std::bind(&Tracker_IFace::objInitializeTracks, this);
    methodmap["debugF2F"] = std::bind(&Tracker_IFace::objDebugF2F, this);
    methodmap["debugCloseGaps"] = std::bind(&Tracker_IFace::objDebugCloseGaps, this);
    methodmap["linkF2F"] = std::bind(&Tracker_IFace::objLinkF2F, this);
    methodmap["closeGaps"] = std::bind(&Tracker_IFace::objCloseGaps, this);
    methodmap["getTracks"] = std::bind(&Tracker_IFace::objGetTracks, this);
    methodmap["getStats"] = std::bind(&Tracker_IFace::objGetStats, this);
    methodmap["generateTracks"] = std::bind(&Tracker_IFace::objGenerateTracks, this);
}


template<class TrackerT>
void Tracker_IFace<TrackerT>::objInitializeTracks()
{
    // [in]
    //  frameIdx - vector giving frame of each localization (1-based)
    //  positions - matrix of positions as columns: [x y].
    //  SE_positions - matrix standard errors of positions as columns: [SE_x SE_y].
    //  features -  [optional] matrix of features as columns: [f1 f2 ... fn].
    //  SE_features - [optional] matrix standard errors of features as columns: [SE_f1 SE_f2 ... SE_fn].
    auto frameIdx = getVec<IdxT>();
    auto position = getMat<FloatT>();
    auto SE_position = getMat<FloatT>();
    if(nrhs==3) {
        obj->initializeTracks(frameIdx,position, SE_position);
    } else if(nrhs==5){
        auto feature = getMat<FloatT>();;
        auto SE_feature = getMat<FloatT>();;
        obj->initializeTracks(frameIdx,position, SE_position, feature, SE_feature);
    } else {
        error("NArgs","Invalid number of arguments!");
    }
}

template<class TrackerT>
void Tracker_IFace<TrackerT>::objLinkF2F()
{
    // [out]
    //  nTracks - number of tracks after frame2frame (-1 for error)
    checkNumArgs(1,0);
    obj->linkF2F();
    output(obj->tracks.size());
}

template<class TrackerT>
void Tracker_IFace<TrackerT>::objCloseGaps()
{
    // [out]
    //  nTracks - number of tracks after gapClose (-1 for error)
    checkNumArgs(1,0);
    obj->closeGaps();
    output(obj->tracks.size());
}

template<class TrackerT>
void Tracker_IFace<TrackerT>::objGenerateTracks()
{
    // [out]
    //  tracks - cell array of vectors.  Each vector is one track and lists the indexs of the localizations.
    checkNumArgs(1,0);
    obj->generateTracks();
    output(obj->tracks);
}

template<class TrackerT>
void Tracker_IFace<TrackerT>::objGetTracks()
{
    //Get the current state of the tracks, which can be called between calls to linkF2F() and closeGaps()
    //[out]
    //  tracks - cell array of vectors.  Each vector is one track and lists the indexs of the localizations.
    checkNumArgs(1,0);
    output(obj->tracks);
}

template<class TrackerT>
void Tracker_IFace<TrackerT>::objDebugF2F()
{
    // [in]
    //  frameIdx - integer
    // [out]
    //  cur_locs - indexs of current frame localizations
    //  next_locs - indexs of next frame localizations
    //  costs - cost matrix
    //  connections - 2xn matrix of connections.  -1 represents birth or death
    //  conn_costs - Costs for selected connections
    checkNumArgs(5,1);
    auto frameIdx = getScalar<typename TrackerT::IdxT>();
    typename TrackerT::IVecT cur_locs;
    typename TrackerT::IVecT next_locs;
    typename TrackerT::SpMatT costs;
    typename TrackerT::IMatT connections;
    typename TrackerT::VecT conn_costs;
    obj->debugF2F(frameIdx, cur_locs, next_locs, costs, connections, conn_costs);
    output(cur_locs);
    output(next_locs);
    output(costs);
    output(connections);
    output(conn_costs);
}

template<class TrackerT>
void Tracker_IFace<TrackerT>::objDebugCloseGaps()
{
    // [in]
    //   -
    // [out]
    //  costs - cost matrix
    //  connections - 2xn matrix of connections.  0 represents birth or death
    //  conn_costs - Costs for selected connections
    checkNumArgs(3,0);
    typename TrackerT::SpMatT costs;
    typename TrackerT::IMatT connections;
    typename TrackerT::VecT conn_costs;
    obj->debugCloseGaps(costs, connections, conn_costs);
    output(costs);
    output(connections);
    output(conn_costs);
}

template<class TrackerT>
void Tracker_IFace<TrackerT>::objGetStats()
{
    // [out]
    //  stats - struct of named doubles with params and various statistics on the tracks
    checkNumArgs(1,0);
    output(obj->getStats());
}

#endif /* TRACKER_TRACKER_IFACE_H */
