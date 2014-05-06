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

#ifndef Register_hpp
#define Register_hpp

#define SERIAL_PREFIX "SS"
#define CHECKSUM_KEY "ShaderCore"

#define APPNAME_SCRAMBLE "8gbdf&*bb00nnj9v#@3.-,l8gc3x.,p"
#define SCRAMBLE31(x, y) {x[0]^y[0], x[1]^y[1], x[2]^y[2], x[3]^y[3], x[4]^y[4], x[5]^y[5], x[6]^y[6], x[7]^y[7], x[9]^y[9], x[10]^y[10], \
                          x[11]^y[11], x[12]^y[12], x[13]^y[13], x[14]^y[14], x[15]^y[15], x[16]^y[16], x[17]^y[17], x[18]^y[18], x[19]^y[19], x[20]^y[20], \
					      x[21]^y[21], x[22]^y[22], x[23]^y[23], x[24]^y[24], x[25]^y[25], x[26]^y[26], x[27]^y[27], x[28]^y[28], x[29]^y[29], x[30]^y[30], 0};

extern const char registeredApp[32];

extern char validationKey[32];
extern char validationApp[32];

void InitValidationApp();

extern "C"
{
	void Register(char *licenseKey);
}

#endif   // Register_hpp
