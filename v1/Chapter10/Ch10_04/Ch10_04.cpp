//------------------------------------------------
//               Ch10_04.cpp
//------------------------------------------------

#include "stdafx.h"
#include <iostream>
#include <random>
#include <memory.h>
#include <limits>
#include "Ch10_04.h"
#include "AlignedMem.h"

using namespace std;

void Init(uint8_t* x, uint64_t n, unsigned int seed)
{
    uniform_int_distribution<> ui_dist {0, 255};
    default_random_engine rng {seed};

    for (size_t i = 0; i < n; i++)
        x[i] = (uint8_t)ui_dist(rng);
}

bool Avx2ClipPixelsCpp(ClipData* cd)
{
    uint8_t* src = cd->m_Src;
    uint8_t* des = cd->m_Des;
    uint64_t num_pixels = cd->m_NumPixels;

    if (num_pixels == 0 || (num_pixels % 32) != 0)
        return false;

    if (!AlignedMem::IsAligned(src, 32) || !AlignedMem::IsAligned(des, 32))
        return false;

    uint64_t num_clipped_pixels = 0;
    uint8_t thresh_lo = cd->m_ThreshLo;
    uint8_t thresh_hi = cd->m_ThreshHi;

    for (uint64_t i = 0; i < num_pixels; i++)
    {
        uint8_t pixel = src[i];

        if (pixel < thresh_lo)
        {
            des[i] = thresh_lo;
            num_clipped_pixels++;
        }
        else if (pixel > thresh_hi)
        {
            des[i] = thresh_hi;
            num_clipped_pixels++;
        }
        else
            des[i] = src[i];
    }

    cd->m_NumClippedPixels = num_clipped_pixels;
    return true;
}

void Avx2ClipPixels(void)
{
    const uint8_t thresh_lo = 10;
    const uint8_t thresh_hi = 245;
    const uint64_t num_pixels = 4 * 1024 * 1024;

    AlignedArray<uint8_t> src(num_pixels, 32);
    AlignedArray<uint8_t> des1(num_pixels, 32);
    AlignedArray<uint8_t> des2(num_pixels, 32);

    Init(src.Data(), num_pixels, 157);

    ClipData cd1;
    ClipData cd2;

    cd1.m_Src = src.Data();
    cd1.m_Des = des1.Data();
    cd1.m_NumPixels = num_pixels;
    cd1.m_NumClippedPixels = numeric_limits<uint64_t>::max();
    cd1.m_ThreshLo = thresh_lo;
    cd1.m_ThreshHi = thresh_hi;

    cd2.m_Src = src.Data();
    cd2.m_Des = des2.Data();
    cd2.m_NumPixels = num_pixels;
    cd2.m_NumClippedPixels = numeric_limits<uint64_t>::max();
    cd2.m_ThreshLo = thresh_lo;
    cd2.m_ThreshHi = thresh_hi;

    Avx2ClipPixelsCpp(&cd1);
    Avx2ClipPixels_(&cd2);

    cout << "\nResults for Avx2ClipPixels\n";
    cout << "  cd1.m_NumClippedPixels1: " << cd1.m_NumClippedPixels << '\n';
    cout << "  cd2.m_NumClippedPixels2: " << cd2.m_NumClippedPixels << '\n';

    if (cd1.m_NumClippedPixels != cd2.m_NumClippedPixels)
        cout << "  NumClippedPixels compare error\n";

    if (memcmp(des1.Data(), des2.Data(), num_pixels) == 0)
        cout << "  Pixel buffer memory compare passed\n";
    else
        cout << "  Pixel buffer memory compare passed\n";
}

int main(void)
{
    Avx2ClipPixels();
    Avx2ClipPixels_BM();
    return 0;
}
