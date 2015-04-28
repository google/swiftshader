LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CLANG := true

LOCAL_MODULE_PATH := $(TARGET_OUT)/vendor/lib/egl
LOCAL_MODULE := libGLESv2_swiftshader

LOCAL_SRC_FILES += \
	Buffer.cpp \
	Context.cpp \
	Device.cpp \
	Fence.cpp \
	Framebuffer.cpp \
	IndexDataManager.cpp \
	libGLESv2.cpp \
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

LOCAL_CFLAGS += -DLOG_TAG=\"libGLESv2_swiftshader\"

# Android's make system also uses NDEBUG, so we need to set/unset it forcefully
# Uncomment for ON:
LOCAL_CFLAGS += -UNDEBUG -g -O0
# Uncomment for OFF:
#LOCAL_CFLAGS += -fomit-frame-pointer -ffunction-sections -fdata-sections -DANGLE_DISABLE_TRACE

LOCAL_CFLAGS += -fno-operator-names -msse2 -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS
LOCAL_CFLAGS += -std=c++11

LOCAL_SHARED_LIBRARIES += libdl liblog libcutils libhardware libui libutils \
    $(GCE_STLPORT_LIBS)

LOCAL_STATIC_LIBRARIES += swiftshader_compiler swiftshader_top libLLVM_swiftshader
LOCAL_LDFLAGS += -Wl,--gc-sections -Wl,--version-script=$(LOCAL_PATH)/exports.map -Wl,--hash-style=sysv

LOCAL_C_INCLUDES += \
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

include external/stlport/libstlport.mk

include $(BUILD_SHARED_LIBRARY)
