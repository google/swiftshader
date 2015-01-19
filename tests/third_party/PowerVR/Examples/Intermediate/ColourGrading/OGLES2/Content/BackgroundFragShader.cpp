// This file was created by Filewrap 1.1
// Little endian mode
// DO NOT EDIT

#include "../PVRTMemoryFileSystem.h"

// using 32 bit to guarantee alignment.
#ifndef A32BIT
 #define A32BIT static const unsigned int
#endif

// ******** Start: BackgroundFragShader.fsh ********

// File data
static const char _BackgroundFragShader_fsh[] = 
	"uniform sampler2D sTexture;\n"
	"\n"
	"varying mediump vec2 texCoords;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    highp vec3 vCol = texture2D(sTexture, texCoords).rgb;\n"
	"    gl_FragColor = vec4(vCol, 1.0);\n"
	"}\n";

// Register BackgroundFragShader.fsh in memory file system at application startup time
static CPVRTMemoryFileSystem RegisterFile_BackgroundFragShader_fsh("BackgroundFragShader.fsh", _BackgroundFragShader_fsh, 172);

// ******** End: BackgroundFragShader.fsh ********

