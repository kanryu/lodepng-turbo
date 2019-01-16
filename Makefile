#
# Use 'make help' to list available targets.
#
# Define V=1 to enable "verbose" mode, showing all executed commands.
#
# Define DECOMPRESSION_ONLY to omit all compression code, building a
# decompression-only library.  If doing this, you must also build a specific
# library target such as 'libdeflate.a', as the programs will no longer compile.
#
# Define DISABLE_GZIP to disable support for the gzip wrapper format.
#
# Define DISABLE_ZLIB to disable support for the zlib wrapper format.
#
##############################################################################

#### Common compiler flags.
#### Flags given here are not intended to be overridden, but you can add more
#### by defining CFLAGS in the environment or on the 'make' command line.

cc-option = $(shell if $(CC) $(1) -c -x c /dev/null -o /dev/null \
	      1>&2 2>/dev/null; then echo $(1); fi)

override CXXFLAGS :=							\
	$(CXXFLAGS) -O2 -fomit-frame-pointer -std=c++11 -I. -Ilibdeflate -Isimdtests/depng \
	-Wall -Wundef							\

# We don't define any CPPFLAGS, but support the user specifying it.

##############################################################################

PREFIX ?= /usr

NASM               := nasm
SOVERSION          := 0
STATIC_LIB_SUFFIX  := .a
SHARED_LIB_SUFFIX  := .so.$(SOVERSION)
SHARED_LIB_CXXFLAGS  := -fPIC
SHARED_LIB_LDFLAGS := -Wl,-soname=liblodepngturbo$(SHARED_LIB_SUFFIX)
PROG_SUFFIX        :=
PROG_CXXFLAGS        :=
HARD_LINKS         := 1

# Compiling for Windows with MinGW?
ifneq ($(findstring -mingw,$(shell $(MAKE) -dumpmachine 2>/dev/null)),)
    SOVERSION          :=
    STATIC_LIB_SUFFIX  := static.lib
    SHARED_LIB_SUFFIX  := .dll
    SHARED_LIB_CXXFLAGS  :=
    SHARED_LIB_LDFLAGS := -Wl,--out-implib,liblodepngturbo.lib
    PROG_SUFFIX        := .exe
    PROG_CXXFLAGS      := -static -municode
    HARD_LINKS         :=
    override CFLAGS    := $(CFLAGS) $(call cc-option,-Wno-pedantic-ms-format)

    # If AR was not already overridden, then derive it from $(CC).
    # Note that CC may take different forms, e.g. "cc", "gcc",
    # "x86_64-w64-mingw32-gcc", or "x86_64-w64-mingw32-gcc-6.3.1".
    # On Windows it may also have a .exe extension.
    ifeq ($(AR),ar)
        AR := $(shell echo $(CC) | \
                sed -E 's/g?cc(-?[0-9]+(\.[0-9]+)*)?(\.exe)?$$/ar\3/')
    endif
endif

##############################################################################

#### Quiet make is enabled by default.  Define V=1 to disable.

ifneq ($(findstring s,$(MAKEFLAGS)),s)
ifneq ($(V),1)
        QUIET_CC       = @echo '  CC      ' $@;
        QUIET_CCLD     = @echo '  CCLD    ' $@;
        QUIET_AR       = @echo '  AR      ' $@;
        QUIET_LN       = @echo '  LN      ' $@;
        QUIET_CP       = @echo '  CP      ' $@;
        QUIET_GEN      = @echo '  GEN     ' $@;
        QUIET_NASM     = @echo '  NASM    ' $@;
endif
endif

##############################################################################

COMMON_HEADERS := lodepng.h libdeflate/libdeflate.h
DEFAULT_TARGETS :=

#### Library

STATIC_LIB := liblodepngturbo$(STATIC_LIB_SUFFIX)
SHARED_LIB := liblodepngturbo$(SHARED_LIB_SUFFIX)

LIB_CXXFLAGS += $(CXXFLAGS) -fvisibility=hidden -D_ANSI_SOURCE
LIB_CXXFLAGS += -DLODEPNG_TURBO_COMPILE
LIB_CXXFLAGS += -DLODEPNG_NO_COMPILE_CRC
LIB_CXXFLAGS += -DLODEPNG_NO_COMPILE_ZLIB

LIB_HEADERS := lodepng.h libdeflate/libdeflate.h

LIB_SRC := lodepng.cpp lodepng-turbo.cpp

DEPNG_SRC := simdtests/depng/depng_ref.cpp simdtests/depng/depng_sse2.cpp

#LIB_LIBDEFLATE := -L./libdeflate -llibdeflatestatic
LIB_LIBDEFLATE := ./libdeflate/libdeflatestatic.lib

STATIC_LIB_OBJ := $(LIB_SRC:.cpp=.o) 
SHARED_LIB_OBJ := $(LIB_SRC:.cpp=.shlib.o)

STATIC_DEPNG_OBJ := $(DEPNG_SRC:.cpp=.o) 
SHARED_DEPNG_OBJ := $(DEPNG_SRC:.cpp=.shlib.o)

# Compile static library object files
$(STATIC_LIB_OBJ): %.o: %.cpp $(LIB_HEADERS) $(COMMON_HEADERS) .lib-cflags
	$(CXX) -o $@ -c $(CXXFLAGS) $(LIB_CXXFLAGS) $<

$(STATIC_DEPNG_OBJ): simdtests/depng/%.o: simdtests/depng/%.cpp $(LIB_HEADERS) $(COMMON_HEADERS) .lib-cflags
	echo $(DEPNG_SRC) $(STATIC_DEPNG_OBJ)
	$(CXX) -o $@ -c $(CXXFLAGS) $(LIB_CXXFLAGS) $<

# Compile shared library object files
$(SHARED_LIB_OBJ): %.shlib.o: %.cpp $(LIB_HEADERS) $(COMMON_HEADERS) .lib-cflags
	$(CXX) -o $@ -c $(CXXFLAGS) $(LIB_CXXFLAGS) \
		$(SHARED_LIB_CXXFLAGS) $<

$(SHARED_DEPNG_OBJ): simdtests/depng/%.shlib.o: simdtests/depng/%.cpp $(LIB_HEADERS) $(COMMON_HEADERS) .lib-cflags
	$(CXX) -o $@ -c $(CXXFLAGS) $(LIB_CXXFLAGS) \
		$(SHARED_LIB_CXXFLAGS) $<

# Create static library
$(STATIC_LIB):$(STATIC_LIB_OBJ) $(STATIC_DEPNG_OBJ) 
	$(QUIET_AR) $(AR) cr $@ $+

$(LIB_LIBDEFLATE):
	(cd libdeflate; $(MAKE))

DEFAULT_TARGETS += $(STATIC_LIB)

# Create shared library
$(SHARED_LIB):$(SHARED_LIB_OBJ) $(LIB_LIBDEFLATE) $(SHARED_DEPNG_OBJ) 
	$(QUIET_CCLD) $(CXX) -o $@ $(LDFLAGS) $(LIB_CXXFLAGS) \
		$(SHARED_LIB_LDFLAGS) -shared $+

# parng/prediction-x86_64-avx.o: parng/prediction-x86_64-avx.asm
# 	$(QUIET_CCLD) $(NASM) -fwin64 -o $@ $<

DEFAULT_TARGETS += $(SHARED_LIB)

ifdef SOVERSION
# Create the symlink liblodepngturbo.so => liblodepngturbo.so.$SOVERSION
liblodepngturbo.so:$(SHARED_LIB)
	$(QUIET_LN) ln -sf $+ $@
DEFAULT_TARGETS += liblodepngturbo.so
endif

# Rebuild if CC, LIB_CFLAGS, or CPPFLAGS changed
.lib-cflags: FORCE
	@flags='$(CC):$(LIB_CFLAGS):$(CPPFLAGS)'; \
	if [ "$$flags" != "`cat $@ 2>/dev/null`" ]; then \
		[ -e $@ ] && echo "Rebuilding library due to new compiler flags"; \
		echo "$$flags" > $@; \
	fi

##############################################################################

#### Programs

# PROG_CFLAGS += $(CFLAGS)		 \
# 	       -D_POSIX_C_SOURCE=200809L \
# 	       -D_FILE_OFFSET_BITS=64	 \
# 	       -DHAVE_CONFIG_H

# ALL_PROG_COMMON_HEADERS := programs/config.h \
# 			   programs/prog_util.h \
# 			   programs/test_util.h
# PROG_COMMON_SRC      := programs/prog_util.c \
# 			programs/tgetopt.c
# NONTEST_PROG_SRC     := programs/gzip.c
# TEST_PROG_COMMON_SRC := programs/test_util.c
# TEST_PROG_SRC        := programs/benchmark.c \
# 			programs/checksum.c \
# 			programs/test_checksums.c \
# 			programs/test_incomplete_codes.c \
# 			programs/test_slow_decompression.c
PROG_CXXFLAGS := $(LIB_CXXFLAGS)
PROG_COMMON_SRC      := lodepng_util.cpp
NONTEST_PROG_SRC     := pngdetail.cpp
TEST_PROG_COMMON_SRC := 
TEST_PROG_SRC        := lodepng_unittest.cpp

NONTEST_PROGRAMS := $(NONTEST_PROG_SRC:%.cpp=%$(PROG_SUFFIX))
DEFAULT_TARGETS  += $(NONTEST_PROGRAMS)
TEST_PROGRAMS    := $(TEST_PROG_SRC:%.cpp=%$(PROG_SUFFIX))

PROG_COMMON_OBJ      := $(PROG_COMMON_SRC:%.cpp=%.o)
NONTEST_PROG_OBJ     := $(NONTEST_PROG_SRC:%.cpp=%.o)
TEST_PROG_COMMON_OBJ := $(TEST_PROG_COMMON_SRC:%.cpp=%.o)
TEST_PROG_OBJ        := $(TEST_PROG_SRC:%.cpp=%.o)

ALL_PROG_OBJ	     := $(PROG_COMMON_OBJ) $(NONTEST_PROG_OBJ) \
			$(TEST_PROG_COMMON_OBJ) $(TEST_PROG_OBJ)

# # Generate autodetected configuration header
# programs/config.h:programs/detect.sh .prog-cflags
# 	$(QUIET_GEN) CC="$(CC)" CFLAGS="$(PROG_CFLAGS)" $< > $@

# Compile program object files
$(ALL_PROG_OBJ): %.o: %.cpp $(ALL_PROG_COMMON_HEADERS) $(COMMON_HEADERS) \
			.prog-cflags
	$(CXX) -o $@ -c $(CXXFLAGS) $(PROG_CXXFLAGS) $<

# Link the programs.
#
# Note: the test programs are not compiled by default.  One reason is that the
# test programs must be linked with zlib for doing comparisons.

$(NONTEST_PROGRAMS): %$(PROG_SUFFIX): %.o $(PROG_COMMON_OBJ) \
			$(STATIC_LIB) $(LIB_LIBDEFLATE)
	$(CXX) -o $@ $(LDFLAGS) $(PROG_CXXFLAGS) $+

$(TEST_PROGRAMS): %$(PROG_SUFFIX): %.o $(PROG_COMMON_OBJ) \
			$(TEST_PROG_COMMON_OBJ) $(STATIC_LIB) $(LIB_LIBDEFLATE)
	$(CXX) -o $@ $(LDFLAGS) $(PROG_CXXFLAGS) $+ -lz

# ifdef HARD_LINKS
# # Hard link gunzip to gzip
# gunzip$(PROG_SUFFIX):gzip$(PROG_SUFFIX)
# 	$(QUIET_LN) ln -f $< $@
# else
# # No hard links; copy gzip to gunzip
# gunzip$(PROG_SUFFIX):gzip$(PROG_SUFFIX)
# 	$(QUIET_CP) cp -f $< $@
# endif

# DEFAULT_TARGETS += gunzip$(PROG_SUFFIX)

# Rebuild if CC, PROG_CFLAGS, or CPPFLAGS changed
.prog-cflags: FORCE
	@flags='$(CC):$(PROG_CFLAGS):$(CPPFLAGS)'; \
	if [ "$$flags" != "`cat $@ 2>/dev/null`" ]; then \
		[ -e $@ ] && echo "Rebuilding programs due to new compiler flags"; \
		echo "$$flags" > $@; \
	fi

##############################################################################

all:$(DEFAULT_TARGETS)
	echo $(DEFAULT_TARGETS)

install:all
	install -Dm644 -t $(DESTDIR)$(PREFIX)/lib $(STATIC_LIB)
	install -Dm755 -t $(DESTDIR)$(PREFIX)/lib $(SHARED_LIB)
	ln -sf $(SHARED_LIB) $(DESTDIR)$(PREFIX)/lib/libdeflate.so
	install -Dm644 -t $(DESTDIR)$(PREFIX)/include libdeflate.h
	install -Dm755 gzip $(DESTDIR)$(PREFIX)/bin/libdeflate-gzip
	ln -f $(DESTDIR)$(PREFIX)/bin/libdeflate-gzip $(DESTDIR)$(PREFIX)/bin/libdeflate-gunzip

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/$(STATIC_LIB) \
		$(DESTDIR)$(PREFIX)/lib/$(SHARED_LIB) \
		$(DESTDIR)$(PREFIX)/lib/libdeflate.so \
		$(DESTDIR)$(PREFIX)/include/libdeflate.h \
		$(DESTDIR)$(PREFIX)/bin/libdeflate-gzip \
		$(DESTDIR)$(PREFIX)/bin/libdeflate-gunzip

test_programs:$(TEST_PROGRAMS)

help:
	@echo "Available targets:"
	@echo "------------------"
	@for target in $(DEFAULT_TARGETS) $(TEST_PROGRAMS); do \
		echo -e "$$target";		\
	done

clean:
	rm -f *.a *.dll *.exe *.exp *.so \
		*.o parng/*.o simdtests/depng/*.o \
		*.obj \
		*.dllobj \
		programs/*.o programs/*.obj \
		$(DEFAULT_TARGETS) $(TEST_PROGRAMS) programs/config.h \
		liblodepngturbo.lib liblodepngturbostatic.lib \
		.lib-cflags .prog-cflags

clean-all: clean
	(cd libdeflate;$(MAKE) clean)

realclean: clean
	rm -f tags cscope* run_tests.log

FORCE:

.PHONY: all install uninstall test_programs help clean realclean

.DEFAULT_GOAL = all
