﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once
#include "saiga/config.h"

namespace Saiga
{
struct LinearSolverOptions
{
    // Base Options used by almost every solver
    enum class SolverType : int
    {
        Iterative = 0,
        Direct    = 1
    };
    SolverType solverType      = SolverType::Iterative;
    int maxIterativeIterations = 50;
    double iterativeTolerance  = 1e-5;

    // Schur complement options (not used by every solver)
    bool buildExplizitSchur = false;
    bool cholmod            = false;
};

/**
 * A solver for linear systems of equations. Ax=b
 * This class is spezialized for different structures of A.
 */
template <typename AType, typename XType>
struct MixedSymmetricRecursiveSolver
{
};


}  // namespace Saiga
