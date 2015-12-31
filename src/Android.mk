LOCAL_PATH:= $(call my-dir)

COMMON_C_INCLUDES += \
	bionic \
	$(LOCAL_PATH)/OpenGL/include \
	$(LOCAL_PATH)/OpenGL/ \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/Renderer/ \
	$(LOCAL_PATH)/Common/ \
	$(LOCAL_PATH)/Shader/ \
	$(LOCAL_PATH)/LLVM/include \
	$(LOCAL_PATH)/Main/

# Marshmallow does not have stlport, but comes with libc++ by default
ifeq ($(shell test $(PLATFORM_SDK_VERSION) -lt 23 && echo PreMarshmallow),PreMarshmallow)
COMMON_C_INCLUDES += external/stlport/stlport
endif

COMMON_SRC_FILES := \
	Common/CPUID.cpp \
	Common/Configurator.cpp \
	Common/DebugAndroid.cpp \
	Common/GrallocAndroid.cpp \
	Common/Half.cpp \
	Common/Math.cpp \
	Common/Memory.cpp \
	Common/Resource.cpp \
	Common/Socket.cpp \
	Common/Thread.cpp \
	Common/Timer.cpp

COMMON_SRC_FILES += \
	Main/Config.cpp \
	Main/FrameBuffer.cpp \
	Main/FrameBufferAndroid.cpp \
	Main/Logo.cpp \
	Main/Register.cpp \
	Main/SwiftConfig.cpp \
	Main/crc.cpp \
	Main/serialvalid.cpp \

COMMON_SRC_FILES += \
	Reactor/Nucleus.cpp \
	Reactor/Routine.cpp \
	Reactor/RoutineManager.cpp

COMMON_SRC_FILES += \
	Renderer/Blitter.cpp \
	Renderer/Clipper.cpp \
	Renderer/Color.cpp \
	Renderer/Context.cpp \
	Renderer/ETC_Decoder.cpp \
	Renderer/Matrix.cpp \
	Renderer/PixelProcessor.cpp \
	Renderer/Plane.cpp \
	Renderer/Point.cpp \
	Renderer/QuadRasterizer.cpp \
	Renderer/Rasterizer.cpp \
	Renderer/Renderer.cpp \
	Renderer/Sampler.cpp \
	Renderer/SetupProcessor.cpp \
	Renderer/Surface.cpp \
	Renderer/TextureStage.cpp \
	Renderer/Vector.cpp \
	Renderer/VertexProcessor.cpp \

COMMON_SRC_FILES += \
	Shader/Constants.cpp \
	Shader/PixelPipeline.cpp \
	Shader/PixelProgram.cpp \
	Shader/PixelRoutine.cpp \
	Shader/PixelShader.cpp \
	Shader/SamplerCore.cpp \
	Shader/SetupRoutine.cpp \
	Shader/Shader.cpp \
	Shader/ShaderCore.cpp \
	Shader/VertexPipeline.cpp \
	Shader/VertexProgram.cpp \
	Shader/VertexRoutine.cpp \
	Shader/VertexShader.cpp \

COMMON_SRC_FILES += \
	OpenGL/common/Image.cpp \
	OpenGL/common/NameSpace.cpp \
	OpenGL/common/Object.cpp \
	OpenGL/common/MatrixStack.cpp \

COMMON_CFLAGS := \
	-DLOG_TAG=\"swiftshader\" \
	-Wno-unused-parameter \
	-Wno-implicit-exception-spec-mismatch \
	-Wno-overloaded-virtual \
	-fno-operator-names \
	-msse2 \
	-D__STDC_CONSTANT_MACROS \
	-D__STDC_LIMIT_MACROS \
	-DANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION) \
	-std=c++11

ifneq ($(filter gce_x86 gce calypso, $(TARGET_DEVICE)),)
COMMON_CFLAGS += -DDISPLAY_LOGO=0
endif

ifneq (16,${PLATFORM_SDK_VERSION})
COMMON_CFLAGS += -Xclang -fuse-init-array
else
COMMON_CFLAGS += -D__STDC_INT64__
endif

include $(CLEAR_VARS)
LOCAL_CLANG := true
LOCAL_MODULE := swiftshader_top_release
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(COMMON_SRC_FILES)
LOCAL_CFLAGS := $(COMMON_CFLAGS) -fomit-frame-pointer -ffunction-sections -fdata-sections -DANGLE_DISABLE_TRACE
LOCAL_C_INCLUDES := $(COMMON_C_INCLUDES)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_CLANG := true
LOCAL_MODULE := swiftshader_top_debug
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(COMMON_SRC_FILES)
LOCAL_CFLAGS := $(COMMON_CFLAGS) -UNDEBUG -g -O0 -DDEFAULT_THREAD_COUNT=1
LOCAL_C_INCLUDES := $(COMMON_C_INCLUDES)
include $(BUILD_STATIC_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
