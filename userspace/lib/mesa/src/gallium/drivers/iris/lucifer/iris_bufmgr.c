// iris_bufmgr.c
// VesperaOS - operating system for the x86_64 architecture
//
// Copyright (c) 2026 Linus Genz <linuslinuxgenz@gmail.com>
//
// Created by Linus Genz on 07.09.26.
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

#include "vespera/dev/lucifer_drm.h"
#include "common/intel_gem.h"
#include "iris/iris_bufmgr.h"

bool
iris_lucifer_init_global_vm(struct iris_bufmgr *bufmgr, uint32_t *vm_id)
{
    struct lucifer_vm_create create = {
        .flags = LUCIFER_VM_CREATE_FLAG_NONE,
     };
    if (intel_ioctl(iris_bufmgr_get_fd(bufmgr), LUCIFER_IOCTL_VM_CREATE, &create))
        return false;
    *vm_id = create.vm_id;
    return true;
}

bool
iris_lucifer_destroy_global_vm(struct iris_bufmgr *bufmgr)
{
    struct lucifer_vm_destroy destroy = {
        .vm_id = iris_bufmgr_get_global_vm_id(bufmgr),
     };
    return intel_ioctl(iris_bufmgr_get_fd(bufmgr), LUCIFER_IOCTL_VM_DESTROY,
                       &destroy) == 0;
}
