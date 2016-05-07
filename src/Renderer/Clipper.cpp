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

#include "Clipper.hpp"

#include "Polygon.hpp"
#include "Renderer.hpp"
#include "Debug.hpp"

namespace sw
{
	Clipper::Clipper()
	{
	}

	Clipper::~Clipper()
	{
	}

	bool Clipper::clip(Polygon &polygon, int clipFlagsOr, const DrawCall &draw)
	{
		DrawData &data = *draw.data;

		polygon.b = 0;

		if(clipFlagsOr & 0x0000003F)
		{
			if(clipFlagsOr & CLIP_NEAR)   clipNear(polygon);
			if(polygon.n >= 3) {
			if(clipFlagsOr & CLIP_FAR)    clipFar(polygon);
			if(polygon.n >= 3) {
			if(clipFlagsOr & CLIP_LEFT)   clipLeft(polygon, data);
			if(polygon.n >= 3) {
			if(clipFlagsOr & CLIP_RIGHT)  clipRight(polygon, data);
			if(polygon.n >= 3) {
			if(clipFlagsOr & CLIP_TOP)    clipTop(polygon, data);
			if(polygon.n >= 3) {
			if(clipFlagsOr & CLIP_BOTTOM) clipBottom(polygon, data);
			}}}}}
		}

		if(clipFlagsOr & 0x00003F00)
		{
			if(polygon.n >= 3) {
			if(draw.clipFlags & CLIP_PLANE0) clipPlane(polygon, data.clipPlane[0]);
			if(polygon.n >= 3) {
			if(draw.clipFlags & CLIP_PLANE1) clipPlane(polygon, data.clipPlane[1]);
			if(polygon.n >= 3) {
			if(draw.clipFlags & CLIP_PLANE2) clipPlane(polygon, data.clipPlane[2]);
			if(polygon.n >= 3) {
			if(draw.clipFlags & CLIP_PLANE3) clipPlane(polygon, data.clipPlane[3]);
			if(polygon.n >= 3) {
			if(draw.clipFlags & CLIP_PLANE4) clipPlane(polygon, data.clipPlane[4]);
			if(polygon.n >= 3) {
			if(draw.clipFlags & CLIP_PLANE5) clipPlane(polygon, data.clipPlane[5]);
			}}}}}}
		}

		return polygon.n >= 3;
	}

	void Clipper::clipNear(Polygon &polygon)
	{
		if(polygon.n == 0) return;

		const float4 **V = polygon.P[polygon.i];
		const float4 **T = polygon.P[polygon.i + 1];

		int t = 0;

		for(int i = 0; i < polygon.n; i++)
		{
			int j = i == polygon.n - 1 ? 0 : i + 1;

			float di = V[i]->z;
			float dj = V[j]->z;

			if(di >= 0)
			{
				T[t++] = V[i];

				if(dj < 0)
				{
					clipEdge(polygon.B[polygon.b], *V[i], *V[j], di, dj);
					polygon.B[polygon.b].z = 0;
					T[t++] = &polygon.B[polygon.b++];
				}
			}
			else
			{
				if(dj > 0)
				{
					clipEdge(polygon.B[polygon.b], *V[j], *V[i], dj, di);
					polygon.B[polygon.b].z = 0;
					T[t++] = &polygon.B[polygon.b++];
				}
			}
		}

		polygon.n = t;
		polygon.i += 1;
	}

	void Clipper::clipFar(Polygon &polygon)
	{
		if(polygon.n == 0) return;

		const float4 **V = polygon.P[polygon.i];
		const float4 **T = polygon.P[polygon.i + 1];

		int t = 0;

		for(int i = 0; i < polygon.n; i++)
		{
			int j = i == polygon.n - 1 ? 0 : i + 1;

			float di = V[i]->w - V[i]->z;
			float dj = V[j]->w - V[j]->z;

			if(di >= 0)
			{
				T[t++] = V[i];

				if(dj < 0)
				{
					clipEdge(polygon.B[polygon.b], *V[i], *V[j], di, dj);
					polygon.B[polygon.b].z = polygon.B[polygon.b].w;
					T[t++] = &polygon.B[polygon.b++];
				}
			}
			else
			{
				if(dj > 0)
				{
					clipEdge(polygon.B[polygon.b], *V[j], *V[i], dj, di);
					polygon.B[polygon.b].z = polygon.B[polygon.b].w;
					T[t++] = &polygon.B[polygon.b++];
				}
			}
		}

		polygon.n = t;
		polygon.i += 1;
	}

	void Clipper::clipLeft(Polygon &polygon, const DrawData &data)
	{
		if(polygon.n == 0) return;

		const float4 **V = polygon.P[polygon.i];
		const float4 **T = polygon.P[polygon.i + 1];

		int t = 0;

		for(int i = 0; i < polygon.n; i++)
		{
			int j = i == polygon.n - 1 ? 0 : i + 1;

			float di = V[i]->w + V[i]->x;
			float dj = V[j]->w + V[j]->x;

			if(di >= 0)
			{
				T[t++] = V[i];

				if(dj < 0)
				{
					clipEdge(polygon.B[polygon.b], *V[i], *V[j], di, dj);
				//	polygon.B[polygon.b].x = -polygon.B[polygon.b].w;
					T[t++] = &polygon.B[polygon.b++];
				}
			}
			else
			{
				if(dj > 0)
				{
					clipEdge(polygon.B[polygon.b], *V[j], *V[i], dj, di);
				//	polygon.B[polygon.b].x = -polygon.B[polygon.b].w;
					T[t++] = &polygon.B[polygon.b++];
				}
			}
		}

		polygon.n = t;
		polygon.i += 1;
	}

	void Clipper::clipRight(Polygon &polygon, const DrawData &data)
	{
		if(polygon.n == 0) return;

		const float4 **V = polygon.P[polygon.i];
		const float4 **T = polygon.P[polygon.i + 1];

		int t = 0;

		for(int i = 0; i < polygon.n; i++)
		{
			int j = i == polygon.n - 1 ? 0 : i + 1;

			float di = V[i]->w - V[i]->x;
			float dj = V[j]->w - V[j]->x;

			if(di >= 0)
			{
				T[t++] = V[i];

				if(dj < 0)
				{
					clipEdge(polygon.B[polygon.b], *V[i], *V[j], di, dj);
				//	polygon.B[polygon.b].x = polygon.B[polygon.b].w;
					T[t++] = &polygon.B[polygon.b++];
				}
			}
			else
			{
				if(dj > 0)
				{
					clipEdge(polygon.B[polygon.b], *V[j], *V[i], dj, di);
				//	polygon.B[polygon.b].x = polygon.B[polygon.b].w;
					T[t++] = &polygon.B[polygon.b++];
				}
			}
		}

		polygon.n = t;
		polygon.i += 1;
	}

	void Clipper::clipTop(Polygon &polygon, const DrawData &data)
	{
		if(polygon.n == 0) return;

		const float4 **V = polygon.P[polygon.i];
		const float4 **T = polygon.P[polygon.i + 1];

		int t = 0;

		for(int i = 0; i < polygon.n; i++)
		{
			int j = i == polygon.n - 1 ? 0 : i + 1;

			float di = V[i]->w - V[i]->y;
			float dj = V[j]->w - V[j]->y;

			if(di >= 0)
			{
				T[t++] = V[i];

				if(dj < 0)
				{
					clipEdge(polygon.B[polygon.b], *V[i], *V[j], di, dj);
				//	polygon.B[polygon.b].y = polygon.B[polygon.b].w;
					T[t++] = &polygon.B[polygon.b++];
				}
			}
			else
			{
				if(dj > 0)
				{
					clipEdge(polygon.B[polygon.b], *V[j], *V[i], dj, di);
				//	polygon.B[polygon.b].y = polygon.B[polygon.b].w;
					T[t++] = &polygon.B[polygon.b++];
				}
			}
		}

		polygon.n = t;
		polygon.i += 1;
	}

	void Clipper::clipBottom(Polygon &polygon, const DrawData &data)
	{
		if(polygon.n == 0) return;

		const float4 **V = polygon.P[polygon.i];
		const float4 **T = polygon.P[polygon.i + 1];

		int t = 0;

		for(int i = 0; i < polygon.n; i++)
		{
			int j = i == polygon.n - 1 ? 0 : i + 1;

			float di = V[i]->w + V[i]->y;
			float dj = V[j]->w + V[j]->y;

			if(di >= 0)
			{
				T[t++] = V[i];

				if(dj < 0)
				{
					clipEdge(polygon.B[polygon.b], *V[i], *V[j], di, dj);
				//	polygon.B[polygon.b].y = -polygon.B[polygon.b].w;
					T[t++] = &polygon.B[polygon.b++];
				}
			}
			else
			{
				if(dj > 0)
				{
					clipEdge(polygon.B[polygon.b], *V[j], *V[i], dj, di);
				//	polygon.B[polygon.b].y = -polygon.B[polygon.b].w;
					T[t++] = &polygon.B[polygon.b++];
				}
			}
		}

		polygon.n = t;
		polygon.i += 1;
	}

	void Clipper::clipPlane(Polygon &polygon, const Plane &p)
	{
		if(polygon.n == 0) return;

		const float4 **V = polygon.P[polygon.i];
		const float4 **T = polygon.P[polygon.i + 1];

		int t = 0;

		for(int i = 0; i < polygon.n; i++)
		{
			int j = i == polygon.n - 1 ? 0 : i + 1;

			float di = p.A * V[i]->x + p.B * V[i]->y + p.C * V[i]->z + p.D * V[i]->w;
			float dj = p.A * V[j]->x + p.B * V[j]->y + p.C * V[j]->z + p.D * V[j]->w;

			if(di >= 0)
			{
				T[t++] = V[i];

				if(dj < 0)
				{
					clipEdge(polygon.B[polygon.b], *V[i], *V[j], di, dj);
					T[t++] = &polygon.B[polygon.b++];
				}
			}
			else
			{
				if(dj > 0)
				{
					clipEdge(polygon.B[polygon.b], *V[j], *V[i], dj, di);
					T[t++] = &polygon.B[polygon.b++];
				}
			}
		}

		polygon.n = t;
		polygon.i += 1;
	}

	inline void Clipper::clipEdge(float4 &Vo, const float4 &Vi, const float4 &Vj, float di, float dj) const
	{
		float D = 1.0f / (dj - di);

		Vo.x = (dj * Vi.x - di * Vj.x) * D;
		Vo.y = (dj * Vi.y - di * Vj.y) * D;
		Vo.z = (dj * Vi.z - di * Vj.z) * D;
		Vo.w = (dj * Vi.w - di * Vj.w) * D;
	}
}
