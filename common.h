#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include <list>

/*** Generic used typedefs ***/
typedef uint8_t byte;
typedef std::vector<byte> bytearr;
/* Buffer for parsing/decoding */
typedef struct {
  const byte* start;
  const byte* ptr;
  const byte* end;
} buffer_t;
/***************/


/*** Global trace/err/disassemble flags and macros. ***/
#define TRACE(...) do { if(g_trace) fprintf(stderr, __VA_ARGS__); } while(0)
#define DISASS(...) do { if(g_disassemble) fprintf(stderr, __VA_ARGS__); } while(0)
#define ERR(...) fprintf(stderr, __VA_ARGS__)
extern int g_trace;
extern int g_disassemble;
/***************/


/*** Parsing macros ***/
#define RD_U32()        read_u32leb(&buf)
#define RD_I32()        read_i32leb(&buf)
#define RD_U64()        read_u64leb(&buf)
#define RD_I64()        read_i64leb(&buf)
#define RD_NAME()       read_name(&buf)
#define RD_BYTESTR(len) read_bytes(&buf, len)

#define RD_BYTE()       read_u8(&buf)
#define RD_U32_RAW()    read_u32(&buf)
#define RD_U64_RAW()    read_u64(&buf)
/********************/


/*** Access macros ***/
#define GET_LIST_ELEM(ll, idx) ({ \
  auto it = std::next(ll.begin(), idx); \
  &(*it);  \
})
/********************/


/* Load a file into memory; initialize start and end.
* Returns < 0 on failure. */
ssize_t load_file(const char* path, byte** start, byte** end);

/* Unload a file previously loaded into memory using {load_file}. */
ssize_t unload_file(byte** start, byte** end);



/*** Decode Operations ***/

#define MAX_FILE_SIZE 2000000000

/* Decode LEB128 values, enforcing the specified signedness and maximum range.
* The argument {ptr} will be advanced to point after the decoded value.
* If an error occurs (e.g. unterminated LEB, invalid value), {ptr} will be set to
* NULL. */
int32_t  decode_i32leb(const byte* ptr, const byte* limit, ssize_t* len);
uint32_t decode_u32leb(const byte* ptr, const byte* limit, ssize_t* len);
int64_t  decode_i64leb(const byte* ptr, const byte* limit, ssize_t* len);
uint64_t decode_u64leb(const byte* ptr, const byte* limit, ssize_t* len);

/* Decode fixed-size integer values. */
uint32_t decode_u32(const byte* ptr, const byte* limit, ssize_t* len);



/* Read an unsigned(u)/signed(i) X-bit LEB, advancing the {ptr} in buffer */
uint32_t read_u32leb(buffer_t* buf);
int32_t read_i32leb(buffer_t* buf);
uint64_t read_u64leb(buffer_t* buf);
int64_t read_i64leb(buffer_t* buf);

/* Read a raw X-bit value, advancing the {ptr} in buffer*/
uint8_t read_u8(buffer_t* buf);
uint32_t read_u32(buffer_t* buf);
uint64_t read_u64(buffer_t* buf);

/* Read a name (32leb + string), advancing the {ptr} in buffer */
std::string read_name(buffer_t* buf);
/* Read num_bytes, advancing the {ptr} in buffer */
bytearr read_bytes(buffer_t* buf, uint32_t num_bytes);



/*** Encode Operations ***/
