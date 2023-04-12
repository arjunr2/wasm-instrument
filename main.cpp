#include <sstream>
#include <iostream>
#include <cstdio>
#include <chrono>
#include <cstring>
#include <getopt.h>

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


std::vector<WasmModule> instrument_call (WasmModule &module, std::string routine, 
    std::vector<std::string> args, bool &is_batch) {

  TRACE("Running instrumentation: %s\n", routine.c_str());
  for (auto &a : args)
    printf("Args: %s\n", a.c_str());

  std::vector<WasmModule> out_modules;
  is_batch = false;

  if (routine == "empty") { }

  else if (routine == "memaccess") { 
    if (args.size() > 1) {
      throw std::runtime_error("memaccess needs 0/1 args");
    }
    std::string path = ((args.size() == 1) ? args[0] : std::string());
    memaccess_instrument(module, path);
  }
  
  else if (routine == "memaccess-stochastic") {
    if ((args.size() < 2) || (args.size() > 3)) { 
      throw std::runtime_error("memaccess stochastic needs 2/3 args");
    }
    int percent = stoi(args[0]);
    int cluster_size = stoi(args[1]);
    std::string path = ((args.size() == 3) ? args[2] : std::string());
    out_modules = memaccess_stochastic_instrument(module, percent, cluster_size, path);
    is_batch = true;
  }

  else if (routine == "memaccess-balanced") {
    if ((args.size() > 2) || (args.size() < 1)) { 
      throw std::runtime_error("memaccess stochastic needs 1/2 args");
    }
    int cluster_size = stoi(args[0]);
    std::string path = ((args.size() == 2) ? args[1] : std::string());
    out_modules = memaccess_balanced_instrument(module, cluster_size, path);
    is_batch = true;
  }
  
  else if (routine == "sample") { sample_instrument(module); }
  else if (routine == "func-weight") { all_funcs_weight_instrument(module); }
  else if (routine == "loop-count") { loop_instrument(module); }
  else {
    printf("Unsupported instrumentation scheme\n");
  }

  return out_modules.empty() ? std::vector<WasmModule>(1, module) : out_modules;
}

#define TIME_SECTION(nest, logstr, code)  \
  start_time = std::chrono::high_resolution_clock::now();  \
  code;  \
  end_time = std::chrono::high_resolution_clock::now(); \
  { \
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);  \
    printf("%*s%-25s:%8ld us\n", nest*4, "", logstr, elapsed.count()); \
  }

  

// Main function.
// Parses arguments and either runs a file with arguments.
//  -trace: enable tracing to stderr
int main(int argc, char *argv[]) {
  auto start_time = std::chrono::high_resolution_clock::now();
  auto end_time = std::chrono::high_resolution_clock::now();

  auto begin_time = start_time;

  args_t args = parse_args(argc, argv);
    
  byte* start = NULL;
  byte* end = NULL;

  ssize_t r = load_file(args.infile, &start, &end);
  if (r < 0) {
    ERR("failed to load: %s\n", args.infile);
    return 1;
  }

TIME_SECTION(0, "Time to load module", 
  TRACE("loaded %s: %ld bytes\n", args.infile, r);
  WasmModule module = parse_bytecode(start, end);
  unload_file(&start, &end);
)

  /* Instrument */
  /* Get argument in vector format */
  std::vector<std::string> arg_vec;
  std::string inst_args = (args.inst_args ? std::string(args.inst_args) : std::string());
  std::stringstream ss(inst_args);
  std::string s;
  while (std::getline(ss, s, ' ')) {
    arg_vec.push_back(s);
  }

  bool is_batch;
TIME_SECTION(0, "Time to instrument",
  std::vector<WasmModule> out_modules = instrument_call(module, args.scheme, arg_vec, is_batch);
)

  /* Encode instrumented module */
  if (!is_batch) {
TIME_SECTION(0, "Time to encode module",
    bytedeque bq = module.encode_module(args.outfile);
)
  }
  else {
    std::string outfile_template(args.outfile);
    std::size_t splitidx = outfile_template.find_last_of("/");
    outfile_template.insert(splitidx + 1, "part%d.");

    int i = 1;
    for (auto &mod : out_modules) {
      char outfile[200];
      sprintf(outfile, outfile_template.data(), i);
      bytedeque bq = mod.encode_module(outfile);
      i++;
    }
  }

  free_args(args);

  printf("--------------------------------------\n");
  auto final_time = std::chrono::high_resolution_clock::now();
  {
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(final_time - begin_time);
    printf("%-25s:%8ld us\n", "Total Time", elapsed.count());
  }
  printf("--------------------------------------\n");


  return 0;
}




