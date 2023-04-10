/**
 * @file loop_count.cpp
 * @brief Loop counting with python bindings
 */
#include <Python.h>

#include "routine_common.h"
#include "ir.h"


/** Get value from packed bit array */
static bool packbits_get(byte *start, byte *end, int idx) {
  if (start + idx / 8 < end) {
    return start[idx / 8] >> (7 - idx % 8);
  } else {
    return 0;
  }
}

/** Instrument single opcode */
static void instrument(WasmModule &mod, InstList &insts, int idx) {
  GlobalDecl global = {
    .type = WASM_TYPE_I64, 
    .is_mutable = true,
    .init_expr_bytes = INIT_EXPR (I64_CONST, 0)
  };
  GlobalDecl *gref = mod->add_global(
    global, (std::string("_lc_") + std::to_string(idx)).c_str());
  auto loc = std::next(institr);
  InstList addinst;
  addinst.push_back(InstBasePtr(new GlobalGetInst(gref)));
  addinst.push_back(InstBasePtr(new I64ConstInst(1)));
  addinst.push_back(InstBasePtr(new I64AddInst()));
  addinst.push_back(InstBasePtr(new GlobalSetInst(gref)));
  insts.insert(loc, addinst.begin(), addinst.end());
}

/** Run full loop instrumentation on entire binary */
bytedeque &loop_instrument(
  byte *mod_start, byte *mod_end, byte *mask_start, byte *mask_end
) {
  WasmModule mod = parse_bytecode(mod_start, mod_end);

  uint64_t num_loops = 0;
  for (auto &func : mod.Funcs()) {
    InstList &insts = func.instructions;
    for (auto institr = insts.begin(); institr != insts.end(); ++institr) {
      InstBasePtr &instruction = *institr;
      // Increment global after loop
      if (
        instruction->is(WASM_OP_LOOP)
        || instruction->is(WASM_OP_BR)
        || instruction->is(WASM_OP_BR_IF)
        || instruction->is(WASM_OP_BR_ON_NON_NULL)
        || instruction->is(WASM_OP_BR_ON_NULL)
        || instruction->is(WASM_OP_BR_TABLE)
      ) {
        if (packbits_get(mask_start, mask_end, num_loops)) {
          instrument(mod, insts, num_loops);
        }
        num_loops += 1;
      }
    }
  }

  return mod.encode_module(NULL);
}

/** Python export */
PyObject *py_loop_instrument(PyObject *Py_UNUSED(self), PyObject *args) {
  Py_buffer *wasm;
  Py_buffer *mask;
  if(!PyArg_ParseTuple(args, "s*s*", wasm, mask)) { return NULL; }

  bytedeque &out = loop_instrument(
    wasm->buf, wasm->buf + wasm->len, mask->buf, mask->buf + mask->len);
  out.shrink_to_fit();
  char *buf = calloc(out.size(), sizeof(char));
  int i = 0;
  for(auto iter = buf.begin(); iter != buf.end(); ++buf) {
    buf[i++] = iter;
  }
  PyObject *result = PyBytes_FromStringAndSize(buf, out.size());

  PyBuffer_Release(wasm);
  PyBuffer_Release(mask);
  free(buf);

  return result;
}
