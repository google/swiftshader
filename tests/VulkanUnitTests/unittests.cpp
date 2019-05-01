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

namespace
{
    size_t alignUp(size_t val, size_t alignment)
    {
        return alignment * ((val + alignment - 1) / alignment);
    }
} // anonymous namespace

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
    size_t numElements;
    int localSizeX;
    int localSizeY;
    int localSizeZ;

    friend std::ostream& operator<<(std::ostream& os, const ComputeParams& params) {
        return os << "ComputeParams{" <<
            "numElements: " << params.numElements << ", " <<
            "localSizeX: " << params.localSizeX << ", " <<
            "localSizeY: " << params.localSizeY << ", " <<
            "localSizeZ: " << params.localSizeZ <<
            "}";
    }
};

// Base class for compute tests that read from an input buffer and write to an
// output buffer of same length.
class SwiftShaderVulkanBufferToBufferComputeTest : public testing::TestWithParam<ComputeParams>
{
public:
    void test(const std::string& shader,
        std::function<uint32_t(uint32_t idx)> input,
        std::function<uint32_t(uint32_t idx)> expected);
};

void SwiftShaderVulkanBufferToBufferComputeTest::test(
        const std::string& shader,
        std::function<uint32_t(uint32_t idx)> input,
        std::function<uint32_t(uint32_t idx)> expected)
{
    auto code = compileSpirv(shader.c_str());

    Driver driver;
    ASSERT_TRUE(driver.loadSwiftShader());

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

    std::unique_ptr<Device> device;
    VK_ASSERT(Device::CreateComputeDevice(&driver, instance, device));
    ASSERT_TRUE(device->IsValid());

    // struct Buffers
    // {
    //     uint32_t pad0[63];
    //     uint32_t magic0;
    //     uint32_t in[NUM_ELEMENTS]; // Aligned to 0x100
    //     uint32_t magic1;
    //     uint32_t pad1[N];
    //     uint32_t magic2;
    //     uint32_t out[NUM_ELEMENTS]; // Aligned to 0x100
    //     uint32_t magic3;
    // };
    static constexpr uint32_t magic0 = 0x01234567;
    static constexpr uint32_t magic1 = 0x89abcdef;
    static constexpr uint32_t magic2 = 0xfedcba99;
    static constexpr uint32_t magic3 = 0x87654321;
    size_t numElements = GetParam().numElements;
    size_t alignElements = 0x100 / sizeof(uint32_t);
    size_t magic0Offset = alignElements - 1;
    size_t inOffset = 1 + magic0Offset;
    size_t magic1Offset = numElements + inOffset;
    size_t magic2Offset = alignUp(magic1Offset+1, alignElements) - 1;
    size_t outOffset = 1 + magic2Offset;
    size_t magic3Offset = numElements + outOffset;
    size_t buffersTotalElements = alignUp(1 + magic3Offset, alignElements);
    size_t buffersSize = sizeof(uint32_t) * buffersTotalElements;

    VkDeviceMemory memory;
    VK_ASSERT(device->AllocateMemory(buffersSize,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &memory));

    uint32_t* buffers;
    VK_ASSERT(device->MapMemory(memory, 0, buffersSize, 0, (void**)&buffers));

    buffers[magic0Offset] = magic0;
    buffers[magic1Offset] = magic1;
    buffers[magic2Offset] = magic2;
    buffers[magic3Offset] = magic3;

    for(size_t i = 0; i < numElements; i++)
    {
        buffers[inOffset + i] = input(i);
    }

    device->UnmapMemory(memory);
    buffers = nullptr;

    VkBuffer bufferIn;
    VK_ASSERT(device->CreateStorageBuffer(memory,
            sizeof(uint32_t) * numElements,
            sizeof(uint32_t) * inOffset,
            &bufferIn));

    VkBuffer bufferOut;
    VK_ASSERT(device->CreateStorageBuffer(memory,
            sizeof(uint32_t) * numElements,
            sizeof(uint32_t) * outOffset,
            &bufferOut));

    VkShaderModule shaderModule;
    VK_ASSERT(device->CreateShaderModule(code, &shaderModule));

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
    VK_ASSERT(device->CreateDescriptorSetLayout(descriptorSetLayoutBindings, &descriptorSetLayout));

    VkPipelineLayout pipelineLayout;
    VK_ASSERT(device->CreatePipelineLayout(descriptorSetLayout, &pipelineLayout));

    VkPipeline pipeline;
    VK_ASSERT(device->CreateComputePipeline(shaderModule, pipelineLayout, &pipeline));

    VkDescriptorPool descriptorPool;
    VK_ASSERT(device->CreateStorageBufferDescriptorPool(2, &descriptorPool));

    VkDescriptorSet descriptorSet;
    VK_ASSERT(device->AllocateDescriptorSet(descriptorPool, descriptorSetLayout, &descriptorSet));

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
    device->UpdateStorageBufferDescriptorSets(descriptorSet, descriptorBufferInfos);

    VkCommandPool commandPool;
    VK_ASSERT(device->CreateCommandPool(&commandPool));

    VkCommandBuffer commandBuffer;
    VK_ASSERT(device->AllocateCommandBuffer(commandPool, &commandBuffer));

    VK_ASSERT(device->BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, commandBuffer));

    driver.vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

    driver.vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet,
                                   0, nullptr);

    driver.vkCmdDispatch(commandBuffer, numElements / GetParam().localSizeX, 1, 1);

    VK_ASSERT(driver.vkEndCommandBuffer(commandBuffer));

    VK_ASSERT(device->QueueSubmitAndWait(commandBuffer));

    VK_ASSERT(device->MapMemory(memory, 0, buffersSize, 0, (void**)&buffers));

    for (size_t i = 0; i < numElements; ++i)
    {
        auto got = buffers[i + outOffset];
        EXPECT_EQ(expected(i), got) << "Unexpected output at " << i;
    }

    // Check for writes outside of bounds.
    EXPECT_EQ(buffers[magic0Offset], magic0);
    EXPECT_EQ(buffers[magic1Offset], magic1);
    EXPECT_EQ(buffers[magic2Offset], magic2);
    EXPECT_EQ(buffers[magic3Offset], magic3);

    device->UnmapMemory(memory);
    buffers = nullptr;
}

INSTANTIATE_TEST_SUITE_P(ComputeParams, SwiftShaderVulkanBufferToBufferComputeTest, testing::Values(
    ComputeParams{512, 1, 1, 1},
    ComputeParams{512, 2, 1, 1},
    ComputeParams{512, 4, 1, 1},
    ComputeParams{512, 8, 1, 1},
    ComputeParams{512, 16, 1, 1},
    ComputeParams{512, 32, 1, 1},

    // Non-multiple of SIMD-lane.
    ComputeParams{3, 1, 1, 1},
    ComputeParams{2, 1, 1, 1}
));

TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, Memcpy)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%11 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %11 Uniform\n"        // struct{ int32[] }* in
        "%12 = OpConstant %9 0\n"               // int32(0)
        "%13 = OpConstant %10 0\n"              // uint32(0)
        "%14 = OpTypeVector %10 3\n"            // vec3<int32>
        "%15 = OpTypePointer Input %14\n"       // vec3<int32>*
         "%2 = OpVariable %15 Input\n"          // gl_GlobalInvocationId
        "%16 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %11 Uniform\n"        // struct{ int32[] }* out
        "%17 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%18 = OpLabel\n"
        "%19 = OpAccessChain %16 %2 %13\n"      // &gl_GlobalInvocationId.x
        "%20 = OpLoad %10 %19\n"                // gl_GlobalInvocationId.x
        "%21 = OpAccessChain %17 %6 %12 %20\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%22 = OpLoad %9 %21\n"                 // out.arr[gl_GlobalInvocationId.x]
        "%23 = OpAccessChain %17 %5 %12 %20\n"  // &out.arr[gl_GlobalInvocationId.x]
              "OpStore %23 %22\n"               // out.arr[gl_GlobalInvocationId.x] = in[gl_GlobalInvocationId.x]
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return i; });
}

TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, GlobalInvocationId)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%11 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %11 Uniform\n"        // struct{ int32[] }* in
        "%12 = OpConstant %9 0\n"               // int32(0)
        "%13 = OpConstant %9 1\n"               // int32(1)
        "%14 = OpConstant %10 0\n"              // uint32(0)
        "%15 = OpConstant %10 1\n"              // uint32(1)
        "%16 = OpConstant %10 2\n"              // uint32(2)
        "%17 = OpTypeVector %10 3\n"            // vec3<int32>
        "%18 = OpTypePointer Input %17\n"       // vec3<int32>*
         "%2 = OpVariable %18 Input\n"          // gl_GlobalInvocationId
        "%19 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %11 Uniform\n"        // struct{ int32[] }* out
        "%20 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%21 = OpLabel\n"
        "%22 = OpAccessChain %19 %2 %14\n"      // &gl_GlobalInvocationId.x
        "%23 = OpAccessChain %19 %2 %15\n"      // &gl_GlobalInvocationId.y
        "%24 = OpAccessChain %19 %2 %16\n"      // &gl_GlobalInvocationId.z
        "%25 = OpLoad %10 %22\n"                // gl_GlobalInvocationId.x
        "%26 = OpLoad %10 %23\n"                // gl_GlobalInvocationId.y
        "%27 = OpLoad %10 %24\n"                // gl_GlobalInvocationId.z
        "%28 = OpAccessChain %20 %6 %12 %25\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%29 = OpLoad %9 %28\n"                 // out.arr[gl_GlobalInvocationId.x]
        "%30 = OpIAdd %9 %29 %26\n"             // in[gl_GlobalInvocationId.x] + gl_GlobalInvocationId.y
        "%31 = OpIAdd %9 %30 %27\n"             // in[gl_GlobalInvocationId.x] + gl_GlobalInvocationId.y + gl_GlobalInvocationId.z
        "%32 = OpAccessChain %20 %5 %12 %25\n"  // &out.arr[gl_GlobalInvocationId.x]
              "OpStore %32 %31\n"               // out.arr[gl_GlobalInvocationId.x] = in[gl_GlobalInvocationId.x] + gl_GlobalInvocationId.y + gl_GlobalInvocationId.z
              "OpReturn\n"
              "OpFunctionEnd\n";

    // gl_GlobalInvocationId.y and gl_GlobalInvocationId.z should both be zero.
    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return i; });
}

TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, BranchSimple)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%11 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %11 Uniform\n"        // struct{ int32[] }* in
        "%12 = OpConstant %9 0\n"               // int32(0)
        "%13 = OpConstant %10 0\n"              // uint32(0)
        "%14 = OpTypeVector %10 3\n"            // vec3<int32>
        "%15 = OpTypePointer Input %14\n"       // vec3<int32>*
         "%2 = OpVariable %15 Input\n"          // gl_GlobalInvocationId
        "%16 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %11 Uniform\n"        // struct{ int32[] }* out
        "%17 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%18 = OpLabel\n"
        "%19 = OpAccessChain %16 %2 %13\n"      // &gl_GlobalInvocationId.x
        "%20 = OpLoad %10 %19\n"                // gl_GlobalInvocationId.x
        "%21 = OpAccessChain %17 %6 %12 %20\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%22 = OpLoad %9 %21\n"                 // in.arr[gl_GlobalInvocationId.x]
        "%23 = OpAccessChain %17 %5 %12 %20\n"  // &out.arr[gl_GlobalInvocationId.x]
    // Start of branch logic
    // %22 = in value
              "OpBranch %24\n"
        "%24 = OpLabel\n"
              "OpBranch %25\n"
        "%25 = OpLabel\n"
              "OpBranch %26\n"
        "%26 = OpLabel\n"
    // %22 = out value
    // End of branch logic
              "OpStore %23 %22\n"
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return i; });
}

TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, BranchDeclareSSA)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%11 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %11 Uniform\n"        // struct{ int32[] }* in
        "%12 = OpConstant %9 0\n"               // int32(0)
        "%13 = OpConstant %10 0\n"              // uint32(0)
        "%14 = OpTypeVector %10 3\n"            // vec3<int32>
        "%15 = OpTypePointer Input %14\n"       // vec3<int32>*
         "%2 = OpVariable %15 Input\n"          // gl_GlobalInvocationId
        "%16 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %11 Uniform\n"        // struct{ int32[] }* out
        "%17 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%18 = OpLabel\n"
        "%19 = OpAccessChain %16 %2 %13\n"      // &gl_GlobalInvocationId.x
        "%20 = OpLoad %10 %19\n"                // gl_GlobalInvocationId.x
        "%21 = OpAccessChain %17 %6 %12 %20\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%22 = OpLoad %9 %21\n"                 // in.arr[gl_GlobalInvocationId.x]
        "%23 = OpAccessChain %17 %5 %12 %20\n"  // &out.arr[gl_GlobalInvocationId.x]
    // Start of branch logic
    // %22 = in value
              "OpBranch %24\n"
        "%24 = OpLabel\n"
        "%25 = OpIAdd %9 %22 %22\n"             // %25 = in*2
              "OpBranch %26\n"
        "%26 = OpLabel\n"
              "OpBranch %27\n"
        "%27 = OpLabel\n"
    // %25 = out value
    // End of branch logic
              "OpStore %23 %25\n"               // use SSA value from previous block
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return i * 2; });
}

TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, BranchConditionalSimple)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
        "%11 = OpTypeBool\n"
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%12 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %12 Uniform\n"        // struct{ int32[] }* in
        "%13 = OpConstant %9 0\n"               // int32(0)
        "%14 = OpConstant %9 2\n"               // int32(2)
        "%15 = OpConstant %10 0\n"              // uint32(0)
        "%16 = OpTypeVector %10 3\n"            // vec4<int32>
        "%17 = OpTypePointer Input %16\n"       // vec4<int32>*
         "%2 = OpVariable %17 Input\n"          // gl_GlobalInvocationId
        "%18 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %12 Uniform\n"        // struct{ int32[] }* out
        "%19 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%20 = OpLabel\n"
        "%21 = OpAccessChain %18 %2 %15\n"      // &gl_GlobalInvocationId.x
        "%22 = OpLoad %10 %21\n"                // gl_GlobalInvocationId.x
        "%23 = OpAccessChain %19 %6 %13 %22\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%24 = OpLoad %9 %23\n"                 // in.arr[gl_GlobalInvocationId.x]
        "%25 = OpAccessChain %19 %5 %13 %22\n"  // &out.arr[gl_GlobalInvocationId.x]
    // Start of branch logic
    // %24 = in value
        "%26 = OpSMod %9 %24 %14\n"             // in % 2
        "%27 = OpIEqual %11 %26 %13\n"          // (in % 2) == 0
              "OpSelectionMerge %28 None\n"
              "OpBranchConditional %27 %28 %28\n" // Both go to %28
        "%28 = OpLabel\n"
    // %26 = out value
    // End of branch logic
              "OpStore %25 %26\n"               // use SSA value from previous block
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return i%2; });
}

TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, BranchConditionalTwoEmptyBlocks)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
        "%11 = OpTypeBool\n"
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%12 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %12 Uniform\n"        // struct{ int32[] }* in
        "%13 = OpConstant %9 0\n"               // int32(0)
        "%14 = OpConstant %9 2\n"               // int32(2)
        "%15 = OpConstant %10 0\n"              // uint32(0)
        "%16 = OpTypeVector %10 3\n"            // vec4<int32>
        "%17 = OpTypePointer Input %16\n"       // vec4<int32>*
         "%2 = OpVariable %17 Input\n"          // gl_GlobalInvocationId
        "%18 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %12 Uniform\n"        // struct{ int32[] }* out
        "%19 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%20 = OpLabel\n"
        "%21 = OpAccessChain %18 %2 %15\n"      // &gl_GlobalInvocationId.x
        "%22 = OpLoad %10 %21\n"                // gl_GlobalInvocationId.x
        "%23 = OpAccessChain %19 %6 %13 %22\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%24 = OpLoad %9 %23\n"                 // in.arr[gl_GlobalInvocationId.x]
        "%25 = OpAccessChain %19 %5 %13 %22\n"  // &out.arr[gl_GlobalInvocationId.x]
    // Start of branch logic
    // %24 = in value
        "%26 = OpSMod %9 %24 %14\n"             // in % 2
        "%27 = OpIEqual %11 %26 %13\n"          // (in % 2) == 0
              "OpSelectionMerge %28 None\n"
              "OpBranchConditional %27 %29 %30\n"
        "%29 = OpLabel\n"                       // (in % 2) == 0
              "OpBranch %28\n"
        "%30 = OpLabel\n"                       // (in % 2) != 0
              "OpBranch %28\n"
        "%28 = OpLabel\n"
    // %26 = out value
    // End of branch logic
              "OpStore %25 %26\n"               // use SSA value from previous block
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return i%2; });
}

// TODO: Test for parallel assignment
TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, BranchConditionalStore)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
        "%11 = OpTypeBool\n"
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%12 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %12 Uniform\n"        // struct{ int32[] }* in
        "%13 = OpConstant %9 0\n"               // int32(0)
        "%14 = OpConstant %9 1\n"               // int32(1)
        "%15 = OpConstant %9 2\n"               // int32(2)
        "%16 = OpConstant %10 0\n"              // uint32(0)
        "%17 = OpTypeVector %10 3\n"            // vec4<int32>
        "%18 = OpTypePointer Input %17\n"       // vec4<int32>*
         "%2 = OpVariable %18 Input\n"          // gl_GlobalInvocationId
        "%19 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %12 Uniform\n"        // struct{ int32[] }* out
        "%20 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%21 = OpLabel\n"
        "%22 = OpAccessChain %19 %2 %16\n"      // &gl_GlobalInvocationId.x
        "%23 = OpLoad %10 %22\n"                // gl_GlobalInvocationId.x
        "%24 = OpAccessChain %20 %6 %13 %23\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%25 = OpLoad %9 %24\n"                 // in.arr[gl_GlobalInvocationId.x]
        "%26 = OpAccessChain %20 %5 %13 %23\n"  // &out.arr[gl_GlobalInvocationId.x]
    // Start of branch logic
    // %25 = in value
        "%27 = OpSMod %9 %25 %15\n"             // in % 2
        "%28 = OpIEqual %11 %27 %13\n"          // (in % 2) == 0
              "OpSelectionMerge %29 None\n"
              "OpBranchConditional %28 %30 %31\n"
        "%30 = OpLabel\n"                       // (in % 2) == 0
              "OpStore %26 %14\n"               // write 1
              "OpBranch %29\n"
        "%31 = OpLabel\n"                       // (in % 2) != 0
              "OpStore %26 %15\n"               // write 2
              "OpBranch %29\n"
        "%29 = OpLabel\n"
    // End of branch logic
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return (i % 2) == 0 ? 1 : 2; });
}

TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, BranchConditionalReturnTrue)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
        "%11 = OpTypeBool\n"
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%12 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %12 Uniform\n"        // struct{ int32[] }* in
        "%13 = OpConstant %9 0\n"               // int32(0)
        "%14 = OpConstant %9 1\n"               // int32(1)
        "%15 = OpConstant %9 2\n"               // int32(2)
        "%16 = OpConstant %10 0\n"              // uint32(0)
        "%17 = OpTypeVector %10 3\n"            // vec4<int32>
        "%18 = OpTypePointer Input %17\n"       // vec4<int32>*
         "%2 = OpVariable %18 Input\n"          // gl_GlobalInvocationId
        "%19 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %12 Uniform\n"        // struct{ int32[] }* out
        "%20 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%21 = OpLabel\n"
        "%22 = OpAccessChain %19 %2 %16\n"      // &gl_GlobalInvocationId.x
        "%23 = OpLoad %10 %22\n"                // gl_GlobalInvocationId.x
        "%24 = OpAccessChain %20 %6 %13 %23\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%25 = OpLoad %9 %24\n"                 // in.arr[gl_GlobalInvocationId.x]
        "%26 = OpAccessChain %20 %5 %13 %23\n"  // &out.arr[gl_GlobalInvocationId.x]
    // Start of branch logic
    // %25 = in value
        "%27 = OpSMod %9 %25 %15\n"             // in % 2
        "%28 = OpIEqual %11 %27 %13\n"          // (in % 2) == 0
              "OpSelectionMerge %29 None\n"
              "OpBranchConditional %28 %30 %29\n"
        "%30 = OpLabel\n"                       // (in % 2) == 0
              "OpReturn\n"
        "%29 = OpLabel\n"                       // merge
              "OpStore %26 %15\n"               // write 2
    // End of branch logic
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return (i % 2) == 0 ? 0 : 2; });
}

// TODO: Test for parallel assignment
TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, BranchConditionalPhi)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
        "%11 = OpTypeBool\n"
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%12 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %12 Uniform\n"        // struct{ int32[] }* in
        "%13 = OpConstant %9 0\n"               // int32(0)
        "%14 = OpConstant %9 1\n"               // int32(1)
        "%15 = OpConstant %9 2\n"               // int32(2)
        "%16 = OpConstant %10 0\n"              // uint32(0)
        "%17 = OpTypeVector %10 3\n"            // vec4<int32>
        "%18 = OpTypePointer Input %17\n"       // vec4<int32>*
         "%2 = OpVariable %18 Input\n"          // gl_GlobalInvocationId
        "%19 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %12 Uniform\n"        // struct{ int32[] }* out
        "%20 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%21 = OpLabel\n"
        "%22 = OpAccessChain %19 %2 %16\n"      // &gl_GlobalInvocationId.x
        "%23 = OpLoad %10 %22\n"                // gl_GlobalInvocationId.x
        "%24 = OpAccessChain %20 %6 %13 %23\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%25 = OpLoad %9 %24\n"                 // in.arr[gl_GlobalInvocationId.x]
        "%26 = OpAccessChain %20 %5 %13 %23\n"  // &out.arr[gl_GlobalInvocationId.x]
    // Start of branch logic
    // %25 = in value
        "%27 = OpSMod %9 %25 %15\n"             // in % 2
        "%28 = OpIEqual %11 %27 %13\n"          // (in % 2) == 0
              "OpSelectionMerge %29 None\n"
              "OpBranchConditional %28 %30 %31\n"
        "%30 = OpLabel\n"                       // (in % 2) == 0
              "OpBranch %29\n"
        "%31 = OpLabel\n"                       // (in % 2) != 0
              "OpBranch %29\n"
        "%29 = OpLabel\n"
        "%32 = OpPhi %9 %14 %30 %15 %31\n"      // (in % 2) == 0 ? 1 : 2
    // End of branch logic
              "OpStore %26 %32\n"
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return (i % 2) == 0 ? 1 : 2; });
}

TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, SwitchEmptyCases)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
        "%11 = OpTypeBool\n"
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%12 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %12 Uniform\n"        // struct{ int32[] }* in
        "%13 = OpConstant %9 0\n"               // int32(0)
        "%14 = OpConstant %9 2\n"               // int32(2)
        "%15 = OpConstant %10 0\n"              // uint32(0)
        "%16 = OpTypeVector %10 3\n"            // vec4<int32>
        "%17 = OpTypePointer Input %16\n"       // vec4<int32>*
         "%2 = OpVariable %17 Input\n"          // gl_GlobalInvocationId
        "%18 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %12 Uniform\n"        // struct{ int32[] }* out
        "%19 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%20 = OpLabel\n"
        "%21 = OpAccessChain %18 %2 %15\n"      // &gl_GlobalInvocationId.x
        "%22 = OpLoad %10 %21\n"                // gl_GlobalInvocationId.x
        "%23 = OpAccessChain %19 %6 %13 %22\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%24 = OpLoad %9 %23\n"                 // in.arr[gl_GlobalInvocationId.x]
        "%25 = OpAccessChain %19 %5 %13 %22\n"  // &out.arr[gl_GlobalInvocationId.x]
    // Start of branch logic
    // %24 = in value
        "%26 = OpSMod %9 %24 %14\n"             // in % 2
              "OpSelectionMerge %27 None\n"
              "OpSwitch %26 %27 0 %28 1 %29\n"
        "%28 = OpLabel\n"                       // (in % 2) == 0
              "OpBranch %27\n"
        "%29 = OpLabel\n"                       // (in % 2) == 1
              "OpBranch %27\n"
        "%27 = OpLabel\n"
    // %26 = out value
    // End of branch logic
              "OpStore %25 %26\n"               // use SSA value from previous block
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return i%2; });
}

TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, SwitchStore)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
        "%11 = OpTypeBool\n"
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%12 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %12 Uniform\n"        // struct{ int32[] }* in
        "%13 = OpConstant %9 0\n"               // int32(0)
        "%14 = OpConstant %9 1\n"               // int32(1)
        "%15 = OpConstant %9 2\n"               // int32(2)
        "%16 = OpConstant %10 0\n"              // uint32(0)
        "%17 = OpTypeVector %10 3\n"            // vec4<int32>
        "%18 = OpTypePointer Input %17\n"       // vec4<int32>*
         "%2 = OpVariable %18 Input\n"          // gl_GlobalInvocationId
        "%19 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %12 Uniform\n"        // struct{ int32[] }* out
        "%20 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%21 = OpLabel\n"
        "%22 = OpAccessChain %19 %2 %16\n"      // &gl_GlobalInvocationId.x
        "%23 = OpLoad %10 %22\n"                // gl_GlobalInvocationId.x
        "%24 = OpAccessChain %20 %6 %13 %23\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%25 = OpLoad %9 %24\n"                 // in.arr[gl_GlobalInvocationId.x]
        "%26 = OpAccessChain %20 %5 %13 %23\n"  // &out.arr[gl_GlobalInvocationId.x]
    // Start of branch logic
    // %25 = in value
        "%27 = OpSMod %9 %25 %15\n"             // in % 2
              "OpSelectionMerge %28 None\n"
              "OpSwitch %27 %28 0 %29 1 %30\n"
        "%29 = OpLabel\n"                       // (in % 2) == 0
              "OpStore %26 %15\n"               // write 2
              "OpBranch %28\n"
        "%30 = OpLabel\n"                       // (in % 2) == 1
              "OpStore %26 %14\n"               // write 1
              "OpBranch %28\n"
        "%28 = OpLabel\n"
    // End of branch logic
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return (i % 2) == 0 ? 2 : 1; });
}

TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, SwitchCaseReturn)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
        "%11 = OpTypeBool\n"
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%12 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %12 Uniform\n"        // struct{ int32[] }* in
        "%13 = OpConstant %9 0\n"               // int32(0)
        "%14 = OpConstant %9 1\n"               // int32(1)
        "%15 = OpConstant %9 2\n"               // int32(2)
        "%16 = OpConstant %10 0\n"              // uint32(0)
        "%17 = OpTypeVector %10 3\n"            // vec4<int32>
        "%18 = OpTypePointer Input %17\n"       // vec4<int32>*
         "%2 = OpVariable %18 Input\n"          // gl_GlobalInvocationId
        "%19 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %12 Uniform\n"        // struct{ int32[] }* out
        "%20 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%21 = OpLabel\n"
        "%22 = OpAccessChain %19 %2 %16\n"      // &gl_GlobalInvocationId.x
        "%23 = OpLoad %10 %22\n"                // gl_GlobalInvocationId.x
        "%24 = OpAccessChain %20 %6 %13 %23\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%25 = OpLoad %9 %24\n"                 // in.arr[gl_GlobalInvocationId.x]
        "%26 = OpAccessChain %20 %5 %13 %23\n"  // &out.arr[gl_GlobalInvocationId.x]
    // Start of branch logic
    // %25 = in value
        "%27 = OpSMod %9 %25 %15\n"             // in % 2
              "OpSelectionMerge %28 None\n"
              "OpSwitch %27 %28 0 %29 1 %30\n"
        "%29 = OpLabel\n"                       // (in % 2) == 0
              "OpBranch %28\n"
        "%30 = OpLabel\n"                       // (in % 2) == 1
              "OpReturn\n"
        "%28 = OpLabel\n"
              "OpStore %26 %14\n"               // write 1
    // End of branch logic
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return (i % 2) == 1 ? 0 : 1; });
}

TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, SwitchDefaultReturn)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
        "%11 = OpTypeBool\n"
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%12 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %12 Uniform\n"        // struct{ int32[] }* in
        "%13 = OpConstant %9 0\n"               // int32(0)
        "%14 = OpConstant %9 1\n"               // int32(1)
        "%15 = OpConstant %9 2\n"               // int32(2)
        "%16 = OpConstant %10 0\n"              // uint32(0)
        "%17 = OpTypeVector %10 3\n"            // vec4<int32>
        "%18 = OpTypePointer Input %17\n"       // vec4<int32>*
         "%2 = OpVariable %18 Input\n"          // gl_GlobalInvocationId
        "%19 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %12 Uniform\n"        // struct{ int32[] }* out
        "%20 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%21 = OpLabel\n"
        "%22 = OpAccessChain %19 %2 %16\n"      // &gl_GlobalInvocationId.x
        "%23 = OpLoad %10 %22\n"                // gl_GlobalInvocationId.x
        "%24 = OpAccessChain %20 %6 %13 %23\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%25 = OpLoad %9 %24\n"                 // in.arr[gl_GlobalInvocationId.x]
        "%26 = OpAccessChain %20 %5 %13 %23\n"  // &out.arr[gl_GlobalInvocationId.x]
    // Start of branch logic
    // %25 = in value
        "%27 = OpSMod %9 %25 %15\n"             // in % 2
              "OpSelectionMerge %28 None\n"
              "OpSwitch %27 %29 1 %30\n"
        "%30 = OpLabel\n"                       // (in % 2) == 1
              "OpBranch %28\n"
        "%29 = OpLabel\n"                       // (in % 2) != 1
              "OpReturn\n"
        "%28 = OpLabel\n"                       // merge
              "OpStore %26 %14\n"               // write 1
    // End of branch logic
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return (i % 2) == 1 ? 1 : 0; });
}

TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, SwitchCaseFallthrough)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
        "%11 = OpTypeBool\n"
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%12 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %12 Uniform\n"        // struct{ int32[] }* in
        "%13 = OpConstant %9 0\n"               // int32(0)
        "%14 = OpConstant %9 1\n"               // int32(1)
        "%15 = OpConstant %9 2\n"               // int32(2)
        "%16 = OpConstant %10 0\n"              // uint32(0)
        "%17 = OpTypeVector %10 3\n"            // vec4<int32>
        "%18 = OpTypePointer Input %17\n"       // vec4<int32>*
         "%2 = OpVariable %18 Input\n"          // gl_GlobalInvocationId
        "%19 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %12 Uniform\n"        // struct{ int32[] }* out
        "%20 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%21 = OpLabel\n"
        "%22 = OpAccessChain %19 %2 %16\n"      // &gl_GlobalInvocationId.x
        "%23 = OpLoad %10 %22\n"                // gl_GlobalInvocationId.x
        "%24 = OpAccessChain %20 %6 %13 %23\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%25 = OpLoad %9 %24\n"                 // in.arr[gl_GlobalInvocationId.x]
        "%26 = OpAccessChain %20 %5 %13 %23\n"  // &out.arr[gl_GlobalInvocationId.x]
    // Start of branch logic
    // %25 = in value
        "%27 = OpSMod %9 %25 %15\n"             // in % 2
              "OpSelectionMerge %28 None\n"
              "OpSwitch %27 %29 0 %30 1 %31\n"
        "%30 = OpLabel\n"                       // (in % 2) == 0
        "%32 = OpIAdd %9 %27 %14\n"             // generate an intermediate
              "OpStore %26 %32\n"               // write a value (overwritten later)
              "OpBranch %31\n"                  // fallthrough
        "%31 = OpLabel\n"                       // (in % 2) == 1
              "OpStore %26 %15\n"               // write 2
              "OpBranch %28\n"
        "%29 = OpLabel\n"                       // unreachable
              "OpUnreachable\n"
        "%28 = OpLabel\n"                       // merge
    // End of branch logic
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return 2; });
}

TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, SwitchDefaultFallthrough)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
        "%11 = OpTypeBool\n"
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%12 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %12 Uniform\n"        // struct{ int32[] }* in
        "%13 = OpConstant %9 0\n"               // int32(0)
        "%14 = OpConstant %9 1\n"               // int32(1)
        "%15 = OpConstant %9 2\n"               // int32(2)
        "%16 = OpConstant %10 0\n"              // uint32(0)
        "%17 = OpTypeVector %10 3\n"            // vec4<int32>
        "%18 = OpTypePointer Input %17\n"       // vec4<int32>*
         "%2 = OpVariable %18 Input\n"          // gl_GlobalInvocationId
        "%19 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %12 Uniform\n"        // struct{ int32[] }* out
        "%20 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%21 = OpLabel\n"
        "%22 = OpAccessChain %19 %2 %16\n"      // &gl_GlobalInvocationId.x
        "%23 = OpLoad %10 %22\n"                // gl_GlobalInvocationId.x
        "%24 = OpAccessChain %20 %6 %13 %23\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%25 = OpLoad %9 %24\n"                 // in.arr[gl_GlobalInvocationId.x]
        "%26 = OpAccessChain %20 %5 %13 %23\n"  // &out.arr[gl_GlobalInvocationId.x]
    // Start of branch logic
    // %25 = in value
        "%27 = OpSMod %9 %25 %15\n"             // in % 2
              "OpSelectionMerge %28 None\n"
              "OpSwitch %27 %29 0 %30 1 %31\n"
        "%30 = OpLabel\n"                       // (in % 2) == 0
        "%32 = OpIAdd %9 %27 %14\n"             // generate an intermediate
              "OpStore %26 %32\n"               // write a value (overwritten later)
              "OpBranch %29\n"                  // fallthrough
        "%29 = OpLabel\n"                       // default
        "%33 = OpIAdd %9 %27 %14\n"             // generate an intermediate
              "OpStore %26 %33\n"               // write a value (overwritten later)
              "OpBranch %31\n"                  // fallthrough
        "%31 = OpLabel\n"                       // (in % 2) == 1
              "OpStore %26 %15\n"               // write 2
              "OpBranch %28\n"
        "%28 = OpLabel\n"                       // merge
    // End of branch logic
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return 2; });
}

TEST_P(SwiftShaderVulkanBufferToBufferComputeTest, SwitchPhi)
{
    std::stringstream src;
    src <<
              "OpCapability Shader\n"
              "OpMemoryModel Logical GLSL450\n"
              "OpEntryPoint GLCompute %1 \"main\" %2\n"
              "OpExecutionMode %1 LocalSize " <<
                GetParam().localSizeX << " " <<
                GetParam().localSizeY << " " <<
                GetParam().localSizeZ << "\n" <<
              "OpDecorate %3 ArrayStride 4\n"
              "OpMemberDecorate %4 0 Offset 0\n"
              "OpDecorate %4 BufferBlock\n"
              "OpDecorate %5 DescriptorSet 0\n"
              "OpDecorate %5 Binding 1\n"
              "OpDecorate %2 BuiltIn GlobalInvocationId\n"
              "OpDecorate %6 DescriptorSet 0\n"
              "OpDecorate %6 Binding 0\n"
         "%7 = OpTypeVoid\n"
         "%8 = OpTypeFunction %7\n"             // void()
         "%9 = OpTypeInt 32 1\n"                // int32
        "%10 = OpTypeInt 32 0\n"                // uint32
        "%11 = OpTypeBool\n"
         "%3 = OpTypeRuntimeArray %9\n"         // int32[]
         "%4 = OpTypeStruct %3\n"               // struct{ int32[] }
        "%12 = OpTypePointer Uniform %4\n"      // struct{ int32[] }*
         "%5 = OpVariable %12 Uniform\n"        // struct{ int32[] }* in
        "%13 = OpConstant %9 0\n"               // int32(0)
        "%14 = OpConstant %9 1\n"               // int32(1)
        "%15 = OpConstant %9 2\n"               // int32(2)
        "%16 = OpConstant %10 0\n"              // uint32(0)
        "%17 = OpTypeVector %10 3\n"            // vec4<int32>
        "%18 = OpTypePointer Input %17\n"       // vec4<int32>*
         "%2 = OpVariable %18 Input\n"          // gl_GlobalInvocationId
        "%19 = OpTypePointer Input %10\n"       // uint32*
         "%6 = OpVariable %12 Uniform\n"        // struct{ int32[] }* out
        "%20 = OpTypePointer Uniform %9\n"      // int32*
         "%1 = OpFunction %7 None %8\n"         // -- Function begin --
        "%21 = OpLabel\n"
        "%22 = OpAccessChain %19 %2 %16\n"      // &gl_GlobalInvocationId.x
        "%23 = OpLoad %10 %22\n"                // gl_GlobalInvocationId.x
        "%24 = OpAccessChain %20 %6 %13 %23\n"  // &in.arr[gl_GlobalInvocationId.x]
        "%25 = OpLoad %9 %24\n"                 // in.arr[gl_GlobalInvocationId.x]
        "%26 = OpAccessChain %20 %5 %13 %23\n"  // &out.arr[gl_GlobalInvocationId.x]
    // Start of branch logic
    // %25 = in value
        "%27 = OpSMod %9 %25 %15\n"             // in % 2
              "OpSelectionMerge %28 None\n"
              "OpSwitch %27 %29 1 %30\n"
        "%30 = OpLabel\n"                       // (in % 2) == 1
              "OpBranch %28\n"
        "%29 = OpLabel\n"                       // (in % 2) != 1
              "OpBranch %28\n"
        "%28 = OpLabel\n"                       // merge
        "%31 = OpPhi %9 %14 %30 %15 %29\n"      // (in % 2) == 1 ? 1 : 2
              "OpStore %26 %31\n"
    // End of branch logic
              "OpReturn\n"
              "OpFunctionEnd\n";

    test(src.str(), [](uint32_t i) { return i; }, [](uint32_t i) { return (i % 2) == 1 ? 1 : 2; });
}
