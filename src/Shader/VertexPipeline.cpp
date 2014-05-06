// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "VertexPipeline.hpp"

#include "Vertex.hpp"
#include "Renderer.hpp"
#include "Debug.hpp"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#undef max
#undef min

namespace sw
{
	VertexPipeline::VertexPipeline(const VertexProcessor::State &state) : VertexRoutine(state)
	{
	}

	VertexPipeline::~VertexPipeline()
	{
	}

	Color4f VertexPipeline::transformBlend(Registers &r, Color4f &src, Pointer<Byte> &matrix, bool homogeneous)
	{
		Color4f dst;

		if(state.vertexBlendMatrixCount == 0)
		{
			dst = transform(src, matrix, homogeneous);
		}
		else
		{
			UInt index0[4];
			UInt index1[4];
			UInt index2[4];
			UInt index3[4];

			if(state.indexedVertexBlendEnable)
			{
				for(int i = 0; i < 4; i++)
				{
					UInt indices;
					
					switch(i)
					{
					case 0: indices = As<UInt>(Float(r.v[BlendIndices].x.x)); break;
					case 1: indices = As<UInt>(Float(r.v[BlendIndices].x.y)); break;
					case 2: indices = As<UInt>(Float(r.v[BlendIndices].x.z)); break;
					case 3: indices = As<UInt>(Float(r.v[BlendIndices].x.w)); break;
					}

					index0[i] = (indices & UInt(0x000000FF)) << UInt(6);   // FIXME: (indices & 0x000000FF) << 6
					index1[i] = (indices & UInt(0x0000FF00)) >> UInt(2);
					index2[i] = (indices & UInt(0x00FF0000)) >> UInt(10);
					index3[i] = (indices & UInt(0xFF000000)) >> UInt(18);
				}
			}
			else
			{
				for(int i = 0; i < 4; i++)
				{
					index0[i] = Int(0 * 64);   // FIXME: index0 = 0 * 64;
					index1[i] = Int(1 * 64);   // FIXME: index1 = 1 * 64;
					index2[i] = Int(2 * 64);   // FIXME: index2 = 2 * 64;
					index3[i] = Int(3 * 64);   // FIXME: index3 = 3 * 64;
				}
			}

			Float4 weight0;
			Float4 weight1;
			Float4 weight2;
			Float4 weight3;

			switch(state.vertexBlendMatrixCount)
			{
			case 4: weight2 = r.v[BlendWeight].z;
			case 3: weight1 = r.v[BlendWeight].y;
			case 2: weight0 = r.v[BlendWeight].x;
			case 1:
				break;
			}

			if(state.vertexBlendMatrixCount == 1)
			{
				dst = transform(src, matrix, index0, homogeneous);
			}
			else if(state.vertexBlendMatrixCount == 2)
			{
				weight1 = Float4(1.0f) - weight0;

				Color4f pos0;
				Color4f pos1;

				pos0 = transform(src, matrix, index0, homogeneous);
				pos1 = transform(src, matrix, index1, homogeneous);

				dst.x = pos0.x * weight0 + pos1.x * weight1;   // FIXME: Color4f operators
				dst.y = pos0.y * weight0 + pos1.y * weight1;
				dst.z = pos0.z * weight0 + pos1.z * weight1;
				dst.w = pos0.w * weight0 + pos1.w * weight1;
			}
			else if(state.vertexBlendMatrixCount == 3)
			{
				weight2 = Float4(1.0f) - (weight0 + weight1);

				Color4f pos0;
				Color4f pos1;
				Color4f pos2;

				pos0 = transform(src, matrix, index0, homogeneous);
				pos1 = transform(src, matrix, index1, homogeneous);
				pos2 = transform(src, matrix, index2, homogeneous);

				dst.x = pos0.x * weight0 + pos1.x * weight1 + pos2.x * weight2;
				dst.y = pos0.y * weight0 + pos1.y * weight1 + pos2.y * weight2;
				dst.z = pos0.z * weight0 + pos1.z * weight1 + pos2.z * weight2;
				dst.w = pos0.w * weight0 + pos1.w * weight1 + pos2.w * weight2;
			}
			else if(state.vertexBlendMatrixCount == 4)
			{
				weight3 = Float4(1.0f) - (weight0 + weight1 + weight2);

				Color4f pos0;
				Color4f pos1;
				Color4f pos2;
				Color4f pos3;

				pos0 = transform(src, matrix, index0, homogeneous);
				pos1 = transform(src, matrix, index1, homogeneous);
				pos2 = transform(src, matrix, index2, homogeneous);
				pos3 = transform(src, matrix, index3, homogeneous);

				dst.x = pos0.x * weight0 + pos1.x * weight1 + pos2.x * weight2 + pos3.x * weight3;
				dst.y = pos0.y * weight0 + pos1.y * weight1 + pos2.y * weight2 + pos3.y * weight3;
				dst.z = pos0.z * weight0 + pos1.z * weight1 + pos2.z * weight2 + pos3.z * weight3;
				dst.w = pos0.w * weight0 + pos1.w * weight1 + pos2.w * weight2 + pos3.w * weight3;
			}
		}

		return dst;
	}

	void VertexPipeline::pipeline(Registers &r)
	{
		Color4f position;
		Color4f normal;

		if(!state.preTransformed)
		{
			position = transformBlend(r, r.v[Position], Pointer<Byte>(r.data + OFFSET(DrawData,ff.transformT)), true);
		}
		else
		{
			position = r.v[PositionT];
		}

		r.ox[Pos] = position.x;
		r.oy[Pos] = position.y;
		r.oz[Pos] = position.z;
		r.ow[Pos] = position.w;

		if(state.vertexNormalActive)
		{
			normal = transformBlend(r, r.v[Normal], Pointer<Byte>(r.data + OFFSET(DrawData,ff.normalTransformT)), false);

			if(state.normalizeNormals)
			{
				normal = normalize(normal);
			}
		}

		if(!state.vertexLightingActive)
		{
			// FIXME: Don't process if not used at all
			if(state.diffuseActive && state.input[Color0])
			{
				Color4f diffuse = r.v[Color0];

				r.ox[D0] = diffuse.x;
				r.oy[D0] = diffuse.y;
				r.oz[D0] = diffuse.z;
				r.ow[D0] = diffuse.w;
			}
			else
			{
				r.ox[D0] = Float4(1.0f, 1.0f, 1.0f, 1.0f);
				r.oy[D0] = Float4(1.0f, 1.0f, 1.0f, 1.0f);
				r.oz[D0] = Float4(1.0f, 1.0f, 1.0f, 1.0f);
				r.ow[D0] = Float4(1.0f, 1.0f, 1.0f, 1.0f);
			}

			// FIXME: Don't process if not used at all
			if(state.specularActive && state.input[Color1])
			{
				Color4f specular = r.v[Color1];

				r.ox[D1] = specular.x;
				r.oy[D1] = specular.y;
				r.oz[D1] = specular.z;
				r.ow[D1] = specular.w;
			}
			else
			{
				r.ox[D1] = Float4(0.0f, 0.0f, 0.0f, 0.0f);
				r.oy[D1] = Float4(0.0f, 0.0f, 0.0f, 0.0f);
				r.oz[D1] = Float4(0.0f, 0.0f, 0.0f, 0.0f);
				r.ow[D1] = Float4(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
		else
		{
			Color4f diffuseSum;

			r.ox[D0] = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			r.oy[D0] = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			r.oz[D0] = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			r.ow[D0] = Float4(0.0f, 0.0f, 0.0f, 0.0f);

			r.ox[D1] = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			r.oy[D1] = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			r.oz[D1] = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			r.ow[D1] = Float4(0.0f, 0.0f, 0.0f, 0.0f);

			diffuseSum.x = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			diffuseSum.y = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			diffuseSum.z = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			diffuseSum.w = Float4(0.0f, 0.0f, 0.0f, 0.0f);

			Color4f vertexPosition = transformBlend(r, r.v[Position], Pointer<Byte>(r.data + OFFSET(DrawData,ff.cameraTransformT)), true);

			for(int i = 0; i < 8; i++)
			{
				if(!(state.vertexLightActive & (1 << i)))
				{
					continue;
				}

				Color4f L;    // Light vector
				Float4 att;   // Attenuation

				// Attenuation
				{
					Float4 d;   // Distance

					L.x = L.y = L.z = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.lightPosition[i]));   // FIXME: Unpack
					L.x = L.x.xxxx;
					L.y = L.y.yyyy;
					L.z = L.z.zzzz;

					L.x -= vertexPosition.x;
					L.y -= vertexPosition.y;
					L.z -= vertexPosition.z;
					d = dot3(L, L);
					d = RcpSqrt_pp(d);     // FIXME: Sufficient precision?
					L.x *= d;
					L.y *= d;
					L.z *= d;
					d = Rcp_pp(d);       // FIXME: Sufficient precision?

					Float4 q = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.attenuationQuadratic[i]));
					Float4 l = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.attenuationLinear[i]));
					Float4 c = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.attenuationConstant[i]));

					att = Rcp_pp((q * d + l) * d + c);
				}

				// Ambient per light
				{
					Float4 lightAmbient = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.lightAmbient[i]));   // FIXME: Unpack

					r.ox[D0] = r.ox[D0] + lightAmbient.x * att;
					r.oy[D0] = r.oy[D0] + lightAmbient.y * att;
					r.oz[D0] = r.oz[D0] + lightAmbient.z * att;
				}

				// Diffuse
				if(state.vertexNormalActive)
				{
					Float4 dot;

					dot = dot3(L, normal);
					dot = Max(dot, Float4(0.0f, 0.0f, 0.0f, 0.0f));
					dot *= att;

					Color4f diff;

					if(state.vertexDiffuseMaterialSourceActive == Context::MATERIAL)
					{
						diff.x = diff.y = diff.z = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.materialDiffuse));   // FIXME: Unpack
						diff.x = diff.x.xxxx;
						diff.y = diff.y.yyyy;
						diff.z = diff.z.zzzz;
					}
					else if(state.vertexDiffuseMaterialSourceActive == Context::COLOR1)
					{
						diff = r.v[Color0];
					}
					else if(state.vertexDiffuseMaterialSourceActive == Context::COLOR2)
					{
						diff = r.v[Color1];
					}
					else ASSERT(false);

					Float4 lightDiffuse = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.lightDiffuse[i]));

					diffuseSum.x += diff.x * dot * lightDiffuse.x;   // FIXME: Clamp first?
					diffuseSum.y += diff.y * dot * lightDiffuse.y;   // FIXME: Clamp first?
					diffuseSum.z += diff.z * dot * lightDiffuse.z;   // FIXME: Clamp first?
				}

				// Specular
				if(state.vertexSpecularActive)
				{
					Color4f S;
					Color4f C;   // Camera vector
					Float4 pow;

					pow = *Pointer<Float>(r.data + OFFSET(DrawData,ff.materialShininess));

					S.x = Float4(0.0f, 0.0f, 0.0f, 0.0f) - vertexPosition.x;
					S.y = Float4(0.0f, 0.0f, 0.0f, 0.0f) - vertexPosition.y;
					S.z = Float4(0.0f, 0.0f, 0.0f, 0.0f) - vertexPosition.z;
					C = normalize(S);

					S.x = L.x + C.x;
					S.y = L.y + C.y;
					S.z = L.z + C.z;
					C = normalize(S);

					Float4 dot = Max(dot3(C, normal), Float4(0.0f));   // FIXME: max(dot3(C, normal), 0)

					Float4 P = power(dot, pow);
					P *= att;

					Color4f spec;

					if(state.vertexSpecularMaterialSourceActive == Context::MATERIAL)
					{
						Float4 materialSpecular = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.materialSpecular));   // FIXME: Unpack

						spec.x = materialSpecular.x;
						spec.y = materialSpecular.y;
						spec.z = materialSpecular.z;
					}
					else if(state.vertexSpecularMaterialSourceActive == Context::COLOR1)
					{
						spec = r.v[Color0];
					}
					else if(state.vertexSpecularMaterialSourceActive == Context::COLOR2)
					{
						spec = r.v[Color1];
					}
					else ASSERT(false);

					Float4 lightSpecular = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.lightSpecular[i]));

					spec.x *= lightSpecular.x;
					spec.y *= lightSpecular.y;
					spec.z *= lightSpecular.z;

					spec.x *= P;
					spec.y *= P;
					spec.z *= P;

					spec.x = Max(spec.x, Float4(0.0f, 0.0f, 0.0f, 0.0f));
					spec.y = Max(spec.y, Float4(0.0f, 0.0f, 0.0f, 0.0f));
					spec.z = Max(spec.z, Float4(0.0f, 0.0f, 0.0f, 0.0f));

					r.ox[D1] = r.ox[D1] + spec.x;
					r.oy[D1] = r.oy[D1] + spec.y;
					r.oz[D1] = r.oz[D1] + spec.z;
				}
			}

			Float4 globalAmbient = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.globalAmbient));   // FIXME: Unpack

			r.ox[D0] = r.ox[D0] + globalAmbient.x;
			r.oy[D0] = r.oy[D0] + globalAmbient.y;
			r.oz[D0] = r.oz[D0] + globalAmbient.z;

			if(state.vertexAmbientMaterialSourceActive == Context::MATERIAL)
			{
				Float4 materialAmbient = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.materialAmbient));   // FIXME: Unpack

				r.ox[D0] = r.ox[D0] * materialAmbient.x;
				r.oy[D0] = r.oy[D0] * materialAmbient.y;
				r.oz[D0] = r.oz[D0] * materialAmbient.z;
			}
			else if(state.vertexAmbientMaterialSourceActive == Context::COLOR1)
			{
				Color4f materialDiffuse = r.v[Color0];

				r.ox[D0] = r.ox[D0] * materialDiffuse.x;
				r.oy[D0] = r.oy[D0] * materialDiffuse.y;
				r.oz[D0] = r.oz[D0] * materialDiffuse.z;
			}
			else if(state.vertexAmbientMaterialSourceActive == Context::COLOR2)
			{
				Color4f materialSpecular = r.v[Color1];

				r.ox[D0] = r.ox[D0] * materialSpecular.x;
				r.oy[D0] = r.oy[D0] * materialSpecular.y;
				r.oz[D0] = r.oz[D0] * materialSpecular.z;
			}
			else ASSERT(false);

			r.ox[D0] = r.ox[D0] + diffuseSum.x;
			r.oy[D0] = r.oy[D0] + diffuseSum.y;
			r.oz[D0] = r.oz[D0] + diffuseSum.z;

			// Emissive
			if(state.vertexEmissiveMaterialSourceActive == Context::MATERIAL)
			{
				Float4 materialEmission = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.materialEmission));   // FIXME: Unpack

				r.ox[D0] = r.ox[D0] + materialEmission.x;
				r.oy[D0] = r.oy[D0] + materialEmission.y;
				r.oz[D0] = r.oz[D0] + materialEmission.z;
			}
			else if(state.vertexEmissiveMaterialSourceActive == Context::COLOR1)
			{
				Color4f materialSpecular = r.v[Color0];

				r.ox[D0] = r.ox[D0] + materialSpecular.x;
				r.oy[D0] = r.oy[D0] + materialSpecular.y;
				r.oz[D0] = r.oz[D0] + materialSpecular.z;
			}
			else if(state.vertexEmissiveMaterialSourceActive == Context::COLOR2)
			{
				Color4f materialSpecular = r.v[Color1];

				r.ox[D0] = r.ox[D0] + materialSpecular.x;
				r.oy[D0] = r.oy[D0] + materialSpecular.y;
				r.oz[D0] = r.oz[D0] + materialSpecular.z;
			}
			else ASSERT(false);

			// Diffuse alpha component
			if(state.vertexDiffuseMaterialSourceActive == Context::MATERIAL)
			{
				r.ow[D0] = Float4(*Pointer<Float4>(r.data + OFFSET(DrawData,ff.materialDiffuse[0]))).wwww;   // FIXME: Unpack
			}
			else if(state.vertexDiffuseMaterialSourceActive == Context::COLOR1)
			{
				Color4f alpha = r.v[Color0];
				r.ow[D0] = alpha.w;
			}
			else if(state.vertexDiffuseMaterialSourceActive == Context::COLOR2)
			{
				Color4f alpha = r.v[Color1];
				r.ow[D0] = alpha.w;
			}
			else ASSERT(false);

			if(state.vertexSpecularActive)
			{
				// Specular alpha component
				if(state.vertexSpecularMaterialSourceActive == Context::MATERIAL)
				{
					r.ow[D1] = Float4(*Pointer<Float4>(r.data + OFFSET(DrawData,ff.materialSpecular[3]))).wwww;   // FIXME: Unpack
				}
				else if(state.vertexSpecularMaterialSourceActive == Context::COLOR1)
				{
					Color4f alpha = r.v[Color0];
					r.ow[D1] = alpha.w;
				}
				else if(state.vertexSpecularMaterialSourceActive == Context::COLOR2)
				{
					Color4f alpha = r.v[Color1];
					r.ow[D1] = alpha.w;
				}
				else ASSERT(false);
			}
		}

		if(state.fogActive)
		{
			switch(state.vertexFogMode)
			{
			case Context::FOG_NONE:
				if(state.specularActive)
				{
					r.ox[Fog] = r.ow[D1];
				}
				else
				{
					r.ox[Fog] = Float4(0.0f, 0.0f, 0.0f, 0.0f);
				}
				break;
			case Context::FOG_LINEAR:
			case Context::FOG_EXP:
			case Context::FOG_EXP2:
				if(!state.rangeFogActive)
				{
					r.ox[Fog] = r.oz[Pos];
				}
				else
				{
					Color4f pos;

					pos.x = r.ox[Pos];
					pos.y = r.oy[Pos];
					pos.z = r.oz[Pos];
					pos.w = r.ow[Pos];

					r.ox[Fog] = Sqrt(dot3(pos, pos));   // FIXME: oFog = length(o[Pos]);
				}

				r.ox[Fog] = r.ox[Fog] * *Pointer<Float4>(r.data + OFFSET(DrawData,fog.scale)) + *Pointer<Float4>(r.data + OFFSET(DrawData,fog.offset));
				break;
			default:
				ASSERT(false);
			}
		}

		for(int stage = 0; stage < 8; stage++)
		{
			processTextureCoordinate(r, stage, normal, position);
		}

		processPointSize(r);
	}

	void VertexPipeline::processTextureCoordinate(Registers &r, int stage, Color4f &normal, Color4f &position)
	{
		if(state.output[T0 + stage].write)
		{
			int i = state.textureState[stage].texCoordIndexActive;

			switch(state.textureState[stage].texGenActive)
			{
			case Context::TEXGEN_PASSTHRU:
				{
					Color4f v = r.v[TexCoord0 + i];

					r.ox[T0 + stage] = v.x;
					r.oy[T0 + stage] = v.y;
					r.oz[T0 + stage] = v.z;
					r.ow[T0 + stage] = v.w;

					if(state.input[TexCoord0 + i].type == STREAMTYPE_FLOAT)
					{
						switch(state.input[TexCoord0 + i].count)
						{
						case 1:
							r.oy[T0 + stage] = Float4(1.0f, 1.0f, 1.0f, 1.0f);
							r.oz[T0 + stage] = Float4(0.0f, 0.0f, 0.0f, 0.0f);
							r.ow[T0 + stage] = Float4(0.0f, 0.0f, 0.0f, 0.0f);
							break;
						case 2:
							r.oz[T0 + stage] = Float4(1.0f, 1.0f, 1.0f, 1.0f);
							r.ow[T0 + stage] = Float4(0.0f, 0.0f, 0.0f, 0.0f);
							break;
						case 3:
							r.ow[T0 + stage] = Float4(1.0f, 1.0f, 1.0f, 1.0f);
							break;
						case 4:
							break;
						default:
							ASSERT(false);
						}
					}
					else ASSERT(!state.input[TexCoord0 + i]);   // Point sprite; coordinates provided by setup
				}
				break;
			case Context::TEXGEN_NORMAL:
				{
					Color4f Nc;   // Normal vector in camera space

					if(state.vertexNormalActive)
					{
						Nc = normal;
					}
					else
					{
						Nc.x = Float4(0.0f, 0.0f, 0.0f, 0.0f);
						Nc.y = Float4(0.0f, 0.0f, 0.0f, 0.0f);
						Nc.z = Float4(0.0f, 0.0f, 0.0f, 0.0f);
					}

					Nc.w = Float4(1.0f, 1.0f, 1.0f, 1.0f);
					
					r.ox[T0 + stage] = Nc.x;
					r.oy[T0 + stage] = Nc.y;
					r.oz[T0 + stage] = Nc.z;
					r.ow[T0 + stage] = Nc.w;
				}
				break;
			case Context::TEXGEN_POSITION:
				{
					Color4f Pn = transformBlend(r, r.v[Position], Pointer<Byte>(r.data + OFFSET(DrawData,ff.cameraTransformT)), true);   // Position in camera space

					Pn.w = Float4(1.0f, 1.0f, 1.0f, 1.0f);
					
					r.ox[T0 + stage] = Pn.x;
					r.oy[T0 + stage] = Pn.y;
					r.oz[T0 + stage] = Pn.z;
					r.ow[T0 + stage] = Pn.w;
				}
				break;
			case Context::TEXGEN_REFLECTION:
				{
					Color4f R;   // Reflection vector

					if(state.vertexNormalActive)
					{
						Color4f Nc;   // Normal vector in camera space

						Nc = normal;

						if(state.localViewerActive)
						{
							Color4f Ec;   // Eye vector in camera space
							Color4f N2;

							Ec = transformBlend(r, r.v[Position], Pointer<Byte>(r.data + OFFSET(DrawData,ff.cameraTransformT)), true);
							Ec = normalize(Ec);

							// R = E - 2 * N * (E . N)
							Float4 dot = Float4(2.0f, 2.0f, 2.0f, 2.0f) * dot3(Ec, Nc);

							R.x = Ec.x - Nc.x * dot;
							R.y = Ec.y - Nc.y * dot;
							R.z = Ec.z - Nc.z * dot;
						}
						else
						{
							// u = -2 * Nz * Nx
							// v = -2 * Nz * Ny
							// w = 1 - 2 * Nz * Nz

							R.x = -Float4(2.0f, 2.0f, 2.0f, 2.0f) * Nc.z * Nc.x;
							R.y = -Float4(2.0f, 2.0f, 2.0f, 2.0f) * Nc.z * Nc.y;
							R.z = Float4(1.0f, 1.0f, 1.0f, 1.0f) - Float4(2.0f, 2.0f, 2.0f, 2.0f) * Nc.z * Nc.z;
						}
					}
					else
					{
						R.x = Float4(0.0f, 0.0f, 0.0f, 0.0f);
						R.y = Float4(0.0f, 0.0f, 0.0f, 0.0f);
						R.z = Float4(0.0f, 0.0f, 0.0f, 0.0f);
					}

					R.w = Float4(1.0f, 1.0f, 1.0f, 1.0f);

					r.ox[T0 + stage] = R.x;
					r.oy[T0 + stage] = R.y;
					r.oz[T0 + stage] = R.z;
					r.ow[T0 + stage] = R.w;
				}
				break;
			case Context::TEXGEN_SPHEREMAP:
				{
					Color4f R;   // Reflection vector

					if(state.vertexNormalActive)
					{
						Color4f Nc;   // Normal vector in camera space

						Nc = normal;

						if(state.localViewerActive)
						{
							Color4f Ec;   // Eye vector in camera space
							Color4f N2;

							Ec = transformBlend(r, r.v[Position], Pointer<Byte>(r.data + OFFSET(DrawData,ff.cameraTransformT)), true);
							Ec = normalize(Ec);

							// R = E - 2 * N * (E . N)
							Float4 dot = Float4(2.0f, 2.0f, 2.0f, 2.0f) * dot3(Ec, Nc);
							
							R.x = Ec.x - Nc.x * dot;
							R.y = Ec.y - Nc.y * dot;
							R.z = Ec.z - Nc.z * dot;
						}
						else
						{
							// u = -2 * Nz * Nx
							// v = -2 * Nz * Ny
							// w = 1 - 2 * Nz * Nz

							R.x = -Float4(2.0f, 2.0f, 2.0f, 2.0f) * Nc.z * Nc.x;
							R.y = -Float4(2.0f, 2.0f, 2.0f, 2.0f) * Nc.z * Nc.y;
							R.z = Float4(1.0f, 1.0f, 1.0f, 1.0f) - Float4(2.0f, 2.0f, 2.0f, 2.0f) * Nc.z * Nc.z;
						}
					}
					else
					{
						R.x = Float4(0.0f, 0.0f, 0.0f, 0.0f);
						R.y = Float4(0.0f, 0.0f, 0.0f, 0.0f);
						R.z = Float4(0.0f, 0.0f, 0.0f, 0.0f);
					}

					R.z -= Float4(1.0f, 1.0f, 1.0f, 1.0f);
					R = normalize(R);
					R.x = Float4(0.5f, 0.5f, 0.5f, 0.5f) * R.x + Float4(0.5f, 0.5f, 0.5f, 0.5f);
					R.y = Float4(0.5f, 0.5f, 0.5f, 0.5f) * R.y + Float4(0.5f, 0.5f, 0.5f, 0.5f);

					R.z = Float4(1.0f, 1.0f, 1.0f, 1.0f);
					R.w = Float4(0.0f, 0.0f, 0.0f, 0.0f);

					r.ox[T0 + stage] = R.x;
					r.oy[T0 + stage] = R.y;
					r.oz[T0 + stage] = R.z;
					r.ow[T0 + stage] = R.w;
				}
				break;
			default:
				ASSERT(false);
			}

			Color4f texTrans0;
			Color4f texTrans1;
			Color4f texTrans2;
			Color4f texTrans3;

			Color4f T;
			Color4f t;

			T.x = r.ox[T0 + stage];
			T.y = r.oy[T0 + stage];
			T.z = r.oz[T0 + stage];
			T.w = r.ow[T0 + stage];

			switch(state.textureState[stage].textureTransformCountActive)
			{
			case 4:
				texTrans3.x = texTrans3.y = texTrans3.z = texTrans3.w = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.textureTransform[stage][3]));   // FIXME: Unpack
				texTrans3.x = texTrans3.x.xxxx;
				texTrans3.y = texTrans3.y.yyyy;
				texTrans3.z = texTrans3.z.zzzz;
				texTrans3.w = texTrans3.w.wwww;
				t.w = dot4(T, texTrans3);
			case 3:
				texTrans2.x = texTrans2.y = texTrans2.z = texTrans2.w = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.textureTransform[stage][2]));   // FIXME: Unpack
				texTrans2.x = texTrans2.x.xxxx;
				texTrans2.y = texTrans2.y.yyyy;
				texTrans2.z = texTrans2.z.zzzz;
				texTrans2.w = texTrans2.w.wwww;
				t.z = dot4(T, texTrans2);
			case 2:
				texTrans1.x = texTrans1.y = texTrans1.z = texTrans1.w = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.textureTransform[stage][1]));   // FIXME: Unpack
				texTrans1.x = texTrans1.x.xxxx;
				texTrans1.y = texTrans1.y.yyyy;
				texTrans1.z = texTrans1.z.zzzz;
				texTrans1.w = texTrans1.w.wwww;
				t.y = dot4(T, texTrans1);
			case 1:
				texTrans0.x = texTrans0.y = texTrans0.z = texTrans0.w = *Pointer<Float4>(r.data + OFFSET(DrawData,ff.textureTransform[stage][0]));   // FIXME: Unpack
				texTrans0.x = texTrans0.x.xxxx;
				texTrans0.y = texTrans0.y.yyyy;
				texTrans0.z = texTrans0.z.zzzz;
				texTrans0.w = texTrans0.w.wwww;
				t.x = dot4(T, texTrans0);

				r.ox[T0 + stage] = t.x;
				r.oy[T0 + stage] = t.y;
				r.oz[T0 + stage] = t.z;
				r.ow[T0 + stage] = t.w;
			case 0:
				break;
			default:
				ASSERT(false);
			}
		}
	}

	void VertexPipeline::processPointSize(Registers &r)
	{
		if(!state.pointSizeActive)
		{
			return;   // Use global pointsize
		}

		if(state.input[PSize])
		{
			r.oy[Pts] = r.v[PSize].x;
		}
		else
		{
			r.oy[Pts] = *Pointer<Float4>(r.data + OFFSET(DrawData,point.pointSize));
		}

		if(state.pointScaleActive && !state.preTransformed)
		{
			Color4f p = transformBlend(r, r.v[Position], Pointer<Byte>(r.data + OFFSET(DrawData,ff.cameraTransformT)), true);

			Float4 d = Sqrt(dot3(p, p));   // FIXME: length(p);

			Float4 A = *Pointer<Float>(r.data + OFFSET(DrawData,point.pointScaleA));   // FIXME: Unpack
			Float4 B = *Pointer<Float>(r.data + OFFSET(DrawData,point.pointScaleB));   // FIXME: Unpack
			Float4 C = *Pointer<Float>(r.data + OFFSET(DrawData,point.pointScaleC));   // FIXME: Unpack

			A = RcpSqrt_pp(A + d * (B + d * C));

			r.oy[Pts] = r.oy[Pts] * Float4(*Pointer<Float>(r.data + OFFSET(DrawData,viewportHeight))) * A;   // FIXME: Unpack
		}
	}

	Color4f VertexPipeline::transform(Color4f &src, Pointer<Byte> &matrix, bool homogeneous)
	{
		Color4f dst;

		Color4f row0;
		Color4f row1;
		Color4f row2;
		Color4f row3;

		if(homogeneous)
		{
			Float4 m[4][4];

			for(int j = 0; j < 4; j++)
			{
				for(int i = 0; i < 4; i++)
				{
					m[j][i].x = *Pointer<Float>(matrix + 16 * i + 4 * j);
					m[j][i].y = *Pointer<Float>(matrix + 16 * i + 4 * j);
					m[j][i].z = *Pointer<Float>(matrix + 16 * i + 4 * j);
					m[j][i].w = *Pointer<Float>(matrix + 16 * i + 4 * j);
				}
			}

			dst.x = src.x * m[0][0] + src.y * m[0][1] + src.z * m[0][2] + m[0][3];
			dst.y = src.x * m[1][0] + src.y * m[1][1] + src.z * m[1][2] + m[1][3];
			dst.z = src.x * m[2][0] + src.y * m[2][1] + src.z * m[2][2] + m[2][3];
			dst.w = src.x * m[3][0] + src.y * m[3][1] + src.z * m[3][2] + m[3][3];
		}
		else
		{
			Float4 m[3][3];

			for(int j = 0; j < 3; j++)
			{
				for(int i = 0; i < 3; i++)
				{
					m[j][i].x = *Pointer<Float>(matrix + 16 * i + 4 * j);
					m[j][i].y = *Pointer<Float>(matrix + 16 * i + 4 * j);
					m[j][i].z = *Pointer<Float>(matrix + 16 * i + 4 * j);
					m[j][i].w = *Pointer<Float>(matrix + 16 * i + 4 * j);
				}
			}

			dst.x = src.x * m[0][0] + src.y * m[0][1] + src.z * m[0][2];
			dst.y = src.x * m[1][0] + src.y * m[1][1] + src.z * m[1][2];
			dst.z = src.x * m[2][0] + src.y * m[2][1] + src.z * m[2][2];
		}

		return dst;
	}

	Color4f VertexPipeline::transform(Color4f &src, Pointer<Byte> &matrix, UInt index[4], bool homogeneous)
	{
		Color4f dst;
		
		Color4f row0;
		Color4f row1;
		Color4f row2;
		Color4f row3;

		if(homogeneous)
		{
			Float4 m[4][4];

			for(int j = 0; j < 4; j++)
			{
				for(int i = 0; i < 4; i++)
				{
					m[j][i].x = *Pointer<Float>(matrix + 16 * i + 4 * j + index[0]);
					m[j][i].y = *Pointer<Float>(matrix + 16 * i + 4 * j + index[1]);
					m[j][i].z = *Pointer<Float>(matrix + 16 * i + 4 * j + index[2]);
					m[j][i].w = *Pointer<Float>(matrix + 16 * i + 4 * j + index[3]);
				}
			}

			dst.x = src.x * m[0][0] + src.y * m[0][1] + src.z * m[0][2] + m[0][3];
			dst.y = src.x * m[1][0] + src.y * m[1][1] + src.z * m[1][2] + m[1][3];
			dst.z = src.x * m[2][0] + src.y * m[2][1] + src.z * m[2][2] + m[2][3];
			dst.w = src.x * m[3][0] + src.y * m[3][1] + src.z * m[3][2] + m[3][3];
		}
		else
		{
			Float4 m[3][3];

			for(int j = 0; j < 3; j++)
			{
				for(int i = 0; i < 3; i++)
				{
					m[j][i].x = *Pointer<Float>(matrix + 16 * i + 4 * j + index[0]);
					m[j][i].y = *Pointer<Float>(matrix + 16 * i + 4 * j + index[1]);
					m[j][i].z = *Pointer<Float>(matrix + 16 * i + 4 * j + index[2]);
					m[j][i].w = *Pointer<Float>(matrix + 16 * i + 4 * j + index[3]);
				}
			}

			dst.x = src.x * m[0][0] + src.y * m[0][1] + src.z * m[0][2];
			dst.y = src.x * m[1][0] + src.y * m[1][1] + src.z * m[1][2];
			dst.z = src.x * m[2][0] + src.y * m[2][1] + src.z * m[2][2];
		}

		return dst;
	}

	Color4f VertexPipeline::normalize(Color4f &src)
	{
		Color4f dst;

		Float4 rcpLength = RcpSqrt_pp(dot3(src, src));
		
		dst.x = src.x * rcpLength;
		dst.y = src.y * rcpLength;
		dst.z = src.z * rcpLength;

		return dst;
	}

	Float4 VertexPipeline::power(Float4 &src0, Float4 &src1)
	{
		Float4 dst = src0;
				
		dst = dst * dst;
		dst = dst * dst;
		dst = Float4(As<Int4>(dst) - As<Int4>(Float4(1.0f, 1.0f, 1.0f, 1.0f)));
				
		dst *= src1;

		dst = As<Float4>(Int4(dst) + As<Int4>(Float4(1.0f, 1.0f, 1.0f, 1.0f)));
		dst = RcpSqrt_pp(dst);
		dst = RcpSqrt_pp(dst);

		return dst;
	}
}
