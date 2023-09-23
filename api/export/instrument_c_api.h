#ifndef _INSTRUMENT_C_API_H_
#define _INSTRUMENT_C_API_H_

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t byte;
typedef struct WasmModule* wasm_instrument_mod_t;


/** Decoding/Encoding: User is responsible for destroying **/
wasm_instrument_mod_t 
decode_instrument_module (byte* file_buf, uint32_t file_size);

byte* 
encode_file_buf_from_module (wasm_instrument_mod_t mod, uint32_t* file_size);


/** Instrumentation (in-place) **/
void 
instrument_module (wasm_instrument_mod_t mod, const char* routine, 
                    const char** args, uint32_t num_args);

/** Deep-Copy modules **/
wasm_instrument_mod_t
copy_instrument_module (wasm_instrument_mod_t mod);

/** Free methods **/
void 
destroy_instrument_module (wasm_instrument_mod_t mod);

void 
destroy_file_buf (byte* file_buf);


/* Load a file into memory; initialize start and end.
* Returns < 0 on failure. */
ssize_t 
load_file(const char* path, byte** start, byte** end);

/* Unload a file previously loaded into memory using {load_file}. */
ssize_t 
unload_file(byte** start, byte** end);


#ifdef __cplusplus
}
#endif

#endif /* end of _INSTRUMENT_C_API_H_ */
