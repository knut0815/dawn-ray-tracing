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

#include "dawn_native/d3d12/RayTracingShaderBindingTableD3D12.h"

#include "dawn_native/d3d12/AdapterD3D12.h"
#include "dawn_native/d3d12/DeviceD3D12.h"

namespace dawn_native { namespace d3d12 {

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
    }

    MaybeError RayTracingShaderBindingTable::Initialize(
        const RayTracingShaderBindingTableDescriptor* descriptor) {
        return {};
    }

    RayTracingShaderBindingTable::~RayTracingShaderBindingTable() {
        DestroyInternal();
    }

}}  // namespace dawn_native::d3d12
