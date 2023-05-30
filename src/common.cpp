#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#include "common.h"

/* The global flags */
int g_trace = 0;
int g_time = 0;
int g_threads = 0;

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


ssize_t unload_file(uint8_t** start, uint8_t** end) {
  free(*start);
  *start = NULL;
  *end = NULL;
  return 0;
}


/*** Decode Operations ***/

#define MORE(b) (((b) & 0x80) != 0)
#define ERROR (len != NULL ? *len = -(ptr - start) : 0), 0

#define DECODE_BODY(type, mask, legal)						\
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
  DECODE_BODY(int32_t, 0xF8, 0x78);
}

uint32_t decode_u32leb(const uint8_t* ptr, const uint8_t* limit, ssize_t *len) {
  DECODE_BODY(uint32_t, 0xF8, 0x08);
}

int64_t decode_i64leb(const uint8_t* ptr, const uint8_t* limit, ssize_t *len) {
  DECODE_BODY(int64_t, 0xFF, 0x7F);
}

uint64_t decode_u64leb(const uint8_t* ptr, const uint8_t* limit, ssize_t *len) {
  DECODE_BODY(uint64_t, 0xFF, 0x01);
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


/* Unsigned-32 LEB */
uint32_t read_u32leb(buffer_t* buf) {
  ssize_t leblen = 0;
  if (buf->ptr >= buf->end) return 0;
  uint32_t val = decode_u32leb(buf->ptr, buf->end, &leblen);
  if (leblen <= 0) buf->ptr = buf->end; // force failure
  else buf->ptr += leblen;
  return val;
}

/* Signed-32 LEB */
int32_t read_i32leb(buffer_t* buf) {
  ssize_t leblen = 0;
  if (buf->ptr >= buf->end) return 0;
  int32_t val = decode_i32leb(buf->ptr, buf->end, &leblen);
  if (leblen <= 0) buf->ptr = buf->end; // force failure
  else buf->ptr += leblen;
  return val;
}

/* Unsigned-64 LEB */
uint64_t read_u64leb(buffer_t* buf) {
  ssize_t leblen = 0;
  if (buf->ptr >= buf->end) return 0;
  uint64_t val = decode_u64leb(buf->ptr, buf->end, &leblen);
  if (leblen <= 0) buf->ptr = buf->end; // force failure
  else buf->ptr += leblen;
  return val;
}

/* Signed-64 LEB */
int64_t read_i64leb(buffer_t* buf) {
  ssize_t leblen = 0;
  if (buf->ptr >= buf->end) return 0;
  int64_t val = decode_i64leb(buf->ptr, buf->end, &leblen);
  if (leblen <= 0) buf->ptr = buf->end; // force failure
  else buf->ptr += leblen;
  return val;
}

/* Raw byte */
uint8_t read_u8(buffer_t* buf) {
  if (buf->ptr >= buf->end) {
    ERR("u8 read out of bounds");
    return 0;
  }
  byte val = *buf->ptr;
  buf->ptr++;
  return val;
}

/* Raw-32 */
uint32_t read_u32(buffer_t* buf) {
  ssize_t len;
  uint32_t val = decode_u32(buf->ptr, buf->ptr + 4, &len);
  buf->ptr += 4;
  return val;
}

/* Raw-64 */
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


/* Name (Unsigned-32leb + string) */
std::string read_name(buffer_t* buf) {
  uint32_t sz = read_u32leb(buf);
  if (buf->ptr + sz > buf->end) {
    ERR("string read out of bounds\n");
    return NULL;
  }
 
  std::string str((const char*) buf->ptr, (size_t) sz);
  buf->ptr += sz;
  return str;  

}

/* Raw-{num_bytes} */ 
bytearr read_bytes(buffer_t* buf, uint32_t num_bytes) {
  if (buf->ptr + num_bytes > buf->end) {
    ERR("bytes read out of bounds\n");
    return bytearr(buf->ptr, buf->ptr);
  }
  bytearr bytes(buf->ptr, buf->ptr + num_bytes);
  buf->ptr += num_bytes;
  return bytes;
}




/*** Encode Operations ***/
#define MORE_ENC()   b |= 0x80
#define MORE_SIGNED(val, b) \
  if ( !( ((val == 0) && !(b & 0x40)) || \
          ((val == -1) && (b & 0x40)) )) \
          { MORE_ENC(); }

#define MORE_UNSIGNED(val, b)  \
  if (val != 0) { MORE_ENC(); }


#define ENCODE_BODY(sign)  \
  byte b = 0xFF; \
  while (b & 0x80) { \
    b = val & 0x7F; \
    val >>= 7;  \
    MORE_##sign(val, b);  \
    bdeq.push_back(b);  \
  }

#define PREENCODE_BODY(sign)  \
  bytearr bint; \
  byte b = 0xFF; \
  while (b & 0x80) { \
    b = val & 0x7F; \
    val >>= 7;  \
    MORE_##sign(val, b);  \
    bint.push_back(b);  \
  } \
  bdeq.insert(bdeq.begin(), bint.begin(), bint.end());


void encode_i32leb (bytedeque &bdeq, int32_t val) {
  ENCODE_BODY(SIGNED);
}

void encode_u32leb (bytedeque &bdeq, uint32_t val) {
  ENCODE_BODY(UNSIGNED);
}

void encode_i64leb (bytedeque &bdeq, int64_t val) {
  ENCODE_BODY(SIGNED);
}

void encode_u64leb (bytedeque &bdeq, uint64_t val) {
  ENCODE_BODY(UNSIGNED);
}

#define ENCODE_RAW(val) { \
  char* addr = (char*) &val;  \
  bdeq.insert(bdeq.end(), addr, addr + sizeof(val));  \
}

void encode_u8 (bytedeque &bdeq, uint8_t val) {
  bdeq.push_back(val);
}

void encode_u32 (bytedeque &bdeq, uint32_t val) {
  ENCODE_RAW(val);
}

void encode_u64 (bytedeque &bdeq, uint64_t val) {
  ENCODE_RAW(val);
}

void encode_name (bytedeque &bdeq, std::string name) {
  const char* addr = name.c_str();
  uint32_t sz = name.size();
  encode_u32leb(bdeq, sz);
  bdeq.insert(bdeq.end(), addr, addr + sz);
}

void encode_bytes (bytedeque &bdeq, bytearr &bytes) {
  bdeq.insert(bdeq.end(), bytes.begin(), bytes.end());
}


void preencode_u32leb (bytedeque &bdeq, uint32_t val) {
  PREENCODE_BODY(UNSIGNED);
}
void preencode_u8 (bytedeque &bdeq, uint8_t val) {
  bdeq.push_front(val);
}
