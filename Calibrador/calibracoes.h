//---------------------------------------------------------------------------

#ifndef calibracoesH
#define calibracoesH

#include <math.h>
#include <windows.h>

ULONG64 CSM[20][26] = {
// 00 = Armor
{0}
//{{0x2B,0x55,0xAA,0xDA,0xAD,0x56,0x15},{0x2B,0x55,0xAA,0xF5,0x5A,0xAD,0x02},{0x2B,0x55,0xAA,0xEA,0xB5,0x5A,0x01},{0x2B,0x55,0xAA,0xD5,0x6A,0x5A},{0x2B,0x55,0xAB,0xAA,0xD5,0x0D},{0x2B,0x55,0xAB,0x55,0xAA,0x04},{0x2B,0x55,0xAE,0xAB,0x55,0x01},{0x2B,0x55,0xAD,0x56,0x55},{0x2B,0x55,0xBA,0xAC,0x0A},{0x2B,0x55,0xB5,0x5A,0x05},{0x2B,0x55,0xEA,0xB5,0x00},{0x2B,0x55,0xD5,0x35},{0x2B,0x55,0xAA,0x1A},{0x2B,0x57,0x55,0x05},{0x2B,0x56,0xAB,0x00},{0x2B,0x5D,0x2B},{0x2B,0x5A,0x15},{0x2B,0x75,0x02},{0x2B,0x6A,0x01},{0x2B,0x2A},{0x2B,0x15},{0x2B,0x02},{0x2E,0x01},{0x16},{0x07},{0x01}}
//{0x2B55AADAAD5615,0x2B55AAF55AAD02,0x2B55AAEAB55A01,0x2B55AAD56A5A,0x2B55ABAAD50D,0x2B55AB55AA04,0x2B55AEAB5501,0x2B55AD5655,0x2B55BAAD0A,0x2B55B55A05,0x2B55EAB500,0x2B55D535,0x2B55AA1A,0x2B575505,0x2B56AB00,0x2B5D2B,0x2B5A15,0x2B7502,0x2B6A01,0x2B2A,0x2B15,0x2B02,0x2E01,0x16,0x07,0x01}
//{0x2B55AADAAD56A8,0x2B55AAF55AAD80,0x2B55AAEAB55A80,0x2B55AAD56AB4,0x2B55ABAAD5D0,0x2B55AB55AA80,0x2B55AEAB5580,0x2B55AD56AA,0x2B55BAADA0,0x2B55B55AA0,0x2B55EAB500,0x2B55D5D4,0x2B55AAD0,0x2B5755A0,0x2B56AB00,0x2B5DAC,0x2B5AA8,0x2B7580,0x2B6A80,0x2BA8,0x2BA8,0x2B80,0x2E80,0xB0,0xE0,0x80}
};

double DSM[20][2] = {
// {0.5725, 0.8307}
 {0.5573, 0.8414}
};

double GSM[20] = {
 {71.85}
};

//---------------------------------------------------------------------------
#endif
