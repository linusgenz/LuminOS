// iris_kmd_backend.c
// VesperaOS - operating system for the x86_64 architecture
//
// Copyright (c) 2026 Linus Genz <linuslinuxgenz@gmail.com>
//
// Created by Linus Genz on 17.08.26.
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

#include "iris/iris_kmd_backend.h"

#include <sys/mman.h>

#include "common/intel_gem.h"
#include "iris/iris_bufmgr.h"
#include "iris/iris_batch.h"
#include "iris/iris_context.h"

#include "vespera/dev/lucifer_drm.h"

static uint32_t
lucifer_gem_create(
    struct iris_bufmgr* bufmgr,
    const struct intel_memory_class_instance** regions,
    uint16_t regions_count, uint64_t size,
    enum iris_heap heap_flags, enum bo_alloc_flags alloc_flags
) {
    const struct intel_device_info* devinfo = iris_bufmgr_get_device_info(bufmgr);

    struct lucifer_gem_create gem_create = {
        .size = align64(size, devinfo->mem_alignment),
    };

    /* Gen9.5 has no local/VRAM memory classes, so region instances collapse
     * onto a single system-memory placement bit */
    for (uint16_t i = 0; i < regions_count; i++)
        gem_create.placement |= BITFIELD_BIT(regions[i]->instance);

    if (alloc_flags & BO_ALLOC_SCANOUT)
        gem_create.flags |= LUCIFER_GEM_CREATE_FLAG_SCANOUT;

    const struct intel_device_info_pat_entry* pat_entry =
        iris_heap_to_pat_entry(devinfo, heap_flags, alloc_flags & BO_ALLOC_SCANOUT);
    switch (pat_entry->mmap) {
    case INTEL_DEVICE_INFO_MMAP_MODE_WC:
        gem_create.cpu_caching = LUCIFER_GEM_CPU_CACHING_WC;
        break;
    case INTEL_DEVICE_INFO_MMAP_MODE_WB:
    case INTEL_DEVICE_INFO_MMAP_MODE_INVALID:
    default:
        gem_create.cpu_caching = LUCIFER_GEM_CPU_CACHING_WB;
        break;
    }

    if (intel_ioctl(iris_bufmgr_get_fd(bufmgr), LUCIFER_IOCTL_GEM_CREATE, &gem_create))
        return 0;

    return gem_create.handle;
}

static uint32_t
lucifer_gem_create_userptr(struct iris_bufmgr* bufmgr, void* ptr, uint64_t size) {
    struct lucifer_gem_userptr userptr = {
        .ptr = (uintptr_t)ptr,
        .size = size,
    };

    if (intel_ioctl(iris_bufmgr_get_fd(bufmgr), LUCIFER_IOCTL_GEM_USERPTR, &userptr))
        return 0;

    return userptr.handle;
}

static int
lucifer_gem_close(struct iris_bufmgr* bufmgr, struct iris_bo* bo) {
    if (bo->real.userptr)
        return 0;

    struct lucifer_gem_close close = {
        .handle = bo->gem_handle,
    };
    return intel_ioctl(iris_bufmgr_get_fd(bufmgr), LUCIFER_IOCTL_GEM_CLOSE, &close);
}

static bool
lucifer_bo_madvise(struct iris_bo* bo, enum iris_madvice state) {
    printf("lucifer_bo_madvise called\n");
    UNREACHABLE("lucifer_bo_madvise not implemented");
    return false;
}

static int
lucifer_bo_set_caching(struct iris_bo* bo, bool cached) {
    UNREACHABLE("lucifer_bo_set_caching not implemented");
    return 0;
}

static void*
lucifer_gem_mmap(struct iris_bufmgr* bufmgr, struct iris_bo* bo) {
    UNREACHABLE("lucifer_gem_mmap not implemented");
    return NULL;
}

static enum pipe_reset_status
lucifer_batch_check_for_reset(struct iris_batch* batch) {
    UNREACHABLE("lucifer_batch_check_for_reset not implemented");
    return (enum pipe_reset_status)0;
}

static int
lucifer_batch_submit(struct iris_batch* batch) {
    UNREACHABLE("lucifer_batch_submit not implemented");
    return 0;
}

static bool
lucifer_gem_vm_bind(struct iris_bo* bo, enum bo_alloc_flags flags) {
    UNREACHABLE("lucifer_gem_vm_bind not implemented");
    return false;
}

static bool
lucifer_gem_vm_unbind(struct iris_bo* bo) {
    UNREACHABLE("lucifer_gem_vm_unbind not implemented");
    return false;
}

const struct iris_kmd_backend* lucifer_get_backend(void) {
    static const struct iris_kmd_backend lucifer_kmd_backend = {
        .gem_create            = lucifer_gem_create,
        .gem_create_userptr    = lucifer_gem_create_userptr,
        .gem_close             = lucifer_gem_close,
        .bo_madvise            = lucifer_bo_madvise,
        .bo_set_caching        = lucifer_bo_set_caching,
        .gem_mmap              = lucifer_gem_mmap,
        .batch_check_for_reset = lucifer_batch_check_for_reset,
        .batch_submit          = lucifer_batch_submit,
        .gem_vm_bind           = lucifer_gem_vm_bind,
        .gem_vm_unbind         = lucifer_gem_vm_unbind,
    };
    return &lucifer_kmd_backend;
}
