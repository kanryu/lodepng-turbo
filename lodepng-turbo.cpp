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

#include "lodepng.h"
#include "libdeflate/libdeflate.h"

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
