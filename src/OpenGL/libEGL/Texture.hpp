#ifndef egl_Texture_hpp
#define egl_Texture_hpp

#include "common/Object.hpp"

namespace sw
{
	class Resource;
}

namespace egl
{
class Texture : public gl::NamedObject
{
public:
	Texture(GLuint name) : NamedObject(name) {}

	virtual void releaseTexImage() = 0;
	virtual sw::Resource *getResource() const = 0;

	virtual void sweep() = 0;   // Garbage collect if no external references

	void release() override
	{
		int refs = dereference();

		if(refs > 0)
		{
			sweep();
		}
		else
		{
			delete this;
		}
	}
};
}

#endif   // egl_Texture_hpp
