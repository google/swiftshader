#
# Copyright 2015 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

# LLVM version for SwiftShader
SWIFTSHADER_LLVM_VERSION ?= 3


ifeq ($(SWIFTSHADER_LLVM_VERSION),3)
# Reactor with LLVM 3.0 doesn't support ARM.  Use Subzero as the Reactor JIT
# back-end on ARM.
ifeq ($(TARGET_ARCH),$(filter $(TARGET_ARCH),arm))
SWIFTSHADER_USE_SUBZERO := true
endif
endif


# Check whether SwiftShader requires full C++ 11 support.
ifdef SWIFTSHADER_USE_SUBZERO
SWIFTSHADER_REQUIRES_CXX11 := true
endif

ifeq ($(SWIFTSHADER_LLVM_VERSION),7)
SWIFTSHADER_REQUIRES_CXX11 := true
endif

ifeq ($(SWIFTSHADER_REQUIRES_CXX11),true)
# Full C++ 11 support is only available from Marshmallow and up.
ifeq ($(shell test $(PLATFORM_SDK_VERSION) -lt 23 && echo PreMarshmallow),PreMarshmallow)
SWIFTSHADER_UNSUPPORTED_BUILD := true
endif
endif


# Check whether $(TARGET_ARCH) is supported.
ifeq ($(SWIFTSHADER_LLVM_VERSION),3)
ifneq ($(TARGET_ARCH),$(filter $(TARGET_ARCH),x86 x86_64 arm))
SWIFTSHADER_UNSUPPORTED_BUILD := true
endif
endif

ifeq ($(SWIFTSHADER_LLVM_VERSION),7)
ifneq ($(TARGET_ARCH),$(filter $(TARGET_ARCH),x86 x86_64 arm arm64))
SWIFTSHADER_UNSUPPORTED_BUILD := true
endif
endif


ifndef SWIFTSHADER_UNSUPPORTED_BUILD
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
