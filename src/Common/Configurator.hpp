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

#ifndef sw_Configurator_hpp
#define sw_Configurator_hpp

#include <string>
#include <vector>

#include <stdlib.h>

namespace sw
{
	class Configurator  
	{
	public:
		Configurator(std::string iniPath = "");

		~Configurator();

		std::string getValue(std::string sectionName, std::string valueName, std::string defaultValue = "") const; 
		int getInteger(std::string sectionName, std::string valueName, int defaultValue = 0) const;
		bool getBoolean(std::string sectionName, std::string valueName, bool defaultValue = false) const;
		double getFloat(std::string sectionName, std::string valueName, double defaultValue = 0.0) const;
		unsigned int getFormatted(std::string sectionName, std::string valueName, char *format,
								void *v1 = 0, void *v2 = 0, void *v3 = 0, void *v4 = 0,
  								void *v5 = 0, void *v6 = 0, void *v7 = 0, void *v8 = 0,
  								void *v9 = 0, void *v10 = 0, void *v11 = 0, void *v12 = 0,
  								void *v13 = 0, void *v14 = 0, void *v15 = 0, void *v16 = 0);

		void addValue(std::string sectionName, std::string valueName, std::string value);

		void writeFile(std::string title = "Configuration File");

	private:
		bool readFile();

		unsigned int addKeyName(std::string sectionName);
		int findKey(std::string sectionName) const;
		int findValue(unsigned int sectionID, std::string valueName) const;

		std::string path;

		struct Section
		{
			std::vector<std::string> names;
			std::vector<std::string> values; 
		};

		std::vector<Section> sections;
		std::vector<std::string> names;
	};
}

#endif   // sw_Configurator_hpp
