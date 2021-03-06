//------------------------------------------------
//               Ch09_05.cpp
//------------------------------------------------

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include "Ch09_05.h"
#include "Matrix.h"

using namespace std;

void AvxMat4x4TransposeF64(Matrix<double>& m_src1)
{
    const size_t nr = 4;
    const size_t nc = 4;
    Matrix<double> m_des1(nr ,nc);
    Matrix<double> m_des2(nr ,nc);

    Matrix<double>::Transpose(m_des1, m_src1);
    AvxMat4x4TransposeF64_(m_des2.Data(), m_src1.Data());

    cout << fixed << setprecision(1);
    m_src1.SetOstream(12, "  ");
    m_des1.SetOstream(12, "  ");
    m_des2.SetOstream(12, "  ");

    cout << "Results for AvxMat4x4TransposeF64\n";
    cout << "Matrix m_src1\n" << m_src1 << '\n';
    cout << "Matrix m_des1\n" << m_des1 << '\n';
    cout << "Matrix m_des2\n" << m_des2 << '\n';

    if (m_des1 != m_des2)
        cout << "\nMatrix compare failed - AvxMat4x4TransposeF64\n";
}

void AvxMat4x4MulF64(Matrix<double>& m_src1, Matrix<double>& m_src2)
{
    const size_t nr = 4;
    const size_t nc = 4;
    Matrix<double> m_des1(nr ,nc);
    Matrix<double> m_des2(nr ,nc);

    Matrix<double>::Mul(m_des1, m_src1, m_src2);
    AvxMat4x4MulF64_(m_des2.Data(), m_src1.Data(), m_src2.Data());

    cout << fixed << setprecision(1);

    m_src1.SetOstream(12, "  ");
    m_src2.SetOstream(12, "  ");
    m_des1.SetOstream(12, "  ");
    m_des2.SetOstream(12, "  ");

    cout << "\nResults for AvxMat4x4MulF64\n";
    cout << "Matrix m_src1\n" << m_src1 << '\n';
    cout << "Matrix m_src2\n" << m_src2 << '\n';
    cout << "Matrix m_des1\n" << m_des1 << '\n';
    cout << "Matrix m_des2\n" << m_des2 << '\n';

    if (m_des1 != m_des2)
        cout << "\nMatrix compare failed - AvxMat4x4MulF64\n";
}

int main()
{
    const size_t nr = 4;
    const size_t nc = 4;
    Matrix<double> m_src1(nr ,nc);
    Matrix<double> m_src2(nr ,nc);

    const double src1_row0[] = { 10, 11, 12, 13 };
    const double src1_row1[] = { 20, 21, 22, 23 };
    const double src1_row2[] = { 30, 31, 32, 33 };
    const double src1_row3[] = { 40, 41, 42, 43 };

    const double src2_row0[] = { 100, 101, 102, 103 };
    const double src2_row1[] = { 200, 201, 202, 203 };
    const double src2_row2[] = { 300, 301, 302, 303 };
    const double src2_row3[] = { 400, 401, 402, 403 };

    m_src1.SetRow(0, src1_row0);
    m_src1.SetRow(1, src1_row1);
    m_src1.SetRow(2, src1_row2);
    m_src1.SetRow(3, src1_row3);

    m_src2.SetRow(0, src2_row0);
    m_src2.SetRow(1, src2_row1);
    m_src2.SetRow(2, src2_row2);
    m_src2.SetRow(3, src2_row3);

    // Test functions
    AvxMat4x4TransposeF64(m_src1);
    AvxMat4x4MulF64(m_src1, m_src2);

    // Benchmark functions
    AvxMat4x4TransposeF64_BM();
    AvxMat4x4MulF64_BM();
    return 0;
}
