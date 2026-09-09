// intel_gem_backing.cpp
// VesperaOS - operating system for the x86_64 architecture
//
// Copyright (c) 2026 Linus Genz <linuslinuxgenz@gmail.com>
//
// Created by Linus Genz on 09.09.26.
//
// This file is part of VesperaOS.
//
// VesperaOS is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// VesperaOS is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VesperaOS. If not, see <https://www.gnu.org/licenses/>.

#include "intel_gem_backing.h"

#include "intel_gpu_device.h"

namespace gpu::intel::core {

    IntelGemBackingObject::IntelGemBackingObject(IntelGpuDevice* device, const u32 handle)
        : device_(device)
        , handle_(handle) {
    }

    phys_addr_t IntelGemBackingObject::get_page(const usize offset_in_bytes) {
        IntelGpuDevice::GemObjectInfo info{};
        if (!device_->query_gem_object(handle_, &info)) {
            return phys_addr_t{};
        }

        if (offset_in_bytes >= info.size) {
            return phys_addr_t{};
        }

        // GEM_CREATE's backing is contiguous (see LucGemObject), so any
        // in-range offset is just phys_addr + offset — no page table walk.
        return phys_add(info.phys_addr, offset_in_bytes);
    }

    usize IntelGemBackingObject::get_size() const {
        IntelGpuDevice::GemObjectInfo info{};
        if (!device_->query_gem_object(handle_, &info)) {
            return 0;
        }
        return info.size;
    }

} // namespace gpu::intel::core
