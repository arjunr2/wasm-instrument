#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <getopt.h>
#include <math.h>
#include <limits.h>

#include "common.h"
#include "parse.h"
#include "ir.h"


static struct option long_options[] = {
  {"trace", no_argument,  &g_trace, 1},
  {"dis", no_argument, &g_disassemble, 1},
  {"help", no_argument, NULL, 'h'}
};

char* parse_args(int argc, char* argv[]) {
  int opt;
  optind = 0;
  while ((opt = getopt_long_only(argc, argv, "h", long_options, NULL)) != -1) {
    switch(opt) {
      case 0: break;
      case 'h':
      default:
        ERR("Usage: %s [--trace] [--dis] <filename>\n", argv[0]);
        exit(opt != 'h');
    }
  }

  if ((optind + 1) != argc) {
    ERR("Executable takes one positional argument \"filename\"\n");
    exit(1);
  }
  
  //printf("Trace: %d | Dis: %d\n", g_trace, g_disassemble);
  return argv[optind];
}

// Main function.
// Parses arguments and either runs a file with arguments.
//  -trace: enable tracing to stderr
int main(int argc, char *argv[]) {
  char* filename = parse_args(argc, argv);
    
  byte* start = NULL;
  byte* end = NULL;

  ssize_t r = load_file(filename, &start, &end);
  if (r < 0) {
    ERR("failed to load: %s\n", filename);
    return 1;
  }

  TRACE("loaded %s: %ld bytes\n", filename, r);
  WasmModule module = parse_bytecode(start, end);
  unload_file(&start, &end);

  /* Instrument */
  /* Global inmodule */
  GlobalDecl global = { 
    .type = WASM_TYPE_I32, 
    .is_mutable = true,
    .init_expr_bytes = INIT_EXPR (I32_CONST, 5)
  };
  auto &fiveglob = module.add_global(global, "inmodule_global");

  /* Global import */
  ImportInfo iminfo = {
    .mod_name = "instrumentest",
    .member_name = "newglob"
  };
  GlobalInfo imglob = {
    .type = WASM_TYPE_EXTERNREF,
    .is_mutable = false
  };
  module.add_import(iminfo, imglob);

  /* Function import */
  iminfo.member_name = "newfunc";
  SigDecl imfunc = {
    .params = {WASM_TYPE_I32},
    .results = {WASM_TYPE_F64}
  };
  module.add_import(iminfo, imfunc);

  /* Export add */
  //module.add_export("inmodule_global", fiveglob);

  /* Encode instrumented module */
  bytedeque bq = module.encode_module();
  return 0;
}




