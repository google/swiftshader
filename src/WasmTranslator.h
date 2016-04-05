//===- subzero/src/WasmTranslator.h - WASM to Subzero Translation ---------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares a driver for translating Wasm bitcode into PNaCl bitcode.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_WASMTRANSLATOR_H
#define SUBZERO_SRC_WASMTRANSLATOR_H

#include "IceGlobalContext.h"
#include "IceTranslator.h"

namespace v8 {
namespace internal {
class Zone;
namespace wasm {
class FunctionEnv;
} // end of namespace wasm
} // end of namespace internal
} // end of namespace v8

namespace Ice {

class WasmTranslator : public Translator {
  WasmTranslator() = delete;
  WasmTranslator(const WasmTranslator &) = delete;
  WasmTranslator &operator=(const WasmTranslator &) = delete;

  template <typename F = std::function<void(Ostream &)>> void log(F Fn) {
    if (BuildDefs::dump() && (getFlags().getVerbose() & IceV_Wasm)) {
      Fn(Ctx->getStrDump());
      Ctx->getStrDump().flush();
    }
  }

public:
  explicit WasmTranslator(GlobalContext *Ctx);

  void translate(const std::string &IRFilename,
                 std::unique_ptr<llvm::DataStreamer> InputStream);

  /// Translates a single Wasm function.
  ///
  /// Parameters:
  ///   Zone - an arena for the V8 code to allocate from.
  ///   Env - information about the function (signature, variable count, etc.).
  ///   Base - a pointer to the start of the Wasm module.
  ///   Start - a pointer to the start of the function within the module.
  ///   End - a pointer to the end of the function.
  std::unique_ptr<Cfg> translateFunction(v8::internal::Zone *Zone,
                                         v8::internal::wasm::FunctionEnv *Env,
                                         const uint8_t *Base,
                                         const uint8_t *Start,
                                         const uint8_t *End);

private:
  std::unique_ptr<uint8_t[]> Buffer;
  SizeT BufferSize;
};
}
#endif // SUBZERO_SRC_WASMTRANSLATOR_H
