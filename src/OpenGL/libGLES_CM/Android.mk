LOCAL_PATH:= $(call my-dir)

COMMON_CFLAGS := \
	-DLOG_TAG=\"libGLES_CM_swiftshader\" \
	-std=c++11 \
	-fno-operator-names \
	-msse2 \
	-D__STDC_CONSTANT_MACROS \
	-D__STDC_LIMIT_MACROS \
	-DEGLAPI= \
	-DGL_API= \
	-DGL_APICALL= \
	-DGL_GLEXT_PROTOTYPES


COMMON_SRC_FILES := \
	Buffer.cpp \
	Context.cpp \
	Device.cpp \
	Framebuffer.cpp \
	IndexDataManager.cpp \
	libGLES_CM.cpp \
	main.cpp \
	Renderbuffer.cpp \
	ResourceManager.cpp \
	Texture.cpp \
	utilities.cpp \
	VertexDataManager.cpp

COMMON_C_INCLUDES := \
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

COMMON_STATIC_LIBRARIES := swiftshader_top libLLVM_swiftshader

COMMON_SHARED_LIBRARIES := \
	libdl \
	liblog \
	libcutils \
	libhardware \
	libui \
	libutils \
	$(GCE_STLPORT_LIBS)

COMMON_LDFLAGS := \
	-Wl,--gc-sections \
	-Wl,--version-script=$(LOCAL_PATH)/exports.map \
	-Wl,--hash-style=sysv

include $(CLEAR_VARS)

LOCAL_MODULE_PATH := vendor/transgaming/swiftshader/$(TARGET_ARCH)/debug/obj
LOCAL_UNSTRIPPED_PATH := vendor/transgaming/swiftshader/$(TARGET_ARCH)/debug/sym
LOCAL_MODULE := libGLESv1_CM_swiftshader_vendor_debug
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := libGLESv1_CM_swiftshader.so
LOCAL_CFLAGS += $(COMMON_CFLAGS) -UNDEBUG -g -O0

LOCAL_CLANG := true
LOCAL_SRC_FILES += $(COMMON_SRC_FILES)
LOCAL_C_INCLUDES += $(COMMON_C_INCLUDES)
LOCAL_STATIC_LIBRARIES += $(COMMON_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES += $(COMMON_SHARED_LIBRARIES)
LOCAL_LDFLAGS += $(COMMON_LDFLAGS)
include external/stlport/libstlport.mk
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE_PATH := vendor/transgaming/swiftshader/$(TARGET_ARCH)/release/obj
LOCAL_UNSTRIPPED_PATH := vendor/transgaming/swiftshader/$(TARGET_ARCH)/release/sym
LOCAL_MODULE := libGLESv1_CM_swiftshader_vendor_release
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := libGLESv1_CM_swiftshader.so
LOCAL_CFLAGS += \
	$(COMMON_CFLAGS) \
	-fomit-frame-pointer \
	-ffunction-sections \
	-fdata-sections \
	-DANGLE_DISABLE_TRACE

LOCAL_CLANG := true
LOCAL_SRC_FILES += $(COMMON_SRC_FILES)
LOCAL_C_INCLUDES += $(COMMON_C_INCLUDES)
LOCAL_STATIC_LIBRARIES += $(COMMON_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES += $(COMMON_SHARED_LIBRARIES)
LOCAL_LDFLAGS += $(COMMON_LDFLAGS)
include external/stlport/libstlport.mk
include $(BUILD_SHARED_LIBRARY)
