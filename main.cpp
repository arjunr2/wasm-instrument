#include <sstream>

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
#include "views.h"
#include "ir.h"
#include "instructions.h"
#include "routines.h"


static struct option long_options[] = {
  {"trace", no_argument,  &g_trace, 1},
  {"dis", no_argument, &g_disassemble, 1},
  {"scheme", optional_argument, NULL, 's'},
  {"out", required_argument, NULL, 'o'},
  {"args", optional_argument, NULL, 'a'},
  {"help", no_argument, NULL, 'h'}
};

typedef struct {
  char* infile;
  char* scheme;
  char* inst_args;
  char* outfile;
} args_t;

args_t parse_args(int argc, char* argv[]) {
  int opt;
  args_t args = {0};
  optind = 0;
  while ((opt = getopt_long_only(argc, argv, "o:s:a:h", long_options, NULL)) != -1) {
    switch(opt) {
      case 0: break;
      case 'o': args.outfile = strdup(optarg); break;
      case 's': args.scheme = strdup(optarg); break;
      case 'a': args.inst_args = strdup(optarg); break;
      case 'h':
      default:
        ERR("Usage: %s [--trace] [--scheme SCHEME] [--args SCHEME_ARGS (optional)] [--out OUTFILE] <infile>\n", argv[0]);
        exit(opt != 'h');
    }
  }

  if ( ((optind + 1) != argc)) {
    ERR("Executable takes one one positional argument \"filename\"\n");
    exit(1);
  }

  if (!args.outfile) {
    ERR("Executable requires outfile \"--out\"\n");
    exit(1);
  }
  
  // Run sample instrumentation by default
  if (!args.scheme) { args.scheme = strdup("sample"); } 
  args.infile = argv[optind];
  return args;
}

void free_args (args_t args) {
  free (args.outfile);
  free (args.scheme);
  free (args.inst_args);
}


void instrument_call (WasmModule &module, std::string routine, std::vector<std::string> args) {

  printf("Running instrumentation: %s\n", routine.c_str());
  for (auto &a : args)
    printf("Args: %s\n", a.c_str());
  if (routine == "empty") { return; }
  else if (routine == "memaccess") { memaccess_instrument(module); }
  else if (routine == "memshared") { memshared_instrument(module, args[0]); }
  else if (routine == "memshared-stochastic") { 
    if (args.size() != 3) { throw std::runtime_error("memshared stochastic needs 3 args"); }
    int percent = stoi(args[1]);
    int cluster_size = stoi(args[2]);
    memshared_stochastic_instrument(module, args[0], percent, cluster_size);
  }
  else if (routine == "sample") { sample_instrument(module); }
  else if (routine == "func-weight") { all_funcs_weight_instrument(module); }
  else if (routine == "loop-count") { loop_instrument(module); }
  else {
    printf("Unsupported instrumentation scheme\n");
  }
}

// Main function.
// Parses arguments and either runs a file with arguments.
//  -trace: enable tracing to stderr
int main(int argc, char *argv[]) {
  args_t args = parse_args(argc, argv);
    
  byte* start = NULL;
  byte* end = NULL;

  ssize_t r = load_file(args.infile, &start, &end);
  if (r < 0) {
    ERR("failed to load: %s\n", args.infile);
    return 1;
  }

  TRACE("loaded %s: %ld bytes\n", args.infile, r);
  WasmModule module = parse_bytecode(start, end);
  unload_file(&start, &end);

  /* Instrument */
  /* Get argument in vector format */
  std::vector<std::string> arg_vec;
  std::string inst_args = (args.inst_args ? std::string(args.inst_args) : std::string());
  std::stringstream ss(inst_args);
  std::string s;
  while (std::getline(ss, s, ' ')) {
    arg_vec.push_back(s);
  }
  instrument_call(module, args.scheme, arg_vec);

  /* Encode instrumented module */
  bytedeque bq = module.encode_module(args.outfile);

  free_args(args);
  return 0;
}




