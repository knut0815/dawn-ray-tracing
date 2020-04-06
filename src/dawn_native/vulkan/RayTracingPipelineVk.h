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

#ifndef DAWNNATIVE_VULKAN_RAY_TRACING_PIPELINE_H_
#define DAWNNATIVE_VULKAN_RAY_TRACING_PIPELINE_H_

#include "dawn_native/RayTracingPipeline.h"
#include "dawn_native/DynamicUploader.h"
#include "dawn_native/ResourceMemoryAllocation.h"

#include "common/vulkan_platform.h"
#include "dawn_native/Error.h"

#include "dawn_native/vulkan/BufferVk.h"

namespace dawn_native { namespace vulkan {

    class Device;

    class RayTracingPipeline : public RayTracingPipelineBase {
      public:
        static ResultOrError<RayTracingPipeline*> Create(
            Device* device,
            const RayTracingPipelineDescriptor* descriptor);
        ~RayTracingPipeline();

        VkPipeline GetHandle() const;

      private:
        using RayTracingPipelineBase::RayTracingPipelineBase;
        MaybeError Initialize(const RayTracingPipelineDescriptor* descriptor);

        VkPipeline mHandle = VK_NULL_HANDLE;
    };

}}  // namespace dawn_native::vulkan

#endif  // DAWNNATIVE_VULKAN_RAY_TRACING_PIPELINE_H_
