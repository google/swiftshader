LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CLANG := true

LOCAL_MODULE_PATH := $(TARGET_OUT)/vendor/lib/egl
LOCAL_MODULE := libGLESv2_swiftshader

LOCAL_SRC_FILES := \
	../../Common/CPUID.cpp \
	../../Common/Configurator.cpp \
	../../Common/Debug.cpp \
	../../Common/Half.cpp \
	../../Common/Math.cpp \
	../../Common/Memory.cpp \
	../../Common/Resource.cpp \
	../../Common/Socket.cpp \
	../../Common/Thread.cpp \
	../../Common/Timer.cpp

LOCAL_SRC_FILES += \
	../../Main/Config.cpp \
	../../Main/FrameBuffer.cpp \
	../../Main/FrameBufferAndroid.cpp \
	../../Main/Logo.cpp \
	../../Main/Register.cpp \
	../../Main/SwiftConfig.cpp \
	../../Main/crc.cpp \
	../../Main/serialvalid.cpp \

LOCAL_SRC_FILES += \
	../../Reactor/Nucleus.cpp \
	../../Reactor/Routine.cpp \
	../../Reactor/RoutineManager.cpp

LOCAL_SRC_FILES += \
	../../Renderer/Blitter.cpp \
	../../Renderer/Clipper.cpp \
	../../Renderer/Color.cpp \
	../../Renderer/Context.cpp \
	../../Renderer/Matrix.cpp \
	../../Renderer/PixelProcessor.cpp \
	../../Renderer/Plane.cpp \
	../../Renderer/Point.cpp \
	../../Renderer/QuadRasterizer.cpp \
	../../Renderer/Rasterizer.cpp \
	../../Renderer/Renderer.cpp \
	../../Renderer/Sampler.cpp \
	../../Renderer/SetupProcessor.cpp \
	../../Renderer/Surface.cpp \
	../../Renderer/TextureStage.cpp \
	../../Renderer/Vector.cpp \
	../../Renderer/VertexProcessor.cpp \

LOCAL_SRC_FILES += \
	../../Shader/Constants.cpp \
	../../Shader/PixelRoutine.cpp \
	../../Shader/PixelShader.cpp \
	../../Shader/SamplerCore.cpp \
	../../Shader/SetupRoutine.cpp \
	../../Shader/Shader.cpp \
	../../Shader/ShaderCore.cpp \
	../../Shader/VertexPipeline.cpp \
	../../Shader/VertexProgram.cpp \
	../../Shader/VertexRoutine.cpp \
	../../Shader/VertexShader.cpp \

LOCAL_SRC_FILES += \
	../common/NameSpace.cpp \
	../common/Object.cpp \
	../common/debug.cpp \

LOCAL_SRC_FILES += \
	../compiler/preprocessor/Diagnostics.cpp \
	../compiler/preprocessor/DirectiveHandler.cpp \
	../compiler/preprocessor/DirectiveParser.cpp \
	../compiler/preprocessor/ExpressionParser.cpp \
	../compiler/preprocessor/Input.cpp \
	../compiler/preprocessor/Lexer.cpp \
	../compiler/preprocessor/Macro.cpp \
	../compiler/preprocessor/MacroExpander.cpp \
	../compiler/preprocessor/Preprocessor.cpp \
	../compiler/preprocessor/Token.cpp \
	../compiler/preprocessor/Tokenizer.cpp \
	../compiler/AnalyzeCallDepth.cpp \
	../compiler/Compiler.cpp \
	../compiler/debug.cpp \
	../compiler/Diagnostics.cpp \
	../compiler/DirectiveHandler.cpp \
	../compiler/glslang_lex.cpp \
	../compiler/glslang_tab.cpp \
	../compiler/InfoSink.cpp \
	../compiler/Initialize.cpp \
	../compiler/InitializeParseContext.cpp \
	../compiler/IntermTraverse.cpp \
	../compiler/Intermediate.cpp \
	../compiler/intermOut.cpp \
	../compiler/ossource_posix.cpp \
	../compiler/OutputASM.cpp \
	../compiler/parseConst.cpp \
	../compiler/ParseHelper.cpp \
	../compiler/PoolAlloc.cpp \
	../compiler/SymbolTable.cpp \
	../compiler/TranslatorASM.cpp \
	../compiler/util.cpp \
	../compiler/ValidateLimitations.cpp \

LOCAL_SRC_FILES += \
	Buffer.cpp \
	Context.cpp \
	Device.cpp \
	Fence.cpp \
	Framebuffer.cpp \
	Image.cpp \
	IndexDataManager.cpp \
	libGLESv2.cpp \
	main.cpp \
	Program.cpp \
	Query.cpp \
	Renderbuffer.cpp \
	ResourceManager.cpp \
	Shader.cpp \
	Texture.cpp \
	utilities.cpp \
	VertexDataManager.cpp \

LOCAL_CFLAGS += -DLOG_TAG=\"libGLESv2_swiftshader\"
LOCAL_CFLAGS += -fomit-frame-pointer -ffunction-sections -fdata-sections -DNDEBUG -DANGLE_DISABLE_TRACE
LOCAL_CFLAGS += -fno-operator-names -msse2 -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS
LOCAL_CFLAGS += -std=c++11

LOCAL_SHARED_LIBRARIES += libdl liblog libcutils libhardware libui
LOCAL_STATIC_LIBRARIES += libLLVM_swiftshader
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
