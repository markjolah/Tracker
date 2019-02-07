/** @file LAPTrack_Iface.cpp
 *  @author Mark J. Olah (mjo at cs.unm.edu)
 *  @date 2015-2019
 *  @brief The entry point for LAPTrack_Iface mex module.
 * 
 */
#include "Tracker/LAPTrack.h"
#include "Tracker_IFace.h"

Tracker_IFace<tracker::LAPTrack> iface; /**< Global iface object provides a iface.mexFunction */

void mexFunction(int nlhs, mxArray *lhs[], int nrhs, const mxArray *rhs[])
{
    iface.mexFunction(nlhs, lhs, nrhs, rhs);
}
