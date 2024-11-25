#include "export/instrument_c_api.h"
#include "ir.h"
#include "routines.h"

#include <vector>
#include <queue>
#include <deque>
#include <iostream>
#include <string>
#include <cstring>
#include <random>


typedef std::queue<std::string> ArgVec;

typedef enum {
  StringArgs,
  AnonArrArgs
} InstrumentArgType;

typedef union {
  void (*string_arg) (WasmModule&, ArgVec);
  void (*anonarr_arg) (WasmModule&, void*, uint32_t, int64_t);
} InstrumentFn;

typedef struct {
  const char* name;
  uint32_t min_args;
  uint32_t max_args;
  InstrumentArgType args_type;
  InstrumentFn fn;
} routines_t;


#define POP(q) ({ \
  auto &v = q.front();  \
  q.pop();  \
  v;  \
})

#define POP_INT(k) stoi(POP(k))
#define POP_STR(k) POP(k)


/* Handlers for each instrumentation type */
void memaccess_stochastic_insthandle (WasmModule &module, ArgVec args) {
  int percent = POP_INT(args);
  int cluster_size = POP_INT(args);
  if (cluster_size > 1) {
    throw std::runtime_error("Cluster size > 1 not supported yet");
  }
  std::string path = (args.empty() ? std::string() : POP_STR(args));
  std::vector<WasmModule> outs = 
    memaccess_stochastic_instrument(module, percent, cluster_size, path);
  module = std::move(outs[0]);
}

void memaccess_balanced_insthandle (WasmModule &module, ArgVec args) {
  int cluster_size = POP_INT(args);
  if (cluster_size > 0) {
    throw std::runtime_error("Cluster size > 1 not supported yet");
  }
  std::string path = (args.empty() ? POP_STR(args) : std::string());
  std::vector<WasmModule> outs = 
    memaccess_balanced_instrument(module, cluster_size, path);
  module = std::move(outs[0]);
}

void sample_insthandle (WasmModule &module, ArgVec args) {
  sample_instrument(module);
}

void func_weight_insthandle (WasmModule &module, ArgVec args) {
  all_funcs_weight_instrument(module);
}

void loop_count_insthandle (WasmModule &module, ArgVec args) {
  loop_instrument(module);
}

void opcode_count_insthandle (WasmModule &module, ArgVec args) {
  opcode_count_instrument(module);
}

void r3_record_insthandle (WasmModule &module, ArgVec args) {
  r3_record_instrument(module);
}

void plc_trace_insthandle (WasmModule &module, ArgVec args) {
  plc_trace_instrument(module);
}

void empty_insthandle (WasmModule &module, ArgVec args) { return; }
/* */


static std::vector<routines_t> inst_routines = {
  { "memaccess-stochastic",   2,  3, StringArgs,  InstrumentFn { .string_arg = memaccess_stochastic_insthandle } },
  { "memaccess-balanced"  ,   2,  3, StringArgs,  InstrumentFn { .string_arg = memaccess_balanced_insthandle } },
  { "sample"              ,   0,  0, StringArgs,  InstrumentFn { .string_arg = sample_insthandle } },
  { "func-weight"         ,   0,  0, StringArgs,  InstrumentFn { .string_arg = func_weight_insthandle } },
  { "loop-count"          ,   0,  0, StringArgs,  InstrumentFn { .string_arg = loop_count_insthandle } },
  { "opcode-count"        ,   0,  0, StringArgs,  InstrumentFn { .string_arg = opcode_count_insthandle } },
  { "r3-record"           ,   0,  0, StringArgs,  InstrumentFn { .string_arg = r3_record_insthandle } },
  { "r3-replay-generator" ,   0,  UINT32_MAX, AnonArrArgs, InstrumentFn { .anonarr_arg = r3_replay_instrument } },
  { "plc-trace"           ,   0,  0, StringArgs,  InstrumentFn { .string_arg = plc_trace_insthandle } },
  { "empty"               ,   0,  0, StringArgs,  InstrumentFn { .string_arg = empty_insthandle   }}
};




WasmModule*
decode_instrument_module (byte* file_buf, uint32_t file_size) {
  WasmModule *module = new WasmModule();
  *module = parse_bytecode(file_buf, file_buf + file_size);
  return module;
}

byte* 
encode_file_buf_from_module (WasmModule* module, uint32_t* file_size) {
  bytedeque bq = module->encode_module(NULL);
  byte* res = (byte*) malloc(bq.size());
  uint32_t i = 0;
  for (auto &belem : bq) {
    res[i++] = belem; 
  }
  *file_size = bq.size();
  return res;
}


/** Instrumentation (in-place). No batch mode supported yet **/
void 
instrument_module (WasmModule* mod, const char* scheme, void* args, uint32_t num_args, int64_t flags) {
  bool match = false;
  for (auto &routine : inst_routines) {
    if (routine.name == std::string(scheme)) {
      match = true;
      if ((num_args < routine.min_args) || (num_args > routine.max_args)) {
        printf("%s scheme takes %d <= n <= %d args (provided %d)\n", 
          routine.name, routine.min_args, routine.max_args, num_args);
        throw std::runtime_error("Invalid args");
      }
      else {
        // Found routine; call the handler function
        if (routine.args_type == StringArgs) {
          ArgVec arg_vec;
          for (int i = 0; i < num_args; i++) {
            arg_vec.push(std::string(((char**)args)[i]));
          }
          routine.fn.string_arg(*mod, arg_vec);
        } else if (routine.args_type == AnonArrArgs) {
          routine.fn.anonarr_arg(*mod, args, num_args, flags);
        } else {
          throw std::runtime_error("Invalid instrumentation args type");
        }
      }
    }
  }
  if (!match) {
    printf("Scheme %s not found\n", scheme);
    throw std::runtime_error("Invalid scheme");
  }
}


/** Instrumentation from raw buffer.
 * Copy returned that should be explicitly freed using 'destroy_file_buf' */
byte*
instrument_module_buffer (byte* inbuf, uint32_t insize, uint32_t *outsize,
              const char* scheme, void* args, uint32_t num_args, int64_t flags) {
  WasmModule *mod = decode_instrument_module (inbuf, insize);
  instrument_module (mod, scheme, args, num_args, flags);
  byte* outbuf = encode_file_buf_from_module (mod, outsize);
  destroy_instrument_module(mod);
  return outbuf;
}

/** Deep-Copy modules **/
WasmModule* 
copy_instrument_module (WasmModule *mod) {
  return new WasmModule(*mod);
}


/** Free methods **/
void 
destroy_instrument_module (WasmModule* mod) {
  delete mod;
}

void 
destroy_file_buf (byte* file_buf) {
  free(file_buf);
}


void fill_rand_instmask(byte *mask, int percent, uint32_t len) {
  std::vector<byte> v(len, 0);
  uint32_t count = (len * percent) / 100;
  std::fill_n(v.begin(), count, 1);
  std::shuffle(v.begin(), v.end(), std::mt19937{std::random_device{}()});

  memcpy(mask, v.data(), len);
}


