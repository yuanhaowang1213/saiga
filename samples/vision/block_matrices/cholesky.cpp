/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */



#include "cholesky.h"

#include "saiga/time/timer.h"
#include "saiga/util/random.h"
#include "saiga/vision/BlockRecursiveBATemplates.h"
#include "saiga/vision/MatrixScalar.h"
#include "saiga/vision/VisionIncludes.h"
#include "saiga/vision/recursiveMatrices/Cholesky.h"
#include "saiga/vision/recursiveMatrices/ForwardBackwardSubs.h"
#include "saiga/vision/recursiveMatrices/Inverse.h"
#include "saiga/vision/recursiveMatrices/NeutralElements.h"
#include "saiga/vision/recursiveMatrices/Transpose.h"

using Block  = Eigen::Matrix<double, 2, 2>;
using Vector = Eigen::Matrix<double, 2, 1>;

using namespace Saiga;



namespace Saiga
{
template <typename _Scalar, int _Rows, int _Cols>
Eigen::Matrix<_Scalar, _Rows, 1> solveLLT(const Eigen::Matrix<_Scalar, _Rows, _Cols>& A,
                                          const Eigen::Matrix<_Scalar, _Rows, 1>& b)
{
    Eigen::Matrix<_Scalar, _Rows, _Cols> L;
    Eigen::Matrix<_Scalar, _Rows, 1> x, z;

    // compute L
    for (int i = 0; i < _Rows; i++)
    {
        // for all cols until diagonal
        for (int j = 0; j <= i; j++)
        {
            double s = 0;
            // dot product of row i with row j,
            // but only until col j
            // this requires the top left block to be computed
            for (int k = 0; k < j; k++)
            {
                s += L(i, k) * L(j, k);
            }
            s       = A(i, j) - s;
            L(i, j) = (i == j) ? sqrt(s) : (1.0 / L(j, j) * (s));
            L(j, i) = L(i, j);
        }
    }

    // forward / backward substituion
    z = L.template triangularView<Eigen::Lower>().solve(b);
    x = L.template triangularView<Eigen::Upper>().solve(z);

    return x;
}

inline double inverse(double d)
{
    return 1.0 / d;
}

template <typename _Scalar, int _Rows, int _Cols>
Eigen::Matrix<_Scalar, _Rows, 1> solveLDLT(const Eigen::Matrix<_Scalar, _Rows, _Cols>& A,
                                           const Eigen::Matrix<_Scalar, _Rows, 1>& b)
{
    Eigen::Matrix<_Scalar, _Rows, _Cols> L;
    Eigen::DiagonalMatrix<_Scalar, _Rows> D;
    Eigen::DiagonalMatrix<_Scalar, _Rows> Dinv;
    Eigen::Matrix<_Scalar, _Rows, 1> x, y, z;

    // compute L
    for (int i = 0; i < _Rows; i++)
    {
        // compute Dj
        _Scalar sumd = _Scalar(0);

        for (int j = 0; j < i; ++j)
        {
            // compute all l's for this row

            _Scalar sum = _Scalar(0);
            for (int k = 0; k < j; ++k)
            {
                sum += L(i, k) * L(j, k) * D.diagonal()(k);
            }
            L(i, j) = Dinv.diagonal()(j) * (A(i, j) - sum);
            L(j, i) = 0;
            sumd += L(i, j) * L(i, j) * D.diagonal()(j);
        }
        L(i, i)            = 1;
        D.diagonal()(i)    = A(i, i) - sumd;
        Dinv.diagonal()(i) = inverse(D.diagonal()(i));
    }

    //    cout << L << endl << endl;

    // forward / backward substituion
    z = L.template triangularView<Eigen::Lower>().solve(b);
    y = Dinv * z;
    x = L.transpose().template triangularView<Eigen::Upper>().solve(y);

    return x;
}



template <typename MatrixType, typename VectorType>
VectorType solveLDLT2(const MatrixType& A, const VectorType& b)
{
    SAIGA_ASSERT(A.rows() == A.cols() && A.rows() == b.rows());
    using MatrixScalar = typename MatrixType::Scalar;
    using VectorScalar = typename VectorType::Scalar;

    MatrixType L;
    L.resize(A.rows(), A.cols());

    Eigen::DiagonalMatrix<MatrixScalar, MatrixType::RowsAtCompileTime> D;
    Eigen::DiagonalMatrix<MatrixScalar, MatrixType::RowsAtCompileTime> Dinv;
    D.resize(A.rows());
    Dinv.resize(A.rows());

    VectorType x, y, z;
    x.resize(A.rows());
    y.resize(A.rows());
    z.resize(A.rows());


    // compute L
    for (int i = 0; i < A.rows(); i++)
    {
        // compute Dj
        MatrixScalar sumd = AdditiveNeutral<MatrixScalar>::get();

        for (int j = 0; j < i; ++j)
        {
            // compute all l's for this row
            MatrixScalar sum = AdditiveNeutral<MatrixScalar>::get();

            // dot product of row i with row j
            // but only until column j
            for (int k = 0; k < j; ++k)
            {
                sum += L(i, k) * D.diagonal()(k) * transpose(L(j, k));
            }

            L(i, j) = (A(i, j) - sum) * Dinv.diagonal()(j);
            L(j, i) = AdditiveNeutral<MatrixScalar>::get();
            sumd += L(i, j) * D.diagonal()(j) * transpose(L(i, j));
        }
        L(i, i)            = MultiplicativeNeutral<MatrixScalar>::get();
        D.diagonal()(i)    = A(i, i) - sumd;
        Dinv.diagonal()(i) = inverse(D.diagonal()(i));
    }

    z = forwardSubstituteDiagOne(L, b);
    //    z = forwardSubstituteDiagOne2(L, b);
    y = multDiagVector(Dinv, z);
    x = backwardSubstituteDiagOneTranspose(L, y);


#if 0
    // Test if (Ax-b)==0
    double test =
        (fixedBlockMatrixToMatrix(A) * fixedBlockMatrixToMatrix(x) - fixedBlockMatrixToMatrix(b)).squaredNorm();
    cout << "error solveLDLT2: " << test << endl;
#endif


    return x;
}

void testBlockCholesky()
{
    cout << "testBlockCholesky" << endl;
    using CompleteMatrix = Eigen::Matrix<double, 4, 4>;
    using CompleteVector = Eigen::Matrix<double, 4, 1>;



    CompleteMatrix A;
    CompleteVector x;
    CompleteVector b;


    CompleteVector t = CompleteVector::Random();
    A                = t * t.transpose();
    b                = CompleteVector::Random();
    A.diagonal() += CompleteVector::Ones();

    Eigen::Matrix<MatrixScalar<Block>, 2, 2> bA;
    Eigen::Matrix<MatrixScalar<Vector>, 2, 1> bx, bb;
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            bA(i, j) = A.block(i * 2, j * 2, 2, 2);
        }
        bb(i) = b.segment(i * 2, 2);
    }

#if 1

    {
        x = A.llt().solve(b);
        cout << "x " << x.transpose() << endl;
        cout << "error: " << (A * x - b).squaredNorm() << endl;
    }
    //    {
    //        x = solveLLT(A, b);
    //        cout << "x " << x.transpose() << endl;
    //        cout << "error: " << (A * x - b).squaredNorm() << endl;
    //    }

    {
        x = solveLDLT(A, b);
        cout << "x " << x.transpose() << endl;
        cout << "error: " << (A * x - b).squaredNorm() << endl;
    }

    {
        //        x = solveLDLT2(A, b);
        DenseLDLT<decltype(A), decltype(b)> ldlt;
        ldlt.compute(A);
        x = ldlt.solve(b);

        cout << "x " << x.transpose() << endl;
        cout << "error: " << (A * x - b).squaredNorm() << endl;
    }

    {
        bx = solveLDLT2(bA, bb);

        x = fixedBlockMatrixToMatrix(bx);
        cout << "x " << x.transpose() << endl;
        cout << "error: " << (A * x - b).squaredNorm() << endl;
    }
#endif
    {
        DenseLDLT<decltype(bA), decltype(bb)> ldlt;
        ldlt.compute(bA);
        bx = ldlt.solve(bb);


        //        decltype(A) inv = inverseCholesky(A);
        //        cout << (inv) << endl << endl;
        //        cout << fixedBlockMatrixToMatrix(bA).inverse() << endl << endl;

        //        return;
        //        bx = solveLDLT2(bA, bb);

        x = fixedBlockMatrixToMatrix(bx);
        cout << "x " << x.transpose() << endl;
        cout << "error: " << (A * x - b).squaredNorm() << endl;
    }
}

}  // namespace Saiga
