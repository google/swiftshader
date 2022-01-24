// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef sw_Configurator_hpp
#define sw_Configurator_hpp

#include <optional>
#include <string>
#include <unordered_map>

namespace sw {

class Configurator
{
public:
	// Construct a Configurator given a configuration file.
	explicit Configurator(const std::string &filePath);

	// Construct a Configurator given an in-memory stream.
	explicit Configurator(std::istream &str);

	void writeFile(const std::string &filePath, const std::string &title = "Configuration File");

	int getInteger(const std::string &sectionName, const std::string &keyName, int defaultValue = 0) const;
	bool getBoolean(const std::string &sectionName, const std::string &keyName, bool defaultValue = false) const;
	double getFloat(const std::string &sectionName, const std::string &keyName, double defaultValue = 0.0) const;

	std::string getValue(const std::string &sectionName, const std::string &keyName, const std::string &defaultValue = "") const;
	void addValue(const std::string &sectionName, const std::string &keyName, const std::string &value);

private:
	bool readConfiguration(std::istream &str);

	std::optional<std::string> getValueIfExists(const std::string &sectionName, const std::string &keyName) const;

	struct Section
	{
		std::unordered_map<std::string, std::string> keyValuePairs;
	};
	std::unordered_map<std::string, Section> sections;
};

}  // namespace sw

#endif  // sw_Configurator_hpp
