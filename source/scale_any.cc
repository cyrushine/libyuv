/*
 *  Copyright 2015 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/scale.h"
#include "libyuv/scale_row.h"

#include "libyuv/basic_types.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// Definition for ScaleFilterCols, ScaleARGBCols and ScaleARGBFilterCols
#define CANY(NAMEANY, TERP_SIMD, TERP_C, BPP, MASK)                            \
    void NAMEANY(uint8* dst_ptr, const uint8* src_ptr,                         \
                 int dst_width, int x, int dx) {                               \
      int n = dst_width & ~MASK;                                               \
      if (n > 0) {                                                             \
        TERP_SIMD(dst_ptr, src_ptr, n, x, dx);                                 \
      }                                                                        \
      TERP_C(dst_ptr + n * BPP, src_ptr,                                       \
             dst_width & MASK, x + n * dx, dx);                                \
    }

#ifdef HAS_SCALEFILTERCOLS_NEON
CANY(ScaleFilterCols_Any_NEON, ScaleFilterCols_NEON, ScaleFilterCols_C, 1, 7)
#endif
#ifdef HAS_SCALEARGBCOLS_NEON
CANY(ScaleARGBCols_Any_NEON, ScaleARGBCols_NEON, ScaleARGBCols_C, 4, 7)
#endif
#ifdef HAS_SCALEARGBFILTERCOLS_NEON
CANY(ScaleARGBFilterCols_Any_NEON, ScaleARGBFilterCols_NEON,
     ScaleARGBFilterCols_C, 4, 3)
#endif
#undef CANY

// Fixed scale down.
#define SDANY(NAMEANY, SCALEROWDOWN_SIMD, SCALEROWDOWN_C, FACTOR, BPP, MASK)   \
    void NAMEANY(const uint8* src_ptr, ptrdiff_t src_stride,                   \
                 uint8* dst_ptr, int dst_width) {                              \
      int r = (int)((unsigned int)dst_width % (MASK + 1));                     \
      int n = dst_width - r;                                                   \
      if (n > 0) {                                                             \
        SCALEROWDOWN_SIMD(src_ptr, src_stride, dst_ptr, n);                    \
      }                                                                        \
      SCALEROWDOWN_C(src_ptr + (n * FACTOR) * BPP, src_stride,                 \
                     dst_ptr + n * BPP, r);                                    \
    }


#ifdef HAS_SCALEROWDOWN2_SSE2
SDANY(ScaleRowDown2_Any_SSE2, ScaleRowDown2_SSE2, ScaleRowDown2_C, 2, 1, 15)
SDANY(ScaleRowDown2Linear_Any_SSE2, ScaleRowDown2Linear_SSE2,
      ScaleRowDown2Linear_C, 2, 1, 15)
SDANY(ScaleRowDown2Box_Any_SSE2, ScaleRowDown2Box_SSE2, ScaleRowDown2Box_C,
      2, 1, 15)
#endif
#ifdef HAS_SCALEROWDOWN2_AVX2
SDANY(ScaleRowDown2_Any_AVX2, ScaleRowDown2_AVX2, ScaleRowDown2_C, 2, 1, 31)
SDANY(ScaleRowDown2Linear_Any_AVX2, ScaleRowDown2Linear_AVX2,
      ScaleRowDown2Linear_C, 2, 1, 31)
SDANY(ScaleRowDown2Box_Any_AVX2, ScaleRowDown2Box_AVX2, ScaleRowDown2Box_C,
      2, 1, 31)
#endif
#ifdef HAS_SCALEROWDOWN2_NEON
SDANY(ScaleRowDown2_Any_NEON, ScaleRowDown2_NEON, ScaleRowDown2_C, 2, 1, 15)
SDANY(ScaleRowDown2Linear_Any_NEON, ScaleRowDown2Linear_NEON,
      ScaleRowDown2Linear_C, 2, 1, 15)
SDANY(ScaleRowDown2Box_Any_NEON, ScaleRowDown2Box_NEON,
      ScaleRowDown2Box_C, 2, 1, 15)
#endif
#ifdef HAS_SCALEROWDOWN4_SSE2
SDANY(ScaleRowDown4_Any_SSE2, ScaleRowDown4_SSE2, ScaleRowDown4_C, 4, 1, 7)
SDANY(ScaleRowDown4Box_Any_SSE2, ScaleRowDown4Box_SSE2, ScaleRowDown4Box_C,
      4, 1, 7)
#endif
#ifdef HAS_SCALEROWDOWN4_NEON
SDANY(ScaleRowDown4_Any_NEON, ScaleRowDown4_NEON, ScaleRowDown4_C, 4, 1, 7)
SDANY(ScaleRowDown4Box_Any_NEON, ScaleRowDown4Box_NEON, ScaleRowDown4Box_C,
      4, 1, 7)
#endif


#ifdef HAS_SCALEROWDOWN34_SSSE3
SDANY(ScaleRowDown34_Any_SSSE3, ScaleRowDown34_SSSE3, ScaleRowDown34_C,
      3 / 4, 1, 23)
SDANY(ScaleRowDown34_0_Box_Any_SSSE3, ScaleRowDown34_0_Box_SSSE3,
      ScaleRowDown34_0_Box_C, 3 / 4, 1, 23)
SDANY(ScaleRowDown34_1_Box_Any_SSSE3, ScaleRowDown34_1_Box_SSSE3,
      ScaleRowDown34_1_Box_C, 3 / 4, 1, 23)
#endif
#ifdef HAS_SCALEROWDOWN34_NEON
SDANY(ScaleRowDown34_Any_NEON, ScaleRowDown34_NEON, ScaleRowDown34_C,
      3 / 4, 1, 23)
SDANY(ScaleRowDown34_0_Box_Any_NEON, ScaleRowDown34_0_Box_NEON,
      ScaleRowDown34_0_Box_C, 3 / 4, 1, 23)
SDANY(ScaleRowDown34_1_Box_Any_NEON, ScaleRowDown34_1_Box_NEON,
      ScaleRowDown34_1_Box_C, 3 / 4, 1, 23)
#endif
#ifdef HAS_SCALEROWDOWN38_SSSE3
SDANY(ScaleRowDown38_Any_SSSE3, ScaleRowDown38_SSSE3, ScaleRowDown38_C,
      3 / 4, 1, 11)
SDANY(ScaleRowDown38_3_Box_Any_SSSE3, ScaleRowDown38_3_Box_SSSE3,
      ScaleRowDown38_3_Box_C, 3 / 4, 1, 5)
SDANY(ScaleRowDown38_2_Box_Any_SSSE3, ScaleRowDown38_2_Box_SSSE3,
      ScaleRowDown38_2_Box_C, 3 / 4, 1, 5)
#endif
#ifdef HAS_SCALEROWDOWN38_NEON
SDANY(ScaleRowDown38_Any_NEON, ScaleRowDown38_NEON, ScaleRowDown38_C,
      3 / 4, 1, 11)
SDANY(ScaleRowDown38_3_Box_Any_NEON, ScaleRowDown38_3_Box_NEON,
      ScaleRowDown38_3_Box_C, 3 / 4, 1, 11)
SDANY(ScaleRowDown38_2_Box_Any_NEON, ScaleRowDown38_2_Box_NEON,
      ScaleRowDown38_2_Box_C, 3 / 4, 1, 11)
#endif
#undef SDANY

// Fixed scale down.
#define SAANY(NAMEANY, SCALEADDROWS_SIMD, SCALEADDROWS_C, MASK)                \
  void NAMEANY(const uint8* src_ptr, ptrdiff_t src_stride,                     \
               uint16* dst_ptr, int src_width, int src_height) {               \
      int n = src_width & ~MASK;                                               \
      if (n > 0) {                                                             \
        SCALEADDROWS_SIMD(src_ptr, src_stride, dst_ptr, n, src_height);        \
      }                                                                        \
      SCALEADDROWS_C(src_ptr + n, src_stride,                                  \
                     dst_ptr + n, src_width & MASK, src_height);               \
    }

#ifdef HAS_SCALEADDROWS_SSE2
SAANY(ScaleAddRows_Any_SSE2, ScaleAddRows_SSE2, ScaleAddRows_C, 15)
#endif
#ifdef HAS_SCALEADDROWS_AVX2
SAANY(ScaleAddRows_Any_AVX2, ScaleAddRows_AVX2, ScaleAddRows_C, 31)
#endif
#ifdef HAS_SCALEADDROWS_NEON
SAANY(ScaleAddRows_Any_NEON, ScaleAddRows_NEON, ScaleAddRows_C, 15)
#endif
#undef SAANY

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif





