﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */


#pragma once

#include "BABase.h"
//#define RECURSIVE_BA_VECTORIZE
//#define RECURSIVE_BA_FLOAT
#define RECURSIVE_BA_USE_TIMERS false
#include "saiga/vision/recursiveMatrices/BlockRecursiveBATemplates.h"
namespace Saiga
{
class SAIGA_VISION_API BARec : public BABase
{
   public:
    BARec() : BABase("Recursive BA"), U(A.u), V(A.v), W(A.w) {}
    virtual ~BARec() {}
    virtual void solve(Scene& scene, const BAOptions& options) override;

   private:
    // ==== Structure information ====
    int n, m;
    int observations;
    int schurEdges;
    std::vector<std::vector<int>> schurStructure;

    // Number of seen world points for each camera + the corresponding exclusive scan and sum
    std::vector<int> cameraPointCounts, cameraPointCountsScan;
    // Number of observing cameras for each world point+ the corresponding exclusive scan and sum
    std::vector<int> pointCameraCounts, pointCameraCountsScan;

    // Main (recursive) Variables for the system Ax=b
    SymmetricMixedMatrix2<UType, VType, WType> A;

    //    SymmetricMixedMatrix22<
    //        Eigen::DiagonalMatrix<MatrixScalar<Eigen::Matrix<BlockBAScalar, blockSizeCamera, blockSizeCamera>>, -1>,
    //        Eigen::DiagonalMatrix<MatrixScalar<Eigen::Matrix<BlockBAScalar, blockSizePoint, blockSizePoint>>, -1>,
    //        Eigen::SparseMatrix<MatrixScalar<Eigen::Matrix<BlockBAScalar, blockSizeCamera, blockSizePoint>>,
    //                            Eigen::RowMajor>,
    //        Eigen::SparseMatrix<MatrixScalar<Eigen::Matrix<BlockBAScalar, blockSizePoint, blockSizeCamera>>,
    //                            Eigen::RowMajor>>
    //        A;

    //    MixedVector2<DAType, DBType> x, b;
    MixedVector2<Eigen::Matrix<MatrixScalar<Eigen::Matrix<BlockBAScalar, blockSizeCamera, 1>>, -1, 1>,
                 Eigen::Matrix<MatrixScalar<Eigen::Matrix<BlockBAScalar, blockSizePoint, 1>>, -1, 1>>
        delta_x, b;

    MixedRecursiveSolver<SymmetricMixedMatrix2<UType, VType, WType>, MixedVector2<DAType, DBType>> solver;

    // These are only reference into the global matrix A
    UType& U;
    VType& V;
    WType& W;

    AlignedVector<SE3> x_u, oldx_u;
    AlignedVector<Vec3> x_v, oldx_v;

    //    DAType& da;
    //    DBType& db;
    //    DAType& ea;
    //    DBType& eb;


    //    std::vector<int> imageIds;
    std::vector<int> validImages;
    std::vector<int> validPoints;
    std::vector<int> pointToValidMap;

    BAOptions options;

    double chi2;
    double lambda = 1.0 / 1.00e+04;

    void plus();

    void initStructure(Scene& scene);
    bool computeUVW(Scene& scene);
    void updateScene(Scene& scene);

    bool explizitSchur = false;
    bool computeWT     = true;
};


}  // namespace Saiga
