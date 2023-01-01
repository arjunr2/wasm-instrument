#pragma once 

/* String operations */
#include <stdint.h>
#include "common.h"

typedef struct {
  char *v;
  uint32_t cap;
} string;

string stralloc();

string strappend(string s, const char* c);

string strappend_int32(string s, uint32_t val, bool sgn);
string strappend_byte(string s, byte val);
string strappend_hex64(string s, uint64_t val);

string strclear(string s);

string strip_chars(string s, int n);

void strdelete(string s);

