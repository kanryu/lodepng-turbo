# lodepng-turbo
lodepng-turbo is a PNG image codec that uses SIMD instructions (MMX, SSE2, AVX2, NEON) to accelerate baseline PNG decompression on x86, x86-64, ARM systems.

## Background
lodepng-turbo is a PNG image codec that uses SIMD instructions (MMX, SSE2, AVX2, NEON) 
to accelerate baseline PNG decompression on x86, x86-64, ARM systems. 
On x86 and x86-64 systems. lodepng-turbo is twice as fast as libpng, but it may be slower elsewhere. 
With lodepng-turbo, its highly optimized libdeflate and parng prediction routines perform significantly better than lodepng and libpng.

lodepng-turbo implements both the traditional lodepng API and the less powerful but more direct TurboPNG API.

## How to build

Build lodepng-turbo with gcc/clang and GNU make.
For Windows, the built library can also be used under msvc.

```shell
$ make
```
When it is built, static libraries and shared libraries are created.

liblodepngturbo.dll(so) is a shared library and can be used alone. For Windows, use the import library (liblodepngturbo.lib).

liblodepngturbostatic.a (lib) is a static library and can be statically linked to your library. In this case libdeflatestatic.a also needs to be linked.

## for MSVC

Attention: When using libpng-turbo with msvc, libraries built with msys2 (g++) can not be used because of different naming conventions.

In addition to msvc, g++ of msys2 is necessary for building this library.
To build libdeflatestatic.lib in the msys2 environment first.

Open the solution file in the vstudio folder and build it on msvc. The setting of the project file is set to build the static library of libpng - turbo, so change the setting if you want dll.

## Acknowledgments

lodepng is a very easy-to-understand png library, which is the foundation of this library.

libdeflate is a very fast zlib compatible codec, and substantial decode processing is realized with this library.

parng is an ambitious project to realize high-speed png decoding by parallel processing, but it is not actually operated and is not maintained. I used the unfilter of this project.

## License

zlib

## Copyright

Copyright 2019 KATO Kanryu<k.kanryu@gmail.com>

