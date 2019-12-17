// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
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

#include "BC_Decoder.hpp"

namespace {
static constexpr int BlockWidth = 4;
static constexpr int BlockHeight = 4;

struct BC_color
{
	void decode(unsigned char *dst, int x, int y, int dstW, int dstH, int dstPitch, int dstBpp, bool hasAlphaChannel, bool hasSeparateAlpha) const
	{
		Color c[4];
		c[0].extract565(c0);
		c[1].extract565(c1);
		if(hasSeparateAlpha || (c0 > c1))
		{
			c[2] = ((c[0] * 2) + c[1]) / 3;
			c[3] = ((c[1] * 2) + c[0]) / 3;
		}
		else
		{
			c[2] = (c[0] + c[1]) >> 1;
			if(hasAlphaChannel)
			{
				c[3].clearAlpha();
			}
		}

		for(int j = 0; j < BlockHeight && (y + j) < dstH; j++)
		{
			int dstOffset = j * dstPitch;
			int idxOffset = j * BlockHeight;
			for(int i = 0; i < BlockWidth && (x + i) < dstW; i++, idxOffset++, dstOffset += dstBpp)
			{
				*reinterpret_cast<unsigned int *>(dst + dstOffset) = c[getIdx(idxOffset)].pack8888();
			}
		}
	}

private:
	struct Color
	{
		Color()
		{
			c[0] = c[1] = c[2] = 0;
			c[3] = 0xFF000000;
		}

		void extract565(const unsigned int c565)
		{
			c[0] = ((c565 & 0x0000001F) << 3) | ((c565 & 0x0000001C) >> 2);
			c[1] = ((c565 & 0x000007E0) >> 3) | ((c565 & 0x00000600) >> 9);
			c[2] = ((c565 & 0x0000F800) >> 8) | ((c565 & 0x0000E000) >> 13);
		}

		unsigned int pack8888() const
		{
			return ((c[2] & 0xFF) << 16) | ((c[1] & 0xFF) << 8) | (c[0] & 0xFF) | c[3];
		}

		void clearAlpha()
		{
			c[3] = 0;
		}

		Color operator*(int factor) const
		{
			Color res;
			for(int i = 0; i < 4; ++i)
			{
				res.c[i] = c[i] * factor;
			}
			return res;
		}

		Color operator/(int factor) const
		{
			Color res;
			for(int i = 0; i < 4; ++i)
			{
				res.c[i] = c[i] / factor;
			}
			return res;
		}

		Color operator>>(int shift) const
		{
			Color res;
			for(int i = 0; i < 4; ++i)
			{
				res.c[i] = c[i] >> shift;
			}
			return res;
		}

		Color operator+(Color const &obj) const
		{
			Color res;
			for(int i = 0; i < 4; ++i)
			{
				res.c[i] = c[i] + obj.c[i];
			}
			return res;
		}

	private:
		int c[4];
	};

	unsigned int getIdx(int i) const
	{
		int offset = i << 1;  // 2 bytes per index
		return (idx & (0x3 << offset)) >> offset;
	}

	unsigned short c0;
	unsigned short c1;
	unsigned int idx;
};

struct BC_channel
{
	void decode(unsigned char *dst, int x, int y, int dstW, int dstH, int dstPitch, int dstBpp, int channel, bool isSigned) const
	{
		int c[8] = { 0 };

		if(isSigned)
		{
			c[0] = static_cast<signed char>(data & 0xFF);
			c[1] = static_cast<signed char>((data & 0xFF00) >> 8);
		}
		else
		{
			c[0] = static_cast<unsigned char>(data & 0xFF);
			c[1] = static_cast<unsigned char>((data & 0xFF00) >> 8);
		}

		if(c[0] > c[1])
		{
			for(int i = 2; i < 8; ++i)
			{
				c[i] = ((8 - i) * c[0] + (i - 1) * c[1]) / 7;
			}
		}
		else
		{
			for(int i = 2; i < 6; ++i)
			{
				c[i] = ((6 - i) * c[0] + (i - 1) * c[1]) / 5;
			}
			c[6] = isSigned ? -128 : 0;
			c[7] = isSigned ? 127 : 255;
		}

		for(int j = 0; j < BlockHeight && (y + j) < dstH; j++)
		{
			for(int i = 0; i < BlockWidth && (x + i) < dstW; i++)
			{
				dst[channel + (i * dstBpp) + (j * dstPitch)] = static_cast<unsigned char>(c[getIdx((j * BlockHeight) + i)]);
			}
		}
	}

private:
	unsigned char getIdx(int i) const
	{
		int offset = i * 3 + 16;
		return static_cast<unsigned char>((data & (0x7ull << offset)) >> offset);
	}

	unsigned long long data;
};

struct BC_alpha
{
	void decode(unsigned char *dst, int x, int y, int dstW, int dstH, int dstPitch, int dstBpp) const
	{
		dst += 3;  // Write only to alpha (channel 3)
		for(int j = 0; j < BlockHeight && (y + j) < dstH; j++, dst += dstPitch)
		{
			unsigned char *dstRow = dst;
			for(int i = 0; i < BlockWidth && (x + i) < dstW; i++, dstRow += dstBpp)
			{
				*dstRow = getAlpha(j * BlockHeight + i);
			}
		}
	}

private:
	unsigned char getAlpha(int i) const
	{
		int offset = i << 2;
		int alpha = (data & (0xFull << offset)) >> offset;
		return static_cast<unsigned char>(alpha | (alpha << 4));
	}

	unsigned long long data;
};
}  // end namespace

// Decodes 1 to 4 channel images to 8 bit output
bool BC_Decoder::Decode(const unsigned char *src, unsigned char *dst, int w, int h, int dstW, int dstH, int dstPitch, int dstBpp, int n, bool isNoAlphaU)
{
	static_assert(sizeof(BC_color) == 8, "BC_color must be 8 bytes");
	static_assert(sizeof(BC_channel) == 8, "BC_channel must be 8 bytes");
	static_assert(sizeof(BC_alpha) == 8, "BC_alpha must be 8 bytes");

	const int dx = BlockWidth * dstBpp;
	const int dy = BlockHeight * dstPitch;
	const bool isAlpha = (n == 1) && !isNoAlphaU;
	const bool isSigned = ((n == 4) || (n == 5) || (n == 6)) && !isNoAlphaU;

	switch(n)
	{
		case 1:  // BC1
		{
			const BC_color *color = reinterpret_cast<const BC_color *>(src);
			for(int y = 0; y < h; y += BlockHeight, dst += dy)
			{
				unsigned char *dstRow = dst;
				for(int x = 0; x < w; x += BlockWidth, ++color, dstRow += dx)
				{
					color->decode(dstRow, x, y, dstW, dstH, dstPitch, dstBpp, isAlpha, false);
				}
			}
		}
		break;
		case 2:  // BC2
		{
			const BC_alpha *alpha = reinterpret_cast<const BC_alpha *>(src);
			const BC_color *color = reinterpret_cast<const BC_color *>(src + 8);
			for(int y = 0; y < h; y += BlockHeight, dst += dy)
			{
				unsigned char *dstRow = dst;
				for(int x = 0; x < w; x += BlockWidth, alpha += 2, color += 2, dstRow += dx)
				{
					color->decode(dstRow, x, y, dstW, dstH, dstPitch, dstBpp, isAlpha, true);
					alpha->decode(dstRow, x, y, dstW, dstH, dstPitch, dstBpp);
				}
			}
		}
		break;
		case 3:  // BC3
		{
			const BC_channel *alpha = reinterpret_cast<const BC_channel *>(src);
			const BC_color *color = reinterpret_cast<const BC_color *>(src + 8);
			for(int y = 0; y < h; y += BlockHeight, dst += dy)
			{
				unsigned char *dstRow = dst;
				for(int x = 0; x < w; x += BlockWidth, alpha += 2, color += 2, dstRow += dx)
				{
					color->decode(dstRow, x, y, dstW, dstH, dstPitch, dstBpp, isAlpha, true);
					alpha->decode(dstRow, x, y, dstW, dstH, dstPitch, dstBpp, 3, isSigned);
				}
			}
		}
		break;
		case 4:  // BC4
		{
			const BC_channel *red = reinterpret_cast<const BC_channel *>(src);
			for(int y = 0; y < h; y += BlockHeight, dst += dy)
			{
				unsigned char *dstRow = dst;
				for(int x = 0; x < w; x += BlockWidth, ++red, dstRow += dx)
				{
					red->decode(dstRow, x, y, dstW, dstH, dstPitch, dstBpp, 0, isSigned);
				}
			}
		}
		break;
		case 5:  // BC5
		{
			const BC_channel *red = reinterpret_cast<const BC_channel *>(src);
			const BC_channel *green = reinterpret_cast<const BC_channel *>(src + 8);
			for(int y = 0; y < h; y += BlockHeight, dst += dy)
			{
				unsigned char *dstRow = dst;
				for(int x = 0; x < w; x += BlockWidth, red += 2, green += 2, dstRow += dx)
				{
					red->decode(dstRow, x, y, dstW, dstH, dstPitch, dstBpp, 0, isSigned);
					green->decode(dstRow, x, y, dstW, dstH, dstPitch, dstBpp, 1, isSigned);
				}
			}
		}
		break;
		default:
			return false;
	}

	return true;
}
