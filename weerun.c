#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <limits.h>

#include "common.h"
#include "weedis.h"
#include "vm.h"
#include "test.h"
#include "illegal.h"
#include "ir.h"


// Disassembles and runs a wasm module.
wasm_value_t run(bool jit, const byte* start, const byte* end, uint32_t num_args, wasm_value_t* args, bool time);

// Main function.
// Parses arguments and either runs the tests or runs a file with arguments.
//  -trace: enable tracing to stderr
//  -test: run internal tests
int main(int argc, char *argv[]) {
  bool jit = false;
  bool time = false;
  for (int i = 1; i < argc; i++) {
    char *arg = argv[i];
    if (strcmp(arg, "-test") == 0) return run_tests();
    if (strcmp(arg, "-trace") == 0) {
      g_trace = 1;
      continue;
    }
    if (strcmp(arg, "-disassemble") == 0) {
      g_disassemble = 1;
      continue;
    }
    if (strcmp(arg, "-jit") == 0) {
      jit = true;
      continue;
    }
    if (strcmp(arg, "-time") == 0) {
      time = true;
      continue;
    }
    
    byte* start = NULL;
    byte* end = NULL;
    ssize_t r = load_file(arg, &start, &end);
    if (r >= 0) {
      TRACE("loaded %s: %ld bytes\n", arg, r);
      int num_args = argc - i - 1;
      wasm_value_t* args = NULL;
      if (num_args > 0) {
	      args = (wasm_value_t*)malloc(sizeof(wasm_value_t)*num_args);
	      for (int j = i + 1; j < argc; j++) {
	        int a = j - i - 1;
	        args[a] = parse_wasm_value(argv[j]);
	        TRACE("args[%d] = ", a);
	        trace_wasm_value(args[a]);
	        TRACE("\n");
	      }
      }
      wasm_value_t result = run(jit, start, end, num_args, args, time);
      unload_file(&start, &end);
      print_wasm_value(result);
      printf("\n");
      if (num_args) { free(args); }
      exit(0);
      return 0;
    } else {
      ERR("failed to load: %s\n", arg);
      return 1;
    }
  }
  return 0;
}


// A pair of a buffer and a module into which to parse the bytes.
typedef struct {
  buffer_t* buf;
  wasm_module_t* module;
} mparse_t;

//=== SOLUTION CODE -- YOUR SOLUTION HERE ==================================
int parse(wasm_module_t* module, buffer_t* buf, bool jit);


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

  FREE(mod->data, mod->num_data);

  for (uint32_t i = 0; i < mod->num_elems; i++) {
    FREE(mod->elems[i].func_indexes, mod->elems[i].length);
  }
  FREE(mod->elems, mod->num_elems);

}


wasm_value_t run(bool jit, const byte* start, const byte* end, uint32_t num_args, wasm_value_t* args, bool time) {
  buffer_t onstack_buf = {
    start,
    start,
    end
  };
  wasm_module_t onstack_module;
  memset(&onstack_module, 0, sizeof(onstack_module));
  int presult = parse(&onstack_module, &onstack_buf, jit);
  if (presult < 0) return wasm_i32_value(-1);

  wasm_value_t res = (jit ? 
                        jit_run(onstack_buf, &onstack_module, num_args, args, time) :
                        interp_run(onstack_buf, &onstack_module, num_args, args, time));
  module_free(&onstack_module);
  return res;
}



// The main disassembly routine.
// Don't replace branches in JIT
int parse(wasm_module_t* module, buffer_t* buf, bool jit) {
  disassemble(module, buf->start, buf->end, !jit);
  return 0;
}
