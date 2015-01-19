// This file was created by Filewrap 1.1
// Little endian mode
// DO NOT EDIT

#include "../PVRTMemoryFileSystem.h"

// using 32 bit to guarantee alignment.
#ifndef A32BIT
 #define A32BIT static const unsigned int
#endif

// ******** Start: VertShader.vsh ********

// File data
static const char _VertShader_vsh[] = 
	"attribute highp   vec4 inVertex;\n"
	"attribute mediump vec2 inTexCoord;\n"
	"\n"
	"varying   mediump vec2 texCoords;\n"
	"\t\t\n"
	"void main() \n"
	"{ \n"
	"\tgl_Position = inVertex;\n"
	"\ttexCoords   = inTexCoord;\n"
	"} \n";

// Register VertShader.vsh in memory file system at application startup time
static CPVRTMemoryFileSystem RegisterFile_VertShader_vsh("VertShader.vsh", _VertShader_vsh, 177);

// ******** End: VertShader.vsh ********

