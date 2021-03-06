// Copyright 2018 The Dawn Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "dawn_native/vulkan/RayTracingShaderBindingTableVk.h"

#include "dawn_native/vulkan/AdapterVk.h"
#include "dawn_native/vulkan/DeviceVk.h"
#include "dawn_native/vulkan/FencedDeleter.h"
#include "dawn_native/vulkan/ResourceHeapVk.h"
#include "dawn_native/vulkan/ShaderModuleVk.h"
#include "dawn_native/vulkan/UtilsVulkan.h"
#include "dawn_native/vulkan/VulkanError.h"

namespace dawn_native { namespace vulkan {

    namespace {

    }  // anonymous namespace

    // static
    ResultOrError<RayTracingShaderBindingTable*> RayTracingShaderBindingTable::Create(
        Device* device,
        const RayTracingShaderBindingTableDescriptor* descriptor) {
        std::unique_ptr<RayTracingShaderBindingTable> sbt =
            std::make_unique<RayTracingShaderBindingTable>(device, descriptor);
        DAWN_TRY(sbt->Initialize(descriptor));
        return sbt.release();
    }

    void RayTracingShaderBindingTable::DestroyImpl() {
        Device* device = ToBackend(GetDevice());
        if (mGroupBuffer != VK_NULL_HANDLE) {
            device->DeallocateMemory(&mGroupBufferResource);
            device->GetFencedDeleter()->DeleteWhenUnused(mGroupBuffer);
            mGroupBuffer = VK_NULL_HANDLE;
        }
    }

    MaybeError RayTracingShaderBindingTable::Initialize(
        const RayTracingShaderBindingTableDescriptor* descriptor) {
        Device* device = ToBackend(GetDevice());
        Adapter* adapter = ToBackend(device->GetAdapter());

        VkPhysicalDeviceRayTracingPropertiesKHR rtProperties =
            GetPhysicalDeviceRayTracingProperties(*adapter);

        mShaderGroupHandleSize = rtProperties.shaderGroupHandleSize;

        mStages.reserve(descriptor->stageCount);
        for (unsigned int ii = 0; ii < descriptor->stageCount; ++ii) {
            const RayTracingShaderBindingTableStageDescriptor& stage = descriptor->stages[ii];
            VkPipelineShaderStageCreateInfo stageInfo;
            stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageInfo.pNext = nullptr;
            stageInfo.flags = 0;
            stageInfo.stage =
                static_cast<VkShaderStageFlagBits>(ToVulkanShaderStageFlags(stage.stage));
            stageInfo.module = ToBackend(stage.module)->GetHandle();
            stageInfo.pName = "main";
            stageInfo.pSpecializationInfo = nullptr;
            mStages.push_back(stageInfo);
        }

        mGroups.reserve(descriptor->groupCount);
        for (unsigned int ii = 0; ii < descriptor->groupCount; ++ii) {
            const RayTracingShaderBindingTableGroupDescriptor& group = descriptor->groups[ii];
            VkRayTracingShaderGroupCreateInfoKHR groupInfo;
            groupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
            groupInfo.pNext = nullptr;
            groupInfo.type = ToVulkanShaderBindingTableGroupType(group.type);
            groupInfo.generalShader = VK_SHADER_UNUSED_KHR;
            groupInfo.closestHitShader = VK_SHADER_UNUSED_KHR;
            groupInfo.anyHitShader = VK_SHADER_UNUSED_KHR;
            groupInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
            groupInfo.pShaderGroupCaptureReplayHandle = nullptr;
            if (group.generalIndex != -1) {
                groupInfo.generalShader = group.generalIndex;
            }
            if (group.closestHitIndex != -1) {
                groupInfo.closestHitShader = group.closestHitIndex;
            }
            if (group.anyHitIndex != -1) {
                groupInfo.anyHitShader = group.anyHitIndex;
            }
            if (group.intersectionIndex != -1) {
                groupInfo.intersectionShader = group.intersectionIndex;
            }
            mGroups.push_back(groupInfo);
        }

        uint64_t bufferSize = mGroups.size() * rtProperties.shaderGroupHandleSize;

        VkBufferCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.size = bufferSize;
        createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;

        DAWN_TRY(CheckVkSuccess(
            device->fn.CreateBuffer(device->GetVkDevice(), &createInfo, nullptr, &*mGroupBuffer),
            "vkCreateBuffer"));

        VkMemoryRequirements requirements;
        device->fn.GetBufferMemoryRequirements(device->GetVkDevice(), mGroupBuffer, &requirements);

        DAWN_TRY_ASSIGN(mGroupBufferResource, device->AllocateMemory(requirements, true));

        DAWN_TRY(CheckVkSuccess(device->fn.BindBufferMemory(
                                    device->GetVkDevice(), mGroupBuffer,
                                    ToBackend(mGroupBufferResource.GetResourceHeap())->GetMemory(),
                                    mGroupBufferResource.GetOffset()),
                                "vkBindBufferMemory"));

        return {};
    }

    RayTracingShaderBindingTable::~RayTracingShaderBindingTable() {
        DestroyInternal();
    }

    std::vector<VkRayTracingShaderGroupCreateInfoKHR>& RayTracingShaderBindingTable::GetGroups() {
        return mGroups;
    }

    std::vector<VkPipelineShaderStageCreateInfo>& RayTracingShaderBindingTable::GetStages() {
        return mStages;
    }

    VkBuffer RayTracingShaderBindingTable::GetGroupBufferHandle() const {
        return mGroupBuffer;
    }

    ResourceMemoryAllocation RayTracingShaderBindingTable::GetGroupBufferResource() const {
        return mGroupBufferResource;
    }

    uint32_t RayTracingShaderBindingTable::GetShaderGroupHandleSize() const {
        return mShaderGroupHandleSize;
    }

}}  // namespace dawn_native::vulkan
