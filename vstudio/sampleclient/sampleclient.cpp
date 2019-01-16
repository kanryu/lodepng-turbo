// sampleclient.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "lodepng.h"
#include "depng.h"

///*Try the code, if it returns error, also return the error.*/
//#define CERROR_TRY_RETURN(call)\
//{\
//  unsigned error = call;\
//  if(error) return error;\
//}
//
//unsigned unfilterScanline(unsigned char* recon, const unsigned char* scanline, const unsigned char* precon,
//	size_t bytewidth, unsigned char filterType, size_t length);
//
//void writeoutdata(const char* filename, const unsigned char* pbuff, size_t size)
//{
//	FILE* f = fopen(filename, "wb");
//	fwrite(pbuff, 1, size, f);
//	fclose(f);
//}
//
//
//unsigned unfilter(unsigned char* out, const unsigned char* in, unsigned w, unsigned h, unsigned bpp, LodePNGState* state)
//{
//	/*
//	For PNG filter method 0
//	this function unfilters a single image (e.g. without interlacing this is called once, with Adam7 seven times)
//	out must have enough bytes allocated already, in must have the scanlines + 1 filtertype byte per scanline
//	w and h are image dimensions or dimensions of reduced image, bpp is bits per pixel
//	in and out are allowed to be the same memory address (but aren't the same size since in has the extra filter bytes)
//	*/
//
//	unsigned y;
//	unsigned char* prevline = 0;
//
//	/*bytewidth is used for filtering, is 1 when bpp < 8, number of bytes per pixel otherwise*/
//	size_t bytewidth = (bpp + 7) / 8;
//	size_t linebytes = (w * bpp + 7) / 8;
//
//	size_t png_stride = 1 + linebytes;
//
//	unsigned char* depng_buff = (unsigned char*)malloc(h*png_stride);
//
//	memcpy(depng_buff, in, h*png_stride);
//	//for (y = 0; y < h; ++y)
//	//{
//	//	size_t outindex = linebytes * y;
//	//	size_t inindex = (1 + linebytes) * y; /*the extra filterbyte added to each row*/
//	//	unsigned char filterType = in[inindex];
//
//	//	switch ((filterType << 8) + bytewidth) {
//	//	//default:
//	//	//	CERROR_TRY_RETURN(unfilterScanline(&out[outindex], &in[inindex + 1], prevline, bytewidth, filterType, linebytes));
//	//	//	break;
//	//		//case (PNG_FILTER_VALUE_NONE << 8) + 1: parng_predict_scanline_none_packed_8bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_NONE << 8) + 2: parng_predict_scanline_none_packed_16bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_NONE << 8) + 3: parng_predict_scanline_none_strided_24bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_NONE << 8) + 4: parng_predict_scanline_none_strided_32bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_SUB << 8) + 1: parng_predict_scanline_left_packed_8bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_SUB << 8) + 2: parng_predict_scanline_left_packed_16bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_SUB << 8) + 3: parng_predict_scanline_left_strided_24bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_SUB << 8) + 4: parng_predict_scanline_left_strided_32bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_UP << 8) + 1: parng_predict_scanline_up_packed_8bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_UP << 8) + 2: parng_predict_scanline_up_packed_16bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_UP << 8) + 3: parng_predict_scanline_up_strided_24bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_UP << 8) + 4: parng_predict_scanline_up_strided_32bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_AVG << 8) + 3: parng_predict_scanline_average_strided_24bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_AVG << 8) + 4: parng_predict_scanline_average_strided_32bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_PAETH << 8) + 3: parng_predict_scanline_paeth_strided_24bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//		//case (PNG_FILTER_VALUE_PAETH << 8) + 4: parng_predict_scanline_paeth_strided_32bpp(&out[outindex], &in[inindex + 1], prevline, bytewidth, linebytes); break;
//	//	}
//
//	//	prevline = &out[outindex];
//	//}
//	depng_filter_sse2((unsigned char*) in, h, bytewidth, png_stride);
//	for (y = 0; y < h; ++y)
//	{
//		memcpy(&out[linebytes * y], &in[1 + png_stride * y], linebytes);
//	}
//	free(depng_buff);
//	//writeoutdata("decoded.dat", in, (w + 1)*h*bytewidth);
//	writeoutdata("unfiltered3.dat", out, w*h*bytewidth);
//
//	return 0;
//}
int main(int argc, char** argv)
{
	unsigned error;
	unsigned char* image = nullptr;
	unsigned width, height;
	char* filename = "techno_x10_pngout.png";
	bool customed = true;
	if (argc > 1)
	{
		int ac = 1;
		if (std::string(argv[1]) == "-c") {
			customed = true;
			ac++;
			puts("customed");
		}
		filename = argv[ac++];
	}


	for (int i = 0; i < 1; i++) {
		error = lodepng_decode24_file(&image, &width, &height, filename);
		if (error) printf("error %u: %s\n", error, lodepng_error_text(error));
		free(image);
	}

	/*use image here*/
	printf("width:%d, height:%d", width, height);

	return 0;
}

