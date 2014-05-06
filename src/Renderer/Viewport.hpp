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

#ifndef sw_Viewport_hpp
#define sw_Viewport_hpp

namespace sw
{
	class Viewport
	{
	public:
		Viewport();

		~Viewport();

		void setLeft(float l);
		void setTop(float t);
		void setWidth(float w);
		void setHeight(float h);
		void setNear(float n);
		void setFar(float f);

		float getLeft() const;
		float getTop() const;
		float getWidth() const;
		float getHeight() const;
		float getNear() const;
		float getFar() const;

	private:
		float left;     // Leftmost pixel column
		float top;      // Highest pixel row
		float width;    // Width in pixels
		float height;   // Height in pixels
		float min;
		float max;
	};
}

#endif   // sw_Viewport_hpp
