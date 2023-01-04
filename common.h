#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include <list>

typedef uint8_t byte;
typedef std::vector<byte> bytearr;

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

// Limit file size to something reasonable.
#define MAX_FILE_SIZE 2000000000

// Decode LEB128 values, enforcing the specified signedness and maximum range.
// The argument {ptr} will be advanced to point after the decoded value.
// If an error occurs (e.g. unterminated LEB, invalid value), {ptr} will be set to
// NULL.
int32_t  decode_i32leb(const byte* ptr, const byte* limit, ssize_t* len);
uint32_t decode_u32leb(const byte* ptr, const byte* limit, ssize_t* len);
int64_t  decode_i64leb(const byte* ptr, const byte* limit, ssize_t* len);
uint64_t decode_u64leb(const byte* ptr, const byte* limit, ssize_t* len);

// Decode fixed-size integer values.
uint32_t decode_u32(const byte* ptr, const byte* limit, ssize_t* len);

// Load a file into memory, initializing pointers to the start and end of
// the memory range containing the data. Returns < 0 on failure.
ssize_t load_file(const char* path, byte** start, byte** end);

// Unload a file previously loaded into memory using {load_file}.
ssize_t unload_file(byte** start, byte** end);

// The global trace flag.
extern int g_trace;
extern int g_disassemble;

// Helper macros to trace and to print errors.
#define TRACE(...) do { if(g_trace) fprintf(stderr, __VA_ARGS__); } while(0)
#define DISASS(...) do { if(g_disassemble) fprintf(stderr, __VA_ARGS__); } while(0)
#define ERR(...) fprintf(stderr, __VA_ARGS__)

// A helper struct that keeps track of the start and the current pointer of
// a buffer.
typedef struct {
  const byte* start;
  const byte* ptr;
  const byte* end;
} buffer_t;

// Read an unsigned 32-bit LEB, advancing the {ptr} in the buffer.
uint32_t read_u32leb(buffer_t* buf);

// Read a signed 32-bit LEB, advancing the {ptr} in the buffer.
int32_t read_i32leb(buffer_t* buf);

// Read an unsigned 64-bit LEB, advancing the buffer.
uint64_t read_u64leb(buffer_t* buf);

// Read a signed 64-bit LEB, advancing the buffer.
int64_t read_i64leb(buffer_t* buf);

// Read an unsigned 8-bit byte, advancing the {ptr} in the buffer.
uint8_t read_u8(buffer_t* buf);

// Read an unsigned 32-bit int, advancing the {ptr} in the buffer.
uint32_t read_u32(buffer_t* buf);

// Read a 64-bit unsigned int, advancing the buffer
uint64_t read_u64(buffer_t* buf);

// Read a name, advancing the buffer (len + string)
std::string read_name(buffer_t* buf);

// Read num_bytes, advancing the buffer
bytearr read_bytes(buffer_t* buf, uint32_t num_bytes);

// List get method
template <typename T>
inline T* get_list_elem(std::list<T> &list, uint32_t index) {
  auto it = std::next(list.begin(), index);
  return &(*it);
}

