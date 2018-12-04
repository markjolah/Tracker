/** @file Tracker.h
* @author Mark J. Olah (mjo\@cs.unm.edu)
* @date 02-2015
* @brief The class declaration and inline and templated functions for Tracker.
*
* The base class for all Tracking models
* 
* 
* Insted of templating on the FloatT type, which is problematic for inheritance hierarchies of templated
* base classes.  Instead wuse a typedef to allow configuration of use with either float/double.  Default
* is double.
* 
*/
#ifndef TRACKER_TRACKER_H
#define TRACKER_TRACKER_H

#include <cstdint>
#include <armadillo>
#include <map>
#include <string>
#include <list>
#include <vector>

#include <BacktraceException/
namespace tracker {


using TrackerError = backtrace_exception::BacktraceException;


/** @brief Parameter value is not valid.
 */
struct ParameterValueError : public TrackerError
{
    ParameterValueError(std::string message) : TrackerError("ParameterValueError",message) {}
};

/** @brief Parameter value is not valid.
 */
struct LogicalError : public TrackerError
{
    LogicalError(std::string message) : TrackerError("LogicalError",message) {}
};

class Tracker {
public:
    using FloatT = double; /* Set this to control float/double settings */
    using IdxT = int32_t;
    using VecT = arma::Col<FloatT>;
    using MatT = arma::Mat<FloatT>;
    using IVecT = arma::Col<IdxT>;
    using IMatT = arma::Mat<IdxT>;
    using IVecFieldT = arma::field<IVecT>;
    using IndexVectorT = std::vector<IdxT>;
    using TrackT =  std::list<IdxT>; /**< A type for an individual track*/
    using TrackVecT = std::vector<TrackT>;       /**< A type for a vector of tracks*/
    using ParamT = std::map<std::string,FloatT>;  /**< A convenient form for reporting dictionaries of named FP data to matlab */
    using VecParamT = std::map<std::string,VecT>;  /**< A convenient form for reporting dictionaries of named FP data to matlab */

    IdxT N = 0; // Number of emitters
    IdxT nDims = 0; //number of columns for postions
    IdxT nFeatures = 0;  //number of columns for features
    IVecT frameIdx; // length: N
    MatT position; // N x nDims;
    MatT SE_position; // N x nDims;
    MatT feature; // N x nFeatures;
    MatT SE_feature; // N x nFeatures;
    IdxT firstFrame = 0; //index of first frame
    IdxT lastFrame = 0; //index of last frame
    IdxT nFrames = 0; //lastFrame-firstFrame+1

    //Pre-computed on initialization
    IVecT nFrameLocs; //number of localizations for each frame, continuous indexing from firstFrame=0 to lastFrame=nFrames-1
    IVecFieldT frameLocIdx; //A field for each frame giving the indexes of localizations, continuous indexing from firstFrame=0 to lastFrame=nFrames-1

    //Computed by tracking
    TrackVecT tracks; //A vector of vectors.  Each vector reprsents a track by a sequence of localization indexes

    /**
     * param - A dictionary of floating point values to pass in.  This is a flexible interface to
     * the higher-level matlab code allowing each subclass to take in arbitrary floating point arguments.
     */
    Tracker(const VecParamT &param);
    virtual ~Tracker() {}
    virtual VecParamT getStats() const;
    virtual void initializeTracks(const IVecT &frameIdx_, const MatT &position_, const MatT &SE_position_);
    virtual void initializeTracks(const IVecT &frameIdx_, const MatT &position_, const MatT &SE_position_, const MatT &feature_, const MatT &SE_feature_);
    virtual void generateTracks()=0;
    
    void printTracks() const;
protected:
    static const FloatT log2pi;// = log(2*pi);
    IVecT trackAssignment; //A vector giving the track index of each localizations
};

} /* namespace tracker */

#endif /* TRACKER_TRACKER_H */
