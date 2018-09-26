{
global:
	# Loader-ICD interface functions
	vk_icdGetInstanceProcAddr;

	# Type-strings and type-infos required by sanitizers
	_ZTS*;
	_ZTI*;

local:
	*;
};

