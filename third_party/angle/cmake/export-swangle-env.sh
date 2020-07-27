#!/usr/bin/env bash
export VK_ICD_FILENAMES=set VK_ICD_FILENAMES=${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/vk_swiftshader_icd.json
export ANGLE_DEFAULT_PLATFORM="vulkan"
