//------------------------------------------------
//               Ch07_02.cpp
//------------------------------------------------

#include "stdafx.h"
#include <iostream>
#include "XmmVal.h"

using namespace std;

// The order of the name constants in the following enum must
// correspond to the table values defined in .asm file.

enum ShiftOp : unsigned int
{
    U16_LL,     // shift left logical - word
    U16_RL,     // shift right logical - word
    U16_RA,     // shift right arithmetic - word
    U32_LL,     // shift left logical - doubleword
    U32_RL,     // shift right logical - doubleword
    U32_RA,     // shift right arithmetic - doubleword
};

extern "C" bool AvxPackedIntegerShift_(XmmVal& b, const XmmVal& a, ShiftOp shift_op, unsigned int count);

void AvxPackedIntegerShiftU16(void)
{
    unsigned int count = 2;
    alignas(16) XmmVal a;
    alignas(16) XmmVal b;

    a.m_U16[0] = 0x1234;
    a.m_U16[1] = 0xFF00;
    a.m_U16[2] = 0x00CC;
    a.m_U16[3] = 0x8080;
    a.m_U16[4] = 0x00FF;
    a.m_U16[5] = 0xAAAA;
    a.m_U16[6] = 0x0F0F;
    a.m_U16[7] = 0x0101;

    AvxPackedIntegerShift_(b, a, U16_LL, count);
    cout << "\nResults for ShiftOp::U16_LL (count = " << count << ")\n";
    cout << "a: " << a.ToStringX16() << '\n';
    cout << "b: " << b.ToStringX16() << '\n';

    AvxPackedIntegerShift_(b, a, U16_RL, count);
    cout << "\nResults for ShiftOp::U16_RL (count = " << count << ")\n";
    cout << "a: " << a.ToStringX16() << '\n';
    cout << "b: " << b.ToStringX16() << '\n';

    AvxPackedIntegerShift_(b, a, U16_RA, count);
    cout << "\nResults for ShiftOp::U16_RA (count = " << count << ")\n";
    cout << "a: " << a.ToStringX16() << '\n';
    cout << "b: " << b.ToStringX16() << '\n';
}

void AvxPackedIntegerShiftU32(void)
{
    unsigned int count = 4;
    alignas(16) XmmVal a;
    alignas(16) XmmVal b;

    a.m_U32[0] = 0x12345678;
    a.m_U32[1] = 0xFF00FF00;
    a.m_U32[2] = 0x03030303;
    a.m_U32[3] = 0x80800F0F;

    AvxPackedIntegerShift_(b, a, U32_LL, count);
    cout << "\nResults for ShiftOp::U32_LL (count = " << count << ")\n";
    cout << "a: " << a.ToStringX32() << '\n';
    cout << "b: " << b.ToStringX32() << '\n';

    AvxPackedIntegerShift_(b, a, U32_RL, count);
    cout << "\nResults for ShiftOp::U32_RL (count = " << count << ")\n";
    cout << "a: " << a.ToStringX32() << '\n';
    cout << "b: " << b.ToStringX32() << '\n';

    AvxPackedIntegerShift_(b, a, U32_RA, count);
    cout << "\nResults for ShiftOp::U32_RA (count = " << count << ")\n";
    cout << "a: " << a.ToStringX32() << '\n';
    cout << "b: " << b.ToStringX32() << '\n';
}

int main(void)
{
    string sep(75, '-');

    AvxPackedIntegerShiftU16();
    cout << '\n' << sep << '\n';
    AvxPackedIntegerShiftU32();
    return 0;
}
