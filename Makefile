#Inside of your sidekiq_sdk & repository directory

include tools.mk
CFLAGS += -std=gnu11
CFLAGS += -I../sidekiq_core/inc -I../arg_parser/inc
OUT_DIR = ./bin/
SRC_DIR = ./src/

# for a native build configuration, choose the underlying cross-compiled build configuration
ifeq ($(BUILD_CONFIG),aarch64.native)
override BUILD_CONFIG=aarch64.gcc6.3
endif

# define any directories containing header files other than /usr/include
INCLUDES =

# define any libraries to link into executable:
ifneq ($(PLATFORM),undefined)
STATIC_LIBS = ../lib/libsidekiq__$(PLATFORM).a ../arg_parser/lib/arg_parser__$(PLATFORM).a
SUPPORT = ../lib/support/$(PLATFORM)
else
STATIC_LIBS = ../lib/libsidekiq__$(BUILD_CONFIG).a ../arg_parser/lib/arg_parser__$(BUILD_CONFIG).a
SUPPORT = ../lib/support/$(BUILD_CONFIG)
endif
LIBS+= $(STATIC_LIBS)

# glib2 / libusb / libiio locations / flags
LDFLAGS+= -L$(SUPPORT)/usr/lib/epiq
LIBS+= -lusb-1.0 -lglib-2.0
ifeq ($(BUILD_CONFIG),arm_cortex-a9.gcc4.8_uclibc_openwrt)
LIBS+= -lglib-2.0 -lintl -liconv
else ifeq ($(BUILD_CONFIG),arm_cortex-a9.gcc4.9.2_gnueabi)
LIBS+= -liio -lz -lxml2
else ifeq ($(BUILD_CONFIG),arm_cortex-a9.gcc7.2.1_gnueabihf)
LIBS+= -liio
endif

# define any libraries to link into executable:
LIBS+= -lpthread -lrt -lm

#
# build executables to look for glib-2.0 in the same directory or a directory
# relative to the default locations in the SDK.
#
# regarding the irregular escaping of $ORIGIN:
#   make first translates $$ => $
#   bash then sees \$ORIGIN, so it does not try to evaluate it, but removes \
#   the linker then sees $ORIGIN as needed
#
ifeq ($(BUILD_CONFIG),arm_cortex-a9.gcc4.8_uclibc_openwrt)
LDFLAGS+= -Wl,--enable-new-dtags -Wl,-rpath,/usr/lib/epiq
else ifneq ($(PLATFORM),undefined)
LDFLAGS+= -Wl,--enable-new-dtags -Wl,-rpath,\$$ORIGIN,-rpath,\$$ORIGIN/../../lib/support/$(PLATFORM)/usr/lib/epiq,-rpath,/usr/lib/epiq -Wl,-z,origin
else
LDFLAGS+= -Wl,--enable-new-dtags -Wl,-rpath,\$$ORIGIN,-rpath,\$$ORIGIN/../../lib/support/$(BUILD_CONFIG)/usr/lib/epiq,-rpath,/usr/lib/epiq -Wl,-z,origin
endif

# the below apps are released to customers as part of the Sidekiq SDK
SRCS=
SRCS+=

TESTAPPS= $(patsubst src/%.c,bin/%,$(filter src/%.c,$(SRCS)))
TESTAPPS+= $(patsubst %.c,%,$(filter %.c,$(filter-out src/%.c,$(SRCS))))

all: $(TESTAPPS)

clean:
	rm -f src/*.o
	rm -f bin/*

$(TESTAPPS): $(STATIC_LIBS)

# build the test executable in bin/ from src/
bin/%: src/%.o
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) -o $@ $(CFLAGS) -Wl,--start-group $^ $(LIBS) $(LDLIBS) -Wl,--end-group
ifeq ($(STRIP_ENABLED),enabled)
	$(STRIP) $@
endif
