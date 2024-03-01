#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <chrono>
#include <cstring>
#include <getopt.h>

#include "common.h"
#include "parse.h"
#include "ir.h"
#include "api/inspections.h"
#include "BS_thread_pool_light.hpp"

static int num_thread_workers = std::thread::hardware_concurrency() - 2;

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
  char* outfile;
  char* infile;
  char* scheme;
  char* inst_args;
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
        ERR("Supported schemes: \'call-count\', \'func-idx\'\n");
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

  args.infile = argv[optind];
  return args;
}

void free_args (args_t args) {
  free (args.outfile);
  free (args.scheme);
  free (args.inst_args);
}


std::string inspect_call (WasmModule &module, std::string routine, std::vector<std::string> args) {

  DEF_TIME_VAR();
  TRACE("Running inspection: %s\n", routine.c_str());
  for (auto &a : args)
    printf("Args: %s\n", a.c_str());

  std::string ret;

  if (routine == "empty") { }
  else if (routine == "call-count") { 
    ret = call_count_inspect(module);
  }
  else if (routine == "func-idx") {
    ret = func_indexes_inspect(module);
  }

  return ret;
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
  std::string out_string;
TIME_SECTION(0, "Time to inspect",
  out_string = inspect_call(module, args.scheme, arg_vec);
)

  std::ofstream fout(args.outfile);
TIME_SECTION(0, "Time to write output",
  fout << out_string;
  fout.close();
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




