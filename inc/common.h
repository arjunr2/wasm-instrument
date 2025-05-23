#pragma once
#include <array>
#include <deque>
#include <list>
#include <stdbool.h>
#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <vector>

#include "api/export/instrument_c_api.h"

/*** Generic used typedefs ***/
typedef uint8_t byte;
typedef std::vector<byte> bytearr;
typedef std::deque<byte> bytedeque;
typedef uint32_t Opcode_t;
/* Buffer for parsing/decoding */
typedef struct {
    const byte *start;
    const byte *ptr;
    const byte *end;
} buffer_t;
/***************/

extern int g_threads;
/*** Global trace/err/disassemble flags and macros. ***/
#define TRACE(...)                                                             \
    do {                                                                       \
        if (g_trace)                                                           \
            fprintf(stderr, __VA_ARGS__);                                      \
    } while (0)
#define ERR(...)                                                               \
    {                                                                          \
        char internal_buf[400];                                                \
        char outerr[500];                                                      \
        sprintf(internal_buf, __VA_ARGS__);                                    \
        sprintf(outerr, "\033[1;31m%s\033[0m", internal_buf);                  \
        fprintf(stderr, "%s", outerr);                                         \
    }
#define WARN(...)                                                              \
    {                                                                          \
        char internal_buf[400];                                                \
        char outerr[500];                                                      \
        sprintf(internal_buf, __VA_ARGS__);                                    \
        sprintf(outerr, "\033[1;33m%s\033[0m", internal_buf);                  \
        fprintf(stderr, "%s", outerr);                                         \
    }
extern int g_trace;
/***************/

extern int g_time;
/*** Timing macros ***/
#define TIME_SECTION(nest, time_logstr, ...)                                   \
    {                                                                          \
        start_time = std::chrono::high_resolution_clock::now();                \
    }                                                                          \
    __VA_ARGS__;                                                               \
    {                                                                          \
        end_time = std::chrono::high_resolution_clock::now();                  \
        if (g_time) {                                                          \
            auto elapsed =                                                     \
                std::chrono::duration_cast<std::chrono::milliseconds>(         \
                    end_time - start_time);                                    \
            printf("%*s%-25s:%8ld ms\n", nest * 4, "", time_logstr,            \
                   elapsed.count());                                           \
            fflush(stdout);                                                    \
        }                                                                      \
    }

#define DEF_TIME_VAR()                                                         \
    auto start_time = std::chrono::high_resolution_clock::now();               \
    auto end_time = std::chrono::high_resolution_clock::now();
/***************/

/*** Parsing macros ***/
#define RD_U32() read_u32leb(&buf)
#define RD_I32() read_i32leb(&buf)
#define RD_U64() read_u64leb(&buf)
#define RD_I64() read_i64leb(&buf)
#define RD_NAME() read_name(&buf)
#define RD_BYTESTR(len) read_bytes(&buf, len)

#define RD_BYTE() read_u8(&buf)
#define RD_U32_RAW() read_u32(&buf)
#define RD_U64_RAW() read_u64(&buf)

#define VALIDATE_OP(b)                                                         \
    {                                                                          \
        opcode_entry_t op_entry = opcode_table[b];                             \
        if (op_entry.invalid || (op_entry.mnemonic == 0)) {                    \
            ERR("Unimplemented opcode %d: %s\n", b, op_entry.mnemonic);        \
            throw std::runtime_error("Unimplemented");                         \
        }                                                                      \
    }

#define RD_U32_RAW_BE()                                                        \
    ({                                                                         \
        uint32_t res = 0;                                                      \
        byte opclass;                                                          \
        byte by = opclass = res = RD_BYTE();                                   \
        VALIDATE_OP(opclass);                                                  \
        if ((opclass >= 0xFB) && (opclass <= 0xFE)) {                          \
            while (by & 0x80) {                                                \
                by = RD_BYTE();                                                \
                res = ((res << 8) + by);                                       \
            }                                                                  \
        }                                                                      \
        res;                                                                   \
    })

#define RD_OPCODE()                                                            \
    ({                                                                         \
        Opcode_t lb = RD_U32_RAW_BE();                                         \
        VALIDATE_OP(lb);                                                       \
        lb;                                                                    \
    })
/********************/

/*** Encoding macros ***/
#define WR_U32(val) encode_u32leb(bdeq, val)
#define WR_I32(val) encode_i32leb(bdeq, val)
#define WR_U64(val) encode_u64leb(bdeq, val)
#define WR_I64(val) encode_i64leb(bdeq, val)
#define WR_NAME(val) encode_name(bdeq, val)
#define WR_BYTESTR(val) encode_bytes(bdeq, val)

#define WR_BYTE(val) encode_u8(bdeq, val)
#define WR_U32_RAW(val) encode_u32(bdeq, val)
#define WR_U64_RAW(val) encode_u64(bdeq, val)

#define WR_SECBYTE_PRE(val) preencode_u8(secdeq, val)
#define WR_SECLEN_PRE() preencode_u32leb(secdeq, secdeq.size())

#define WR_U32_RAW_BE(val)                                                     \
    ({                                                                         \
        int32_t shf = 0;                                                       \
        while ((val >> (shf + 8)) & 0xFF) {                                    \
            shf += 8;                                                          \
        }                                                                      \
        byte opclass = ((val >> shf) & 0xFF);                                  \
        VALIDATE_OP(opclass);                                                  \
        WR_BYTE(opclass);                                                      \
        shf -= 8;                                                              \
        if ((opclass >= 0xFB) && (opclass <= 0xFE)) {                          \
            while (shf >= 0) {                                                 \
                WR_BYTE((val >> shf) & 0xFF);                                  \
                shf -= 8;                                                      \
            }                                                                  \
        }                                                                      \
    })

#define WR_OPCODE(op_in)                                                       \
    {                                                                          \
        Opcode_t lb = op_in;                                                   \
        VALIDATE_OP(lb);                                                       \
        WR_U32_RAW_BE(lb);                                                     \
    }
/********************/

/*** List macros ***/
#define PRINT_LIST_PTRS(ll)                                                    \
    {                                                                          \
        uint32_t i = 0;                                                        \
        for (auto &it : ll) {                                                  \
            ERR("Idx: %u | Ptr: %p\n", i, &it);                                \
            i++;                                                               \
        }                                                                      \
        ERR("\n");                                                             \
    }

#define GET_LIST_ELEM(ll, idx)                                                 \
    ({                                                                         \
        auto it = std::next(ll.begin(), idx);                                  \
        &(*it);                                                                \
    })

#define GET_LIST_IDX(ll, ptr)                                                  \
    ({                                                                         \
        auto it = ll.begin();                                                  \
        uint32_t i = 0;                                                        \
        for (it = ll.begin(); it != ll.end(); ++it, ++i) {                     \
            if (&(*it) == ptr) {                                               \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        if (it == ll.end()) {                                                  \
            ERR("Ptr val: %p\n", ptr);                                         \
            PRINT_LIST_PTRS(ll);                                               \
            throw std::runtime_error("Element not found in list!\n");          \
        }                                                                      \
        i;                                                                     \
    })

#define GET_DEQUE_ELEM(deq, idx) ({ &deq[idx]; })
#define GET_DEQUE_IDX(deq, ptr)                                                \
    ({                                                                         \
        auto res = std::find_if(deq.begin(), deq.end(),                        \
                                [ptr](auto &i) { return ptr == &i; });         \
        std::distance(deq.begin(), res);                                       \
    })

/********************/

/* Load a file into memory; initialize start and end.
 * Returns < 0 on failure. */
ssize_t load_file(const char *path, byte **start, byte **end);

/* Unload a file previously loaded into memory using {load_file}. */
ssize_t unload_file(byte **start, byte **end);

/*** Decode Operations ***/

#define MAX_FILE_SIZE 2000000000

/* Decode LEB128 values, enforcing the specified signedness and maximum range.
 * The argument {ptr} will be advanced to point after the decoded value.
 * If an error occurs (e.g. unterminated LEB, invalid value), {ptr} will be set
 * to NULL. */
int32_t decode_i32leb(const byte *ptr, const byte *limit, ssize_t *len);
uint32_t decode_u32leb(const byte *ptr, const byte *limit, ssize_t *len);
int64_t decode_i64leb(const byte *ptr, const byte *limit, ssize_t *len);
uint64_t decode_u64leb(const byte *ptr, const byte *limit, ssize_t *len);

/* Decode fixed-size integer values. */
uint32_t decode_u32(const byte *ptr, const byte *limit, ssize_t *len);

/* Read an unsigned(u)/signed(i) X-bit LEB, advancing the {ptr} in buffer */
uint32_t read_u32leb(buffer_t *buf);
int32_t read_i32leb(buffer_t *buf);
uint64_t read_u64leb(buffer_t *buf);
int64_t read_i64leb(buffer_t *buf);

/* Read a raw X-bit value, advancing the {ptr} in buffer*/
uint8_t read_u8(buffer_t *buf);
uint32_t read_u32(buffer_t *buf);
uint64_t read_u64(buffer_t *buf);

/* Read a name (32leb + string), advancing the {ptr} in buffer */
std::string read_name(buffer_t *buf);
/* Read num_bytes, advancing the {ptr} in buffer */
bytearr read_bytes(buffer_t *buf, uint32_t num_bytes);

/*** Encode Operations ***/

/* Encode into LEB128 values, enforcing the specified signedness and maximum
 * range. Bit-width not necessary, but added for readability and params */
void encode_i32leb(bytedeque &bdeq, int32_t val);
void encode_u32leb(bytedeque &bdeq, uint32_t val);
void encode_i64leb(bytedeque &bdeq, int64_t val);
void encode_u64leb(bytedeque &bdeq, uint64_t val);

/* Encode fixed size byte values */
void encode_u8(bytedeque &bdeq, uint8_t val);
void encode_u32(bytedeque &bdeq, uint32_t val);
void encode_u64(bytedeque &bdeq, uint64_t val);

void encode_name(bytedeque &bdeq, std::string name);
void encode_bytes(bytedeque &bdeq, bytearr &bytes);

/* Prepend encoding */
void preencode_u32leb(bytedeque &bdeq, uint32_t val);
void preencode_u8(bytedeque &bdeq, uint8_t val);
