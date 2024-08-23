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
  printf("Code size: %ld\n", end - start);
  wasm_instrument_mod_t mod_base = decode_instrument_module(start, end - start);

  const char *args[] = {
    "40",
    "1"
  };
  int num_args = 2;

  /** Method 1 */
  wasm_instrument_mod_t mod = copy_instrument_module(mod_base);
  instrument_module (mod, "memaccess-stochastic", (void*) args, num_args, 0);

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
  destroy_instrument_module(mod_base);

  /** Method 2 */
  uint32_t outsize;
  byte* filebuf2 = instrument_module_buffer (start, end - start, &outsize, "memaccess-stochastic", (void*)args, num_args, 0);
  printf("Encode size: %d\n", outsize);

  FILE *g = fopen("out2.wasm", "wb");
  if (!fwrite(filebuf2, sizeof(byte), outsize, g)) {
    printf("Error writing to file\n");
    exit(1);
  }
  fclose(g);

  destroy_file_buf(filebuf2);

  unload_file(&start, &end);

  /* Bitmask generator */
  int len = 50;
  byte *mask = (byte*) malloc(len);
  for (int i = 10; i < 40; i+=10) {
    fill_rand_instmask(mask, i, len);
    printf("V: ");
    for (int j = 0; j < len; j++) {
      printf("%d ", mask[j]);
    }
    printf("\n");
  }
  free(mask);
}
