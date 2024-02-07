// FILE NAME  : crypt_aes.c
// AUTHOR     : Rafael Garibotti
// DEVELOPERS : Rafael Garibotti
// E-mail     : rafael.garibotti@pucrs.br
//-----------------------------------------------------------------------------
// RELEASE HISTORY
// VERSION   DATE         DESCRIPTION
// 1.0       2020-05-12   Initial version.
//-----------------------------------------------------------------------------

//~ #include "common.h"
#include "AES.h"

//-----------------------------------------------------------------------------
// AES (128/192/256)
//-----------------------------------------------------------------------------
// The number of columns comprising a state in AES. This is a constant in AES. Value=4
#define Nb 4

static uint8_t Nk;      // The number of 32 bit words in a key
static uint8_t KEYLEN;  // Key length in bytes
static uint8_t Nr;      // The number of rounds in AES Cipher

// state - array holding the intermediate results during decryption.
static uint8_t state[4][4];

// The array that stores the round keys.
static uint8_t RoundKey[240];

// The Key input to the AES Program
static const uint32_t* Key;

// The lookup-tables are marked const so they can be placed in read-only storage instead of RAM
// The numbers below can be computed dynamically trading ROM for RAM - 
// This can be useful in (embedded) bootloader applications, where ROM is often limited.
static const uint8_t sbox[256] = {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
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
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

static const uint8_t rsbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

// The round constant word array, Rcon[i], contains the values given by 
// x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8)
static const uint8_t Rcon[256] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
    0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
    0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
    0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
    0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
    0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
    0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
    0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
    0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
    0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
    0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
    0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
    0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
    0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
    0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
    0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d };

static uint8_t
getSBoxValue(uint8_t num) {
    return sbox[num];
}

static uint8_t
getSBoxInvert(uint8_t num) {
    return rsbox[num];
}

// This function produces Nb(Nr+1) round keys. The round keys are used in each round to decrypt the states. 
static void
KeyExpansion(void) {
    uint32_t i, k;
    uint8_t tempa[4]; // Used for the column/row operations

    // The first round key is the key itself.
    for(i = 0; i < Nk; ++i) {
        RoundKey[(i * 4) + 0] = ((Key[i] >> 24) & 0xFF);
        RoundKey[(i * 4) + 1] = ((Key[i] >> 16) & 0xFF);
        RoundKey[(i * 4) + 2] = ((Key[i] >>  8) & 0xFF);
        RoundKey[(i * 4) + 3] =  (Key[i]        & 0xFF);
    }

    // All other round keys are found from the previous round keys.
    //i == Nk
    for(; i < Nb * (Nr + 1); ++i) {
        tempa[0] = RoundKey[(i-1) * 4 + 0];
        tempa[1] = RoundKey[(i-1) * 4 + 1];
        tempa[2] = RoundKey[(i-1) * 4 + 2];
        tempa[3] = RoundKey[(i-1) * 4 + 3];

        if (i % Nk == 0) {
        // This function shifts the 4 bytes in a word to the left once.
        // [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

            // Function RotWord()
            k = tempa[0];
            tempa[0] = tempa[1];
            tempa[1] = tempa[2];
            tempa[2] = tempa[3];
            tempa[3] = k;

            // SubWord() is a function that takes a four-byte input word and 
            // applies the S-box to each of the four bytes to produce an output word.

            // Function Subword()
            tempa[0] = getSBoxValue(tempa[0]);
            tempa[1] = getSBoxValue(tempa[1]);
            tempa[2] = getSBoxValue(tempa[2]);
            tempa[3] = getSBoxValue(tempa[3]);

            tempa[0] = tempa[0] ^ Rcon[i/Nk];
        }

        if (Nk == 8) {
            if (i % Nk == 4) {
                // Function Subword()
                tempa[0] = getSBoxValue(tempa[0]);
                tempa[1] = getSBoxValue(tempa[1]);
                tempa[2] = getSBoxValue(tempa[2]);
                tempa[3] = getSBoxValue(tempa[3]);
            }
        }
        RoundKey[i * 4 + 0] = RoundKey[(i - Nk) * 4 + 0] ^ tempa[0];
        RoundKey[i * 4 + 1] = RoundKey[(i - Nk) * 4 + 1] ^ tempa[1];
        RoundKey[i * 4 + 2] = RoundKey[(i - Nk) * 4 + 2] ^ tempa[2];
        RoundKey[i * 4 + 3] = RoundKey[(i - Nk) * 4 + 3] ^ tempa[3];
    }
}

// This function adds the round key to state.
// The round key is added to the state by an XOR function.
static void
AddRoundKey(uint8_t round) {
    uint8_t i, j;
    for(i = 0; i < 4; ++i) {
        for(j = 0; j < 4; ++j) {
            state[i][j] ^= RoundKey[round * Nb * 4 + i * Nb + j];
        }
    }
}

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
static void
SubBytes(void) {
    uint8_t i, j;
    for(i = 0; i < 4; ++i) {
        for(j = 0; j < 4; ++j) {
            state[j][i] = getSBoxValue(state[j][i]);
        }
    }
}

// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
static void
ShiftRows(void) {
    uint8_t temp;

    // Rotate first row 1 columns to left  
    temp        = state[0][1];
    state[0][1] = state[1][1];
    state[1][1] = state[2][1];
    state[2][1] = state[3][1];
    state[3][1] = temp;

    // Rotate second row 2 columns to left  
    temp        = state[0][2];
    state[0][2] = state[2][2];
    state[2][2] = temp;

    temp        = state[1][2];
    state[1][2] = state[3][2];
    state[3][2] = temp;

    // Rotate third row 3 columns to left
    temp        = state[0][3];
    state[0][3] = state[3][3];
    state[3][3] = state[2][3];
    state[2][3] = state[1][3];
    state[1][3] = temp;
}

static uint8_t
xtime(uint8_t x) {
    return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
}

// MixColumns function mixes the columns of the state matrix
static void
MixColumns(void) {
    uint8_t i;
    uint8_t Tmp, Tm, t;
    for(i = 0; i < 4; ++i) {  
        t   = state[i][0];
        Tmp = state[i][0] ^ state[i][1] ^ state[i][2] ^ state[i][3] ;
        Tm  = state[i][0] ^ state[i][1] ; Tm = xtime(Tm);  state[i][0] ^= Tm ^ Tmp ;
        Tm  = state[i][1] ^ state[i][2] ; Tm = xtime(Tm);  state[i][1] ^= Tm ^ Tmp ;
        Tm  = state[i][2] ^ state[i][3] ; Tm = xtime(Tm);  state[i][2] ^= Tm ^ Tmp ;
        Tm  = state[i][3] ^ t ;           Tm = xtime(Tm);  state[i][3] ^= Tm ^ Tmp ;
    }
}

// Multiply is used to multiply numbers in the field GF(2^8)
static uint8_t
Multiply(uint8_t x, uint8_t y) {
    return (((y & 1) * x) ^
         ((y>>1 & 1) * xtime(x)) ^
         ((y>>2 & 1) * xtime(xtime(x))) ^
         ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^
         ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))));
}

// MixColumns function mixes the columns of the state matrix.
// The method used to multiply may be difficult to understand for the inexperienced.
// Please use the references to gain more information.
static void
InvMixColumns(void) {
    int i;
    uint8_t a, b, c, d;
    for(i = 0; i < 4; ++i) { 
        a = state[i][0];
        b = state[i][1];
        c = state[i][2];
        d = state[i][3];

        state[i][0] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
        state[i][1] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
        state[i][2] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
        state[i][3] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
    }
}

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
static void
InvSubBytes(void) {
    uint8_t i, j;
    for(i = 0; i < 4; ++i) {
        for(j = 0; j < 4; ++j) {
            state[j][i] = getSBoxInvert(state[j][i]);
        }
    }
}

static void
InvShiftRows(void) {
    uint8_t temp;

    // Rotate first row 1 columns to right  
    temp = state[3][1];
    state[3][1] = state[2][1];
    state[2][1] = state[1][1];
    state[1][1] = state[0][1];
    state[0][1] = temp;

    // Rotate second row 2 columns to right 
    temp = state[0][2];
    state[0][2] = state[2][2];
    state[2][2] = temp;

    temp = state[1][2];
    state[1][2] = state[3][2];
    state[3][2] = temp;

    // Rotate third row 3 columns to right
    temp = state[0][3];
    state[0][3] = state[1][3];
    state[1][3] = state[2][3];
    state[2][3] = state[3][3];
    state[3][3] = temp;
}

// Cipher is the main function that encrypts the PlainText.
static void
Cipher(void) {
    uint8_t round = 0;

    // Add the First round key to the state before starting the rounds.
    AddRoundKey(0); 

    // There will be Nr rounds.
    // The first Nr-1 rounds are identical.
    // These Nr-1 rounds are executed in the loop below.
    for(round = 1; round < Nr; ++round) {
        SubBytes();
        ShiftRows();
        MixColumns();
        AddRoundKey(round);
    }

    // The last round is given below.
    // The MixColumns function is not here in the last round.
    SubBytes();
    ShiftRows();
    AddRoundKey(Nr);
}

static void
InvCipher(void) {
    uint8_t round = 0;

    // Add the First round key to the state before starting the rounds.
    AddRoundKey(Nr); 

    // There will be Nr rounds.
    // The first Nr-1 rounds are identical.
    // These Nr-1 rounds are executed in the loop below.
    for(round = Nr-1; round > 0; round--) {
        InvShiftRows();
        InvSubBytes();
        AddRoundKey(round);
        InvMixColumns();
    }

    // The last round is given below.
    // The MixColumns function is not here in the last round.
    InvShiftRows();
    InvSubBytes();
    AddRoundKey(0);
}

void
aes_ecb(uint32_t* key, uint32_t* input, uint32_t* output, uint8_t* crypt_config) {
    uint8_t i;

    // Copy input to state
    for(i = 0; i < 4; i++) {
        state[i][0] = ((input[i] >> 24) & 0xFF);
        state[i][1] = ((input[i] >> 16) & 0xFF);
        state[i][2] = ((input[i] >>  8) & 0xFF);
        state[i][3] =  (input[i]        & 0xFF);
    }

    // Update some private variables
    switch (crypt_config[2]) {
        case 2  : Nk = 8; KEYLEN = 32; Nr = 14; break; // 256 bits
        case 1  : Nk = 6; KEYLEN = 24; Nr = 12; break; // 192 bits
        default : Nk = 4; KEYLEN = 16; Nr = 10; break; // 128 bits
    }

    // The KeyExpansion routine must be called before encryption
    Key = key;
    KeyExpansion();

    // The next function call encrypts the PlainText with the Key using AES algorithm
    if (crypt_config[0])
        Cipher();
    else
        InvCipher();

    // Copy state to output
    for(i = 0; i < 4; i++) {
        output[i] = (state[i][0] << 24) + (state[i][1] << 16) + (state[i][2] << 8) + state[i][3];
    }
}

//-----------------------------------------------------------------------------
// Main Functions
//-----------------------------------------------------------------------------
static double
crypt_aes(int id, int hw_sw, uint32_t* text) {
	double output=0;
    //~ static const char _start[] = "Start crypt-aes ";
    //~ static const char _end[] = "End crypt-aes";
    static const uint32_t key[8] = {0xDEADBEEF, 0x01234567, 0x89ABCDEF, 0xDEADBEEF, 0xDEADBEEF, 0x01234567, 0x89ABCDEF, 0xDEADBEEF};
    int i;
    uint32_t plan[4], cipher[4];
    uint8_t crypt_config[5]; //0: enc_dec, 1: plan_size, 2: key_size, 3: msg_size(numWords), 4: algorithm

    //~ MappPrintf(id, PRINT_STR, (void *)_start);

    for (i = 0; i < INPUT_SIZE/4; i++){
        //~ error = 0;

        //~ // Testcase name
        //~ switch (i) {
            //~ case 0 : MappPrintf(id, PRINT_STR, (void *)"-- AES-128 (ENC) -- "); break; //AES-128 - ENC
            //~ case 1 : MappPrintf(id, PRINT_STR, (void *)"-- AES-128 (DEC) -- "); break; //AES-128 - DEC
            //~ case 2 : MappPrintf(id, PRINT_STR, (void *)"-- AES-192 (ENC) -- "); break; //AES-192 - ENC
            //~ case 3 : MappPrintf(id, PRINT_STR, (void *)"-- AES-192 (DEC) -- "); break; //AES-192 - DEC
            //~ case 4 : MappPrintf(id, PRINT_STR, (void *)"-- AES-256 (ENC) -- "); break; //AES-256 - ENC
            //~ case 5 : MappPrintf(id, PRINT_STR, (void *)"-- AES-256 (DEC) -- "); break; //AES-256 - DEC
        //~ }

        // Write CRYPT_KEY
        //~ key[0] = 0xDEADBEEF;
        //~ key[1] = 0x01234567;
        //~ key[2] = 0x89ABCDEF;
        //~ key[3] = 0xDEADBEEF;
        //~ key[4] = 0xDEADBEEF;
        //~ key[5] = 0x01234567;
        //~ key[6] = 0x89ABCDEF;
        //~ key[7] = 0xDEADBEEF;
    
        // Write CRYPT_PLAN
        switch (i%2) {
            case 0 : plan[0] = text[4*i+0];  //AES-128 - ENC
					 plan[1] = text[4*i+1];  //AES-128 - ENC
					 plan[2] = text[4*i+2];  //AES-128 - ENC
					 plan[3] = text[4*i+3]; break; //AES-128 - ENC
					 
            case 1 : plan[0] = text[4*i+0];  //AES-128 - DEC
					 plan[1] = text[4*i+1];  //AES-128 - DEC
					 plan[2] = text[4*i+2];  //AES-128 - DEC
					 plan[3] = text[4*i+3]; break; //AES-128 - DEC
        }

        // Write CRYPT_CONFIG
        crypt_config[0] = (i+1) % 2; //0: dec, 1: enc (Enc_Dec)
        crypt_config[1] = 1; //1: 128 bits (Plan Size)
        crypt_config[2] = 0; //0: 128 bits, 1: 192 bits, 2: 256 bits (Key Size)
        crypt_config[3] = 4; //4: Number of words in the message
        crypt_config[4] = 0; //0: AES (algorithm)

        // Call MappCryptography
        //~ MappPrintf(id, PRINT_STR, (void *)"Call MappCryptography ");
        //~ timer = SnifferReadIndex();
        //~ if (hw_sw) {
            //~ MappCryptography(key, plan, cipher, crypt_config); // Hardware Accelerator
        //~ }
        //~ else {
            aes_ecb(key, plan, cipher, crypt_config); // C Application
        //~ }
        //~ timer = SnifferReadIndex();

        // Read CRYPT_CIPHER
        //~ MappPrintf(id, PRINT_HEX, (void *)cipher[0]);
        //~ switch (i) {
            //~ case 0 : if (cipher[0] != 0x237549D4) error = 1; break; //AES-128 - ENC
            //~ case 1 : if (cipher[0] != 0xA5A5A5A5) error = 1; break; //AES-128 - DEC
            //~ case 2 : if (cipher[0] != 0xEBB4CD9E) error = 1; break; //AES-192 - ENC
            //~ case 3 : if (cipher[0] != 0xA5A5A5A5) error = 1; break; //AES-192 - DEC
            //~ case 4 : if (cipher[0] != 0x206BF119) error = 1; break; //AES-256 - ENC
            //~ case 5 : if (cipher[0] != 0xA5A5A5A5) error = 1; break; //AES-256 - DEC
        //~ }
        //~ MappPrintf(id, PRINT_HEX, (void *)cipher[1]);
        //~ switch (i) {
            //~ case 0 : if (cipher[1] != 0xCDCEA7BE) error = 1; break; //AES-128 - ENC
            //~ case 1 : if (cipher[1] != 0x01234567) error = 1; break; //AES-128 - DEC
            //~ case 2 : if (cipher[1] != 0xDA7CBDE0) error = 1; break; //AES-192 - ENC
            //~ case 3 : if (cipher[1] != 0x01234567) error = 1; break; //AES-192 - DEC
            //~ case 4 : if (cipher[1] != 0x7A085AC8) error = 1; break; //AES-256 - ENC
            //~ case 5 : if (cipher[1] != 0x01234567) error = 1; break; //AES-256 - DEC
        //~ }
        //~ MappPrintf(id, PRINT_HEX, (void *)cipher[2]);
        //~ switch (i) {
            //~ case 0 : if (cipher[2] != 0x0FE7D162) error = 1; break; //AES-128 - ENC
            //~ case 1 : if (cipher[2] != 0xFEDCBA98) error = 1; break; //AES-128 - DEC
            //~ case 2 : if (cipher[2] != 0x9E520F4C) error = 1; break; //AES-192 - ENC
            //~ case 3 : if (cipher[2] != 0xFEDCBA98) error = 1; break; //AES-192 - DEC
            //~ case 4 : if (cipher[2] != 0xB31B2AD0) error = 1; break; //AES-256 - ENC
            //~ case 5 : if (cipher[2] != 0xFEDCBA98) error = 1; break; //AES-256 - DEC
        //~ }
        //~ MappPrintf(id, PRINT_HEX, (void *)cipher[3]);
        //~ switch (i) {
            //~ case 0 : if (cipher[3] != 0xCC9161D3) error = 1; break; //AES-128 - ENC
            //~ case 1 : if (cipher[3] != 0x5A5A5A5A) error = 1; break; //AES-128 - DEC
            //~ case 2 : if (cipher[3] != 0x54BE5A98) error = 1; break; //AES-192 - ENC
            //~ case 3 : if (cipher[3] != 0x5A5A5A5A) error = 1; break; //AES-192 - DEC
            //~ case 4 : if (cipher[3] != 0x4843FFC1) error = 1; break; //AES-256 - ENC
            //~ case 5 : if (cipher[3] != 0x5A5A5A5A) error = 1; break; //AES-256 - DEC
        //~ }

        //~ if (error == 1) {
            //~ MappPrintf(id, PRINT_STR, (void *)"CRYPT: ERROR!!! "); 
        //~ }
        //~ else {
            //~ switch (i) {
                //~ case 0 : MappPrintf(id, PRINT_STR, (void *)"AES-128 (ENC): OK "); break; //AES-128 - ENC
                //~ case 1 : MappPrintf(id, PRINT_STR, (void *)"AES-128 (DEC): OK "); break; //AES-128 - DEC
                //~ case 2 : MappPrintf(id, PRINT_STR, (void *)"AES-192 (ENC): OK "); break; //AES-192 - ENC
                //~ case 3 : MappPrintf(id, PRINT_STR, (void *)"AES-192 (DEC): OK "); break; //AES-192 - DEC
                //~ case 4 : MappPrintf(id, PRINT_STR, (void *)"AES-256 (ENC): OK "); break; //AES-256 - ENC
                //~ case 5 : MappPrintf(id, PRINT_STR, (void *)"AES-256 (DEC): OK "); break; //AES-256 - DEC
            //~ }
        //~ }
		//~ printf("%08x %08x %08x %08x\n", cipher[0], cipher[1], cipher[2], cipher[3]);
        output += cipher[0];
		output += cipher[1];
		output += cipher[2];
		output += cipher[3];
		//~ printf("%.2f\n",*output);
		//~ HAL_Delay(100);
		//~ j+=4;
    }
    return output;
	//~ printf("%.2f\n",*output);
    //~ MappPrintf(id, PRINT_STR, (void *)_end);
}

double
app_main(uint32_t* text) {
    int id = 0;
    int hw_sw = 0; // HW: 1, SW: 0

    return crypt_aes(id, hw_sw, text);
}