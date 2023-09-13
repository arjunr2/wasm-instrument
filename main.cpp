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
#include "BS_thread_pool_light.hpp"

int num_thread_workers = std::thread::hardware_concurrency() - 2;

static struct option long_options[] = {
  {"trace", no_argument,  &g_trace, 1},
  {"scheme", required_argument, NULL, 's'},
  {"time", no_argument, &g_time, 1},
  {"multithread", no_argument, &g_threads, 1},
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
  while ((opt = getopt_long_only(argc, argv, "o:s:a::h", long_options, NULL)) != -1) {
    switch(opt) {
      case 0: break;
      case 'o': args.outfile = strdup(optarg); break;
      case 's': args.scheme = strdup(optarg); break;
      case 'a': args.inst_args = strdup(optarg); break;
      case 'h':
      default:
        ERR("Usage: %s [--trace (opt)] [--multithread (opt)] [--time (opt)] "
            "[--scheme SCHEME] [--args SCHEME_ARGS (opt)] [--out OUTFILE] input-file\n", argv[0]);
        ERR("Supported schemes: \'empty\', \'sample\', \'loop-count\'\n");
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
  //if (!args.scheme) { args.scheme = strdup("sample"); } 
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

  DEF_TIME_VAR();
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
      throw std::runtime_error("memaccess balanced needs 1/2 args");
    }
    int cluster_size = stoi(args[0]);
    std::string path = ((args.size() == 2) ? args[1] : std::string());
    out_modules = memaccess_balanced_instrument(module, cluster_size, path);
    is_batch = true;
  }
  
  else if (routine == "sample") { sample_instrument(module); }
  else if (routine == "func-weight") { all_funcs_weight_instrument(module); }
  else if (routine == "loop-count") { loop_instrument(module); }
  else if (routine == "opcode-count") { opcode_count_instrument(module); }
  else {
    printf("Unsupported instrumentation scheme\n");
  }

  if (!is_batch) {
    auto module_vec = std::vector<WasmModule>(1);
    module_vec[0] = std::move(module);
    return module_vec;
  } else {
    return out_modules;
  }
}

  

// Main function.
// Parses arguments and either runs a file with arguments.
//  -trace: enable tracing to stderr
int main(int argc, char *argv[]) {
  DEF_TIME_VAR();

  auto begin_time = start_time;

  args_t args = parse_args(argc, argv);
    
  byte* start = NULL;
  byte* end = NULL;

  ssize_t r = load_file(args.infile, &start, &end);
  if (r < 0) {
    ERR("failed to load: %s\n", args.infile);
    return 1;
  }

TIME_SECTION(0, "Time to parse module", 
  TRACE("loaded %s: %ld bytes\n", args.infile, r);
  WasmModule module = parse_bytecode(start, end);
  unload_file(&start, &end);
);

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
TIME_SECTION(0, "Time to encode modules",
  if (!is_batch) {
    bytedeque bq = module.encode_module(args.outfile);
  }
  else {
    std::string outfile_template(args.outfile);
    std::size_t splitidx = outfile_template.find_last_of("/");
    outfile_template.insert(splitidx + 1, "part%d.");
    auto loop = [&out_modules, &outfile_template](const int a, const int b) {
      for (int i = a; i < b; i++) {
        char outfile[200];
        sprintf(outfile, outfile_template.data(), i+1);
        bytedeque bq = out_modules[i].encode_module(outfile);
      }
    };

    if (!g_threads) {
      loop(0, out_modules.size());
    }
    /* Parallel write */
    else {
      BS::thread_pool_light pool(num_thread_workers);
      pool.push_loop(out_modules.size(), loop);
      pool.wait_for_tasks();
    }
  }
)

  free_args(args);

  printf("--------------------------------------\n");
  auto final_time = std::chrono::high_resolution_clock::now();
  {
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(final_time - begin_time);
    printf("%-25s:%8ld ms\n", "Total Time", elapsed.count());
  }
  printf("--------------------------------------\n");

  return 0;
}




