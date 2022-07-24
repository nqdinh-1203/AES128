#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

unsigned char sBox[256] =
{
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

uint32_t C[11] =
{
    0x00000000, 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000,
    0x80000000, 0x1b000000, 0x36000000
};

void Devideto4(uint32_t* a, unsigned char *b)
{
    uint32_t top = 0;
    for (uint32_t i = 0; i < 16; i += 4)
    {
        a[top] = b[i];
        for(uint32_t j = i+1; j < i+4; j++)
            a[top] = (a[top]<<8) + b[j];
        top++;
    }
}

uint32_t sbox(uint32_t state)
{
    unsigned char a[4];
    for (uint32_t i = 0; i < 4; ++i)
    {
        a[3 - i] = (state >> (i*8));
        a[3 - i] = sBox[a[3 - i]];
        //cout<<(uint32_t)a[3-i]<<'\n';
    }
    uint32_t newstate = a[0];
    for (uint32_t i = 1; i < 4; i++)
        newstate = (newstate<<8) + a[i];
    return newstate;
}

void printHex(uint32_t * s)
{
    for (int i = 0; i < 4; ++i)
    {
        unsigned char a[4];
        for (int j = 0; j < 4; ++j)
            a[3 - j] = (s[i] >> (j*8));
        for (int j = 0; j < 4; ++j)
            cout << hex << (int)a[j] <<' ';
    }
}

void Expand(uint32_t * Ke, uint32_t **eKe)
{
    for (uint32_t i = 0; i < 4; i++)
        eKe[0][i] = Ke[i];
    for (uint32_t i = 1; i < 11; i++)
        for(uint32_t j = 0; j < 4; j++)
        {
            uint32_t tmp;
            if(j == 0)
            {
                uint32_t sboxstate = (eKe[i-1][3] << 8) | (eKe[i-1][3] >> (32 - 8));
                //cout<<eKe[i-1][3]<<' '<<sboxstate<<'\n';
                tmp = sbox(sboxstate) ^ C[i];
            }
            else tmp = eKe[i][j-1];
            eKe[i][j] = eKe[i-1][j] ^ tmp;
        }
    cout<<"------Expand function output------\n";
    for (uint32_t i = 0; i < 11; i++)
        {
            printHex(eKe[i]);
            cout<<"\n\n";
        }
}

uint32_t** splitupS(uint32_t * s)
{
    uint32_t **newMS = new uint32_t*[4];
    for (int i = 0; i < 4; ++i)
        newMS[i] = new uint32_t[4];
    for (int j = 0; j < 4; ++j)
    {
        unsigned char a[4];
        for (int i = 0; i < 4; ++i)
            a[3 - i] = (s[j] >> (i*8));
        for (int i = 0; i < 4; ++i)
            newMS[i][j] = (uint32_t)a[i];
    }
    return newMS;
}

void gather4s(uint32_t * s, uint32_t ** b)
{
    for (int j = 0; j < 4; ++j)
    {
        s[j] = b[0][j];
        for (int i = 1; i < 4; ++i)
            s[j] = (s[j]<<8) + (unsigned char)b[i][j];
    }
}

void shiftrows(uint32_t ** s)
{
    uint32_t tmp;
    //Row 2
    tmp = s[1][0];
    s[1][0] = s[1][1];
    s[1][1] = s[1][2];
    s[1][2] = s[1][3];
    s[1][3] = tmp;

    //Row 3
    tmp = s[2][0];
    s[2][0] = s[2][2];
    s[2][2] = tmp;
    tmp = s[2][1];
    s[2][1] = s[2][3];
    s[2][3] = tmp;

    //Row 4
    tmp = s[3][0];
    s[3][0] = s[3][3];
    s[3][3] = s[3][2];
    s[3][2] = s[3][1];
    s[3][1] = tmp;
}

void mixSinCol(uint32_t * col)
{
    uint32_t a[4];
    uint32_t b[4];
    uint32_t h;
    for (int i = 0; i < 4; i++)
    {
        a[i] = col[i];
        h = (uint32_t)((signed char)col[i] >> 7);
        b[i] = col[i] << 1;
        b[i] ^= 0x1B & h;
    }
    col[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1]; /* 2 * a0 + a3 + a2 + 3 * a1 */
    col[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2]; /* 2 * a1 + a0 + a3 + 3 * a2 */
    col[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3]; /* 2 * a2 + a1 + a0 + 3 * a3 */
    col[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0]; /* 2 * a3 + a2 + a1 + 3 * a0 */
}

void mixcols(uint32_t ** s)
{
    uint32_t *tmp = new uint32_t[4];

    for (int j = 0; j < 4; ++j)
    {
        for (int i = 0; i < 4; ++i)
            tmp[i] = s[i][j];
        mixSinCol(tmp);
        for (int i = 0; i < 4; ++i)
            s[i][j] = tmp[i];
    }
    delete tmp;
}

void AES128(uint32_t * K, uint32_t * M)
{
    cout<<"------Key------\n";
    printHex(K);
    cout<<"\n\n------Message------\n";
    printHex(M);
    cout<<"\n\n";
    uint32_t **expandedKey = new uint32_t*[11];
    for (uint32_t i = 0; i < 11; i++)
        expandedKey[i] = new uint32_t[5];
    Expand(K, expandedKey);
    uint32_t s[4];
    for (int i = 0; i < 4; ++i)
        s[i] = M[i] ^ expandedKey[0][i];
    cout<<"------AES128 Cipher text C[0-9]------\n";
    printHex(s);
    cout<<"\n\n";
    uint32_t **matrix_s = new uint32_t*[4];
    for (int i = 0; i < 4; ++i)
        matrix_s[i] = new uint32_t[4];
    for (int i = 1; i < 11; ++i)
    {
        for(int j = 0; j < 4; j++)
            s[j] = sbox(s[j]);
        matrix_s = splitupS(s);
        shiftrows(matrix_s);
        if(i <= 9)
            mixcols(matrix_s);
        gather4s(s, matrix_s);
        for (int j = 0; j < 4; ++j)
            s[j] = s[j] ^ expandedKey[i][j];
        if(i == 10)
            cout<<"------AES128 Cipher text output C[10]------\n";
        printHex(s);
        cout<<"\n\n";
    }

}

int main()
{
    unsigned char *key = new unsigned char[16];

    char input[1024];
    char block[16];
    cin.getline(input, sizeof(input));
    cin.read(block,16);
    uint32_t Olen = strlen((const char *)input);
    uint32_t padlen = Olen;

    if ((padlen % 16) != 0)
    {
        padlen = (padlen / 16 + 1) * 16;
    }

    unsigned char * mess = new unsigned char[padlen];
    for (uint32_t i = 0; i < padlen; i++)
        if (i >= Olen)
            mess[i] = 0;
        else
            mess[i] = input[i];
    for (uint32_t i = 0; i < 16; i++)
        key[i] = (unsigned char) block[i];
    uint32_t key_uint32_t[4], mess_uint32_t[4];
    Devideto4(key_uint32_t, key);
    Devideto4(mess_uint32_t, mess);
    AES128(key_uint32_t, mess_uint32_t);

    delete[] mess;
}
//Test
//Two One Nine Two
//Thats my Kung Fu


