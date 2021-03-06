//------------------------------------------------
//               Ch14_03.cpp
//------------------------------------------------

#include "stdafx.h"
#include <iostream>
#include <cassert>
#include "Ch14_03.h"
#include "AlignedMem.h"

using namespace std;

extern "C" const size_t c_NumPixelsMax = 16777216;

bool Avx512ComparePixelsCpp(uint8_t* des, const uint8_t* src, size_t num_pixels, CmpOp cmp_op, uint8_t cmp_val)
{
    // Make sure num_pixels is valid
    if ((num_pixels == 0) || (num_pixels > c_NumPixelsMax))
        return false;
    if ((num_pixels & 0x3f) != 0)
        return false;

    // Make sure src and des are aligned on a 64-byte boundary
    if (!AlignedMem::IsAligned(src, 64))
        return false;
    if (!AlignedMem::IsAligned(des, 64))
        return false;

    bool rc = true;
    const uint8_t cmp_false = 0x00;
    const uint8_t cmp_true = 0xff;

    switch (cmp_op)
    {
        case CmpOp::EQ:
            for (size_t i = 0; i < num_pixels; i++)
                des[i] = (src[i] == cmp_val) ? cmp_true : cmp_false;
            break;

        case CmpOp::NE:
            for (size_t i = 0; i < num_pixels; i++)
                des[i] = (src[i] != cmp_val) ? cmp_true : cmp_false;
            break;

        case CmpOp::LT:
            for (size_t i = 0; i < num_pixels; i++)
                des[i] = (src[i] < cmp_val) ? cmp_true : cmp_false;
            break;

        case CmpOp::LE:
            for (size_t i = 0; i < num_pixels; i++)
                des[i] = (src[i] <= cmp_val) ? cmp_true : cmp_false;
            break;

        case CmpOp::GT:
            for (size_t i = 0; i < num_pixels; i++)
                des[i] = (src[i] > cmp_val) ? cmp_true : cmp_false;
            break;

        case CmpOp::GE:
            for (size_t i = 0; i < num_pixels; i++)
                des[i] = (src[i] >= cmp_val) ? cmp_true : cmp_false;
            break;

        default:
            cout << "Invalid CmpOp: " << cmp_op << '\n';
            rc = false;
    }

    return rc;
}

int main()
{
    const size_t align = 64;
    const size_t num_pixels = 4 * 1024 * 1024;
    AlignedArray<uint8_t> src_aa(num_pixels, align);
    AlignedArray<uint8_t> des1_aa(num_pixels, align);
    AlignedArray<uint8_t> des2_aa(num_pixels, align);
    uint8_t* src = src_aa.Data();
    uint8_t* des1 = des1_aa.Data();
    uint8_t* des2 = des2_aa.Data();

    const uint8_t cmp_vals[] {197, 222, 43, 43, 129, 222};
    const CmpOp cmp_ops[] {CmpOp::EQ, CmpOp::NE, CmpOp::LT, CmpOp::LE, CmpOp::GT, CmpOp::GE};
    const size_t num_cmp_vals = sizeof(cmp_vals) / sizeof(uint8_t);
    const size_t num_cmp_ops = sizeof(cmp_ops) / sizeof(CmpOp);

    assert(num_cmp_vals == num_cmp_ops);

    Init(src, num_pixels, 511);

    cout << "Results for Ch14_03\n";

    for (size_t i = 0; i < num_cmp_ops; i++)
    {
        Avx512ComparePixelsCpp(des1, src, num_pixels, cmp_ops[i], cmp_vals[i]);
        Avx512ComparePixels_(des2, src, num_pixels, cmp_ops[i], cmp_vals[i]);
        ShowResults(des1, des2, num_pixels, cmp_ops[i], cmp_vals[i], i + 1);
    }

    return 0;
}
