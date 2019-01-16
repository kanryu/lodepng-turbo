/*
LodePNG Turbo

Copyright (c) 2019 KATO Kanryu

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

#include <stdio.h>
#include "lodepng.h"
#include "libdeflate/libdeflate.h"
#include "parng_emu.h"
#include "depng.h"

/*Try the code, if it returns error, also return the error.*/
#define CERROR_TRY_RETURN(call)\
{\
  unsigned error = call;\
  if(error) return error;\
}


#ifdef LODEPNG_COMPILE_ENCODER

/*this is a good tradeoff between speed and compression ratio*/
#define DEFAULT_WINDOWSIZE 2048

static unsigned custom_zlib_compress(unsigned char** out, size_t* outsize,
	const unsigned char* in, size_t insize,
	const LodePNGCompressSettings* settings, size_t outbuffsize)
{
	/*bit pointer in the "in" data, current byte is bp >> 3, current bit is bp & 0x7 (from lsb to msb of the byte)*/
	//size_t insize_available = insize;
	size_t outsize_available = outbuffsize;// 1 * 1024 * 1024;/*byte position in the out buffer*/
	auto result = libdeflate_zlib_compress(settings->compressor, in, insize,
		*out, outsize_available);
	switch (result) {
	case libdeflate_result::LIBDEFLATE_BAD_DATA: return 20;
	case libdeflate_result::LIBDEFLATE_SHORT_OUTPUT: return 52;
	case libdeflate_result::LIBDEFLATE_INSUFFICIENT_SPACE: return 52;
	default:
	case libdeflate_result::LIBDEFLATE_SUCCESS: return 0;
	}
}

void lodepng_compress_settings_init(LodePNGCompressSettings* settings)
{
  /*compress with dynamic huffman tree (not in the mathematical sense, just not the predefined one)*/
  settings->btype = 2;
  settings->use_lz77 = 1;
  settings->windowsize = DEFAULT_WINDOWSIZE;
  settings->minmatch = 3;
  settings->nicematch = 128;
  settings->lazymatching = 1;

  settings->custom_zlib = custom_zlib_compress;
  settings->custom_deflate = 0;
  settings->custom_context = 0;
  settings->compressor = libdeflate_alloc_compressor(6);
}

const LodePNGCompressSettings lodepng_default_compress_settings = {2, 1, DEFAULT_WINDOWSIZE, 3, 128, 1, 0, 0, 0};

#else
extern const LodePNGCompressSettings lodepng_default_compress_settings;
#endif

#ifdef LODEPNG_COMPILE_DECODER

static unsigned custom_zlib_decompress(unsigned char** out, size_t* outsize,
	const unsigned char* in, size_t insize,
	const LodePNGDecompressSettings* settings, size_t outbuffsize)
{
	/*bit pointer in the "in" data, current byte is bp >> 3, current bit is bp & 0x7 (from lsb to msb of the byte)*/
	//size_t insize_available = insize;
	size_t outsize_available = outbuffsize;// 1 * 1024 * 1024;/*byte position in the out buffer*/
	auto result = libdeflate_zlib_decompress(settings->decompressor, in, insize,
		*out, outsize_available, outsize);
	switch (result) {
	case libdeflate_result::LIBDEFLATE_BAD_DATA: return 20;
	case libdeflate_result::LIBDEFLATE_SHORT_OUTPUT: return 52;
	case libdeflate_result::LIBDEFLATE_INSUFFICIENT_SPACE: return 52;
	default:
	case libdeflate_result::LIBDEFLATE_SUCCESS: return 0;
	}
}

void lodepng_decompress_settings_init(LodePNGDecompressSettings* settings)
{
  settings->ignore_adler32 = 0;

  settings->custom_zlib = custom_zlib_decompress;
  settings->custom_inflate = 0;
  settings->custom_context = 0;
  settings->decompressor = libdeflate_alloc_decompressor();
}

#endif

unsigned lodepng_crc32(const unsigned char* data, size_t length)
{
	return libdeflate_crc32(0, data, length);
}
unsigned unfilterScanline(unsigned char* recon, const unsigned char* scanline, const unsigned char* precon,
                                 size_t bytewidth, unsigned char filterType, size_t length);

void writeoutdata(const char* filename, const unsigned char* pbuff, size_t size)
{
	FILE* f = fopen(filename, "wb");
	fwrite(pbuff, 1, size, f);
	fclose(f);
}


unsigned unfilter(unsigned char* out, const unsigned char* in, unsigned w, unsigned h, unsigned bpp, LodePNGState* state)
{
  /*
  For PNG filter method 0
  this function unfilters a single image (e.g. without interlacing this is called once, with Adam7 seven times)
  out must have enough bytes allocated already, in must have the scanlines + 1 filtertype byte per scanline
  w and h are image dimensions or dimensions of reduced image, bpp is bits per pixel
  in and out are allowed to be the same memory address (but aren't the same size since in has the extra filter bytes)
  */

  unsigned y;
  //unsigned char* prevline = 0;

  /*bytewidth is used for filtering, is 1 when bpp < 8, number of bytes per pixel otherwise*/
  size_t bytewidth = (bpp + 7) / 8;
  size_t linebytes = (w * bpp + 7) / 8;
  size_t png_stride = 1 + linebytes;

 // for(y = 0; y < h; ++y)
 // {
 //   size_t outindex = linebytes * y;
 //   size_t inindex = (1 + linebytes) * y; /*the extra filterbyte added to each row*/
 //   unsigned char filterType = in[inindex];

	//switch ((filterType << 8) + bytewidth) {
	//default:
	//	CERROR_TRY_RETURN(unfilterScanline(&out[outindex], &in[inindex + 1], prevline, bytewidth, filterType, linebytes));
	//	break;
	////case (PNG_FILTER_VALUE_NONE << 8) + 1: parng_predict_scanline_none_packed_8bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_NONE << 8) + 2: parng_predict_scanline_none_packed_16bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_NONE << 8) + 3: parng_predict_scanline_none_strided_24bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_NONE << 8) + 4: parng_predict_scanline_none_strided_32bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_SUB << 8) + 1: parng_predict_scanline_left_packed_8bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_SUB << 8) + 2: parng_predict_scanline_left_packed_16bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_SUB << 8) + 3: parng_predict_scanline_left_strided_24bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_SUB << 8) + 4: parng_predict_scanline_left_strided_32bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_UP << 8) + 1: parng_predict_scanline_up_packed_8bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_UP << 8) + 2: parng_predict_scanline_up_packed_16bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_UP << 8) + 3: parng_predict_scanline_up_strided_24bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_UP << 8) + 4: parng_predict_scanline_up_strided_32bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_AVG << 8) + 3: parng_predict_scanline_average_strided_24bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_AVG << 8) + 4: parng_predict_scanline_average_strided_32bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_PAETH << 8) + 3: parng_predict_scanline_paeth_strided_24bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	////case (PNG_FILTER_VALUE_PAETH << 8) + 4: parng_predict_scanline_paeth_strided_32bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
	//}

 //   prevline = &out[outindex];
 // }
  depng_filter_sse2((unsigned char*)in, h, bytewidth, png_stride);
  for (y = 0; y < h; ++y)
  {
	  memcpy(&out[linebytes * y], &in[1 + png_stride * y], linebytes);
  }
  //writeoutdata("decoded.dat", in, (w+1)*h*bytewidth);
  //writeoutdata("unfiltered.dat", out, w*h*bytewidth);

  return 0;
}