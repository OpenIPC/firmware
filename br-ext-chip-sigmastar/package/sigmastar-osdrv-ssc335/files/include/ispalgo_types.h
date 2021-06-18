/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

 Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
Sigmastar Technology Corp. and be kept in strict confidence
(Sigmastar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of Sigmastar Confidential
Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

/*
 * ispalgo_type.h
 *
 *  Created on:
 *      Author:
 */

/// data type unsigned char, data length 1 byte
typedef unsigned char               MS_U8;                              // 1 byte
/// data type unsigned short, data length 2 byte
typedef unsigned short              MS_U16;                             // 2 bytes
/// data type unsigned int, data length 4 byte
typedef unsigned int                MS_U32;                             // 4 bytes
/// data type unsigned int, data length 8 byte
typedef unsigned long long          MS_U64;                             // 8 bytes
/// data type signed char, data length 1 byte
typedef signed char                 MS_S8;                              // 1 byte
/// data type signed short, data length 2 byte
typedef signed short                MS_S16;                             // 2 bytes
/// data type signed int, data length 4 byte
typedef signed int                  MS_S32;                             // 4 bytes
/// data type signed int, data length 8 byte
typedef signed long long            MS_S64;                             // 8 bytes
/// data type float, data length 4 byte
typedef float                       MS_FLOAT;                           // 4 bytes
/// data type hardware physical address
typedef unsigned long long          MS_PHYADDR;                         // 32bit physical address
/// definition for MS_BOOL
typedef unsigned char               MS_BOOL;

#ifdef SUCCESS
#undef SUCCESS
#endif
#define SUCCESS                    (0)
#ifdef FAIL
#undef FAIL
#endif
#define FAIL                       (-1)

#ifdef TRUE
#undef TRUE
#endif
#define TRUE                       (1)

#ifdef FALSE
#undef FALSE
#endif
#define FALSE                      (0)

#ifdef NULL
#undef NULL
#endif
#define NULL                       (0)

#ifdef sign
#undef sign
#endif
#define sign(a)         (((a) > 0) ? (1) : (-1))

#ifdef LIMIT
#undef LIMIT
#endif
#define LIMIT(x, low, high)     (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#ifdef MIN
#undef MIN
#endif
#define MIN(a,b)    (((a) >= (b))?(b):(a))

#ifdef MIN3
#undef MIN3
#endif
#define MIN3(a, b, c)           (((a) < (b)) ? (MIN(a, c)) : (MIN(b, c)))

#ifdef MAX
#undef MAX
#endif
#define MAX(a, b)             (((a)>(b)) ? (a) : (b))

#ifdef MAX3
#undef MAX3
#endif
#define MAX3(a, b, c)          (((a) > (b)) ? (MAX(a, c)) : (MAX(b, c)))

#ifdef MINMAX
#undef MINMAX
#endif
#define MINMAX(v,a,b)          (((v)<(a))? (a) : ((v)>(b)) ? (b) : (v))

#ifdef ABS
#undef ABS
#endif
#define ABS(x)                 (((x) < 0) ? (-(x)) : (x))

#ifdef DIFF
#undef DIFF
#endif
#define DIFF(a, b)             (((a) > (b)) ? (a-b) : (b-a))

#define lInterp(T, m, n, a, b) (a + (b - a)*(T - m)/(n - m)) //Linear interpolation



