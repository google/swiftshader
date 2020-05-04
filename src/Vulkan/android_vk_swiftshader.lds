# For Android, we must not export anything other than loader-related API
{
global:
	vkGetInstanceProcAddr;

	# Loader-ICD interface functions
	vk_icdGetInstanceProcAddr;
	vk_icdNegotiateLoaderICDInterfaceVersion;

	# Android HAL module info object
	HMI;

local:
	*;
};

