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

#ifndef dx_Unknown_hpp
#define dx_Unknown_hpp

namespace gl
{
	class Unknown
	{
	public:
		Unknown();

		virtual void addRef();
		virtual void release();

		virtual void bind();
		virtual void unbind();

	protected:
		virtual ~Unknown();

	private:
		volatile long referenceCount;
		volatile long bindCount;
	};
}

#endif   // dx_Unknown_hpp
