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

#include "Viewport.hpp"

#include "Math.hpp"

namespace sw
{
	Viewport::Viewport()
	{
		width = 0;
		height = 0;

		left = 0;
		top = 0;
	}

	Viewport::~Viewport()
	{
	}

	void Viewport::setLeft(float l)
	{
		left = l;
	}

	void Viewport::setTop(float t)
	{
		top = t;
	}

	void Viewport::setWidth(float w)
	{
		width = w;
	}

	void Viewport::setHeight(float h)
	{
		height = h;
	}

	void Viewport::setNear(float n)
	{
		min = n;
	}

	void Viewport::setFar(float f)
	{
		max = f;
	}

	float Viewport::getLeft() const
	{
		return left;
	}

	float Viewport::getTop() const
	{
		return top;
	}

	float Viewport::getWidth() const
	{
		return width;
	}

	float Viewport::getHeight() const
	{
		return height;
	}

	float Viewport::getNear() const
	{
		return min;
	}

	float Viewport::getFar() const
	{
		return max;
	}
}
