#include "api/export/instrument_c_api.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
  byte *start = NULL, *end = NULL;
  if (load_file("lua.wasm", &start, &end) < 0) {
    printf("Failed\n");
    exit(1);
  }
  printf("Code size: %d\n", end - start);
  wasm_instrument_mod_t mod = decode_instrument_module(start, end - start);
  unload_file(&start, &end);

  uint32_t enc_size = 0;
  byte* filebuf = encode_file_buf_from_module(mod, &enc_size);
  printf("Encode size: %d\n", enc_size);

  FILE *f = fopen("out.wasm", "wb");
  if (!fwrite(filebuf, sizeof(byte), enc_size, f)) {
    printf("Error writing to file\n");
    exit(1);
  }
  fclose(f);

  destroy_file_buf(filebuf);
  destroy_instrument_module(mod);
}
