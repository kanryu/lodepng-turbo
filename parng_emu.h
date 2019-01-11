#pragma once

#include <limits.h>
#include <stddef.h>

#define PNG_FILTER_VALUE_NONE  0
#define PNG_FILTER_VALUE_SUB   1
#define PNG_FILTER_VALUE_UP    2
#define PNG_FILTER_VALUE_AVG   3
#define PNG_FILTER_VALUE_PAETH 4
#define PNG_FILTER_VALUE_LAST  5

#ifdef __cplusplus
extern "C" {
#endif
void parng_predict_scanline_none_packed_32bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_none_strided_32bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_none_packed_24bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_none_strided_24bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_none_packed_16bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_none_packed_8bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_left_packed_32bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_left_strided_32bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_left_packed_24bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_left_strided_24bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_left_packed_16bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_left_packed_8bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_up_packed_32bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_up_strided_32bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_up_packed_24bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_up_strided_24bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_up_packed_16bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_up_packed_8bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_average_strided_32bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_average_strided_24bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_paeth_strided_32bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);
void parng_predict_scanline_paeth_strided_24bpp(uint8_t* dest, const uint8_t* src, const uint8_t* prev, uint64_t length, uint64_t stride);

#ifdef __cplusplus
}
#endif
