#include "encode.h"
#include "ir.h"

/* Main Encoder routine */
bytedeque WasmModule::encode_module() {
  bytedeque bdeq;
  WR_U32_RAW (this->magic);
  WR_U32_RAW (this->version);

  return bdeq;
}
