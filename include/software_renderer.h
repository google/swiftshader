// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_SWIFTSHADER_INCLUDE_SOFTWARE_RENDERER_H_
#define THIRD_PARTY_SWIFTSHADER_INCLUDE_SOFTWARE_RENDERER_H_

void SetupSoftwareRenderer(base::NativeLibrary egl_library) {
  typedef void (__stdcall *RegisterFunc)(char* key);
  RegisterFunc reg = reinterpret_cast<RegisterFunc>(
      base::GetFunctionPointerFromNativeLibrary(egl_library, "Register"));
  // Secret registration key so that Swift Shader doesn't display
  // watermarks.
  reg("SS3GCKK6B448CF63");
}

#endif  // THIRD_PARTY_SWIFTSHADER_INCLUDE_SOFTWARE_RENDERER_H_

