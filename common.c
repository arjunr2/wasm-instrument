#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#include "common.h"

// The global trace flag
int g_trace = 0;
// The global disassembly flag
int g_disassemble = 0;

#define MORE(b) (((b) & 0x80) != 0)
#define ERROR (len != NULL ? *len = -(ptr - start) : 0), 0

#define BODY(type, mask, legal)						\
  const uint8_t* start = ptr;						\
  type result = 0, shift = 0;						\
  while (ptr < limit) {							\
    uint8_t b = *ptr;							\
    ptr++;								\
    result = result | (((type)b & 0x7F) << shift);			\
    shift += 7;								\
    if (shift > (8*sizeof(type))) {					\
      if (MORE(b) != 0) return ERROR; /* overlong */			\
      uint8_t upper = b & mask;						\
      if (upper != 0 && upper != legal) return ERROR; /* out of range */ \
      if (len != NULL) *len = (ptr - start);				\
      return result;							\
    }									\
    if (MORE(b)) continue;						\
    type rem = ((8*sizeof(type)) - shift);				\
    if (len != NULL) *len = (ptr - start);				\
    return ((0x7F & mask) == legal) ? (result << rem) >> rem : result;	\
  }									\
  return ERROR;



int32_t decode_i32leb(const uint8_t* ptr, const uint8_t* limit, ssize_t *len) {
  BODY(int32_t, 0xF8, 0x78);
}

uint32_t decode_u32leb(const uint8_t* ptr, const uint8_t* limit, ssize_t *len) {
  BODY(uint32_t, 0xF8, 0x08);
}

int64_t decode_i64leb(const uint8_t* ptr, const uint8_t* limit, ssize_t *len) {
  BODY(int64_t, 0xFF, 0x7F);
}

uint64_t decode_u64leb(const uint8_t* ptr, const uint8_t* limit, ssize_t *len) {
  BODY(int64_t, 0xFF, 0x01);
}

uint32_t decode_u32(const uint8_t* ptr, const uint8_t* limit, ssize_t *len) {
  ssize_t remain = limit - ptr;
  if (remain < 4) {
    if (remain > 0) *len = -remain;
    else *len = remain;
    return 0;
  }
  uint32_t b0 = ptr[0];
  uint32_t b1 = ptr[1];
  uint32_t b2 = ptr[2];
  uint32_t b3 = ptr[3];
  *len = 4;
  return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}


ssize_t load_file(const char* path, uint8_t** start, uint8_t** end) {
  // Open the file for reading.
  int fd = open(path, O_RDONLY);
  if (fd < 0) return fd;
  
  // Query the size of the file in bytes.
  struct stat statbuf;
  int r = fstat(fd, &statbuf);
  if (r < 0) { // stat call failed.
    close(fd);
    return r;
  }
  
  // Reject overly large files.
  off_t size = statbuf.st_size;
  if (size > MAX_FILE_SIZE) { // too large.
    close(fd);
    return -1;
  }
  
  // Allocate memory to store the bytes.
  *start = (uint8_t*)malloc(size);
  *end = *start + size;
  off_t remain = size;
  // Read the bytes from the file until done.
  for (uint8_t* p = *start; remain > 0; ) {
    r = read(fd, p, remain);
    if (r < 0) { // reading failed.
      close(fd);
      return -2;
    }
    remain -= r;
    p += r;
  }
  return (ssize_t)size;
}

// Read an unsigned 32-bit LEB, advancing the buffer.
uint32_t read_u32leb(buffer_t* buf) {
  ssize_t leblen = 0;
  if (buf->ptr >= buf->end) return 0;
  uint32_t val = decode_u32leb(buf->ptr, buf->end, &leblen);
  if (leblen <= 0) buf->ptr = buf->end; // force failure
  else buf->ptr += leblen;
  return val;
}

// Read a signed 32-bit LEB, advancing the buffer.
int32_t read_i32leb(buffer_t* buf) {
  ssize_t leblen = 0;
  if (buf->ptr >= buf->end) return 0;
  int32_t val = decode_i32leb(buf->ptr, buf->end, &leblen);
  if (leblen <= 0) buf->ptr = buf->end; // force failure
  else buf->ptr += leblen;
  return val;
}

// Read a 64-bit unsigned int, advancing the buffer
uint64_t read_u64(buffer_t* buf) {
  ssize_t len = 0;
  if (buf->ptr + 8 > buf->end) {
    ERR("u64 read out of bounds");
    return 0;
  }
  uint32_t low = decode_u32(buf->ptr, buf->end, &len);
  buf->ptr += len;
  uint32_t high = decode_u32(buf->ptr, buf->end, &len);
  buf->ptr += len;
  uint64_t val = ((uint64_t)(high) << 32) | low;
  return val;
}

// Read a string of length n, advancing the buffer
char* read_string(buffer_t* buf, uint32_t* len) {
  uint32_t sz = read_u32leb(buf);
  if (buf->ptr + sz > buf->end) {
    ERR("string read out of bounds\n");
    return NULL;
  }
  
  char *res = (char*) malloc((sz + 1) * sizeof(char));
  memcpy(res, buf->ptr, sz);
  res[sz] = 0;

  buf->ptr += sz;
  *len = sz;
  return res;  

}

// Read an unsigned 8-bit byte, advancing the buffer.
uint8_t read_u8(buffer_t* buf) {
  if (buf->ptr >= buf->end) {
    ERR("u8 read out of bounds");
    return 0;
  }
  byte val = *buf->ptr;
  buf->ptr++;
  return val;
}

// Read an unsigned 32-bit byte, advancing the buffer.
uint32_t read_u32(buffer_t* buf) {
  ssize_t len;
  uint32_t val = decode_u32(buf->ptr, buf->ptr + 4, &len);
  buf->ptr += 4;
  return val;
}


/* String operations */
string stralloc() {
  string s;
  s.cap = 16;
  s.v = (char*) malloc(s.cap);
  s.v[0] = 0;
  return s;
}

static uint32_t nextpow2(uint32_t val) {
  val--;
  val |= (val >> 1);
  val |= (val >> 2);
  val |= (val >> 4);
  val |= (val >> 8);
  val |= (val >> 16);
  val++;
  return val;
}

static string grow_string(string s, uint32_t fin_len) {
  uint32_t new_size = nextpow2(fin_len);
  if (new_size == 0) {
    new_size = fin_len;
  }
  s.v = (char*) realloc(s.v, new_size);
  s.cap = new_size;
  return s;
}

string strappend(string s, const char* c) {
  uint32_t len1 = strlen(s.v);
  uint32_t len2 = strlen(c);
  uint32_t fin_len = len1 + len2 + 1;
  if (fin_len > s.cap) {
    s = grow_string(s, fin_len);
  }
  strcat(s.v, c);
  return s;
}

string strappend_int32(string s, uint32_t val, bool sgn) {
  char buffer[15];
  if (sgn) {
    sprintf(buffer, "%d ", val);
  } else {
    sprintf(buffer, "%u ", val);
  }
  return strappend(s, buffer);
}

string strappend_byte(string s, byte val) {
  char buffer[4];
  sprintf(buffer, "%02X ", val);
  return strappend(s, buffer);
}

string strappend_hex64(string s, uint64_t val) {
  char buffer[20];
  sprintf(buffer, "%016lX ", val);
  return strappend(s, buffer);
}

string strclear(string s) {
  s.v[0] = 0;
  return s;
}

string strip_chars(string s, int n) {
  uint32_t len = strlen(s.v);
  s.v[len - n] = 0;
  return s;
}


void strdelete(string s) {
  free(s.v);
}
/*   */


ssize_t unload_file(uint8_t** start, uint8_t** end) {
  free(*start);
  *start = NULL;
  *end = NULL;
  return 0;
}
