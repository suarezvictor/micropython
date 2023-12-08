// This file is Copyright (c) 2023 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: AGPL-3.0-only

#ifndef __ACCEL_CORES_H
#define __ACCEL_CORES_H

#warning Modify accelerators API to cut dependence with registers struct
#include <liblitesdk/csr_helpers.h> //FIXME: duplicated source?
#define ACCEL_STATIC_ASSERT(cond, msg) STATIC_ASSERT(cond, msg);
#include <../../../accel_cores.h>

//accelerator functions return a boolean to indicate if it was run or not, to resort to software renderer if not
//(i.e it can check if the target buffer is within allowable region)

struct accel_rectangle_fill32_layout_t;
struct accel_ellipse_fill32_layout_t;
struct accel_line32_layout_t;

int accel_rectangle_fill32(accel_rectangle_fill32_layout_t *regs, uintptr_t fb_base, int x0, int y0, int x1, int y1, uint32_t rgba, unsigned frame_pitch);
int accel_ellipse_fill32(accel_ellipse_fill32_layout_t *regs, uintptr_t fb_base, int x0, int y0, int x1, int y1, uint32_t rgba, unsigned frame_pitch);
int accel_line32(accel_line32_layout_t *regs, uintptr_t fb_base, int x0, int y0, int x1, int y1, uint32_t rgba, unsigned frame_pitch);

#endif //__ACCEL_CORES_H
