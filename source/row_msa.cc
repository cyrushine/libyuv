/*
 *  Copyright 2016 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/row.h"

// This module is for GCC MSA
#if !defined(LIBYUV_DISABLE_MSA) && defined(__mips_msa)
#include "libyuv/macros_msa.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// Convert 8 pixels of YUV 420 to RGB.
#define I422TORGB(in0, in1, in2, ub, vr, ug, vg,               \
                  bb, bg, br, yg, out0, out1, out2) {          \
  v8i16 vec0_m;                                                \
  v4i32 reg0_m, reg1_m, reg2_m, reg3_m, reg4_m;                \
  v4i32 reg5_m, reg6_m, reg7_m, reg8_m, reg9_m;                \
  v4i32 max_val_m = __msa_ldi_w(255);                          \
  v8i16 zero_m = { 0 };                                        \
                                                               \
  in1 = (v16u8) __msa_ilvr_b((v16i8) in1, (v16i8) in1);        \
  in2 = (v16u8) __msa_ilvr_b((v16i8) in2, (v16i8) in2);        \
  vec0_m = (v8i16) __msa_ilvr_b((v16i8) in0, (v16i8) in0);     \
  reg0_m = (v4i32) __msa_ilvr_h(zero_m, vec0_m);               \
  reg1_m = (v4i32) __msa_ilvl_h(zero_m, vec0_m);               \
  reg0_m *= vec_yg;                                            \
  reg1_m *= vec_yg;                                            \
  reg0_m = __msa_srai_w(reg0_m, 16);                           \
  reg1_m = __msa_srai_w(reg1_m, 16);                           \
  reg4_m = reg0_m + br;                                        \
  reg5_m = reg1_m + br;                                        \
  reg2_m = reg0_m + bg;                                        \
  reg3_m = reg1_m + bg;                                        \
  reg0_m += bb;                                                \
  reg1_m += bb;                                                \
  vec0_m = (v8i16) __msa_ilvr_b((v16i8) zero_m, (v16i8) in1);  \
  reg6_m = (v4i32) __msa_ilvr_h(zero_m, (v8i16) vec0_m);       \
  reg7_m = (v4i32) __msa_ilvl_h(zero_m, (v8i16) vec0_m);       \
  vec0_m = (v8i16) __msa_ilvr_b((v16i8) zero_m, (v16i8) in2);  \
  reg8_m = (v4i32) __msa_ilvr_h(zero_m, (v8i16) vec0_m);       \
  reg9_m = (v4i32) __msa_ilvl_h(zero_m, (v8i16) vec0_m);       \
  reg0_m -= reg6_m * ub;                                       \
  reg1_m -= reg7_m * ub;                                       \
  reg2_m -= reg6_m * ug;                                       \
  reg3_m -= reg7_m * ug;                                       \
  reg4_m -= reg8_m * vr;                                       \
  reg5_m -= reg9_m * vr;                                       \
  reg2_m -= reg8_m * vg;                                       \
  reg3_m -= reg9_m * vg;                                       \
  reg0_m = __msa_srai_w(reg0_m, 6);                            \
  reg1_m = __msa_srai_w(reg1_m, 6);                            \
  reg2_m = __msa_srai_w(reg2_m, 6);                            \
  reg3_m = __msa_srai_w(reg3_m, 6);                            \
  reg4_m = __msa_srai_w(reg4_m, 6);                            \
  reg5_m = __msa_srai_w(reg5_m, 6);                            \
  reg0_m = __msa_maxi_s_w(reg0_m, 0);                          \
  reg1_m = __msa_maxi_s_w(reg1_m, 0);                          \
  reg2_m = __msa_maxi_s_w(reg2_m, 0);                          \
  reg3_m = __msa_maxi_s_w(reg3_m, 0);                          \
  reg4_m = __msa_maxi_s_w(reg4_m, 0);                          \
  reg5_m = __msa_maxi_s_w(reg5_m, 0);                          \
  reg0_m = __msa_min_s_w(reg0_m, max_val_m);                   \
  reg1_m = __msa_min_s_w(reg1_m, max_val_m);                   \
  reg2_m = __msa_min_s_w(reg2_m, max_val_m);                   \
  reg3_m = __msa_min_s_w(reg3_m, max_val_m);                   \
  reg4_m = __msa_min_s_w(reg4_m, max_val_m);                   \
  reg5_m = __msa_min_s_w(reg5_m, max_val_m);                   \
  out0 = __msa_pckev_h((v8i16) reg1_m, (v8i16) reg0_m);        \
  out1 = __msa_pckev_h((v8i16) reg3_m, (v8i16) reg2_m);        \
  out2 = __msa_pckev_h((v8i16) reg5_m, (v8i16) reg4_m);        \
}

void MirrorRow_MSA(const uint8* src, uint8* dst, int width) {
  int x;
  v16u8 src0, src1, src2, src3;
  v16u8 dst0, dst1, dst2, dst3;
  v16i8 shuffler = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
  src += width - 64;

  for (x = 0; x < width; x += 64) {
    LD_UB4(src, 16, src3, src2, src1, src0);
    VSHF_B2_UB(src3, src3, src2, src2, shuffler, shuffler, dst3, dst2);
    VSHF_B2_UB(src1, src1, src0, src0, shuffler, shuffler, dst1, dst0);
    ST_UB4(dst0, dst1, dst2, dst3, dst, 16);
    dst += 64;
    src -= 64;
  }
}

void ARGBMirrorRow_MSA(const uint8* src, uint8* dst, int width) {
  int x;
  v16u8 src0, src1, src2, src3;
  v16u8 dst0, dst1, dst2, dst3;
  v16i8 shuffler = { 12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3 };
  src += width * 4 - 64;

  for (x = 0; x < width; x += 16) {
    LD_UB4(src, 16, src3, src2, src1, src0);
    VSHF_B2_UB(src3, src3, src2, src2, shuffler, shuffler, dst3, dst2);
    VSHF_B2_UB(src1, src1, src0, src0, shuffler, shuffler, dst1, dst0);
    ST_UB4(dst0, dst1, dst2, dst3, dst, 16);
    dst += 64;
    src -= 64;
  }
}

void I422ToYUY2Row_MSA(const uint8* src_y,
                       const uint8* src_u,
                       const uint8* src_v,
                       uint8* dst_yuy2,
                       int width) {
  int x;
  v16u8 src_u0, src_v0, src_y0, src_y1, vec_uv0, vec_uv1;
  v16u8 dst_yuy2_0, dst_yuy2_1, dst_yuy2_2, dst_yuy2_3;

  for (x = 0; x < width; x += 32) {
    src_u0 = LD_UB(src_u);
    src_v0 = LD_UB(src_v);
    LD_UB2(src_y, 16, src_y0, src_y1);
    ILVRL_B2_UB(src_v0, src_u0, vec_uv0, vec_uv1);
    ILVRL_B2_UB(vec_uv0, src_y0, dst_yuy2_0, dst_yuy2_1);
    ILVRL_B2_UB(vec_uv1, src_y1, dst_yuy2_2, dst_yuy2_3);
    ST_UB4(dst_yuy2_0, dst_yuy2_1, dst_yuy2_2, dst_yuy2_3, dst_yuy2, 16);
    src_u += 16;
    src_v += 16;
    src_y += 32;
    dst_yuy2 += 64;
  }
}

void I422ToUYVYRow_MSA(const uint8* src_y,
                       const uint8* src_u,
                       const uint8* src_v,
                       uint8* dst_uyvy,
                       int width) {
  int x;
  v16u8 src_u0, src_v0, src_y0, src_y1, vec_uv0, vec_uv1;
  v16u8 dst_uyvy0, dst_uyvy1, dst_uyvy2, dst_uyvy3;

  for (x = 0; x < width; x += 32) {
    src_u0 = LD_UB(src_u);
    src_v0 = LD_UB(src_v);
    LD_UB2(src_y, 16, src_y0, src_y1);
    ILVRL_B2_UB(src_v0, src_u0, vec_uv0, vec_uv1);
    ILVRL_B2_UB(src_y0, vec_uv0, dst_uyvy0, dst_uyvy1);
    ILVRL_B2_UB(src_y1, vec_uv1, dst_uyvy2, dst_uyvy3);
    ST_UB4(dst_uyvy0, dst_uyvy1, dst_uyvy2, dst_uyvy3, dst_uyvy, 16);
    src_u += 16;
    src_v += 16;
    src_y += 32;
    dst_uyvy += 64;
  }
}

void I422ToARGBRow_MSA(const uint8* src_y, const uint8* src_u,
                       const uint8* src_v, uint8* rgb_buf,
                       const struct YuvConstants* yuvconstants, int width) {
  int x;
  int32 data_u, data_v;
  int64 data_y;
  v16u8 src0, src1, src2, dst0, dst1;
  v8i16 vec0, vec1, vec2;
  v4i32 vec_ub, vec_vr, vec_ug, vec_vg, vec_bb, vec_bg, vec_br, vec_yg;
  v16u8 const_255 = (v16u8) __msa_ldi_b(255);
  v4i32 zero = { 0 };

  vec_ub = __msa_fill_w(yuvconstants->kUVToB[0]);
  vec_vr = __msa_fill_w(yuvconstants->kUVToR[1]);
  vec_ug = __msa_fill_w(yuvconstants->kUVToG[0]);
  vec_vg = __msa_fill_w(yuvconstants->kUVToG[1]);
  vec_bb = __msa_fill_w(yuvconstants->kUVBiasB[0]);
  vec_bg = __msa_fill_w(yuvconstants->kUVBiasG[0]);
  vec_br = __msa_fill_w(yuvconstants->kUVBiasR[0]);
  vec_yg = __msa_fill_w(yuvconstants->kYToRgb[0]);

  for (x = 0; x < width; x += 8) {
    data_y = LD(src_y);
    data_u = LW(src_u);
    data_v = LW(src_v);
    src0 = (v16u8) __msa_insert_d((v2i64) zero, 0, data_y);
    src1 = (v16u8) __msa_insert_w(zero, 0, data_u);
    src2 = (v16u8) __msa_insert_w(zero, 0, data_v);
    I422TORGB(src0, src1, src2, vec_ub, vec_vr, vec_ug, vec_vg,
              vec_bb, vec_bg, vec_br, vec_yg, vec0, vec1, vec2);
    vec0 = (v8i16) __msa_ilvev_b((v16i8) vec1, (v16i8) vec0);
    vec1 = (v8i16) __msa_ilvev_b((v16i8) const_255, (v16i8) vec2);
    dst0 = (v16u8) __msa_ilvr_h((v8i16) vec1, (v8i16) vec0);
    dst1 = (v16u8) __msa_ilvl_h((v8i16) vec1, (v8i16) vec0);
    ST_UB2(dst0, dst1, rgb_buf, 16);
    src_y += 8;
    src_u += 4;
    src_v += 4;
    rgb_buf += 32;
  }
}

void I422ToRGBARow_MSA(const uint8* src_y, const uint8* src_u,
                       const uint8* src_v, uint8* rgb_buf,
                       const struct YuvConstants* yuvconstants, int width) {
  int x;
  int64 data_y;
  int32 data_u, data_v;
  v16u8 src0, src1, src2, dst0, dst1;
  v8i16 vec0, vec1, vec2;
  v4i32 vec_ub, vec_vr, vec_ug, vec_vg, vec_bb, vec_bg, vec_br, vec_yg;
  v16u8 const_255 = (v16u8) __msa_ldi_b(255);
  v4i32 zero = { 0 };

  vec_ub = __msa_fill_w(yuvconstants->kUVToB[0]);
  vec_vr = __msa_fill_w(yuvconstants->kUVToR[1]);
  vec_ug = __msa_fill_w(yuvconstants->kUVToG[0]);
  vec_vg = __msa_fill_w(yuvconstants->kUVToG[1]);
  vec_bb = __msa_fill_w(yuvconstants->kUVBiasB[0]);
  vec_bg = __msa_fill_w(yuvconstants->kUVBiasG[0]);
  vec_br = __msa_fill_w(yuvconstants->kUVBiasR[0]);
  vec_yg = __msa_fill_w(yuvconstants->kYToRgb[0]);

  for (x = 0; x < width; x += 8) {
    data_y = LD(src_y);
    data_u = LW(src_u);
    data_v = LW(src_v);
    src0 = (v16u8) __msa_insert_d((v2i64) zero, 0, data_y);
    src1 = (v16u8) __msa_insert_w(zero, 0, data_u);
    src2 = (v16u8) __msa_insert_w(zero, 0, data_v);
    I422TORGB(src0, src1, src2, vec_ub, vec_vr, vec_ug, vec_vg,
              vec_bb, vec_bg, vec_br, vec_yg, vec0, vec1, vec2);
    vec0 = (v8i16) __msa_ilvev_b((v16i8) vec0, (v16i8) const_255);
    vec1 = (v8i16) __msa_ilvev_b((v16i8) vec2, (v16i8) vec1);
    dst0 = (v16u8) __msa_ilvr_h(vec1, vec0);
    dst1 = (v16u8) __msa_ilvl_h(vec1, vec0);
    ST_UB2(dst0, dst1, rgb_buf, 16);
    src_y += 8;
    src_u += 4;
    src_v += 4;
    rgb_buf += 32;
  }
}

void YUY2ToYRow_MSA(const uint8* src_yuy2, uint8* dst_y, int width) {
  int x;
  v16u8 src0, src1, src2, src3, dst0, dst1;

  for (x = 0; x < width; x += 32) {
    LD_UB4(src_yuy2, 16, src0, src1, src2, src3);
    dst0 = (v16u8) __msa_pckev_b((v16i8) src1, (v16i8) src0);
    dst1 = (v16u8) __msa_pckev_b((v16i8) src3, (v16i8) src2);
    ST_UB2(dst0, dst1, dst_y, 16);
    src_yuy2 += 64;
    dst_y += 32;
  }
}

void YUY2ToUVRow_MSA(const uint8* src_yuy2, int src_stride_yuy2,
                     uint8* dst_u, uint8* dst_v, int width) {
  const uint8* src_yuy2_next = src_yuy2 + src_stride_yuy2;
  int x;
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16u8 vec0, vec1, dst0, dst1;

  for (x = 0; x < width; x += 32) {
    LD_UB4(src_yuy2, 16, src0, src1, src2, src3);
    LD_UB4(src_yuy2_next, 16, src4, src5, src6, src7);
    src0 = (v16u8) __msa_pckod_b((v16i8) src1, (v16i8) src0);
    src1 = (v16u8) __msa_pckod_b((v16i8) src3, (v16i8) src2);
    src2 = (v16u8) __msa_pckod_b((v16i8) src5, (v16i8) src4);
    src3 = (v16u8) __msa_pckod_b((v16i8) src7, (v16i8) src6);
    vec0 = __msa_aver_u_b(src0, src2);
    vec1 = __msa_aver_u_b(src1, src3);
    dst0 = (v16u8) __msa_pckev_b((v16i8) vec1, (v16i8) vec0);
    dst1 = (v16u8) __msa_pckod_b((v16i8) vec1, (v16i8) vec0);
    ST_UB(dst0, dst_u);
    ST_UB(dst1, dst_v);
    src_yuy2 += 64;
    src_yuy2_next += 64;
    dst_u += 16;
    dst_v += 16;
  }
}

void YUY2ToUV422Row_MSA(const uint8* src_yuy2, uint8* dst_u, uint8* dst_v,
                        int width) {
  int x;
  v16u8 src0, src1, src2, src3, dst0, dst1;

  for (x = 0; x < width; x += 32) {
    LD_UB4(src_yuy2, 16, src0, src1, src2, src3);
    src0 = (v16u8) __msa_pckod_b((v16i8) src1, (v16i8) src0);
    src1 = (v16u8) __msa_pckod_b((v16i8) src3, (v16i8) src2);
    dst0 = (v16u8) __msa_pckev_b((v16i8) src1, (v16i8) src0);
    dst1 = (v16u8) __msa_pckod_b((v16i8) src1, (v16i8) src0);
    ST_UB(dst0, dst_u);
    ST_UB(dst1, dst_v);
    src_yuy2 += 64;
    dst_u += 16;
    dst_v += 16;
  }
}

void UYVYToYRow_MSA(const uint8* src_uyvy, uint8* dst_y, int width) {
  int x;
  v16u8 src0, src1, src2, src3, dst0, dst1;

  for (x = 0; x < width; x += 32) {
    LD_UB4(src_uyvy, 16, src0, src1, src2, src3);
    dst0 = (v16u8) __msa_pckod_b((v16i8) src1, (v16i8) src0);
    dst1 = (v16u8) __msa_pckod_b((v16i8) src3, (v16i8) src2);
    ST_UB2(dst0, dst1, dst_y, 16);
    src_uyvy += 64;
    dst_y += 32;
  }
}

void UYVYToUVRow_MSA(const uint8* src_uyvy, int src_stride_uyvy,
                     uint8* dst_u, uint8* dst_v, int width) {
  const uint8 *src_uyvy_next = src_uyvy + src_stride_uyvy;
  int x;
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16u8 vec0, vec1, dst0, dst1;

  for (x = 0; x < width; x += 32) {
    LD_UB4(src_uyvy, 16, src0, src1, src2, src3);
    LD_UB4(src_uyvy_next, 16, src4, src5, src6, src7);
    src0 = (v16u8) __msa_pckev_b((v16i8) src1, (v16i8) src0);
    src1 = (v16u8) __msa_pckev_b((v16i8) src3, (v16i8) src2);
    src2 = (v16u8) __msa_pckev_b((v16i8) src5, (v16i8) src4);
    src3 = (v16u8) __msa_pckev_b((v16i8) src7, (v16i8) src6);
    vec0 = __msa_aver_u_b(src0, src2);
    vec1 = __msa_aver_u_b(src1, src3);
    dst0 = (v16u8) __msa_pckev_b((v16i8) vec1, (v16i8) vec0);
    dst1 = (v16u8) __msa_pckod_b((v16i8) vec1, (v16i8) vec0);
    ST_UB(dst0, dst_u);
    ST_UB(dst1, dst_v);
    src_uyvy += 64;
    src_uyvy_next += 64;
    dst_u += 16;
    dst_v += 16;
  }
}

void UYVYToUV422Row_MSA(const uint8* src_uyvy, uint8* dst_u, uint8* dst_v,
                        int width) {
  int x;
  v16u8 src0, src1, src2, src3, dst0, dst1;

  for (x = 0; x < width; x += 32) {
    LD_UB4(src_uyvy, 16, src0, src1, src2, src3);
    src0 = (v16u8) __msa_pckev_b((v16i8) src1, (v16i8) src0);
    src1 = (v16u8) __msa_pckev_b((v16i8) src3, (v16i8) src2);
    dst0 = (v16u8) __msa_pckev_b((v16i8) src1, (v16i8) src0);
    dst1 = (v16u8) __msa_pckod_b((v16i8) src1, (v16i8) src0);
    ST_UB(dst0, dst_u);
    ST_UB(dst1, dst_v);
    src_uyvy += 64;
    dst_u += 16;
    dst_v += 16;
  }
}

void ARGBToYRow_MSA(const uint8* src_argb0, uint8* dst_y, int width) {
  int x;
  v16u8 src0, src1, src2, src3, vec0, vec1, vec2, vec3, dst0;
  v8u16 reg0, reg1, reg2, reg3, reg4, reg5;
  v16i8 zero = { 0 };
  v8u16 const_0x19 = (v8u16) __msa_ldi_h(0x19);
  v8u16 const_0x81 = (v8u16) __msa_ldi_h(0x81);
  v8u16 const_0x42 = (v8u16) __msa_ldi_h(0x42);
  v8u16 const_0x1080 = (v8u16) __msa_fill_h(0x1080);

  for (x = 0; x < width; x += 16) {
    src0 = (v16u8) __msa_ld_b((v16u8*) src_argb0, 0);
    src1 = (v16u8) __msa_ld_b((v16u8*) src_argb0, 16);
    src2 = (v16u8) __msa_ld_b((v16u8*) src_argb0, 32);
    src3 = (v16u8) __msa_ld_b((v16u8*) src_argb0, 48);
    vec0 = (v16u8) __msa_pckev_b((v16i8) src1, (v16i8) src0);
    vec1 = (v16u8) __msa_pckev_b((v16i8) src3, (v16i8) src2);
    vec2 = (v16u8) __msa_pckod_b((v16i8) src1, (v16i8) src0);
    vec3 = (v16u8) __msa_pckod_b((v16i8) src3, (v16i8) src2);
    reg0 = (v8u16) __msa_ilvev_b(zero, (v16i8) vec0);
    reg1 = (v8u16) __msa_ilvev_b(zero, (v16i8) vec1);
    reg2 = (v8u16) __msa_ilvev_b(zero, (v16i8) vec2);
    reg3 = (v8u16) __msa_ilvev_b(zero, (v16i8) vec3);
    reg4 = (v8u16) __msa_ilvod_b(zero, (v16i8) vec0);
    reg5 = (v8u16) __msa_ilvod_b(zero, (v16i8) vec1);
    reg0 *= const_0x19;
    reg1 *= const_0x19;
    reg2 *= const_0x81;
    reg3 *= const_0x81;
    reg4 *= const_0x42;
    reg5 *= const_0x42;
    reg0 += reg2;
    reg1 += reg3;
    reg0 += reg4;
    reg1 += reg5;
    reg0 += const_0x1080;
    reg1 += const_0x1080;
    reg0 = (v8u16) __msa_srai_h((v8i16) reg0, 8);
    reg1 = (v8u16) __msa_srai_h((v8i16) reg1, 8);
    dst0 = (v16u8) __msa_pckev_b((v16i8) reg1, (v16i8) reg0);
    ST_UB(dst0, dst_y);
    src_argb0 += 64;
    dst_y += 16;
  }
}

void ARGBToUVRow_MSA(const uint8* src_argb0, int src_stride_argb,
                     uint8* dst_u, uint8* dst_v, int width) {
  int x;
  const uint8* src_argb0_next = src_argb0 + src_stride_argb;
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v16u8 vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7, vec8, vec9;
  v8u16 reg0, reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9;
  v16u8 dst0, dst1;
  v8u16 const_0x70 = (v8u16) __msa_ldi_h(0x70);
  v8u16 const_0x4A = (v8u16) __msa_ldi_h(0x4A);
  v8u16 const_0x26 = (v8u16) __msa_ldi_h(0x26);
  v8u16 const_0x5E = (v8u16) __msa_ldi_h(0x5E);
  v8u16 const_0x12 = (v8u16) __msa_ldi_h(0x12);
  v8u16 const_0x8080 = (v8u16) __msa_fill_h(0x8080);

  for (x = 0; x < width; x += 32) {
    src0 = (v16u8) __msa_ld_b((v16u8*) src_argb0, 0);
    src1 = (v16u8) __msa_ld_b((v16u8*) src_argb0, 16);
    src2 = (v16u8) __msa_ld_b((v16u8*) src_argb0, 32);
    src3 = (v16u8) __msa_ld_b((v16u8*) src_argb0, 48);
    src4 = (v16u8) __msa_ld_b((v16u8*) src_argb0, 64);
    src5 = (v16u8) __msa_ld_b((v16u8*) src_argb0, 80);
    src6 = (v16u8) __msa_ld_b((v16u8*) src_argb0, 96);
    src7 = (v16u8) __msa_ld_b((v16u8*) src_argb0, 112);
    vec0 = (v16u8) __msa_pckev_b((v16i8) src1, (v16i8) src0);
    vec1 = (v16u8) __msa_pckev_b((v16i8) src3, (v16i8) src2);
    vec2 = (v16u8) __msa_pckev_b((v16i8) src5, (v16i8) src4);
    vec3 = (v16u8) __msa_pckev_b((v16i8) src7, (v16i8) src6);
    vec4 = (v16u8) __msa_pckod_b((v16i8) src1, (v16i8) src0);
    vec5 = (v16u8) __msa_pckod_b((v16i8) src3, (v16i8) src2);
    vec6 = (v16u8) __msa_pckod_b((v16i8) src5, (v16i8) src4);
    vec7 = (v16u8) __msa_pckod_b((v16i8) src7, (v16i8) src6);
    vec8 = (v16u8) __msa_pckev_b((v16i8) vec1, (v16i8) vec0);
    vec9 = (v16u8) __msa_pckev_b((v16i8) vec3, (v16i8) vec2);
    vec4 = (v16u8) __msa_pckev_b((v16i8) vec5, (v16i8) vec4);
    vec5 = (v16u8) __msa_pckev_b((v16i8) vec7, (v16i8) vec6);
    vec0 = (v16u8) __msa_pckod_b((v16i8) vec1, (v16i8) vec0);
    vec1 = (v16u8) __msa_pckod_b((v16i8) vec3, (v16i8) vec2);
    reg0 = __msa_hadd_u_h(vec8, vec8);
    reg1 = __msa_hadd_u_h(vec9, vec9);
    reg2 = __msa_hadd_u_h(vec4, vec4);
    reg3 = __msa_hadd_u_h(vec5, vec5);
    reg4 = __msa_hadd_u_h(vec0, vec0);
    reg5 = __msa_hadd_u_h(vec1, vec1);
    src0 = (v16u8) __msa_ld_b((v16u8*) src_argb0_next, 0);
    src1 = (v16u8) __msa_ld_b((v16u8*) src_argb0_next, 16);
    src2 = (v16u8) __msa_ld_b((v16u8*) src_argb0_next, 32);
    src3 = (v16u8) __msa_ld_b((v16u8*) src_argb0_next, 48);
    src4 = (v16u8) __msa_ld_b((v16u8*) src_argb0_next, 64);
    src5 = (v16u8) __msa_ld_b((v16u8*) src_argb0_next, 80);
    src6 = (v16u8) __msa_ld_b((v16u8*) src_argb0_next, 96);
    src7 = (v16u8) __msa_ld_b((v16u8*) src_argb0_next, 112);
    vec0 = (v16u8) __msa_pckev_b((v16i8) src1, (v16i8) src0);
    vec1 = (v16u8) __msa_pckev_b((v16i8) src3, (v16i8) src2);
    vec2 = (v16u8) __msa_pckev_b((v16i8) src5, (v16i8) src4);
    vec3 = (v16u8) __msa_pckev_b((v16i8) src7, (v16i8) src6);
    vec4 = (v16u8) __msa_pckod_b((v16i8) src1, (v16i8) src0);
    vec5 = (v16u8) __msa_pckod_b((v16i8) src3, (v16i8) src2);
    vec6 = (v16u8) __msa_pckod_b((v16i8) src5, (v16i8) src4);
    vec7 = (v16u8) __msa_pckod_b((v16i8) src7, (v16i8) src6);
    vec8 = (v16u8) __msa_pckev_b((v16i8) vec1, (v16i8) vec0);
    vec9 = (v16u8) __msa_pckev_b((v16i8) vec3, (v16i8) vec2);
    vec4 = (v16u8) __msa_pckev_b((v16i8) vec5, (v16i8) vec4);
    vec5 = (v16u8) __msa_pckev_b((v16i8) vec7, (v16i8) vec6);
    vec0 = (v16u8) __msa_pckod_b((v16i8) vec1, (v16i8) vec0);
    vec1 = (v16u8) __msa_pckod_b((v16i8) vec3, (v16i8) vec2);
    reg0 += __msa_hadd_u_h(vec8, vec8);
    reg1 += __msa_hadd_u_h(vec9, vec9);
    reg2 += __msa_hadd_u_h(vec4, vec4);
    reg3 += __msa_hadd_u_h(vec5, vec5);
    reg4 += __msa_hadd_u_h(vec0, vec0);
    reg5 += __msa_hadd_u_h(vec1, vec1);
    reg0 = (v8u16) __msa_srai_h((v8i16) reg0, 2);
    reg1 = (v8u16) __msa_srai_h((v8i16) reg1, 2);
    reg2 = (v8u16) __msa_srai_h((v8i16) reg2, 2);
    reg3 = (v8u16) __msa_srai_h((v8i16) reg3, 2);
    reg4 = (v8u16) __msa_srai_h((v8i16) reg4, 2);
    reg5 = (v8u16) __msa_srai_h((v8i16) reg5, 2);
    reg6 = reg0 * const_0x70;
    reg7 = reg1 * const_0x70;
    reg8 = reg2 * const_0x4A;
    reg9 = reg3 * const_0x4A;
    reg6 += const_0x8080;
    reg7 += const_0x8080;
    reg8 += reg4 * const_0x26;
    reg9 += reg5 * const_0x26;
    reg0 *= const_0x12;
    reg1 *= const_0x12;
    reg2 *= const_0x5E;
    reg3 *= const_0x5E;
    reg4 *= const_0x70;
    reg5 *= const_0x70;
    reg2 += reg0;
    reg3 += reg1;
    reg4 += const_0x8080;
    reg5 += const_0x8080;
    reg6 -= reg8;
    reg7 -= reg9;
    reg4 -= reg2;
    reg5 -= reg3;
    reg6 = (v8u16) __msa_srai_h((v8i16) reg6, 8);
    reg7 = (v8u16) __msa_srai_h((v8i16) reg7, 8);
    reg4 = (v8u16) __msa_srai_h((v8i16) reg4, 8);
    reg5 = (v8u16) __msa_srai_h((v8i16) reg5, 8);
    dst0 = (v16u8) __msa_pckev_b((v16i8) reg7, (v16i8) reg6);
    dst1 = (v16u8) __msa_pckev_b((v16i8) reg5, (v16i8) reg4);
    ST_UB(dst0, dst_u);
    ST_UB(dst1, dst_v);
    src_argb0 += 128;
    src_argb0_next += 128;
    dst_u += 16;
    dst_v += 16;
  }
}

void ARGB4444ToARGBRow_MSA(const uint8* src_argb4444, uint8* dst_argb,
                           int width) {
  int x;
  v16u8 src0, src1;
  v8u16 vec0, vec1, vec2, vec3;
  v16u8 dst0, dst1, dst2, dst3;

  for (x = 0; x < width; x += 16) {
    src0 = (v16u8) __msa_ld_b((v16u8*) src_argb4444, 0);
    src1 = (v16u8) __msa_ld_b((v16u8*) src_argb4444, 16);
    vec0 = (v8u16) __msa_andi_b(src0, 0x0F);
    vec1 = (v8u16) __msa_andi_b(src1, 0x0F);
    vec2 = (v8u16) __msa_andi_b(src0, 0xF0);
    vec3 = (v8u16) __msa_andi_b(src1, 0xF0);
    vec0 |= (v8u16) __msa_slli_b((v16i8) vec0, 4);
    vec1 |= (v8u16) __msa_slli_b((v16i8) vec1, 4);
    vec2 |= (v8u16) __msa_srli_b((v16i8) vec2, 4);
    vec3 |= (v8u16) __msa_srli_b((v16i8) vec3, 4);
    dst0 = (v16u8) __msa_ilvr_b((v16i8) vec2, (v16i8) vec0);
    dst1 = (v16u8) __msa_ilvl_b((v16i8) vec2, (v16i8) vec0);
    dst2 = (v16u8) __msa_ilvr_b((v16i8) vec3, (v16i8) vec1);
    dst3 = (v16u8) __msa_ilvl_b((v16i8) vec3, (v16i8) vec1);
    ST_UB4(dst0, dst1, dst2, dst3, dst_argb, 16);
    src_argb4444 += 32;
    dst_argb += 64;
  }
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif

#endif  // !defined(LIBYUV_DISABLE_MSA) && defined(__mips_msa)
