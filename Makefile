LIBO_BASENAMES = osc_match  osc_bundle_s osc_bundle_u osc_bundle_iterator_s osc_bundle_iterator_u osc_error osc_mem osc_message_s osc_message_u osc_message_iterator_s osc_message_iterator_u osc_atom_s osc_atom_u osc_array osc_atom_array_s osc_atom_array_u osc_expr osc_vtable osc_dispatch osc_hashtab osc_linkedlist osc_util osc_rset osc_query osc_strfmt osc_expr_rec osc_typetag contrib/strptime osc_timetag osc_serial

LIBO_CFILES = $(foreach F, $(LIBO_BASENAMES), $(F).c)
LIBO_HFILES = $(foreach F, $(LIBO_BASENAMES), $(F).h) osc.h
LIBO_OFILES = $(foreach F, $(LIBO_BASENAMES), $(F).o)

LIBO_SCANNER_BASENAMES = osc_scanner osc_expr_scanner osc_legacy_scanner
LIBO_SCANNER_LFILES = $(foreach OBJ, $(LIBO_SCANNER_BASENAMES), $(OBJ).l)
LIBO_SCANNER_CFILES = $(foreach OBJ, $(LIBO_SCANNER_BASENAMES), $(OBJ).c)
LIBO_SCANNER_HFILES = $(foreach OBJ, $(LIBO_SCANNER_BASENAMES), $(OBJ).h)
LIBO_SCANNER_OBJECTS = $(foreach OBJ, $(LIBO_SCANNER_BASENAMES), $(OBJ).o)

LIBO_PARSER_BASENAMES = osc_parser osc_expr_parser osc_legacy_parser
LIBO_PARSER_YFILES = $(foreach OBJ, $(LIBO_PARSER_BASENAMES), $(OBJ).y)
LIBO_PARSER_CFILES = $(foreach OBJ, $(LIBO_PARSER_BASENAMES), $(OBJ).c)
LIBO_PARSER_HFILES = $(foreach OBJ, $(LIBO_PARSER_BASENAMES), $(OBJ).h)
LIBO_PARSER_OBJECTS = $(foreach OBJ, $(LIBO_PARSER_BASENAMES), $(OBJ).o)

LIBO_OBJECTS = $(LIBO_OFILES) $(LIBO_SCANNER_OBJECTS) $(LIBO_PARSER_OBJECTS)

RELEASE-CFLAGS += -Wall -Wno-trigraphs -fno-strict-aliasing -O3 -funroll-loops -std=c99
DEBUG-CFLAGS += -Wall -Wno-trigraphs -fno-strict-aliasing -O0 -g -std=c99

# option to set flex location in case of brew install
# currently set to /usr/local/lib for typical make install
FLEX_LIB_FOLDER = /usr/local/lib
#FLEX_LIB_FOLDER = /usr/local/Cellar/flex/2.6.4/lib

MAC-CFLAGS = -arch x86_64 -arch arm64 -mmacosx-version-min=10.10 -std=c99
ARM-CFLAGS = -arch armv7 -arch armv7s
WIN-CFLAGS = -DWIN_VERSION -DWIN_EXT_VERSION -U__STRICT_ANSI__ -U__ANSI_SOURCE
#WIN64-CFLAGS = -DWIN_VERSION -DWIN_EXT_VERSION -U__STRICT_ANSI__ -U__ANSI_SOURCE -fPIC

MAC-INCLUDES = -I/usr/include
ARM-INCLUDES = -I/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk/usr/include -I/usr/include
WIN-INCLUDES = -I\usr\i686-w64-mingw32\sys-root\mingw\include
WIN64-INCLUDES = -I\usr\x86_64-w64-mingw32\sys-root\mingw\include

all: CFLAGS += $(RELEASE-CFLAGS)
all: CFLAGS += $(MAC-CFLAGS)
all: CC = clang
all: I = $(MAC-INCLUDES)
all: libs
# ifeq ($(NOM1), true)
# else
# all: CFLAGS += -arch arm64
# endif
all: STATIC-LINK = libtool -static -o libo.a $(LIBO_OBJECTS) $(FLEX_LIB_FOLDER)/libfl.a
all: DYNAMIC-LINK = clang -dynamiclib $(MAC-CFLAGS) -single_module -compatibility_version 1 -current_version 1 -o libo.dylib $(LIBO_OBJECTS)

arm: CFLAGS += $(RELEASE-CFLAGS)
arm: CFLAGS += $(ARM-CFLAGS)
arm: CC = clang
arm: I = $(ARM-INCLUDES)
arm: libs
arm: STATIC-LINK = libtool -static -o libo.a $(LIBO_OBJECTS) /usr/local/lib/libfl.a
arm: DYNAMIC-LINK = clang -dynamiclib $(MAC-CFLAGS) -single_module -compatibility_version 1 -current_version 1 -o libo.dylib $(LIBO_OBJECTS)

debug: CFLAGS += $(DEBUG-CFLAGS)
debug: CFLAGS += $(MAC-CFLAGS)
debug: CC = clang
debug: I = $(MAC-INCLUDES)
debug: libs
debug: STATIC-LINK = libtool -static -o libo.a $(LIBO_OBJECTS) /usr/local/lib/libfl.a

win: CFLAGS += $(RELEASE-CFLAGS)
win: CFLAGS += $(WIN-CFLAGS)
win: CC = i686-w64-mingw32-gcc
win: I = $(WIN-INCLUDES)
win: libs
win: STATIC-LINK = ar cru libo.a $(LIBO_OBJECTS) /usr/lib/libfl.a
win: PLACE = rm -f libs/i686/*.a; mkdir -p libs/i686; cp libo.a libs/i686

win64: CFLAGS += $(RELEASE-CFLAGS)
win64: CFLAGS += $(WIN-CFLAGS)
win64: CC = x86_64-w64-mingw32-gcc
win64: I = $(WIN64-INCLUDES)
win64: libs
win64: STATIC-LINK = x86_64-w64-mingw32-gcc-ar cru libo.a $(LIBO_OBJECTS) /usr/lib/libfl.a
win64: DYNAMIC-LINK = x86_64-w64-mingw32-gcc *.o contrib/*.o -shared -o libo.dylib

linux: CC = clang
linux: CFLAGS += -std=c99 -fPIC -DLINUX_VERSION -D_XOPEN_SOURCE=500
#linux: LIBTOOL = libtool -static -o libo.a $(LIBO_OBJECTS) /usr/lib/libfl.a
linux: libs
linux: STATIC-LINK = ar rcs libo.a $(LIBO_OBJECTS) /usr/lib/libfl.so
linux: DYNAMIC-LINK = clang *.o -shared -o libo.so

ifeq ($(STATIC), false)
else
libs: libo.a
endif
ifeq ($(DYNAMIC), true)
libs: libo.dylib
endif

swig: CC = clang
swig: CFLAGS += -std=c99
swig: libo.i libo_wrap.c libo.py _libo.so

libo_wrap.c libo.py:
	swig -python -py3 libo.i

_libo.so: libo.py libo_wrap.c setup.py
	python setup.py build_ext --inplace

node: odot_wrap.cxx build/Release/odot.node

odot_wrap.cxx:
	swig -javascript -node -c++ odot.i

build/Release/odot.node:
	node-gyp rebuild


# libo.i:
# 	$(shell echo "%module libo" > libo.i; echo "%{" >> libo.i; for f in $(LIBO_HFILES) osc_scanner.h osc_expr_scanner.h osc_parser.h osc_expr_parser.h; do echo "#include \""$$f"\"" >> libo.i; done; echo "%}" >> libo.i; for f in $(LIBO_HFILES); do echo "%include \""$$f"\"" >> libo.i; done;)

# libo_wrap.c: $(LIBO_CFILES) $(LIBO_HFILES) $(LIBO_SCANNER_CFILES) $(LIBO_SCANNER_HFILES)
# 	swig -Wall -python libo.i

# libo_wrap.o: $(LIBO_OBJECTS)
# 	$(CC) -c $(shell python-config --cflags) libo_wrap.c

# _libo.so: libo_wrap.o
# 	$(CC) -bundle -L/Users/john/anaconda/envs/py2.7/lib/python2.7/config -ldl -framework CoreFoundation -lpython2.7 $(LIBO_OBJECTS) libo_wrap.o -o _libo.so
#	$(CC) -bundle -L/opt/local/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/config -ldl -framework CoreFoundation -lpython2.7 $(LIBO_OBJECTS) libo_wrap.o -o _libo.so

libo.a: $(LIBO_CFILES) $(LIBO_HFILES) $(LIBO_SCANNER_CFILES) $(LIBO_PARSER_CFILES) $(LIBO_OBJECTS)
	rm -f libo.a
	$(STATIC-LINK)
	$(PLACE)

libo.dylib: $(LIBO_CFILES) $(LIBO_HFILES) $(LIBO_SCANNER_CFILES) $(LIBO_PARSER_CFILES) $(LIBO_OBJECTS)
	rm -f libo.dylib
	$(DYNAMIC-LINK)

%.o: %.c
	$(CC) $(CFLAGS) $(I) -c -o $(basename $@).o $(basename $@).c

%_scanner.c: %_scanner.l %_parser.c
	flex -o $(basename $@).c --prefix=$(basename $@)_ --header-file=$(basename $@).h $(basename $@).l

%_parser.c: %_parser.y
	bison -p $(basename $@)_ -d -v --report=itemset -o $(basename $@).c $(basename $@).y

.PHONY: doc
doc:
	cd doc && doxygen Doxyfile

.PHONY: test test-clean
test:
	$(MAKE) -C test

test-clean:
	$(MAKE) -C test clean

.PHONY: clean
clean:
	rm -f *.o libo.a libo.dylib test/osc_test *~ $(LIBO_PARSER_CFILES) $(LIBO_PARSER_HFILES) $(LIBO_SCANNER_CFILES) $(LIBO_SCANNER_HFILES) *.output
	cd doc && rm -rf html latex man
	cd test && $(MAKE) clean
	cd contrib && rm -rf *.o

.PHONY: swig-clean
swig-clean:
	rm -rf libo.py libo_wrap.c _libo.so
node-clean:
	rm -rf build odot_wrap.cxx
