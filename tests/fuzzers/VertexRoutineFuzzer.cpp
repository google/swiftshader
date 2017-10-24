// Copyright 2017 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "OpenGL/compiler/InitializeGlobals.h"
#include "OpenGL/compiler/InitializeParseContext.h"
#include "OpenGL/compiler/TranslatorASM.h"

// TODO: Debug macros of the GLSL compiler clash with core SwiftShader's.
// They should not be exposed through the interface headers above.
#undef ASSERT
#undef UNIMPLEMENTED

#include "Renderer/VertexProcessor.hpp"
#include "Shader/VertexProgram.hpp"

#include <cstdint>
#include <memory>

namespace {

// TODO(cwallez@google.com): Like in ANGLE, disable most of the pool allocator for fuzzing
// This is a helper class to make sure all the resources used by the compiler are initialized
class ScopedPoolAllocatorAndTLS {
	public:
		ScopedPoolAllocatorAndTLS() {
			InitializeParseContextIndex();
			InitializePoolIndex();
			SetGlobalPoolAllocator(&allocator);
		}
		~ScopedPoolAllocatorAndTLS() {
			SetGlobalPoolAllocator(nullptr);
			FreePoolIndex();
			FreeParseContextIndex();
		}

	private:
		TPoolAllocator allocator;
};

// Trivial implementation of the glsl::Shader interface that fakes being an API-level
// shader object.
class FakeVS : public glsl::Shader {
	public:
		FakeVS(sw::VertexShader* bytecode) : bytecode(bytecode) {
		}

		sw::Shader *getShader() const override {
			return bytecode;
		}
		sw::VertexShader *getVertexShader() const override {
			return bytecode;
		}


	private:
		sw::VertexShader* bytecode;
};

// Keep the compiler-related objects between fuzzer runs to speed up the iteration speed.
std::unique_ptr<ScopedPoolAllocatorAndTLS> allocatorAndTLS;
std::unique_ptr<sw::VertexShader> bytecodeShader;
std::unique_ptr<FakeVS> fakeVS;
std::unique_ptr<TranslatorASM> glslCompiler;

} // anonymous namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
	if (allocatorAndTLS == nullptr) {
		allocatorAndTLS.reset(new ScopedPoolAllocatorAndTLS);
	}

	if (bytecodeShader == nullptr) {
		bytecodeShader.reset(new sw::VertexShader);
	}

	if (fakeVS == nullptr) {
		fakeVS.reset(new FakeVS(bytecodeShader.get()));
	}

	if (glslCompiler == nullptr) {
		glslCompiler.reset(new TranslatorASM(fakeVS.get(), GL_VERTEX_SHADER));

		// TODO(cwallez@google.com): have a function to init to default values somewhere
		ShBuiltInResources resources;
		resources.MaxVertexAttribs = sw::MAX_VERTEX_INPUTS;
		resources.MaxVertexUniformVectors = sw::VERTEX_UNIFORM_VECTORS - 3;
		resources.MaxVaryingVectors = MIN(sw::MAX_VERTEX_OUTPUTS, sw::MAX_VERTEX_INPUTS);
		resources.MaxVertexTextureImageUnits = sw::VERTEX_TEXTURE_IMAGE_UNITS;
		resources.MaxCombinedTextureImageUnits = sw::TEXTURE_IMAGE_UNITS + sw::VERTEX_TEXTURE_IMAGE_UNITS;
		resources.MaxTextureImageUnits = sw::TEXTURE_IMAGE_UNITS;
		resources.MaxFragmentUniformVectors = sw::FRAGMENT_UNIFORM_VECTORS - 3;
		resources.MaxDrawBuffers = sw::RENDERTARGETS;
		resources.MaxVertexOutputVectors = 16; // ???
		resources.MaxFragmentInputVectors = 15; // ???
		resources.MinProgramTexelOffset = sw::MIN_PROGRAM_TEXEL_OFFSET;
		resources.MaxProgramTexelOffset = sw::MAX_PROGRAM_TEXEL_OFFSET;
		resources.OES_standard_derivatives = 1;
		resources.OES_fragment_precision_high = 1;
		resources.OES_EGL_image_external = 1;
		resources.EXT_draw_buffers = 1;
		resources.MaxCallStackDepth = 16;

		glslCompiler->Init(resources);
	}

	sw::VertexProcessor::State state;

	// TODO fill in the state

	const char* glslSource = "void main() {gl_Position = vec4(1.0);}";

	if (!glslCompiler->compile(&glslSource, 1, SH_OBJECT_CODE)) {
		return 0;
	}

	sw::VertexProgram program(state, bytecodeShader.get());
	program.generate();

	return 0;
}
