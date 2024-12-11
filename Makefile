.PHONY: build build-wali

WALI := 

-include $(WALI)/wali_config.mk

build:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug
	make -C build -j

build-wali:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=$(WALI_CXX) -DCMAKE_C_COMPILER=$(WALI_CC) -DCMAKE_VERBOSE_MAKEFILE=True \
    -DCMAKE_C_FLAGS="$(WALI_COMMON_CFLAGS)" \
    -DCMAKE_CXX_FLAGS="$(WALI_COMMON_CXXFLAGS)" \
    -DCMAKE_LINKER=$(WALI_LD) \
		-DCMAKE_EXE_LINKER_FLAGS="$(WALI_COMMON_LDFLAGS)" \
    -DCMAKE_AR=$(WALI_AR) \
    -DCMAKE_RANLIB=$(WALI_RANLIB)
	make -C build

clean:
	rm -r build

