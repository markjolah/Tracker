
#include<iostream>
#include<armadillo>
#include "Tracker/LAPTrack.h"
#include "Tracker/LAP_JVSparse.h"

using namespace arma;
using namespace std;
using namespace tracker;

void testLAP()
{
    mat C;
    C<<11.1<<0<<5<<3<<9<<3<<endr
     <<5<<0<<0<<2<<1<<6<<endr
     <<0<<0<<1<<15<<10<<7<<endr
     <<7.1<<7.2<<7.3<<7.4<<7.5<<7.6<<endr
     <<3<<1<<1<<0<<0<<6<<endr
     <<0<<6<<3<<4<<0<<0<<endr;
    sp_mat Csp(C);    
    auto row_sol = LAP_JVSparse<double>::solve(Csp);
    std::cout<<"RowSol: "<<row_sol.t()<<"\n";
}

void testTracking()
{
    int Nframes = 20;
    auto frameIdx = randi<Tracker::IVecT>(Nframes,distr_param(1, 3*Nframes));
    frameIdx+=1000;
    frameIdx=unique(sort(frameIdx));
    int k = static_cast<int>(frameIdx.n_elem);
    if( k < Nframes) {
        frameIdx.resize(Nframes);
        for(int i=k; i<Nframes; i++) frameIdx(i)=frameIdx(i-1)+1;
    }
    std::cout<<"FrameIdx: "<<frameIdx.t()<<"\n";
    
    mat position = randn<mat>(Nframes,2)*3;
    mat SE_position = randn<mat>(Nframes,2)*0.3;
    std::cout<<"Position: "<<position<<"\n";
    std::cout<<"SEPosition: "<<SE_position<<"\n";
    Tracker::VecParamT params;
    params["D"]=0.3;
    params["kon"]=0.1;
    params["koff"]=0.1;
    params["rho"]=0.02;
    params["maxSpeed"] = -1;
    params["maxPositionDisplacementSigma"] = 5;
    params["maxFeatureDisplacementSigma"] = 5;
    params["maxGapCloseFrames"] = 5;
    params["minGapCloseTrackLength"] = 1;
    params["minFinalTrackLength"] = 1;
    params["featureVar"] = {2, 3};
    LAPTrack tracker(params);
    tracker.initializeTracks(frameIdx, position, SE_position);
    LAPTrack::SpMatT C;
    Tracker::IVecT cur_locs, next_locs;
    Tracker::VecT conn_costs;
    Tracker::IMatT connections;
    tracker.debugF2F(frameIdx(0),cur_locs, next_locs,C,connections, conn_costs);
    mat bigC(C);
    std::cout<<"CurLocs:\n"<<cur_locs.t()<<"\n";
    std::cout<<"NextLocs:\n"<<next_locs.t()<<"\n";
    std::cout<<"CostMat:\n"<<bigC<<"\n";
    std::cout<<"Connections: \n"<<connections<<"\n";
    std::cout<<"ConnectionCosts: \n"<<conn_costs<<"\n";
    tracker.printTracks();
    tracker.linkF2F();
    tracker.printTracks();
    tracker.closeGaps();
    tracker.printTracks();
// //     tracker.getTracks();
}

int main()
{
    testLAP();
    cout<<" =========== TRACKING ====================\n";
    testTracking();
    return 0;
}

