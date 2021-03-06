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

#ifndef DAWNNATIVE_D3D12_RAY_TRACING_SHADER_BINDING_TABLE_H_
#define DAWNNATIVE_D3D12_RAY_TRACING_SHADER_BINDING_TABLE_H_

#include <vector>

#include "dawn_native/RayTracingShaderBindingTable.h"
#include "dawn_native/d3d12/BufferD3D12.h"
#include "dawn_native/d3d12/PipelineLayoutD3D12.h"
#include "dawn_native/d3d12/RayTracingPipelineD3D12.h"
#include "dawn_native/d3d12/ShaderModuleD3D12.h"
#include "dawn_native/d3d12/d3d12_platform.h"

namespace dawn_native { namespace d3d12 {

    class Device;

    class RayTracingShaderBindingTable : public RayTracingShaderBindingTableBase {
      public:
        static ResultOrError<RayTracingShaderBindingTable*> Create(
            Device* device,
            const RayTracingShaderBindingTableDescriptor* descriptor);
        ~RayTracingShaderBindingTable() override;

        uint32_t GetTableSize() const;
        ID3D12Resource* GetTableBuffer();

        MaybeError Generate(RayTracingPipeline* pipeline, PipelineLayout* pipelineLayout);

        std::vector<RayTracingShaderBindingTableStageDescriptor>& GetStages();
        std::vector<RayTracingShaderBindingTableGroupDescriptor>& GetGroups();

      private:
        using RayTracingShaderBindingTableBase::RayTracingShaderBindingTableBase;

        std::vector<RayTracingShaderBindingTableStageDescriptor> mStages;
        std::vector<RayTracingShaderBindingTableGroupDescriptor> mGroups;

        ResourceHeapAllocation mTableResource;
        ComPtr<ID3D12Resource> mTableBuffer;
        uint32_t mTableSize;

        void DestroyImpl() override;

        MaybeError Initialize(const RayTracingShaderBindingTableDescriptor* descriptor);
    };

}}  // namespace dawn_native::d3d12

#endif  // DAWNNATIVE_D3D12_RAY_TRACING_SHADER_BINDING_TABLE_H_
