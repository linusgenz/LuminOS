// intel_gem_backing.h
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

#ifndef VESPERAOS_INTEL_GEM_BACKING_H
#define VESPERAOS_INTEL_GEM_BACKING_H

#include <vespera/types.h>
#include <vespera/mm/addr.h>
#include <vespera/mm/vm_backing.h>

namespace gpu::intel::core {
    class IntelGpuDevice;
 
    /**
     * One instance is created per mmap() call (see IntelGpuDevice::
     * get_backing_object()) rather than cached per-handle: bring-up scope
     * doesn't need the shared-object lifetime tracking ShmObject/
     * FileBackingObject do, so add_mapping()/remove_mapping() are no-ops
     * for now. Revisit if multiple concurrent mmap()s of the same handle
     * need to share teardown/refcounting.
     */
    class IntelGemBackingObject final : public kernel::vm::VmBackingObject {
    public:
        IntelGemBackingObject(IntelGpuDevice* device, u32 handle);
        ~IntelGemBackingObject() override = default;

        IntelGemBackingObject(const IntelGemBackingObject&) = delete;
        IntelGemBackingObject& operator=(const IntelGemBackingObject&) = delete;

        phys_addr_t get_page(usize offset_in_bytes) override;
        [[nodiscard]] usize get_size() const override;

        void add_mapping() override {}
        void remove_mapping() override {}

    private:
        IntelGpuDevice* device_;
        u32 handle_;
    };
} // namespace gpu::intel::core

#endif // VESPERAOS_INTEL_GEM_BACKING_H
