# For Android, we must not export anything other than loader-related API
{
global:
	vkGetInstanceProcAddr;

	# Loader-ICD interface functions
	vk_icdGetInstanceProcAddr;
	vk_icdNegotiateLoaderICDInterfaceVersion;

	# Type-strings and type-infos required by sanitizers
	_ZTS*;
	_ZTI*;

	# Android HAL module info object
	HMI;

local:
	*;
};

