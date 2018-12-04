/** @file LAP_JVSparse.h
 * @author Mark J. Olah (mjo\@cs.unm.edu)
 * @date 05-2015
 * @brief The class declaration for the LAP Jonker Volgenant algorithm
 *
 * This is a modern dense/sparse C++ implementation of Jonker Volgenant algoirthm using armadillo
 * and presenting C++ and Matlab interface.
 * 
 * Adapted from text of Jonker and Volgenant. Computing 38, 324-340 (1986)
 * 
 */
#ifndef TRACKER_LAP_JVSPARSE_H
#define TRACKER_LAP_JVSPARSE_H

#include <armadillo>
#include <vector>

namespace tracker {

template<class FloatT>
class LAP_JVSparse {
    using IdxT = int32_t; //The type for the indexes
    using SpMatT = arma::SpMat<FloatT>;
    using VecT = arma::Col<FloatT>;
    using IVecT = arma::Col<IdxT>;
    using IMatT = arma::Mat<IdxT>;

public:
    static IVecT solve(const SpMatT &C);
    static void solveLAP_orig(const SpMatT &C, IVecT &x, IVecT &y, VecT &u, VecT &v);
    static VecT computeCost(const SpMatT &C, const IVecT &row_sol);

    static bool checkCosts(const SpMatT &C);
    static bool checkSolution(const SpMatT &C,const IVecT &x, const IVecT &y, const VecT &u, const VecT &v);

private:
    /* The original sparse lapjv code which is outdated and should be updated. */
    static void lap_orig(IdxT n, const FloatT C_vals[], const IdxT C_cols[], const IdxT C_row_ptrs[],
                           IdxT x[], IdxT y[], FloatT u[], FloatT v[]);    
};

} /* namespace tracker */

#endif /* TRACKER_LAP_JVSPARSE_H */
