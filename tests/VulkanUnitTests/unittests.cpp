// Copyright 2018 The SwiftShader Authors. All Rights Reserved.
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

// Vulkan unit tests that provide coverage for functionality not tested by
// the dEQP test suite. Also used as a smoke test.

#include "Driver.hpp"
#include "Device.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "spirv-tools/libspirv.hpp"

#include <sstream>
#include <cstring>

class SwiftShaderVulkanTest : public testing::Test
{
};

TEST_F(SwiftShaderVulkanTest, ICD_Check)
{
    Driver driver;
    ASSERT_TRUE(driver.loadSwiftShader());

    auto createInstance = driver.vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "vkCreateInstance");
    EXPECT_NE(createInstance, nullptr);

    auto enumerateInstanceExtensionProperties =
        driver.vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceExtensionProperties");
    EXPECT_NE(enumerateInstanceExtensionProperties, nullptr);

    auto enumerateInstanceLayerProperties =
        driver.vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceLayerProperties");
    EXPECT_NE(enumerateInstanceLayerProperties, nullptr);

    auto enumerateInstanceVersion = driver.vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion");
    EXPECT_NE(enumerateInstanceVersion, nullptr);

    auto bad_function = driver.vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, "bad_function");
    EXPECT_EQ(bad_function, nullptr);
}

TEST_F(SwiftShaderVulkanTest, Version)
{
    Driver driver;
    ASSERT_TRUE(driver.loadSwiftShader());

    uint32_t apiVersion = 0;
    VkResult result = driver.vkEnumerateInstanceVersion(&apiVersion);
    EXPECT_EQ(apiVersion, (uint32_t)VK_API_VERSION_1_1);

    const VkInstanceCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,  // sType
        nullptr,                                 // pNext
        0,                                       // flags
        nullptr,                                 // pApplicationInfo
        0,                                       // enabledLayerCount
        nullptr,                                 // ppEnabledLayerNames
        0,                                       // enabledExtensionCount
        nullptr,                                 // ppEnabledExtensionNames
    };
    VkInstance instance = VK_NULL_HANDLE;
    result = driver.vkCreateInstance(&createInfo, nullptr, &instance);
    EXPECT_EQ(result, VK_SUCCESS);

    ASSERT_TRUE(driver.resolve(instance));

    uint32_t pPhysicalDeviceCount = 0;
    result = driver.vkEnumeratePhysicalDevices(instance, &pPhysicalDeviceCount, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(pPhysicalDeviceCount, 1U);

    VkPhysicalDevice pPhysicalDevice = VK_NULL_HANDLE;
    result = driver.vkEnumeratePhysicalDevices(instance, &pPhysicalDeviceCount, &pPhysicalDevice);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_NE(pPhysicalDevice, (VkPhysicalDevice)VK_NULL_HANDLE);

    VkPhysicalDeviceProperties physicalDeviceProperties;
    driver.vkGetPhysicalDeviceProperties(pPhysicalDevice, &physicalDeviceProperties);
    EXPECT_EQ(physicalDeviceProperties.apiVersion, (uint32_t)VK_API_VERSION_1_1);
    EXPECT_EQ(physicalDeviceProperties.deviceID, 0xC0DEU);
    EXPECT_EQ(physicalDeviceProperties.deviceType, VK_PHYSICAL_DEVICE_TYPE_CPU);

    EXPECT_EQ(strncmp(physicalDeviceProperties.deviceName, "SwiftShader Device", VK_MAX_PHYSICAL_DEVICE_NAME_SIZE), 0);
}

std::vector<uint32_t> compileSpirv(const char* assembly)
{
    spvtools::SpirvTools core(SPV_ENV_VULKAN_1_0);

    core.SetMessageConsumer([](spv_message_level_t, const char*, const spv_position_t& p, const char* m) {
        FAIL() << p.line << ":" << p.column << ": " << m;
    });

    std::vector<uint32_t> spirv;
    EXPECT_TRUE(core.Assemble(assembly, &spirv));
    EXPECT_TRUE(core.Validate(spirv));

    // Warn if the disassembly does not match the source assembly.
    // We do this as debugging tests in the debugger is often made much harder
    // if the SSA names (%X) in the debugger do not match the source.
    std::string disassembled;
    core.Disassemble(spirv, &disassembled, SPV_BINARY_TO_TEXT_OPTION_NO_HEADER);
    if (disassembled != assembly)
    {
        printf("-- WARNING: Disassembly does not match assembly: ---\n\n");

        auto splitLines = [](const std::string& str) -> std::vector<std::string>
        {
            std::stringstream ss(str);
            std::vector<std::string> out;
            std::string line;
            while (std::getline(ss, line, '\n')) { out.push_back(line); }
            return out;
        };

        auto srcLines = splitLines(std::string(assembly));
        auto disLines = splitLines(disassembled);

        for (size_t line = 0; line < srcLines.size() && line < disLines.size(); line++)
        {
            auto srcLine = (line < srcLines.size()) ? srcLines[line] : "<missing>";
            auto disLine = (line < disLines.size()) ? disLines[line] : "<missing>";
            if (srcLine != disLine)
            {
                printf("%zu: '%s' != '%s'\n", line, srcLine.c_str(), disLine.c_str());
            }
        }
        printf("\n\n---\n");
    }

    return spirv;
}

#define VK_ASSERT(x) ASSERT_EQ(x, VK_SUCCESS)

struct ComputeParams
{
    int localSizeX;
    int localSizeY;
    int localSizeZ;
};

class SwiftShaderVulkanComputeTest : public testing::TestWithParam<ComputeParams> {};

INSTANTIATE_TEST_CASE_P(ComputeParams, SwiftShaderVulkanComputeTest, testing::Values(
    ComputeParams{1, 1, 1},
    ComputeParams{2, 1, 1},
    ComputeParams{4, 1, 1},
    ComputeParams{8, 1, 1},
    ComputeParams{16, 1, 1},
    ComputeParams{32, 1, 1}
));

TEST_P(SwiftShaderVulkanComputeTest, Memcpy)
{
    Driver driver;
    ASSERT_TRUE(driver.loadSwiftShader());

    auto params = GetParam();

    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                params.localSizeX << " " <<
                params.localSizeY << " " <<
                params.localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 ArrayStride 4\n"
              "OpMemberDecorate %7 0 Offset 0\n"
              "OpDecorate %7 BufferBlock\n"
              "OpDecorate %8 DescriptorSet 0\n"
              "OpDecorate %8 Binding 0\n"
         "%9 = OpTypeVoid\n"
        "%10 = OpTypeFunction %9\n"
        "%11 = OpTypeInt 32 1\n"
         "%3 = OpTypeRuntimeArray %11\n"
         "%4 = OpTypeStruct %3\n"
        "%12 = OpTypePointer Uniform %4\n"
         "%5 = OpVariable %12 Uniform\n"
        "%13 = OpConstant %11 0\n"
        "%14 = OpTypeInt 32 0\n"
        "%15 = OpTypeVector %14 3\n"
        "%16 = OpTypePointer Input %15\n"
         "%2 = OpVariable %16 Input\n"
        "%17 = OpConstant %14 0\n"
        "%18 = OpTypePointer Input %14\n"
         "%6 = OpTypeRuntimeArray %11\n"
         "%7 = OpTypeStruct %6\n"
        "%19 = OpTypePointer Uniform %7\n"
         "%8 = OpVariable %19 Uniform\n"
        "%20 = OpTypePointer Uniform %11\n"
        "%21 = OpConstant %11 1\n"
         "%1 = OpFunction %9 None %10\n"
        "%22 = OpLabel\n"
        "%23 = OpAccessChain %18 %2 %17\n"
        "%24 = OpLoad %14 %23\n"
        "%25 = OpAccessChain %20 %8 %13 %24\n"
        "%26 = OpLoad %11 %25\n"
        "%27 = OpAccessChain %20 %5 %13 %24\n"
              "OpStore %27 %26\n"
              "OpReturn\n"
              "OpFunctionEnd\n";

    auto code = compileSpirv(src.str().c_str());

    const VkInstanceCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,  // sType
        nullptr,                                 // pNext
        0,                                       // flags
        nullptr,                                 // pApplicationInfo
        0,                                       // enabledLayerCount
        nullptr,                                 // ppEnabledLayerNames
        0,                                       // enabledExtensionCount
        nullptr,                                 // ppEnabledExtensionNames
    };

    VkInstance instance = VK_NULL_HANDLE;
    VK_ASSERT(driver.vkCreateInstance(&createInfo, nullptr, &instance));

    ASSERT_TRUE(driver.resolve(instance));

    Device device;
    VK_ASSERT(Device::CreateComputeDevice(&driver, instance, &device));
    ASSERT_TRUE(device.IsValid());

    constexpr int NUM_ELEMENTS = 256;

    struct Buffers
    {
        uint32_t magic0;
        uint32_t in[NUM_ELEMENTS];
        uint32_t magic1;
        uint32_t out[NUM_ELEMENTS];
        uint32_t magic2;
    };

    constexpr uint32_t magic0 = 0x01234567;
    constexpr uint32_t magic1 = 0x89abcdef;
    constexpr uint32_t magic2 = 0xfedcba99;

    VkDeviceMemory memory;
    VK_ASSERT(device.AllocateMemory(sizeof(Buffers),
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &memory));

    Buffers* buffers;
    VK_ASSERT(device.MapMemory(memory, 0, sizeof(Buffers), 0, (void**)&buffers));

    memset(buffers, 0, sizeof(Buffers));

    buffers->magic0 = magic0;
    buffers->magic1 = magic1;
    buffers->magic2 = magic2;

    for(int i = 0; i < NUM_ELEMENTS; i++)
    {
        buffers->in[i] = (uint32_t)i;
    }

    device.UnmapMemory(memory);
    buffers = nullptr;

    VkBuffer bufferIn;
    VK_ASSERT(device.CreateStorageBuffer(memory, sizeof(Buffers::in), offsetof(Buffers, in), &bufferIn));

    VkBuffer bufferOut;
    VK_ASSERT(device.CreateStorageBuffer(memory, sizeof(Buffers::out), offsetof(Buffers, out), &bufferOut));

    VkShaderModule shaderModule;
    VK_ASSERT(device.CreateShaderModule(code, &shaderModule));

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings =
    {
        {
            0,                                  // binding
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,  // descriptorType
            1,                                  // descriptorCount
            VK_SHADER_STAGE_COMPUTE_BIT,        // stageFlags
            0,                                  // pImmutableSamplers
        },
        {
            1,                                  // binding
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,  // descriptorType
            1,                                  // descriptorCount
            VK_SHADER_STAGE_COMPUTE_BIT,        // stageFlags
            0,                                  // pImmutableSamplers
        }
    };

    VkDescriptorSetLayout descriptorSetLayout;
    VK_ASSERT(device.CreateDescriptorSetLayout(descriptorSetLayoutBindings, &descriptorSetLayout));

    VkPipelineLayout pipelineLayout;
    VK_ASSERT(device.CreatePipelineLayout(descriptorSetLayout, &pipelineLayout));

    VkPipeline pipeline;
    VK_ASSERT(device.CreateComputePipeline(shaderModule, pipelineLayout, &pipeline));

    VkDescriptorPool descriptorPool;
    VK_ASSERT(device.CreateStorageBufferDescriptorPool(2, &descriptorPool));

    VkDescriptorSet descriptorSet;
    VK_ASSERT(device.AllocateDescriptorSet(descriptorPool, descriptorSetLayout, &descriptorSet));

    std::vector<VkDescriptorBufferInfo> descriptorBufferInfos =
    {
        {
            bufferIn,       // buffer
            0,              // offset
            VK_WHOLE_SIZE,  // range
        },
        {
            bufferOut,      // buffer
            0,              // offset
            VK_WHOLE_SIZE,  // range
        }
    };
    device.UpdateStorageBufferDescriptorSets(descriptorSet, descriptorBufferInfos);

    VkCommandPool commandPool;
    VK_ASSERT(device.CreateCommandPool(&commandPool));

    VkCommandBuffer commandBuffer;
    VK_ASSERT(device.AllocateCommandBuffer(commandPool, &commandBuffer));

    VK_ASSERT(device.BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, commandBuffer));

    driver.vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

    driver.vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet,
                                   0, nullptr);

    driver.vkCmdDispatch(commandBuffer, NUM_ELEMENTS / params.localSizeX, 1, 1);

    VK_ASSERT(driver.vkEndCommandBuffer(commandBuffer));

    VK_ASSERT(device.QueueSubmitAndWait(commandBuffer));

    VK_ASSERT(device.MapMemory(memory, 0, sizeof(Buffers), 0, (void**)&buffers));

    for (int i = 0; i < NUM_ELEMENTS; ++i)
    {
        EXPECT_EQ(buffers->in[i], buffers->out[i]) << "Unexpected output at " << i;
    }

    // Check for writes outside of bounds.
    EXPECT_EQ(buffers->magic0, magic0);
    EXPECT_EQ(buffers->magic1, magic1);
    EXPECT_EQ(buffers->magic2, magic2);

    device.UnmapMemory(memory);
    buffers = nullptr;
}
