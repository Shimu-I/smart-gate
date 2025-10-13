/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Richard Moore
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 *  Special thanks to Nayuki (https://www.nayuki.io/) from which this library was
 *  heavily inspired and compared against.
 *
 *  See: https://github.com/nayuki/QR-Code-generator/tree/master/cpp
 */

#include "qrcode.h"

typedef struct BitBucket {
    rt_uint32_t bitOffsetOrWidth;
    rt_uint16_t capacityBytes;
    rt_uint8_t *data;
} BitBucket;


#if LOCK_VERSION == 0
static const rt_uint16_t NUM_ERROR_CORRECTION_CODEWORDS[4][40] = {
    // 1,  2,  3,  4,  5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,   25,   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,   36,   37,   38,   39,   40    Error correction level
    { 10, 16, 26, 36, 48,  64,  72,  88, 110, 130, 150, 176, 198, 216, 240, 280, 308, 338, 364, 416, 442, 476, 504, 560,  588,  644,  700,  728,  784,  812,  868,  924,  980, 1036, 1064, 1120, 1204, 1260, 1316, 1372},  // Medium
    {  7, 10, 15, 20, 26,  36,  40,  48,  60,  72,  80,  96, 104, 120, 132, 144, 168, 180, 196, 224, 224, 252, 270, 300,  312,  336,  360,  390,  420,  450,  480,  510,  540,  570,  570,  600,  630,  660,  720,  750},  // Low
    { 17, 28, 44, 64, 88, 112, 130, 156, 192, 224, 264, 308, 352, 384, 432, 480, 532, 588, 650, 700, 750, 816, 900, 960, 1050, 1110, 1200, 1260, 1350, 1440, 1530, 1620, 1710, 1800, 1890, 1980, 2100, 2220, 2310, 2430},  // High
    { 13, 22, 36, 52, 72,  96, 108, 132, 160, 192, 224, 260, 288, 320, 360, 408, 448, 504, 546, 600, 644, 690, 750, 810,  870,  952, 1020, 1050, 1140, 1200, 1290, 1350, 1440, 1530, 1590, 1680, 1770, 1860, 1950, 2040},  // Quartile
};
static const rt_uint8_t NUM_ERROR_CORRECTION_BLOCKS[4][40] = {
    // Version: (note that index 0 is for padding, and is set to an illegal value)
    // 1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
    {  1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},  // Medium
    {  1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},  // Low
    {  1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},  // High
    {  1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},  // Quartile
};
static const rt_uint16_t NUM_RAW_DATA_MODULES[40] = {
    //    1,     2,     3,     4,     5,     6,     7,     8,     9,   10,     11,    12,    13,    14,    15,    16,    17,    18,   19,     20,   
        208,   359,   567,   807,  1079,  1383,  1568,  1936,  2336,  2768,  3232,  3728,  4256,  4651,  5243,  5867,  6523,  7211,  7931,  8683, 
    //   21,    22,    23,    24,    25,   26,    27,     28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,    40
       9252, 10068, 10916, 11796, 12708, 13652, 14628, 15371, 16411, 17483, 18587, 19723, 20891, 22091, 23008, 24272, 25568, 26896, 28256, 29648
};

#elif LOCK_VERSION == 3
static const rt_int16_t NUM_ERROR_CORRECTION_CODEWORDS[4] = {
    26, 15, 44, 36
};
static const rt_int8_t NUM_ERROR_CORRECTION_BLOCKS[4] = {
    1, 1, 2, 2
};
static const rt_uint16_t NUM_RAW_DATA_MODULES = 567;

#else
// @TODO: Put other LOCK_VERSIONS here
# error Unsupported LOCK_VERSION

#endif


static int max(int a, int b) {
    if (a > b) return a;
    return b;
}

static int abs(int a) {
    if (a < 0) return -a;
    return a;
}

static rt_int8_t getAlphanumeric(char c) {
    if (c >= '0' && c <= '9') return (c - '0');
    if (c >= 'A' && c <= 'Z') return (c - 'A' + 10);

    if (' ' == c) return 36;
    else if ('$' == c) return 37;
    else if ('%' == c) return 38;
    else if ('*' == c) return 39;
    else if ('+' == c) return 40;
    else if ('-' == c) return 41;
    else if ('.' == c) return 42;
    else if ('/' == c) return 43;
    else if (':' == c) return 44;
    else
        return -1;
}

static rt_bool_t isAlphanumeric(const char *text, rt_uint16_t length) {
    while (length != 0)
        if (getAlphanumeric(text[--length]) == -1) return RT_FALSE;
    return RT_TRUE;
}

static rt_bool_t isNumeric(const char *text, rt_uint16_t length) {
    while (length != 0) {
        length--;
        if ((text[length] < '0') || (text[length] > '9')) return RT_FALSE;
    }
    return RT_TRUE;
}

// We store the following tightly packed (less 8) in modeInfo
//               <=9  <=26  <= 40
// NUMERIC      ( 10,   12,    14);
// ALPHANUMERIC (  9,   11,    13);
// BYTE         (  8,   16,    16);
static rt_int8_t getModeBits(rt_uint8_t version, rt_uint8_t mode) {
    /* Note: We use 15 instead of 16; since 15 doesn't exist and we cannot
             store 16 (8 + 8) in 3 bits

        hex(int("".join(reversed([('00' + bin(x - 8)[2:])[-3:] \
            for x in [10, 9, 8, 12, 11, 15, 14, 13, 15]])), 2))
     */
    rt_uint32_t modeInfo;
    rt_int8_t result;

    modeInfo = 0x7bbb80a;
#if (LOCK_VERSION == 0) || (LOCK_VERSION > 9)
    if (version > 9) modeInfo >>= 9;
#endif
#if (LOCK_VERSION == 0) || (LOCK_VERSION > 26)
    if (version > 26) modeInfo >>= 9;
#endif

    result = 8 + ((modeInfo >> (3 * mode)) & 0x07);
    if (result == 15) result = 16;

    return result;
}

static rt_uint16_t bb_getGridSizeBytes(rt_uint8_t size) {
    return (((size * size) + 7) / 8);
}

static rt_uint16_t bb_getBufferSizeBytes(rt_uint32_t bits) {
    return ((bits + 7) / 8);
}

static void bb_initBuffer(BitBucket *bitBuffer, rt_uint8_t *data,
    rt_int32_t capacityBytes) {
    bitBuffer->bitOffsetOrWidth = 0;
    bitBuffer->capacityBytes = capacityBytes;
    bitBuffer->data = data;
    rt_memset(data, 0x00, bitBuffer->capacityBytes);
}

static void bb_initGrid(BitBucket *bitGrid, rt_uint8_t *data, rt_uint8_t size) {
    bitGrid->bitOffsetOrWidth = size;
    bitGrid->capacityBytes = bb_getGridSizeBytes(size);
    bitGrid->data = data;
    rt_memset(data, 0x00, bitGrid->capacityBytes);
}

static void bb_appendBits(BitBucket *bitBuffer, rt_uint32_t val,
    rt_uint8_t length) {
    rt_uint32_t offset;
    rt_int8_t i;

    offset = bitBuffer->bitOffsetOrWidth;
    for (i = length - 1; i >= 0; i--, offset++) {
        bitBuffer->data[offset >> 3] |= ((val >> i) & 1) << (7 - (offset & 7));
    }
    bitBuffer->bitOffsetOrWidth = offset;
}

static void bb_setBit(BitBucket *bitGrid, rt_uint8_t x, rt_uint8_t y,
    rt_bool_t on) {
    rt_uint32_t offset;
    rt_uint8_t mask;

    offset = y * bitGrid->bitOffsetOrWidth + x;
    mask = 1 << (7 - (offset & 0x07));
    if (on) {
        bitGrid->data[offset >> 3] |= mask;
    } else {
        bitGrid->data[offset >> 3] &= ~mask;
    }
}

static rt_bool_t bb_getBit(BitBucket *bitGrid, rt_uint8_t x, rt_uint8_t y) {
    rt_uint32_t offset;
    rt_uint8_t mask;

    offset = y * bitGrid->bitOffsetOrWidth + x;
    mask = 1 << (7 - (offset & 0x07));
    return (bitGrid->data[offset >> 3] & mask) != 0;
}

static void bb_invertBit(BitBucket *bitGrid, rt_uint8_t x, rt_uint8_t y,
    rt_bool_t invert) {
    rt_uint32_t offset;
    rt_uint8_t mask;
    rt_bool_t on;

    offset = y * bitGrid->bitOffsetOrWidth + x;
    mask = 1 << (7 - (offset & 0x07));
    on = ((bitGrid->data[offset >> 3] & mask) != 0);
    if (on ^ invert) {
        bitGrid->data[offset >> 3] |= mask;
    } else {
        bitGrid->data[offset >> 3] &= ~mask;
    }
}

/* XORs the data modules in this QR Code with the given mask pattern. Due to
   XOR's mathematical properties, calling applyMask(m) twice with the same
   value is equivalent to no change at all.
   This means it is possible to apply a mask, undo it, and try another mask.
   Note that a final well-formed QR Code symbol needs exactly one mask applied
   (not zero, not two, etc.).
 */
static void applyMask(BitBucket *modules, BitBucket *isFunction,
    rt_uint8_t mask) {
    rt_uint8_t x, y, size;
    rt_bool_t invert;

    size = modules->bitOffsetOrWidth;
    for (y = 0; y < size; y++) {
        for (x = 0; x < size; x++) {
            if (bb_getBit(isFunction, x, y)) continue;

            if (0 == mask) invert = (x + y) % 2 == 0;
            else if (1 == mask) invert = y % 2 == 0;
            else if (2 == mask) invert = x % 3 == 0;
            else if (3 == mask) invert = (x + y) % 3 == 0;
            else if (4 == mask) invert = (x / 3 + y / 2) % 2 == 0;
            else if (5 == mask) invert = x * y % 2 + x * y % 3 == 0;
            else if (6 == mask) invert = (x * y % 2 + x * y % 3) % 2 == 0;
            else if (7 == mask) invert = ((x + y) % 2 + x * y % 3) % 2 == 0;
            else
                invert = 0;
            bb_invertBit(modules, x, y, invert);
        }
    }
}

static void setFunctionModule(BitBucket *modules, BitBucket *isFunction,
    rt_uint8_t x, rt_uint8_t y, rt_bool_t on) {
    bb_setBit(modules, x, y, on);
    bb_setBit(isFunction, x, y, RT_TRUE);
}

/* Draws a 9*9 finder pattern including the border separator, with the center
   module at (x, y).
 */
static void drawFinderPattern(BitBucket *modules, BitBucket *isFunction,
    rt_uint8_t x, rt_uint8_t y) {
    rt_uint8_t size, dist;
    rt_int8_t i, j;
    rt_int16_t xx, yy;

    size = modules->bitOffsetOrWidth;
    for (i = -4; i <= 4; i++) {
        for (j = -4; j <= 4; j++) {
            dist = max(abs(i), abs(j));  // Chebyshev/infinity norm
            xx = x + j;
            yy = y + i;
            if ((0 <= xx) && (xx < size) && (0 <= yy) && (yy < size)) {
                setFunctionModule(modules, isFunction, xx, yy,
                    (dist != 2) && (dist != 4));
            }
        }
    }
}

/* Draws a 5*5 alignment pattern, with the center module at (x, y). */
static void drawAlignmentPattern(BitBucket *modules, BitBucket *isFunction,
    rt_uint8_t x, rt_uint8_t y) {
    rt_int8_t i, j;

    for (i = -2; i <= 2; i++) {
        for (j = -2; j <= 2; j++) {
            setFunctionModule(modules, isFunction, x + j, y + i,
                max(abs(i), abs(j)) != 1);
        }
    }
}

/* Draws two copies of the format bits (with its own error correction code)
   based on the given mask and this object's error correction level field.
 */
static void drawFormatBits(BitBucket *modules, BitBucket *isFunction,
    rt_uint8_t ecc, rt_uint8_t mask) {
    rt_uint8_t size, i;
    rt_uint32_t data, rem;

    size = modules->bitOffsetOrWidth;
    // Calculate error correction code and pack bits
    data = ecc << 3 | mask;  // errCorrLvl is uint2, mask is uint3
    rem = data;
    for (int i = 0; i < 10; i++) {
        rem = (rem << 1) ^ ((rem >> 9) * 0x537);
    }

    data = data << 10 | rem;
    data ^= 0x5412;  // uint15

    // Draw first copy
    for (i = 0; i <= 5; i++) {
        setFunctionModule(modules, isFunction, 8, i, ((data >> i) & 1) != 0);
    }
    setFunctionModule(modules, isFunction, 8, 7, ((data >> 6) & 1) != 0);
    setFunctionModule(modules, isFunction, 8, 8, ((data >> 7) & 1) != 0);
    setFunctionModule(modules, isFunction, 7, 8, ((data >> 8) & 1) != 0);
    for (i = 9; i < 15; i++) {
        setFunctionModule(modules, isFunction, 14 - i, 8,
            ((data >> i) & 1) != 0);
    }

    // Draw second copy
    for (i = 0; i <= 7; i++) {
        setFunctionModule(modules, isFunction, size - 1 - i, 8,
            ((data >> i) & 1) != 0);
    }
    for (i = 8; i < 15; i++) {
        setFunctionModule(modules, isFunction, 8, size - 15 + i,
            ((data >> i) & 1) != 0);
    }
    setFunctionModule(modules, isFunction, 8, size - 8, RT_TRUE);
}

/* Draws two copies of the version bits (with its own error correction code),
   based on this object's version field (which only has an effect for
   7 <= version <= 40).
 */
static void drawVersion(BitBucket *modules, BitBucket *isFunction,
    rt_uint8_t version) {
    rt_int8_t size;
    rt_uint8_t i, a, b;
    rt_bool_t bit;
    rt_uint32_t rem, data;

#if LOCK_VERSION != 0 && LOCK_VERSION < 7
    return;

#else
    if (version < 7) return;

    size = modules->bitOffsetOrWidth;

    // Calculate error correction code and pack bits
    rem = version;  // version is uint6, in the range [7, 40]
    for (i = 0; i < 12; i++) {
        rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
    }

    data = version << 12 | rem;  // uint18
    // Draw two copies
    for (i = 0; i < 18; i++) {
        bit = ((data >> i) & 1) != 0;
        a = size - 11 + i % 3;
        b = i / 3;
        setFunctionModule(modules, isFunction, a, b, bit);
        setFunctionModule(modules, isFunction, b, a, bit);
    }

#endif
}

static void drawFunctionPatterns(BitBucket *modules, BitBucket *isFunction,
    rt_uint8_t version, rt_uint8_t ecc) {
    rt_uint8_t i, size;
    #if LOCK_VERSION == 0 || LOCK_VERSION > 1
        rt_uint8_t alignCount;
        rt_uint8_t step;
        rt_uint8_t alignPositionIndex;
        rt_uint8_t *alignPosition;
        rt_uint8_t j, pos;
    #endif

    // Draw the horizontal and vertical timing patterns
    size = modules->bitOffsetOrWidth;
    for (i = 0; i < size; i++) {
        setFunctionModule(modules, isFunction, 6, i, i % 2 == 0);
        setFunctionModule(modules, isFunction, i, 6, i % 2 == 0);
    }

    // Draw 3 finder patterns
    // (all corners except bottom right; overwrites some timing modules)
    drawFinderPattern(modules, isFunction, 3, 3);
    drawFinderPattern(modules, isFunction, size - 4, 3);
    drawFinderPattern(modules, isFunction, 3, size - 4);

    #if LOCK_VERSION == 0 || LOCK_VERSION > 1
        if (version > 1) {
            // Draw the numerous alignment patterns
            alignCount = version / 7 + 2;
            if (version != 32) { // ceil((size - 13) / (2*numAlign - 2)) * 2
                step = (version * 4 + alignCount * 2 + 1) / \
                       (2 * alignCount - 2) * 2;
            } else { // C-C-C-Combo breaker!
                step = 26;
            }
            
            alignPositionIndex = alignCount - 1;
            alignPosition = (rt_uint8_t *)rt_calloc(1, alignCount);
            if (!alignPosition) {
                LOG_W("No Memory");
                return;
            }

            alignPosition[0] = 6;
            size = version * 4 + 17;
            for (i = 0, pos = size - 7; i < alignCount - 1; i++, pos -= step) {
                alignPosition[alignPositionIndex--] = pos;
            }

            for (i = 0; i < alignCount; i++) {
                for (j = 0; j < alignCount; j++) {
                    if ((i == 0 && j == 0) || \
                        (i == 0 && j == alignCount - 1) || \
                        (i == alignCount - 1 && j == 0)) {
                        continue;  // Skip the three finder corners
                    } else {
                        drawAlignmentPattern(modules, isFunction,
                            alignPosition[i], alignPosition[j]);
                    }
                }
            }
            rt_free(alignPosition);
    }

    #endif

    // Draw configuration data
    // Dummy mask value; overwritten later in the constructor
    drawFormatBits(modules, isFunction, ecc, 0);
    drawVersion(modules, isFunction, version);
}

/* Draws the given sequence of 8-bit codewords (data and error correction)
   onto the entire data area of this QR Code symbol. Function modules need to
   be marked off before this is called.
 */
static void drawCodewords(BitBucket *modules, BitBucket *isFunction,
    BitBucket *codewords) {
    rt_uint32_t bitLength, i, j;
    rt_uint8_t *data;
    rt_uint8_t size, vert, x, y;
    rt_int16_t right;
    rt_bool_t upwards;
    
    bitLength = codewords->bitOffsetOrWidth;
    data = codewords->data;
    size = modules->bitOffsetOrWidth;
    i = 0; // Bit index into the data

    // Do the funny zigzag scan
    // Index of right column in each column pair
    for (right = size - 1; right >= 1; right -= 2) {
        if (right == 6) right = 5;
        
        for (vert = 0; vert < size; vert++) { // Vertical counter
            for (j = 0; j < 2; j++) {
                x = right - j;  // Actual x coordinate
                upwards = ((right & 2) == 0) ^ (x < 6);
                y = upwards ? size - 1 - vert : vert;  // Actual y coordinate
                if (!bb_getBit(isFunction, x, y) && i < bitLength) {
                    bb_setBit(modules, x, y,
                        ((data[i >> 3] >> (7 - (i & 7))) & 1) != 0);
                    i++;
                }
                /* If there are any remainder bits (0 to 7), they are already
                   set to 0/RT_FALSE/white when the grid of modules was
                   initialized
                 */
            }
        }
    }
}

#define PENALTY_N1      3
#define PENALTY_N2      3
#define PENALTY_N3     40
#define PENALTY_N4     10

/* Calculates and returns the penalty score based on state of this QR Code's
   current modules.
   This is used by the automatic mask choice algorithm to find the mask pattern
   that yields the lowest score.
 */
// @TODO: This can be optimized by working with the bytes instead of bits.
static rt_uint32_t getPenaltyScore(BitBucket *modules) {
    rt_uint32_t result;
    rt_uint8_t size, x, y, runX, runY;
    rt_uint16_t black, total, k;

    result = 0;
    size = modules->bitOffsetOrWidth;

    // Adjacent modules in row having same color
    for (y = 0; y < size; y++) {
        rt_bool_t colorX, cx;
        colorX = bb_getBit(modules, 0, y);
        for (x = 1, runX = 1; x < size; x++) {
            cx = bb_getBit(modules, x, y);
            if (cx != colorX) {
                colorX = cx;
                runX = 1;
            } else {
                runX++;
                if (runX == 5) {
                    result += PENALTY_N1;
                } else if (runX > 5) {
                    result++;
                }
            }
        }
    }

    // Adjacent modules in column having same color
    for (x = 0; x < size; x++) {
        rt_bool_t colorY, cy;
        colorY = bb_getBit(modules, x, 0);
        for (y = 1, runY = 1; y < size; y++) {
            cy = bb_getBit(modules, x, y);
            if (cy != colorY) {
                colorY = cy;
                runY = 1;
            } else {
                runY++;
                if (runY == 5) {
                    result += PENALTY_N1;
                } else if (runY > 5) {
                    result++;
                }
            }
        }
    }

    black = 0;
    for (y = 0; y < size; y++) {
        rt_uint16_t bitsRow = 0;
        rt_uint16_t bitsCol = 0;

        for (x = 0; x < size; x++) {
            rt_bool_t color = bb_getBit(modules, x, y);

            // 2*2 blocks of modules having same color
            if (x > 0 && y > 0) {
                rt_bool_t colorUL = bb_getBit(modules, x - 1, y - 1);
                rt_bool_t colorUR = bb_getBit(modules, x, y - 1);
                rt_bool_t colorL = bb_getBit(modules, x - 1, y);

                if (color == colorUL && color == colorUR && color == colorL) {
                    result += PENALTY_N2;
                }
            }

            // Finder-like pattern in rows and columns
            bitsRow = ((bitsRow << 1) & 0x7FF) | color;
            bitsCol = ((bitsCol << 1) & 0x7FF) | bb_getBit(modules, y, x);

            // Needs 11 bits accumulated
            if (x >= 10) {
                if (bitsRow == 0x05D || bitsRow == 0x5D0) {
                    result += PENALTY_N3;
                }
                if (bitsCol == 0x05D || bitsCol == 0x5D0) {
                    result += PENALTY_N3;
                }
            }

            // Balance of black and white modules
            if (color) black++;
        }
    }

    // Find smallest k such that (45-5k)% <= dark/total <= (55+5k)%
    total = size * size;
    for (k = 0;
        ((black * 20) < ((9 - k) * total)) || \
        ((black * 20) > ((11 + k) * total));
        k++) {
        result += PENALTY_N4;
    }

    return result;
}

static rt_uint8_t rs_multiply(rt_uint8_t x, rt_uint8_t y) {
    rt_uint16_t z;
    rt_int8_t i;

    // Russian peasant multiplication
    // See: https://en.wikipedia.org/wiki/Ancient_Egyptian_multiplication
    for (z = 0, i = 7; i >= 0; i--) {
        z = (z << 1) ^ ((z >> 7) * 0x11D);
        z ^= ((y >> i) & 1) * x;
    }
    return z;
}

static void rs_init(rt_uint8_t degree, rt_uint8_t *coeff) {
    rt_uint16_t root;
    rt_uint8_t i, j;

    rt_memset(coeff, 0, degree);
    coeff[degree - 1] = 1;
    
    /* Compute the product polynomial
       (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{degree-1}),
       drop the highest term, and store the rest of the coefficients in order
       of descending powers.
       Note that r = 0x02, which is a generator element of this field
       GF(2^8/0x11D).
     */
    for (root = 1, i = 0; i < degree; i++) {
        // Multiply the current product by (x - r^i)
        for (j = 0; j < degree; j++) {
            coeff[j] = rs_multiply(coeff[j], root);
            if (j + 1 < degree) {
                coeff[j] ^= coeff[j + 1];
            }
        }
        // Multiply by 0x02 mod GF(2^8/0x11D)
        root = (root << 1) ^ ((root >> 7) * 0x11D);
    }
}

static void rs_getRemainder(rt_uint8_t degree, rt_uint8_t *coeff,
    rt_uint8_t *data, rt_uint8_t length, rt_uint8_t *result,
    rt_uint8_t stride) {
    /* Compute the remainder by performing polynomial division */
    rt_uint8_t i, j, factor;

    //for (rt_uint8_t i = 0; i < degree; i++) { result[] = 0; }
    //rt_memset(result, 0, degree);

    for (i = 0; i < length; i++) {
        factor = data[i] ^ result[0];
        for (j = 1; j < degree; j++) {
            result[(j - 1) * stride] = result[j * stride];
        }
        result[(degree - 1) * stride] = 0;
        for (j = 0; j < degree; j++) {
            result[j * stride] ^= rs_multiply(coeff[j], factor);
        }
    }
}

static rt_int8_t encodeDataCodewords(BitBucket *dataCodewords,
    const rt_uint8_t *text, rt_uint16_t length, rt_uint8_t version) {
    rt_int8_t mode;
    rt_uint16_t accumData, i;
    rt_uint8_t accumCount;

    mode = MODE_BYTE;

    if (isNumeric((char*)text, length)) {
        mode = MODE_NUMERIC;
        bb_appendBits(dataCodewords, 1 << MODE_NUMERIC, 4);
        bb_appendBits(dataCodewords, length,
            getModeBits(version, MODE_NUMERIC));

        accumData = 0;
        accumCount = 0;
        for (i = 0; i < length; i++) {
            accumData = accumData * 10 + ((char)(text[i]) - '0');
            accumCount++;
            if (accumCount == 3) {
                bb_appendBits(dataCodewords, accumData, 10);
                accumData = 0;
                accumCount = 0;
            }
        }

        // 1 or 2 digits remaining
        if (accumCount > 0) {
            bb_appendBits(dataCodewords, accumData, accumCount * 3 + 1);
        }

    } else if (isAlphanumeric((char*)text, length)) {
        mode = MODE_ALPHANUMERIC;
        bb_appendBits(dataCodewords, 1 << MODE_ALPHANUMERIC, 4);
        bb_appendBits(dataCodewords, length,
            getModeBits(version, MODE_ALPHANUMERIC));

        accumData = 0;
        accumCount = 0;
        for (i = 0; i  < length; i++) {
            accumData = accumData * 45 + getAlphanumeric((char)(text[i]));
            accumCount++;
            if (accumCount == 2) {
                bb_appendBits(dataCodewords, accumData, 11);
                accumData = 0;
                accumCount = 0;
            }
        }

        // 1 character remaining
        if (accumCount > 0) {
            bb_appendBits(dataCodewords, accumData, 6);
        }

    } else {
        bb_appendBits(dataCodewords, 1 << MODE_BYTE, 4);
        bb_appendBits(dataCodewords, length, getModeBits(version, MODE_BYTE));

        for (i = 0; i < length; i++) {
            bb_appendBits(dataCodewords, (char)(text[i]), 8);
        }
    }

    // bb_setBits(dataCodewords, length, 4, getModeBits(version, mode));
    return mode;
}

static void performErrorCorrection(rt_uint8_t version, rt_uint8_t ecc,
    BitBucket *data) {
    /* See: http://www.thonky.com/qr-code-tutorial/structure-final-message */
    #if (LOCK_VERSION == 0)
        rt_uint8_t numBlocks = NUM_ERROR_CORRECTION_BLOCKS[ecc][version - 1];
        rt_uint16_t totalEcc = NUM_ERROR_CORRECTION_CODEWORDS[ecc][version - 1];
        rt_uint16_t moduleCount = NUM_RAW_DATA_MODULES[version - 1];
    #else
        rt_uint8_t numBlocks = NUM_ERROR_CORRECTION_BLOCKS[ecc];
        rt_uint16_t totalEcc = NUM_ERROR_CORRECTION_CODEWORDS[ecc];
        rt_uint16_t moduleCount = NUM_RAW_DATA_MODULES;
    #endif

    rt_uint8_t blockEccLen = totalEcc / numBlocks;
    rt_uint8_t numShortBlocks = numBlocks - moduleCount / 8 % numBlocks;
    rt_uint8_t shortBlockLen = moduleCount / 8 / numBlocks;
    rt_uint8_t shortDataBlockLen = shortBlockLen - blockEccLen;

    rt_uint8_t *result, *coeff, *dataBytes;
    rt_uint16_t offset;
    rt_uint8_t i, blockNum, blockSize;

    result = (rt_uint8_t *)rt_calloc(1, data->capacityBytes);
    coeff = (rt_uint8_t *)rt_calloc(1, blockEccLen);
    if (!result || !coeff) {
        LOG_W("No Memory");
        rt_free(result);
        rt_free(coeff);
        return;
    }
    rs_init(blockEccLen, coeff);

    offset = 0;
    dataBytes = data->data;

    // Interleave all short blocks
    for (i = 0; i < shortDataBlockLen; i++) {
        rt_uint8_t blockNum;
        rt_uint16_t index = i;
        rt_uint8_t stride = shortDataBlockLen;
        for (blockNum = 0; blockNum < numBlocks; blockNum++) {
            result[offset++] = dataBytes[index];
        #if (LOCK_VERSION == 0) || (LOCK_VERSION >= 5)
            if (blockNum == numShortBlocks) stride++;
        #endif
            index += stride;
        }
    }

    // Version less than 5 only have short blocks
    #if (LOCK_VERSION == 0) || (LOCK_VERSION >= 5)
    {
        // Interleave long blocks
        rt_uint16_t index = shortDataBlockLen * (numShortBlocks + 1);
        rt_uint8_t stride = shortDataBlockLen;
        for (blockNum = 0; blockNum < numBlocks - numShortBlocks; blockNum++) {
            result[offset++] = dataBytes[index];
            if (blockNum == 0) stride++;
            index += stride;
        }
    }
    #endif

    // Add all ecc blocks, interleaved
    blockSize = shortDataBlockLen;
    for (blockNum = 0; blockNum < numBlocks; blockNum++) {
        #if (LOCK_VERSION == 0) || (LOCK_VERSION >= 5)
            if (blockNum == numShortBlocks) blockSize++;
        #endif
        rs_getRemainder(blockEccLen, coeff, dataBytes, blockSize,
            &result[offset + blockNum], numBlocks);
        dataBytes += blockSize;
    }

    rt_memcpy(data->data, result, data->capacityBytes);
    data->bitOffsetOrWidth = moduleCount;

    rt_free(result);
    rt_free(coeff);
}

/* We store the Format bits tightly packed into a single byte (each of the 4
   modes is 2 bits)
   The format bits can be determined by ECC_FORMAT_BITS >> (2 * ecc)
 */
static const rt_uint8_t ECC_FORMAT_BITS = \
    (0x02 << 6) | (0x03 << 4) | (0x00 << 2) | (0x01 << 0);

rt_uint16_t qrcode_getBufferSize(rt_uint8_t version) {
    return bb_getGridSizeBytes(4 * version + 17);
}

// @TODO: Return error if data is too big.
rt_int8_t qrcode_initBytes(QRCode *qrcode, rt_uint8_t *modules,
    rt_uint8_t version, rt_uint8_t ecc, rt_uint8_t *data, rt_uint16_t length) {
    rt_uint8_t size = version * 4 + 17;
    rt_uint8_t eccFormatBits = (ECC_FORMAT_BITS >> (2 * ecc)) & 0x03;

    #if (LOCK_VERSION == 0)
        rt_uint16_t moduleCount = NUM_RAW_DATA_MODULES[version - 1];
        rt_uint16_t dataCapacity = moduleCount / 8 - \
            NUM_ERROR_CORRECTION_CODEWORDS[eccFormatBits][version - 1];
    #else
        version = LOCK_VERSION;
        rt_uint16_t moduleCount = NUM_RAW_DATA_MODULES;
        rt_uint16_t dataCapacity = moduleCount / 8 - \
            NUM_ERROR_CORRECTION_CODEWORDS[eccFormatBits];
    #endif

    struct BitBucket codewords;
    rt_uint8_t *codewordBytes;
    rt_int8_t mode;
    rt_uint32_t padding;
    rt_uint8_t padByte;
    BitBucket modulesGrid, isFunctionGrid;
    rt_uint8_t *isFunctionGridBytes;
    rt_uint8_t mask, i;
    rt_int32_t minPenalty;

    qrcode->version = version;
    qrcode->size = size;
    qrcode->ecc = ecc;
    qrcode->modules = modules;

    codewordBytes = (rt_uint8_t *)rt_calloc(1,
        bb_getBufferSizeBytes(moduleCount));
    if (!codewordBytes) {
        LOG_W("No Memory");
        return -RT_ENOMEM;
    }
    bb_initBuffer(&codewords, codewordBytes,
        (rt_int32_t)bb_getBufferSizeBytes(moduleCount));

    // Place the data code words into the buffer
    mode = encodeDataCodewords(&codewords, data, length, version);
    if (mode < 0) return -1;
    qrcode->mode = mode;

    // Add terminator and pad up to a byte if applicable
    padding = (dataCapacity * 8) - codewords.bitOffsetOrWidth;
    if (padding > 4) { padding = 4; }
    bb_appendBits(&codewords, 0, padding);
    bb_appendBits(&codewords, 0, (8 - codewords.bitOffsetOrWidth % 8) % 8);

    // Pad with alternate bytes until data capacity is reached
    for (padByte = 0xEC;
         codewords.bitOffsetOrWidth < (dataCapacity * 8);
         padByte ^= 0xEC ^ 0x11) {
        bb_appendBits(&codewords, padByte, 8);
    }

    bb_initGrid(&modulesGrid, modules, size);

    isFunctionGridBytes = (rt_uint8_t *)rt_calloc(1, bb_getGridSizeBytes(size));
    if (!isFunctionGridBytes) {
        LOG_W("No Memory");
        return -RT_ENOMEM;
    }
    bb_initGrid(&isFunctionGrid, isFunctionGridBytes, size);

    drawFunctionPatterns(&modulesGrid, &isFunctionGrid, version, eccFormatBits);
    performErrorCorrection(version, eccFormatBits, &codewords);
    drawCodewords(&modulesGrid, &isFunctionGrid, &codewords);

    // Find the best (lowest penalty) mask
    mask = 0;
    minPenalty = 0x7FFFFFFF;
    for (i = 0; i < 8; i++) {
        drawFormatBits(&modulesGrid, &isFunctionGrid, eccFormatBits, i);
        applyMask(&modulesGrid, &isFunctionGrid, i);
        int penalty = getPenaltyScore(&modulesGrid);
        if (penalty < minPenalty) {
            mask = i;
            minPenalty = penalty;
        }
        // Undoes the mask due to XOR
        applyMask(&modulesGrid, &isFunctionGrid, i);
    }

    qrcode->mask = mask;

    // Overwrite old format bits
    drawFormatBits(&modulesGrid, &isFunctionGrid, eccFormatBits, mask);
    // Apply the final choice of mask
    applyMask(&modulesGrid, &isFunctionGrid, mask);

    rt_free(isFunctionGridBytes);
    rt_free(codewordBytes);
    return 0;
}

rt_int8_t qrcode_initText(QRCode *qrcode, rt_uint8_t *modules,
    rt_uint8_t version, rt_uint8_t ecc, const char *data) {
    return qrcode_initBytes(qrcode, modules, version, ecc, (rt_uint8_t*)data,
        rt_strlen(data));
}

rt_bool_t qrcode_getModule(QRCode *qrcode, rt_uint8_t x, rt_uint8_t y) {
    rt_uint32_t offset;

    if (x >= qrcode->size || y >= qrcode->size) {
        return RT_FALSE;
    }

    offset = y * qrcode->size + x;
    return (qrcode->modules[offset >> 3] & (1 << (7 - (offset & 0x07)))) != 0;
}
