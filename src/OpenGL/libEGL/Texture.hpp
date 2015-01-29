#ifndef egl_Texture_hpp
#define egl_Texture_hpp

#include "common/Object.hpp"

namespace egl
{
class Texture : public gl::Object
{
public:
	Texture(GLuint name) : Object(name) {};
	virtual void releaseTexImage() = 0;
};
}

#endif   // egl_Texture_hpp
