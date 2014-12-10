#ifndef egl_Texture_hpp
#define egl_Texture_hpp

#include "common/Object.hpp"

namespace egl
{
class Texture : public gl::RefCountObject
{
public:
	Texture(GLuint id) : RefCountObject(id) {};
	virtual void releaseTexImage() = 0;
};
}

#endif   // egl_Texture_hpp
