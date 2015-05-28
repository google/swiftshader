LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CLANG := true

LOCAL_MODULE := swiftshader_compiler
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
	preprocessor/Diagnostics.cpp \
	preprocessor/DirectiveHandler.cpp \
	preprocessor/DirectiveParser.cpp \
	preprocessor/ExpressionParser.cpp \
	preprocessor/Input.cpp \
	preprocessor/Lexer.cpp \
	preprocessor/Macro.cpp \
	preprocessor/MacroExpander.cpp \
	preprocessor/Preprocessor.cpp \
	preprocessor/Token.cpp \
	preprocessor/Tokenizer.cpp \
	AnalyzeCallDepth.cpp \
	Compiler.cpp \
	debug.cpp \
	Diagnostics.cpp \
	DirectiveHandler.cpp \
	glslang_lex.cpp \
	glslang_tab.cpp \
	InfoSink.cpp \
	Initialize.cpp \
	InitializeParseContext.cpp \
	IntermTraverse.cpp \
	Intermediate.cpp \
	intermOut.cpp \
	ossource_posix.cpp \
	OutputASM.cpp \
	parseConst.cpp \
	ParseHelper.cpp \
	PoolAlloc.cpp \
	SymbolTable.cpp \
	TranslatorASM.cpp \
	util.cpp \
	ValidateLimitations.cpp \

LOCAL_CFLAGS += -DLOG_TAG=\"swiftshader_compiler\" -Wno-unused-parameter

# Android's make system also uses NDEBUG, so we need to set/unset it forcefully
# Uncomment for ON:
# LOCAL_CFLAGS += -UNDEBUG -g -O0
# Uncomment for OFF:
LOCAL_CFLAGS += -fomit-frame-pointer -ffunction-sections -fdata-sections -DANGLE_DISABLE_TRACE

LOCAL_CFLAGS += -fno-operator-names -msse2 -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS
LOCAL_CFLAGS += -std=c++11

LOCAL_C_INCLUDES += \
	bionic \
	$(GCE_STLPORT_INCLUDES) \
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

include $(BUILD_STATIC_LIBRARY)
