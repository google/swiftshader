// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
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

#ifndef D3D8_Direct3DDevice8_hpp
#define D3D8_Direct3DDevice8_hpp

#include "Unknown.hpp"

#include "Direct3D8.hpp"
#include "Direct3DStateBlock8.hpp"
#include "Direct3DVertexDeclaration8.hpp"
#include "Direct3DSwapChain8.hpp"

#include "Stream.hpp"

#include <d3d8.h>
#include <vector>
#include <list>
#include <map>

namespace sw
{
	class Renderer;
	class Context;
}

namespace D3D8
{
	class Direct3DPixelShader8;
	class Direct3DVertexShader8;
	class Direct3DSurface8;
	class Direct3DVertexBuffer8;
	class Direct3DIndexBuffer8;

	class Direct3DDevice8 : public IDirect3DDevice8, protected Unknown
	{
	public:
		Direct3DDevice8(const HINSTANCE instance, Direct3D8 *d3d8, unsigned int adapter, D3DDEVTYPE deviceType, HWND focusWindow, unsigned long behaviourFlags, D3DPRESENT_PARAMETERS *presentParameters);

		virtual ~Direct3DDevice8();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DDevice8 methods
		long __stdcall ApplyStateBlock(unsigned long token);
		long __stdcall BeginScene();
		long __stdcall BeginStateBlock();
		long __stdcall CaptureStateBlock(unsigned long token);
		long __stdcall Clear(unsigned long count, const D3DRECT *rects, unsigned long flags, unsigned long color, float z, unsigned long stencil);
		long __stdcall CopyRects(IDirect3DSurface8 *sourceSurface, const RECT *sourceRectsArray, unsigned int rects, IDirect3DSurface8 *destinationSurface, const POINT *destPointsArray);
		long __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *presentParameters, IDirect3DSwapChain8 **swapChain);
		long __stdcall CreateCubeTexture(unsigned int edgeLength, unsigned int levels, unsigned long usage, D3DFORMAT format, D3DPOOL pool, IDirect3DCubeTexture8 **cubeTexture);
		long __stdcall CreateDepthStencilSurface(unsigned int width, unsigned int height, D3DFORMAT format, D3DMULTISAMPLE_TYPE multiSample, IDirect3DSurface8 **surface);
		long __stdcall CreateImageSurface(unsigned int width, unsigned int height, D3DFORMAT format, IDirect3DSurface8 **surface);
		long __stdcall CreateIndexBuffer(unsigned int length, unsigned long usage, D3DFORMAT format, D3DPOOL pool, IDirect3DIndexBuffer8 **indexBuffer);
		long __stdcall CreatePixelShader(const unsigned long *function, unsigned long *handle);
		long __stdcall CreateRenderTarget(unsigned int width, unsigned int height, D3DFORMAT format, D3DMULTISAMPLE_TYPE multiSample, int lockable, IDirect3DSurface8 **surface);
		long __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE type, unsigned long *token);
		long __stdcall CreateTexture(unsigned int width, unsigned int height, unsigned int levels, unsigned long usage, D3DFORMAT format, D3DPOOL pool, IDirect3DTexture8 **texture);
		long __stdcall CreateVertexBuffer(unsigned int length, unsigned long usage, unsigned long FVF, D3DPOOL, IDirect3DVertexBuffer8 **vertexBuffer);
		long __stdcall CreateVertexShader(const unsigned long *declaration, const unsigned long *function, unsigned long *handle, unsigned long usage);
		long __stdcall CreateVolumeTexture(unsigned int width, unsigned int height, unsigned int depth, unsigned int levels, unsigned long usage, D3DFORMAT format, D3DPOOL pool, IDirect3DVolumeTexture8 **volumeTexture);
		long __stdcall DeletePatch(unsigned int handle);
		long __stdcall DeletePixelShader(unsigned long handle);
		long __stdcall DeleteStateBlock(unsigned long token);
		long __stdcall DeleteVertexShader(unsigned long handle);
		long __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE type, unsigned int minIndex, unsigned int numVertices, unsigned int startIndex, unsigned int primitiveCount);
		long __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE type, unsigned int minVertexIndex, unsigned int numVertexIndices, unsigned int PrimitiveCount, const void *indexData, D3DFORMAT indexDataFormat, const void *vertexStreamZeroData, unsigned int VertexStreamZeroStride);
		long __stdcall DrawPrimitive(D3DPRIMITIVETYPE primitiveType, unsigned int startVertex, unsigned int primiveCount);
		long __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE primitiveType, unsigned int primitiveCount, const void *vertexStreamZeroData, unsigned int vertexStreamZeroStride);
		long __stdcall DrawRectPatch(unsigned int handle, const float *numSegs, const D3DRECTPATCH_INFO *rectPatchInfo);
		long __stdcall DrawTriPatch(unsigned int handle, const float *numSegs, const D3DTRIPATCH_INFO *triPatchInfo);
		long __stdcall EndScene();
		long __stdcall EndStateBlock(unsigned long *token);
		unsigned int __stdcall GetAvailableTextureMem();
		long __stdcall GetBackBuffer(unsigned int index, D3DBACKBUFFER_TYPE type, IDirect3DSurface8 **backBuffer);
		long __stdcall GetClipPlane(unsigned long index, float *plane);
		long __stdcall GetClipStatus(D3DCLIPSTATUS8 *clipStatus);
		long __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *parameters);
		long __stdcall GetCurrentTexturePalette(unsigned int *paletteNumber);
		long __stdcall GetDepthStencilSurface(IDirect3DSurface8 **depthStencilSurface);
		long __stdcall GetDeviceCaps(D3DCAPS8 *caps);
		long __stdcall GetDirect3D(IDirect3D8 **D3D);
		long __stdcall GetDisplayMode(D3DDISPLAYMODE *mode);
		long __stdcall GetFrontBuffer(IDirect3DSurface8 *destSurface);
		void __stdcall GetGammaRamp(D3DGAMMARAMP *ramp);
		long __stdcall GetIndices(IDirect3DIndexBuffer8 **indexData, unsigned int *baseVertexIndex);
		long __stdcall GetInfo(unsigned long devInfoID, void *devInfoStruct, unsigned long devInfoStructSize);
		long __stdcall GetLight(unsigned long index, D3DLIGHT8 *p);
		long __stdcall GetLightEnable(unsigned long index , int *enable);
		long __stdcall GetMaterial(D3DMATERIAL8 *material);
		long __stdcall GetPaletteEntries(unsigned int paletteNumber, PALETTEENTRY *entries);
		long __stdcall GetPixelShader(unsigned long *handle);
		long __stdcall GetPixelShaderFunction(unsigned long handle, void *data, unsigned long *sizeOfData);
		long __stdcall GetPixelShaderConstant(unsigned long startRegister, void *constantData, unsigned long constantCount);
		long __stdcall GetRasterStatus(D3DRASTER_STATUS *rasterStatus);
		long __stdcall GetRenderState(D3DRENDERSTATETYPE State, unsigned long *value);
		long __stdcall GetRenderTarget(IDirect3DSurface8 **renderTarget);
		long __stdcall GetStreamSource(unsigned int streamNumber, IDirect3DVertexBuffer8 **streamData, unsigned int *stride);
		long __stdcall GetTexture(unsigned long stage, IDirect3DBaseTexture8 **texture);
		long __stdcall GetTextureStageState(unsigned long stage, D3DTEXTURESTAGESTATETYPE type, unsigned long *value);
		long __stdcall GetTransform(D3DTRANSFORMSTATETYPE state, D3DMATRIX *matrix);
		long __stdcall GetVertexShader(unsigned long *handle);
		long __stdcall GetVertexShaderConstant(unsigned long startRegister, void *constantData, unsigned long constantCount);
		long __stdcall GetVertexShaderDeclaration(unsigned long handle, void *data, unsigned long *size);
		long __stdcall GetVertexShaderFunction(unsigned long handle, void *data, unsigned long *size);
		long __stdcall GetViewport(D3DVIEWPORT8 *viewport);
		long __stdcall LightEnable(unsigned long index, int enable);
		long __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE state, const D3DMATRIX *matrix);
		long __stdcall Present(const RECT *sourceRect, const RECT *destRect, HWND destWindowOverride, const RGNDATA *dirtyRegion);
		long __stdcall ProcessVertices(unsigned int srcStartIndex, unsigned int destIndex, unsigned int vertexCount, IDirect3DVertexBuffer8 *destBuffer, unsigned long flags);
		long __stdcall Reset(D3DPRESENT_PARAMETERS *presentParameters);
		long __stdcall ResourceManagerDiscardBytes(unsigned long bytes);
		long __stdcall SetClipPlane(unsigned long index, const float *plane);
		long __stdcall SetClipStatus(const D3DCLIPSTATUS8 *clipStatus);
		long __stdcall SetCurrentTexturePalette(unsigned int paletteNumber);
		void __stdcall SetCursorPosition(int x, int y, unsigned long flags);
		void __stdcall SetCursorPosition(unsigned int x, unsigned int y, unsigned long flags);
		long __stdcall SetCursorProperties(unsigned int x, unsigned int y, IDirect3DSurface8 *cursorBitmap);
		void __stdcall SetGammaRamp(unsigned long flags, const D3DGAMMARAMP *ramp);
		long __stdcall SetIndices(IDirect3DIndexBuffer8 *indexData, unsigned int baseVertexIndex);
		long __stdcall SetLight(unsigned long index, const D3DLIGHT8 *light);
		long __stdcall SetMaterial(const D3DMATERIAL8 *material);
		long __stdcall SetPaletteEntries(unsigned int paletteNumber, const PALETTEENTRY *entries);
		long __stdcall SetPixelShader(unsigned long shader);
		long __stdcall SetPixelShaderConstant(unsigned long startRegister, const void *constantData, unsigned long constantCount);
		long __stdcall SetRenderState(D3DRENDERSTATETYPE state, unsigned long value);
		long __stdcall SetRenderTarget(IDirect3DSurface8 *renderTarget, IDirect3DSurface8 *newZStencil);
		long __stdcall SetStreamSource(unsigned int streamNumber, IDirect3DVertexBuffer8 *streamData, unsigned int stride);
		long __stdcall SetTexture(unsigned long stage, IDirect3DBaseTexture8 *texture);
		long __stdcall SetTextureStageState(unsigned long stage, D3DTEXTURESTAGESTATETYPE type, unsigned long value);
		long __stdcall SetTransform(D3DTRANSFORMSTATETYPE state, const D3DMATRIX *matrix);
		long __stdcall SetVertexShader(unsigned long handle);
		long __stdcall SetVertexShaderConstant(unsigned long startRegister, const void *constantData, unsigned long constantCount);
		long __stdcall SetViewport(const D3DVIEWPORT8 *viewport);
		int __stdcall ShowCursor(int show);
		long __stdcall TestCooperativeLevel();
		long __stdcall UpdateTexture(IDirect3DBaseTexture8 *sourceTexture, IDirect3DBaseTexture8 *destinationTexture);
		long __stdcall ValidateDevice(unsigned long *numPasses);

		// Internal methods
		long __stdcall updateSurface(IDirect3DSurface8 *sourceSurface, const RECT *sourceRect, IDirect3DSurface8 *destinationSurface, const POINT *destPoint);

	private:
		static int FVFStride(unsigned long FVF);
		static int typeStride(unsigned char streamType);
		static sw::StreamType streamType(int type);
		bool bindData(Direct3DIndexBuffer8 *indexBuffer, int base);
		void bindStreams(int base);
		void bindIndexBuffer(Direct3DIndexBuffer8 *indexBuffer);
		void bindLights();
		bool bindViewport();
		void bindTextures();
		void bindCursor();

		long updateVolume(IDirect3DVolume8 *sourceVolume, IDirect3DVolume8 *destinationVolume);
		void configureFPU();

		// Creation parameters
		const HINSTANCE instance;
		Direct3D8 *d3d8;
		const unsigned int adapter;
		const D3DDEVTYPE deviceType;
		const HWND focusWindow;
		const unsigned long behaviourFlags;
		const D3DPRESENT_PARAMETERS presentParameters;
		
		HWND windowHandle;

		D3DVIEWPORT8 viewport;
		D3DMATRIX matrix[512];
		Direct3DBaseTexture8 *texture[8];
		D3DMATERIAL8 material;
		float plane[6][4];
		D3DCLIPSTATUS8 clipStatus;

		struct Light : D3DLIGHT8
		{
			Light &operator=(const D3DLIGHT8 &light)
			{
				Type = light.Type;
				Diffuse = light.Diffuse;
				Specular = light.Specular;
				Ambient = light.Ambient;
				Position = light.Position;
				Direction = light.Direction;
				Range = light.Range;
				Falloff = light.Falloff;
				Attenuation0 = light.Attenuation0;
				Attenuation1 = light.Attenuation1;
				Attenuation2 = light.Attenuation2;
				Theta = light.Theta;
				Phi = light.Phi;

				return *this;
			}

			bool enable;
		};

		struct Lights : std::map<int, Light>
		{
			bool exists(int index)
			{
				return find(index) != end();
			}
		};

		Lights light;
		bool lightsDirty;

		Direct3DVertexBuffer8 *dataStream[16];
		int streamStride[16];
		Direct3DIndexBuffer8 *indexData;
		unsigned int baseVertexIndex;

		unsigned long FVF;

		std::vector<Direct3DSwapChain8*> swapChain;

		Direct3DSurface8 *renderTarget;
		Direct3DSurface8 *depthStencil;

		bool recordState;
		std::vector<Direct3DStateBlock8*> stateRecorder;

		unsigned long renderState[D3DRS_NORMALORDER + 1];
		unsigned long textureStageState[8][D3DTSS_RESULTARG + 1];
		bool init;   // TODO: Deprecate when all state changes implemented

		std::vector<Direct3DPixelShader8*> pixelShader;
		std::vector<Direct3DVertexShader8*> vertexShader;
		unsigned long pixelShaderHandle;
		unsigned long vertexShaderHandle;
		const unsigned long *declaration;

		float pixelShaderConstant[8][4];
		float vertexShaderConstant[256][4];

		struct Palette
		{
			PALETTEENTRY entry[256];
		};

		unsigned int currentPalette;
		std::map<int, Palette> palette;

		sw::Context *context;
		sw::Renderer *renderer;

		sw::Surface *cursor;
		bool showCursor;
		HCURSOR nullCursor;
		HCURSOR win32Cursor;
	};
}

#endif   // D3D8_Direct3DDevice8_hpp
