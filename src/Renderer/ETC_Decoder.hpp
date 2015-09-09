// SwiftShader Software Renderer
//
// Copyright(c) 2015 Google Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of Google Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

class ETC_Decoder
{
public:
	enum InputType
	{
		ETC_R_SIGNED,
		ETC_R_UNSIGNED,
		ETC_RG_SIGNED,
		ETC_RG_UNSIGNED,
		ETC_RGB,
		ETC_RGB_PUNCHTHROUGH_ALPHA,
		ETC_RGBA
	};

	/// ETC_Decoder::Decode - Decodes 1 to 4 channel images to 8 bit output
	/// @param src            Pointer to ETC2 encoded image
	/// @param dst            Pointer to BGRA, 8 bit output
	/// @param w              src image width
	/// @param h              src image height
	/// @param dstW           dst image width
	/// @param dstH           dst image height
	/// @param dstPitch       dst image pitch (bytes per row)
	/// @param dstBpp         dst image bytes per pixel
	/// @param inputType      src's format
	/// @return               true if the decoding was performed
	static bool Decode(const unsigned char* src, unsigned char *dst, int w, int h, int dstW, int dstH, int dstPitch, int dstBpp, InputType inputType);
};
