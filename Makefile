.PHONY: build build-wali

WALI_DIR := 

build:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug
	make -C build -j6

build-wali:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_VERBOSE_MAKEFILE=True \
    -DCMAKE_C_FLAGS="--target=wasm32-wasi-threads -pthread --sysroot=/$(WALI_DIR)/wali-musl/sysroot -matomics -mbulk-memory -mmutable-globals -msign-ext" \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++ --target=wasm32-wasi-threads -pthread --sysroot=/$(WALI_DIR)/wali-musl/sysroot -I/$(WALI_DIR)/libcxx/include/c++/v1 -matomics -mbulk-memory -mmutable-globals -msign-ext" \
    -DCMAKE_LINKER=wasm-ld \
		-DCMAKE_EXE_LINKER_FLAGS="-L/$(WALI_DIR)/wali-musl/sysroot/lib -L/$(WALI_DIR)/libcxx/lib -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=2147483648" \
    -DCMAKE_AR=/$(WALI_DIR)/llvm-project/build/bin/llvm-ar \
    -DCMAKE_RANLIB=/$(WALI_DIR)/llvm-project/build/bin/llvm-ranlib
	make -C build -j6


clean:
	rm -r build
