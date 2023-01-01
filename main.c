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
  
  printf("Trace: %d | Dis: %d\n", g_trace, g_disassemble);
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
  wasm_module_t module = parse_bytecode(start, end);
  unload_file(&start, &end);
  return 0;
}


void module_free(wasm_module_t* mod) {

  FREE(mod->table, mod->num_tables);

  for (uint32_t i = 0; i < mod->num_sigs; i++) {
    FREE(mod->sigs[i].params, mod->sigs[i].num_params);
    FREE(mod->sigs[i].results, mod->sigs[i].num_results);
  }
  FREE(mod->sigs, mod->num_sigs);

  for (uint32_t i = 0; i < mod->num_imports; i++) {
    FREE(mod->imports[i].mod_name, mod->imports[i].mod_name_length);
    FREE(mod->imports[i].member_name, mod->imports[i].member_name_length);
  }
  FREE(mod->imports, mod->num_imports);

  for (uint32_t i = 0; i < mod->num_funcs; i++) {
    uint32_t idx = i + mod->num_imports;
    FREE(mod->funcs[idx].local_decl, mod->funcs[idx].num_local_vec);
  }
  FREE(mod->funcs, mod->num_funcs);

  for (uint32_t i = 0; i < mod->num_exports; i++) {
    FREE(mod->exports[i].name, mod->exports[i].length);
  }
  FREE(mod->exports, mod->num_exports);

  FREE(mod->globals, mod->num_globals);

  FREE(mod->datas, mod->num_datas);

  for (uint32_t i = 0; i < mod->num_elems; i++) {
    FREE(mod->elems[i].func_indexes, mod->elems[i].length);
  }
  FREE(mod->elems, mod->num_elems);

}


