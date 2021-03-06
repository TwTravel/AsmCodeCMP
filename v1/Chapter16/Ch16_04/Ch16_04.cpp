//------------------------------------------------
//               Ch16_04.cpp
//------------------------------------------------

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <vector>
#include "Ch16_04.h"
#include "AlignedMem.h"
#include "BmThreadTimer.h"

using namespace std;

// Control flag for streaming thread status information to cout.
const bool c_ThreadMsgEnable = false;

void CalcResultCpp(CalcInfo* ci)
{
    size_t al = c_Align;
    size_t i0 = ci->m_Index0;
    size_t i1 = ci->m_Index1;
    size_t num_elements = i1 - i0 + 1;

    ci->m_Status = 0;

    if (num_elements == 0 || (num_elements % c_BlockSize) != 0)
        return;

    for (size_t i = i0; i <= i1; i++)
    {
        double xx = ci->m_X1[i] - ci->m_X2[i];
        double yy = ci->m_Y1[i] - ci->m_Y2[i];
        double zz = ci->m_Z1[i] - ci->m_Z2[i];

        ci->m_Result[i] = sqrt(1.0 / sqrt(xx * xx + yy * yy + zz * zz));
    }

    ci->m_Status = 1;
}

static void CalcResultThread(CalcInfo* ci, CoutInfo* cout_info)
{
    if (cout_info->m_ThreadMsgEnable)
        DisplayThreadMsg(ci, cout_info, "ENTER CalcResultThread()");

    CalcResult_(ci);

    if (cout_info->m_ThreadMsgEnable)
        DisplayThreadMsg(ci, cout_info, "EXIT CalcResultThread()");
}

void RunMultipleThreads(bool thread_msg_enable)
{
    // Code section #1

    size_t align = c_Align;
    size_t num_elements_max;
    vector<size_t> num_elements_vec = GetNumElementsVec(&num_elements_max);
    vector<size_t> num_threads_vec = GetNumThreadsVec();

    AlignedArray<double> x1_aa(num_elements_max, align);
    AlignedArray<double> x2_aa(num_elements_max, align);
    AlignedArray<double> y1_aa(num_elements_max, align);
    AlignedArray<double> y2_aa(num_elements_max, align);
    AlignedArray<double> z1_aa(num_elements_max, align);
    AlignedArray<double> z2_aa(num_elements_max, align);
    AlignedArray<double> result1_aa(num_elements_max, align);
    AlignedArray<double> result2_aa(num_elements_max, align);

    double* x1 = x1_aa.Data();
    double* x2 = x2_aa.Data();
    double* y1 = y1_aa.Data();
    double* y2 = y2_aa.Data();
    double* z1 = z1_aa.Data();
    double* z2 = z2_aa.Data();
    double* result1 = result1_aa.Data();
    double* result2 = result2_aa.Data();

    cout << "Begin initialization of test arrays\n";
    cout << "  Initializing test arrays x1, x2\n";
    Init(x1, x2, num_elements_max, 307);
    cout << "  Initializing test arrays y1, y2\n";
    Init(y1, y2, num_elements_max, 401);
    cout << "  Initializing test arrays z1, z2\n";
    Init(z1, z2, num_elements_max, 503);
    cout << "Finished initialization of test arrays\n";

    CalcInfo ci1;
    ci1.m_X1 = x1;  ci1.m_X2 = x2;
    ci1.m_Y1 = y1;  ci1.m_Y2 = y2;
    ci1.m_Z1 = z1;  ci1.m_Z2 = z2;
    ci1.m_Result = result1;
    ci1.m_Index0 = 0;
    ci1.m_Index1 = num_elements_max - 1;
    ci1.m_Status = -1;
 
    // CalcResultCpp used for verification purposes
    cout << "Begin execution of CalcResultCpp\n";
    CalcResultCpp(&ci1);
    cout << "Finished execution of CalcResultCpp\n";

    size_t iteration = 0;
    const size_t block_size = c_BlockSize;
    BmThreadTimer bmtt(num_elements_vec.size(), num_threads_vec.size());

    // Code section #2

    cout << "Begin execution of calculating threads\n";

    for (size_t i = 0; i < num_elements_vec.size(); i++)
    {
        size_t num_elements = num_elements_vec[i] * 1024 * 1024;
        size_t num_blocks = num_elements / block_size;
        size_t num_blocks_rem = num_elements % block_size;

        if (num_blocks_rem != 0)
            throw runtime_error("num_elements must be an integer multiple of block_size");

        for (size_t j = 0; j < num_threads_vec.size(); j++)
        {
            size_t num_threads = num_threads_vec[j];

            bmtt.Start(i, j);

            size_t num_blocks_per_thread = num_blocks / num_threads;
            size_t num_blocks_per_thread_rem = num_blocks % num_threads;

            vector<CalcInfo> ci2(num_threads);
            vector<CoutInfo> cout_info(num_threads);
            vector<thread*> threads(num_threads);

            // Thread start code
            for (size_t k = 0; k < num_threads; k++)
            {
                ci2[k].m_X1 = x1;   ci2[k].m_X2 = x2;
                ci2[k].m_Y1 = y1;   ci2[k].m_Y2 = y2;
                ci2[k].m_Z1 = z1;   ci2[k].m_Z2 = z2;

                ci2[k].m_Result = result2;
                ci2[k].m_Index0 = k * num_blocks_per_thread * block_size;
                ci2[k].m_Index1 = (k + 1) * num_blocks_per_thread * block_size - 1;
                ci2[k].m_Status = -1;

                if ((k + 1) == num_threads)
                    ci2[k].m_Index1 += num_blocks_per_thread_rem * block_size;

                cout_info[k].m_ThreadMsgEnable = thread_msg_enable;
                cout_info[k].m_Iteration = iteration;
                cout_info[k].m_NumElements = num_elements;
                cout_info[k].m_NumThreads = num_threads;
                cout_info[k].m_ThreadId = k;

                threads[k] = new thread(CalcResultThread, &ci2[k], &cout_info[k]);
            }

            // Wait for all threads to complete
            for (size_t k = 0; k < num_threads; k++)
                threads[k]->join();

            bmtt.Stop(i, j);

            size_t cmp_index = CompareResults(result1, result2, num_elements);

            if (cmp_index != num_elements)
            {
                ostringstream oss;
                oss << "  compare error detected at index " << cmp_index;
                throw runtime_error(oss.str());
            }

            for (size_t k = 0; k < num_threads; k++)
            {
                if (ci2[k].m_Status != 1)
                {
                    ostringstream oss;
                    oss << "  invalid status code " << ci2[k].m_Status;
                    throw runtime_error(oss.str());
                }

                delete threads[k];
            }
        }

        iteration++;
    }

    cout << "Finished execution of calculating threads\n";

    string fn = bmtt.BuildCsvFilenameString("Ch16_04_MultipleThreads_BM");
    bmtt.SaveElapsedTimes(fn, BmThreadTimer::EtUnit::MilliSec, 0);
    cout << "Benchmark times save to file " << fn << '\n';
}

int main()
{
    try
    {
        RunMultipleThreads(c_ThreadMsgEnable);
    }

    catch (runtime_error& rte)
    {
        cout << "'runtime_error' exception has occurred - " << rte.what() << '\n';
    }

    catch (...)
    {
        cout << "Unexpected exception has occurred\n";
    }

    return 0;
}