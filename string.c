#include <string.h>
#include <stdio.h>

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
