# Jpeg-XS Makefile
# (c) 2015 intoPIX S.A.
#
# Usage:
# - default build:
#     make
# - Disable "dev" options:
#     make DISABLE_PROFILE0=1 DISABLE_PROFILE3=1
#
# Options:
#   BUILD_WIN32 (default: 0)
#   BUILD_WIN64 (default: 1)


vpath %.c ../src:../programs:../programs/converters
OS := $(shell uname -o)

DELIVERY_PREFIX = jxs
INI_DIR      = ../configs
DEC_SRC      = tco_dec_main.c
ENC_SRC      = tco_enc_main.c
EDC_SRC      = tco_enc_dec_main.c

DEC_HEADERS  =


PROFILE_FLAGS=
ifeq ($(DISABLE_PROFILE0),1)
	PROFILE_FLAGS  += -DDISABLE_PROFILE0=1
endif
ifeq ($(DISABLE_PROFILE3),1)
	PROFILE_FLAGS  += -DDISABLE_PROFILE3=1
endif

INI_FILES      += $(INI_DIR)/jpeg_xs_main.ini
INI_FILES      += $(INI_DIR)/jpeg_xs_light.ini
INI_FILES      += $(INI_DIR)/jpeg_xs_light_subline.ini
INI_FILES      += $(INI_DIR)/jpeg_xs_high.ini

PKG_COM         = ../doc/intopix-license.txt ../doc/readme.txt ../doc/requirements.txt
ifeq ($(DISABLE_PROFILE0),1)
	#no need to transmit ini files anymore
	#PKG_COM			+= $(INI_FILES)
else
	#PKG_COM			+= $(shell ls ../configs/tc*.ini)
endif

ENC_HEADERS     = $(patsubst %.ini,%.h,$(INI_FILES))
EDC_HEADERS     = $(patsubst %.ini,%.h,$(INI_FILES))

ENC_NAME     = jxs_encoder
DEC_NAME     = jxs_decoder
EDC_NAME     = jxs_enc_dec
ENC_RES      = $(ENC_NAME).res
DEC_RES      = $(DEC_NAME).res
EDC_RES      = $(EDC_NAME).res

TCO_SRC = bitpacking.c budget.c buf_mgmt.c dwt53.c dwt137.c dwt22.c gcli.c image.c image_transforms.c sig_flags.c \
          int_funcs.c packing.c ra.c rct.c ict.c sig_mag.c tco_enc.c common.c sb_weighting.c precinct_budget.c precinct_budget_table.c \
          tco_dec.c pred.c gcli_budget.c data_budget.c version.c default_conf.c default_weights.c tco_conf.c gcli_methods.c \
          lvls_map.c quant.c precinct.c predbuffer.c rate_control.c sigbuffer.c budget_dump.c xs_markers.c
LIB_SRC = image_open.c v210.c yuv16.c yuv16p.c yuv8p.c uyvy8.c rgb16.c helpers.c config.c ini.c argb.c mono.c ppm.c pgx.c file_io.c file_sequence.c cmdline_options.c
PKG_CHK = ../src/version.h
TGZ_DEPS = $(PKG_COM)
ZIP_DEPS = $(PKG_COM)
BUILD_ID = 0x$(shell hg id|sed 's/[\+ ].*//')
CFLAGS_COMMON = -std=c99 -pedantic -Wall -I../inc -Werror -DINTERNAL_CODE=1 -DBUILD_ID=$(BUILD_ID) $(PROFILE_FLAGS)
LDFLAGS+=$(GM_LDFLAGS)

# Linux32
CFLAGS_LIN32   = $(CFLAGS) -m32 -lm $(CFLAGS_COMMON) $(GM_CFLAGS_LIN32)
LDFLAGS_LIN32  = $(LDFLAGS) -m32 -lm
ifdef DEBUG
  CFLAGS_LIN32  += -ggdb3 -O0
else
  CFLAGS_LIN32 += -O3
  LDFLAGS_LIN32 += -s
endif
TCO_OBJ_LIN32  = $(patsubst %.c, obj/linux32/%.o, $(TCO_SRC))
LIB_OBJ_LIN32  = $(patsubst %.c, obj/linux32/%.o, $(LIB_SRC))
ENC_OBJ_LIN32  = $(patsubst %.c, obj/linux32/%.o, $(ENC_SRC))
DEC_OBJ_LIN32  = $(patsubst %.c, obj/linux32/%.o, $(DEC_SRC))
EDC_OBJ_LIN32  = $(patsubst %.c, obj/linux32/%.o, $(EDC_SRC))
TICOLIB_LIN32  = bin/linux32/libjxs.so
TCUTLIB_LIN32  = bin/linux32/libjxs_utils.so
ENCDEC_LIN32   = bin/linux32/$(EDC_NAME)
ENCODER_LIN32  = bin/linux32/$(ENC_NAME)
DECODER_LIN32  = bin/linux32/$(DEC_NAME)
CC_LIN32       = $(CC)

# Linux64
CFLAGS_LIN64   = $(CFLAGS) -fPIC $(CFLAGS_COMMON) $(GM_CFLAGS_LIN64)
LDFLAGS_LIN64  = $(LDFLAGS) -lm
ifdef DEBUG
  CFLAGS_LIN64  += -ggdb3 -O0
else
  CFLAGS_LIN64 += -O3
  LDFLAGS_LIN64 += -s
endif
TCO_OBJ_LIN64  = $(patsubst %.c, obj/linux64/%.o, $(TCO_SRC))
LIB_OBJ_LIN64  = $(patsubst %.c, obj/linux64/%.o, $(LIB_SRC))
ENC_OBJ_LIN64  = $(patsubst %.c, obj/linux64/%.o, $(ENC_SRC))
DEC_OBJ_LIN64  = $(patsubst %.c, obj/linux64/%.o, $(DEC_SRC))
EDC_OBJ_LIN64  = $(patsubst %.c, obj/linux64/%.o, $(EDC_SRC))
TICOLIB_LIN64  = bin/linux64/libjxs.so
TCUTLIB_LIN64  = bin/linux64/libjxs_utils.so
ENCDEC_LIN64   = bin/linux64/$(EDC_NAME)
ENCODER_LIN64  = bin/linux64/$(ENC_NAME)
DECODER_LIN64  = bin/linux64/$(DEC_NAME)
CC_LIN64       = $(CC)
TGZ_DEPS      += $(ENCODER_LIN64) $(DECODER_LIN64) $(ENCDEC_LIN64)

# Win32
BUILD_WIN32   ?= 0
PREFIX_W32     = i686-w64-mingw32
CFLAGS_W32     = $(CFLAGS_COMMON) $(GM_CFLAGS_WIN32)
LDFLAGS_W32    = -lws2_32
ifdef DEBUG
  CFLAGS_W32  += -g
else
  CFLAGS_W32 += -O3
  LDFLAGS_W32 += -s
endif
TCO_OBJ_W32    = $(patsubst %.c, obj/win32/%.o, $(TCO_SRC)) obj/win32/jxs.res
LIB_OBJ_W32    = $(patsubst %.c, obj/win32/%.o, $(LIB_SRC)) obj/win32/jxs_utils.res
ENC_OBJ_W32    = $(patsubst %.c, obj/win32/%.o, $(ENC_SRC)) obj/win32/$(ENC_RES)
DEC_OBJ_W32    = $(patsubst %.c, obj/win32/%.o, $(DEC_SRC)) obj/win32/$(DEC_RES)
EDC_OBJ_W32    = $(patsubst %.c, obj/win32/%.o, $(EDC_SRC)) obj/win32/$(EDC_RES)
TICOLIB_W32    = bin/win32/jxs.dll
TCUTLIB_W32    = bin/win32/jxs_utils.dll
ENCDEC_W32     = bin/win32/$(EDC_NAME).exe
ENCODER_W32    = bin/win32/$(ENC_NAME).exe
DECODER_W32    = bin/win32/$(DEC_NAME).exe
ifeq ($(OS),GNU/Linux)
CC_W32         = $(PREFIX_W32)-gcc
WINDRES_W32    = $(PREFIX_W32)-windres
STRIP_W32      = $(PREFIX_W32)-strip
else
CC_W32         = gcc
WINDRES_W32    = windres
STRIP_W32      = strip
endif
ifeq ($(BUILD_WIN32),1)
	ZIP_DEPS   += $(ENCODER_W32) $(DECODER_W32) $(ENCDEC_W32) $(TICOLIB_W32) $(TCUTLIB_W32)
endif
TICOLIB_STATIC_W32 = obj/win32/libjxs.a
TCUTLIB_STATIC_W32 = obj/win32/libjxs_utils.a

# Win64
BUILD_WIN64   ?= 1
PREFIX_W64     = x86_64-w64-mingw32
CFLAGS_W64     = $(CFLAGS_COMMON) $(GM_CFLAGS_WIN64)
LDFLAGS_W64    = -lws2_32 $(GM_LDFLAGS)
ifdef DEBUG
  CFLAGS_W64  += -g
else
  CFLAGS_W64  += -O3
  LDFLAGS_W64 += -s
endif
TCO_OBJ_W64    = $(patsubst %.c, obj/win64/%.o, $(TCO_SRC)) obj/win64/jxs.res
LIB_OBJ_W64    = $(patsubst %.c, obj/win64/%.o, $(LIB_SRC)) obj/win64/jxs_utils.res
ENC_OBJ_W64    = $(patsubst %.c, obj/win64/%.o, $(ENC_SRC)) obj/win64/$(ENC_RES)
DEC_OBJ_W64    = $(patsubst %.c, obj/win64/%.o, $(DEC_SRC)) obj/win64/$(DEC_RES)
EDC_OBJ_W64    = $(patsubst %.c, obj/win64/%.o, $(EDC_SRC)) obj/win64/$(EDC_RES)
TICOLIB_W64    = bin/win64/jxs.dll
TCUTLIB_W64    = bin/win64/jxs_utils.dll
ENCDEC_W64     = bin/win64/$(EDC_NAME).exe
ENCODER_W64    = bin/win64/$(ENC_NAME).exe
DECODER_W64    = bin/win64/$(DEC_NAME).exe
ifeq ($(OS),GNU/Linux)
CC_W64         = $(PREFIX_W64)-gcc
WINDRES_W64    = $(PREFIX_W64)-windres
STRIP_W64      = $(PREFIX_W64)-strip
else
CC_W64         = gcc
WINDRES_W64    = windres
STRIP_W64      = strip
endif
ifeq ($(BUILD_WIN64),1)
	ZIP_DEPS   += $(ENCODER_W64) $(DECODER_W64) $(ENCDEC_W64) $(TICOLIB_W64) $(TCUTLIB_W64)
endif

# Common rules

VERSION = $(shell sed '/TICO_VERSION_STR/!d;s/.* "//;s/".*//' ../src/version.h)
TL_VER  = 690e82ab091a

ALL_REQ = $(ENC_SRC) $(DEC_SRC)

ifeq ($(OS),GNU/Linux)
	ALL_REQ    += $(ENCODER_LIN64) $(DECODER_LIN64) $(ENCDEC_LIN64)
#	ALL_REQ    += $(ENCODER_W64) $(DECODER_W64) $(ENCDEC_W64)
#	ALL_REQ    += $(ENCODER_W32) $(DECODER_W32) $(ENCDEC_W32)
	PLATFORM    = linux64
else ifeq ($(OS),Msys)
	CC_W32      = gcc
	WINDRES_W32 = windres
	STRIP_W32   = strip
	ALL_REQ    += $(ENCODER_W32) $(DECODER_W32) $(ENCDEC_W32)
	PLATFORM    = win32
endif

all:	$(ALL_REQ)

linux64:	$(ENC_SRC) $(DEC_SRC) $(ENCODER_LIN64) $(DECODER_LIN64) $(ENCDEC_LIN64)

linux32:	$(ENC_SRC) $(DEC_SRC) $(ENCODER_LIN32) $(DECODER_LIN32) $(ENCDEC_LIN32)

win64:	$(ENC_SRC) $(DEC_SRC) $(ENCODER_W64) $(DECODER_W64) $(ENCDEC_W64)

win32:	$(ENC_SRC) $(DEC_SRC) $(ENCODER_W32) $(DECODER_W32) $(ENCDEC_W32)

clean:
	rm -rf bin obj ../configs/*.h tmp

# Linux64

obj/linux64:
	mkdir -p $@

bin/linux64:
	mkdir -p $@

obj/linux64/%.o:	%.c
	$(CC_LIN64) $(CFLAGS_LIN64) -c -o $@ $<

$(ENC_OBJ_LIN64):	| obj/linux64 bin/linux64
$(ENC_OBJ_LIN64):	$(ENC_HEADERS)
$(DEC_OBJ_LIN64):	$(DEC_HEADERS)
$(EDC_OBJ_LIN64):	$(EDC_HEADERS)

$(TICOLIB_LIN64):	$(TCO_OBJ_LIN64)
	$(CC_LIN64) -o $@ -shared $^ $(LDFLAGS_LIN64)

$(TCUTLIB_LIN64):	$(LIB_OBJ_LIN64)
	$(CC_LIN64) -o $@ -shared $^ $(LDFLAGS_LIN64)

$(ENCODER_LIN64):	$(ENC_OBJ_LIN64) $(TCUTLIB_LIN64) $(TICOLIB_LIN64)
	$(CC_LIN64) -o $@ $< -L bin/linux64 -ljxs -ljxs_utils -Wl,-R,'$$ORIGIN' $(LDFLAGS_LIN64)

$(DECODER_LIN64):	$(DEC_OBJ_LIN64) $(TCUTLIB_LIN64) $(TICOLIB_LIN64)
	$(CC_LIN64) -o $@ $< -L bin/linux64 -ljxs -ljxs_utils -Wl,-R,'$$ORIGIN' $(LDFLAGS_LIN64)

$(ENCDEC_LIN64):	$(EDC_OBJ_LIN64) $(TCUTLIB_LIN64) $(TICOLIB_LIN64)
	$(CC_LIN64) -o $@ $< -L bin/linux64 -ljxs -ljxs_utils -Wl,-R,'$$ORIGIN' $(LDFLAGS_LIN64)


# Linux32

obj/linux32:
	mkdir -p $@

bin/linux32:
	mkdir -p $@

obj/linux32/%.o:	%.c
	$(CC_LIN32) $(CFLAGS_LIN32) -c -o $@ $<

$(ENC_OBJ_LIN32):	| obj/linux32 bin/linux32
$(ENC_OBJ_LIN32):	$(ENC_HEADERS)
$(DEC_OBJ_LIN32):	$(DEC_HEADERS)
$(EDC_OBJ_LIN32):	$(EDC_HEADERS)

$(TICOLIB_LIN32):	$(TCO_OBJ_LIN32)
	$(CC_LIN32) -o $@ -shared $^ $(LDFLAGS_LIN32)

$(TCUTLIB_LIN32):	$(LIB_OBJ_LIN32)
	$(CC_LIN32) -o $@ -shared $^ $(LDFLAGS_LIN32)

$(ENCODER_LIN32):	$(ENC_OBJ_LIN32) $(TCUTLIB_LIN32) $(TICOLIB_LIN32)
	$(CC_LIN32) -o $@ $< -L bin/linux32 -ljxs -ljxs_utils -Wl,-R,'$$ORIGIN' $(LDFLAGS_LIN32)

$(DECODER_LIN32):	$(DEC_OBJ_LIN32) $(TCUTLIB_LIN32) $(TICOLIB_LIN32)
	$(CC_LIN32) -o $@ $< -L bin/linux32 -ljxs -ljxs_utils -Wl,-R,'$$ORIGIN' $(LDFLAGS_LIN32)

$(ENCDEC_LIN32):	$(EDC_OBJ_LIN32) $(TCUTLIB_LIN32) $(TICOLIB_LIN32)
	$(CC_LIN32) -o $@ $< -L bin/linux32 -ljxs -ljxs_utils -Wl,-R,'$$ORIGIN' $(LDFLAGS_LIN32)


# Win32

obj/win32:
	mkdir -p $@

bin/win32:
	mkdir -p $@

obj/win32/%.def:	bin/win32/%.dll

obj/win32/%.res:	../programs/%.rc ../src/version.h
	$(WINDRES_W32) -I ../src -i $< --input-format=rc -O coff -o $@

obj/win32/%.o:	%.c
	$(CC_W32) $(CFLAGS_W32) -c -o $@ $<

$(ENC_OBJ_W32):	| obj/win32 bin/win32
$(ENC_OBJ_W32):	$(ENC_HEADERS)
$(DEC_OBJ_W32):	$(DEC_HEADERS)
$(EDC_OBJ_W32):	$(EDC_HEADERS)

$(TICOLIB_STATIC_W32):	$(TCO_OBJ_W32)
	ar rcs $@ $^

$(TCUTLIB_STATIC_W32):	$(LIB_OBJ_W32)
	ar rcs $@ $^

$(TICOLIB_W32):	$(TCO_OBJ_W32)
	$(CC_W32) $(CFLAGS_W32) $^ -lpthread -pthread -lgdi32 -shared -o $@ -Wl,--output-def,obj/win32/jxs.def,--out-implib,obj/win32/jxs.a $(LDFLAGS_W32)
ifndef DEBUG
	$(STRIP_W32) $@; upx $@; touch $@
endif

$(TCUTLIB_W32):	$(LIB_OBJ_W32) $(TICOLIB_W32)
	$(CC_W32) $(CFLAGS_W32) $^ -lpthread -pthread -lgdi32 -shared -o $@ -Wl,--output-def,obj/win32/jxs_utils.def,--out-implib,obj/win32/jxs_utils.a $(LDFLAGS_W32)
ifndef DEBUG
	$(STRIP_W32) $@; upx $@; touch $@
endif

$(ENCODER_W32):	$(ENC_OBJ_W32) $(TCUTLIB_W32) $(TICOLIB_W32)
	$(CC_W32) -o $@ $^ -Wl,--large-address-aware $(LDFLAGS_W32)
ifndef DEBUG
	$(STRIP_W32) $@; upx $@; touch $@
endif

$(DECODER_W32):	$(DEC_OBJ_W32) $(TCUTLIB_W32) $(TICOLIB_W32)
	$(CC_W32) -o $@ $^ -Wl,--large-address-aware $(LDFLAGS_W32)
ifndef DEBUG
	$(STRIP_W32) $@; upx $@; touch $@
endif

$(ENCDEC_W32):	$(EDC_OBJ_W32) $(TCUTLIB_W32) $(TICOLIB_W32)
	$(CC_W32) -o $@ $^ -Wl,--large-address-aware $(LDFLAGS_W32)
ifndef DEBUG
	$(STRIP_W32) $@; upx $@; touch $@
endif

# Win64

obj/win64:
	mkdir -p $@

bin/win64:
	mkdir -p $@

obj/win64/%.def:   bin/win64/%.dll

obj/win64/%.res:   ../programs/%.rc ../src/version.h
	$(WINDRES_W64) -I ../src -i $< --input-format=rc -O coff -o $@

obj/win64/%.o:	%.c
	$(CC_W64) $(CFLAGS_W64) -c -o $@ $<

$(ENC_OBJ_W64):	| obj/win64 bin/win64
$(ENC_OBJ_W64): $(ENC_HEADERS)
$(DEC_OBJ_W64): $(DEC_HEADERS)
$(EDC_OBJ_W64): $(EDC_HEADERS)

$(TICOLIB_W64):	$(TCO_OBJ_W64)
	$(CC_W64) $(CFLAGS_W64) $^ -lpthread -pthread -lgdi32 -shared -o $@ -Wl,--output-def,obj/win64/jxs.def,--out-implib,obj/win64/jxs.a $(LDFLAGS_W64)
	$(STRIP_W64) $@; upx $@; touch $@

$(TCUTLIB_W64):	$(LIB_OBJ_W64) $(TICOLIB_W64)
	$(CC_W64) $(CFLAGS_W64) $^ -lpthread -pthread -lgdi32 -shared -o $@ -Wl,--output-def,obj/win64/jxs_utils.def,--out-implib,obj/win64/jxs_utils.a $(LDFLAGS_W64)
ifndef DEBUG
	$(STRIP_W64) $@; upx $@; touch $@
endif

$(ENCODER_W64):	$(ENC_OBJ_W64) $(TCUTLIB_W64) $(TICOLIB_W64)
	$(CC_W64) -o $@ $^ $(LDFLAGS_W64)
ifndef DEBUG
	$(STRIP_W64) $@; upx $@; touch $@
endif

$(DECODER_W64):	$(DEC_OBJ_W64) $(TCUTLIB_W64) $(TICOLIB_W64)
	$(CC_W64) -o $@ $^ $(LDFLAGS_W64)
ifndef DEBUG
	$(STRIP_W64) $@; upx $@; touch $@
endif

$(ENCDEC_W64):	$(EDC_OBJ_W64) $(TCUTLIB_W64) $(TICOLIB_W64)
	$(CC_W64) -o $@ $^ $(LDFLAGS_W64)
ifndef DEBUG
	$(STRIP_W64) $@; upx $@; touch $@
endif

# Configurations TC1/TC2
all_config_h: $(ENC_HEADERS) $(EDC_HEADERS)

$(INI_DIR)/%.h: $(INI_DIR)/%.ini ini_to_cconf.py
	python ini_to_cconf.py $< $@


#small header_parser tool (dev feature)
HDR_PARSER_SRC = ../src/tco_hdr.c ../src/tco_conf.c ../src/default_conf.c ../src/image.c

ifeq ($(OS),GNU/Linux)
tco_hdr_parser: $(HDR_PARSER_SRC) ../inc/tco.h ../src/tco_conf.h
	$(CC) -DCOMPILE_TCO_HDR_PARSER -o ./tco_hdr_parser $(HDR_PARSER_SRC) -I../inc -lm
else
tco_hdr_parser: $(HDR_PARSER_SRC) ../inc/tco.h ../src/tco_conf.h
	$(CC) -DCOMPILE_TCO_HDR_PARSER -o ./tco_hdr_parser $(HDR_PARSER_SRC) -I../inc $(LDFLAGS_W32)
endif


# Validation
VALID_LIB         := ipx_valid_lib
VALID_LIB_VERSION := 53e56c6f1e9cef5864815245bfde284fd52e9714

$(VALID_LIB):
	hg clone ssh://hg_rd@10.128.0.246/sw/ipx_valid_lib -r $(VALID_LIB_VERSION)

#BUILD_DIR := build_$(shell date +%Y%m%d_%H%M%S)_$(shell hg id -n)_$(PLATFORM)
BUILD_DIR := build_$(shell date +%Y%m%d_%H%M%S)_$(PLATFORM)

test: clean all _test

_test: $(VALID_LIB)
	mkdir -p $(BUILD_DIR)/bin
	cp -pR bin/$(PLATFORM) $(BUILD_DIR)/bin/
	PYTHONPATH=$(VALID_LIB) python ../tests/run_all.py $(BUILD_DIR)
	@echo "Test reports are in: $(BUILD_DIR)/test_reports"

delivery_and_test:	delivery test



# Delivery
DELIVERY_NAME = $(DELIVERY_PREFIX)_$(VERSION)


### Linux platform ###
delivery:	deliveries/$(DELIVERY_PREFIX)_latest_release.zip deliveries/$(DELIVERY_PREFIX)_latest_release.tar.gz

deliveries:
	mkdir deliveries

deliveries/$(DELIVERY_PREFIX)_latest_release.zip:	$(PKG_CHK) $(ZIP_DEPS) | deliveries
	for f in libwinpthread-* libbz2-* libjpeg-* libpng16-* libtiff-* zlib* libjasper-* liblcms2-* libstdc++-* libgcc_* libxml2-* libwebp-* iconv; do \
		if [ -d bin/win32 ]; then \
			ln -sf /usr/$(PREFIX_W32)/sys-root/mingw/bin/$${f}.dll bin/win32; \
		fi; \
		if [ -d bin/win64 ]; then \
			ln -sf /usr/$(PREFIX_W64)/sys-root/mingw/bin/$${f}.dll bin/win64; \
		fi; \
	done
	if [ -d bin/win32 ]; then \
		for f in libgcc_s_sjlj-1; do \
			ln -sf /usr/$(PREFIX_W32)/sys-root/mingw/bin/$${f}.dll bin/win32; \
		done \
	fi
	rm -f deliveries/$(DELIVERY_NAME).zip
	7z a -l -mx deliveries/$(DELIVERY_NAME).zip bin/win* $(PKG_COM)
	ln -sf $(DELIVERY_NAME).zip deliveries/$(DELIVERY_PREFIX)_latest_release.zip

deliveries/$(DELIVERY_PREFIX)_latest_release.tar.gz:	$(PKG_CHK) $(TGZ_DEPS) | deliveries
	tar czvf deliveries/$(DELIVERY_NAME).tar.gz bin/linux* $(PKG_COM)
	ln -sf $(DELIVERY_NAME).tar.gz deliveries/$(DELIVERY_PREFIX)_latest_release.tar.gz


#Remote build (from windows or linux)
REMOTE_BUILD_USER=intopix
REMOTE_BUILD_SERVER=10.128.0.175
#ipx-build
REMOTE_ADDRESS=$(REMOTE_BUILD_USER)@$(REMOTE_BUILD_SERVER)

#check you have the tools to remote build
SSH-exists:
	@which ssh > /dev/null 2>&1 || (echo "Please install ssh" && exit 1)
RSYNC-exists:
	@which rsync > /dev/null 2>&1 || (echo "Please install rsync" && exit 1)

remote_build_path.txt: | SSH-exists
	ssh $(REMOTE_ADDRESS) "mktemp -d" > remote_build_path.txt

remote_build_path_delete: SSH-exists
	ssh $(REMOTE_ADDRESS) "rm -R `cat remote_build_path.txt`" && rm remote_build_path.txt

remote_build_rsync_root: RSYNC-exists remote_build_path.txt
	rsync -avz ../ $(REMOTE_ADDRESS):`cat remote_build_path.txt`

remote_build_make_delivery: SSH-exists remote_build_rsync_root
	ssh $(REMOTE_ADDRESS) "cd `cat remote_build_path.txt`/build; DISABLE_PROFILE0=$(DISABLE_PROFILE0) DISABLE_PROFILE3=$(DISABLE_PROFILE3) EPOCH_MAX=$(EPOCH_MAX) make delivery"

remote_build_download_deliveries: RSYNC-exists remote_build_make_delivery
	rsync -avzL $(REMOTE_ADDRESS):`cat remote_build_path.txt`/build/deliveries ./

remote_build_unzip_win_delivery: remote_build_download_deliveries
	 unzip -o -d . deliveries/$(DELIVERY_NAME).zip

#this rule creates a bin/win64 directory
#with all executable and mingw everything needed
all_remote : delivery_remote
delivery_remote: remote_build_unzip_win_delivery remote_build_path_delete

include rules.mk
