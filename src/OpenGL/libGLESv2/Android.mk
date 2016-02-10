LOCAL_PATH:= $(call my-dir)

COMMON_CFLAGS := \
	-DLOG_TAG=\"libGLESv2_swiftshader\" \
	-fno-operator-names \
	-msse2 \
	-D__STDC_CONSTANT_MACROS \
	-D__STDC_LIMIT_MACROS \
	-std=c++11 \
	-DGL_API= \
	-DGL_APICALL= \
	-DGL_GLEXT_PROTOTYPES \
	-Wno-unused-parameter \
	-Wno-implicit-exception-spec-mismatch \
	-Wno-overloaded-virtual

ifneq (16,${PLATFORM_SDK_VERSION})
COMMON_CFLAGS += -Xclang -fuse-init-array
else
COMMON_CFLAGS += -D__STDC_INT64__
endif

COMMON_SRC_FILES := \
	Buffer.cpp \
	Context.cpp \
	Device.cpp \
	Fence.cpp \
	Framebuffer.cpp \
	IndexDataManager.cpp \
	libGLESv2.cpp \
	libGLESv3.cpp \
	main.cpp \
	Program.cpp \
	Query.cpp \
	Renderbuffer.cpp \
	ResourceManager.cpp \
	Shader.cpp \
	Texture.cpp \
	TransformFeedback.cpp \
	utilities.cpp \
	VertexArray.cpp \
	VertexDataManager.cpp \

COMMON_C_INCLUDES := \
	bionic \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../ \
	$(LOCAL_PATH)/../../ \
	$(LOCAL_PATH)/../../LLVM/include-android \
	$(LOCAL_PATH)/../../LLVM/include-linux \
	$(LOCAL_PATH)/../../LLVM/include \
	$(LOCAL_PATH)/../../LLVM/lib/Target/X86 \
	$(LOCAL_PATH)/../../Renderer/ \
	$(LOCAL_PATH)/../../Common/ \
	$(LOCAL_PATH)/../../Shader/ \
	$(LOCAL_PATH)/../../Main/

COMMON_STATIC_LIBRARIES := \
	libLLVM_swiftshader

COMMON_SHARED_LIBRARIES := \
	libdl \
	liblog \
	libcutils \
	libhardware \
	libui \
	libutils

# Marshmallow does not have stlport, but comes with libc++ by default
ifeq ($(shell test $(PLATFORM_SDK_VERSION) -lt 23 && echo PreMarshmallow),PreMarshmallow)
COMMON_SHARED_LIBRARIES += libstlport
COMMON_C_INCLUDES += external/stlport/stlport
endif

COMMON_LDFLAGS := \
	-Wl,--gc-sections \
	-Wl,--version-script=$(LOCAL_PATH)/exports.map \
	-Wl,--hash-style=sysv

include $(CLEAR_VARS)
LOCAL_MODULE := libGLESv2_swiftshader_vendor_debug
LOCAL_MODULE_PATH := vendor/transgaming/swiftshader/$(TARGET_ARCH)/debug/obj
LOCAL_UNSTRIPPED_PATH := vendor/transgaming/swiftshader/$(TARGET_ARCH)/debug/sym
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := libGLESv2_swiftshader.so
LOCAL_CFLAGS += $(COMMON_CFLAGS) -UNDEBUG -g -O0
LOCAL_CLANG := true
LOCAL_SRC_FILES += $(COMMON_SRC_FILES)
LOCAL_C_INCLUDES += $(COMMON_C_INCLUDES)
LOCAL_STATIC_LIBRARIES += swiftshader_compiler_debug swiftshader_top_debug $(COMMON_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES += $(COMMON_SHARED_LIBRARIES)
LOCAL_LDFLAGS += $(COMMON_LDFLAGS)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libGLESv2_swiftshader_vendor_release
LOCAL_MODULE_PATH := vendor/transgaming/swiftshader/$(TARGET_ARCH)/release/obj
LOCAL_UNSTRIPPED_PATH := vendor/transgaming/swiftshader/$(TARGET_ARCH)/release/sym
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := libGLESv2_swiftshader.so
LOCAL_CFLAGS += \
	$(COMMON_CFLAGS) \
	-fomit-frame-pointer \
	-ffunction-sections \
	-fdata-sections \
	-DANGLE_DISABLE_TRACE
LOCAL_CLANG := true
LOCAL_SRC_FILES += $(COMMON_SRC_FILES)
LOCAL_C_INCLUDES += $(COMMON_C_INCLUDES)
LOCAL_STATIC_LIBRARIES += swiftshader_compiler_release swiftshader_top_release $(COMMON_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES += $(COMMON_SHARED_LIBRARIES)
LOCAL_LDFLAGS += $(COMMON_LDFLAGS)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libGLESv2_swiftshader
ifdef TARGET_2ND_ARCH
LOCAL_MODULE_PATH_32 := $(TARGET_OUT_VENDOR)/lib/egl
LOCAL_MODULE_PATH_64 := $(TARGET_OUT_VENDOR)/lib64/egl
else
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/lib/egl
endif
LOCAL_MODULE_TAGS := optional
LOCAL_CLANG := true
LOCAL_SRC_FILES += $(COMMON_SRC_FILES)
LOCAL_C_INCLUDES += $(COMMON_C_INCLUDES)
LOCAL_STATIC_LIBRARIES += swiftshader_compiler_$(SWIFTSHADER_OPTIM) swiftshader_top_$(SWIFTSHADER_OPTIM) $(COMMON_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES += $(COMMON_SHARED_LIBRARIES)
LOCAL_LDFLAGS += $(COMMON_LDFLAGS)
ifeq (debug,$(SWIFTSHADER_OPTIM))
LOCAL_CFLAGS += $(COMMON_CFLAGS) -UNDEBUG -g -O0
else
LOCAL_CFLAGS += \
	$(COMMON_CFLAGS) \
	-fomit-frame-pointer \
	-ffunction-sections \
	-fdata-sections \
	-DANGLE_DISABLE_TRACE
endif
include $(BUILD_SHARED_LIBRARY)
