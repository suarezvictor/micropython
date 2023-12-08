// This file is Copyright (c) 2023 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: AGPL-3.0-only

#ifndef __ACCEL_CORES_H
#define __ACCEL_CORES_H

//each accelerator functions return a boolean to indicate if it was run or not, to resort to software renderer if not
//(i.e it can check if the target buffer is within the allowable region)

int accel_rectangle_fill32(uintptr_t fb_base, int x0, int y0, int x1, int y1, uint32_t rgba, unsigned frame_pitch);
int accel_ellipse_fill32(uintptr_t fb_base, int x0, int y0, int x1, int y1, uint32_t rgba, unsigned frame_pitch);
int accel_line32(uintptr_t fb_base, int x0, int y0, int x1, int y1, uint32_t rgba, unsigned frame_pitch);

#endif //__ACCEL_CORES_H
