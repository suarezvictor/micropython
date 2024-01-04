#ifndef DISABLE_HARDWARE_ACCEL
#include <liblitesdk/csr_helpers.h>
#define ACCEL_STATIC_ASSERT(cond, msg) STATIC_ASSERT(cond, msg);
#include <../../../accel_cores.h>
#else
#define VIDEO_FRAMEBUFFER_DEPTH 32 //assumes 32-bit color
#endif

#include "accel_cores.h"

int accel_rectangle_fill32(uintptr_t fb_base, int x0, int y0, int x1, int y1, uint32_t rgba, unsigned frame_pitch)
{
#ifndef DISABLE_HARDWARE_ACCEL
  accel_rectangle_fill32_layout_t *regs = accel_rectangle_fill32_regs;
#ifndef ACCEL_RECTANGLE_FILL32_CSR_PAGE_OFFSET
#error MICROPY_ENABLE_FRAMEBUFFER_ACCEL is defined but ACCEL_RECTANGLE_FILL32_CSR_PAGE_OFFSET is not defined
#endif

#if defined(VRAM_ORIGIN_ACCEL_RECTANGLE_FILL32) && defined(VIDEO_FRAMEBUFFER_BASE) && VRAM_ORIGIN_ACCEL_RECTANGLE_FILL32 != VIDEO_FRAMEBUFFER_BASE
#warning VRAM_ORIGIN_ACCEL_RECTANGLE_FILL32 should be tested
  fb_base += VRAM_ORIGIN_ACCEL_RECTANGLE_FILL32 - VIDEO_FRAMEBUFFER_BASE;
#endif

  regs->run = 0; //stps
  while(regs->done); //wait data latch
#else
  accel_rectangle_fill32_layout_t local_regs;
  accel_rectangle_fill32_layout_t *regs = &local_regs;
#endif

  fb_base += (y0<y1?y0:y1)*frame_pitch + (x0<x1?x0:x1)*sizeof(rgba);
  regs->x0 = x0 < x1 ? x0 : x1;
  regs->x1 = 1 + (x1 > x0 ? x1 : x0);
  regs->y0 = y0 < y1 ? y0 : y1;
  regs->y1 = 1 + (y1 > y0 ? y1 : y0);
  regs->base = fb_base;
  regs->xstride = VIDEO_FRAMEBUFFER_DEPTH/8;
  regs->ystride = frame_pitch;
  regs->rgba = rgba;

#ifndef DISABLE_HARDWARE_ACCEL
  regs->run = 1; //start
  while(!regs->done); //wait until done
#else
  rectangle_fill32(BUSMASTER_ARG, regs->x0, regs->x1, regs->y0, regs->y1, rgba, fb_base, regs->xstride, regs->ystride);
#endif
  
  return 1; //executed
}

int accel_ellipse_fill32(uintptr_t fb_base, int x0, int y0, int x1, int y1, uint32_t rgba, unsigned frame_pitch)
{
#ifndef DISABLE_HARDWARE_ACCEL
  accel_ellipse_fill32_layout_t *regs = accel_ellipse_fill32_regs;
#ifndef ACCEL_ELLIPSE_FILL32_CSR_PAGE_OFFSET
#error MICROPY_ENABLE_FRAMEBUFFER_ACCEL is defined but ACCEL_ELLIPSE_FILL32_CSR_PAGE_OFFSET is not defined
#endif

#if defined(VRAM_ORIGIN_ACCEL_ELLIPSE_FILL32) && defined(VIDEO_FRAMEBUFFER_BASE) && VRAM_ORIGIN_ACCEL_ELLIPSE_FILL32 != VIDEO_FRAMEBUFFER_BASE
#warning VRAM_ORIGIN_ACCEL_ELLIPSE_FILL32 should be tested
  fb_base += VRAM_ORIGIN_ACCEL_ELLIPSE_FILL32 - VIDEO_FRAMEBUFFER_BASE;
#endif

  regs->run = 0; //stops
  while(regs->done); //wait data latch
#else
  accel_ellipse_fill32_layout_t local_regs;
  accel_ellipse_fill32_layout_t *regs = &local_regs;
#endif

  fb_base += (y0<y1?y0:y1)*frame_pitch + (x0<x1?x0:x1)*sizeof(rgba);

  regs->x0 = x0 < x1 ? x0 : x1;
  regs->x1 = 1 + (x1 > x0 ? x1 : x0);
  regs->y0 = y0 < y1 ? y0 : y1;
  regs->y1 = 1 + (y1 > y0 ? y1 : y0);
  regs->base = fb_base;
  regs->xstride = VIDEO_FRAMEBUFFER_DEPTH/8;
  regs->ystride = frame_pitch;
  regs->rgba = rgba;
  
#ifndef DISABLE_HARDWARE_ACCEL
  regs->run = 1; //start
  while(!regs->done); //wait until done
#else
  ellipse_fill32(BUSMASTER_ARG, regs->x0, regs->x1, regs->y0, regs->y1, rgba, fb_base, regs->xstride, regs->ystride);
#endif
  
  return 1; //executed
}

int accel_line32(uintptr_t fb_base, int x0, int y0, int x1, int y1, uint32_t rgba, unsigned frame_pitch)
{
#ifndef DISABLE_HARDWARE_ACCEL
  accel_line32_layout_t *regs = accel_line32_regs;
#ifndef ACCEL_LINE32_CSR_PAGE_OFFSET
#error MICROPY_ENABLE_FRAMEBUFFER_ACCEL is defined but ACCEL_LINE32_CSR_PAGE_OFFSET is not defined
#endif

#if defined(VRAM_ORIGIN_ACCEL_LINE32) && defined(VIDEO_FRAMEBUFFER_BASE) && VRAM_ORIGIN_ACCEL_LINE32 != VIDEO_FRAMEBUFFER_BASE
#warning VRAM_ORIGIN_ACCEL_LINE32 should be tested
  fb_base += VRAM_ORIGIN_ACCEL_LINE32 - VIDEO_FRAMEBUFFER_BASE;
#endif

  regs->run = 0; //stops
  while(regs->done); //wait data latch
#else
  accel_line32_layout_t local_regs;
  accel_line32_layout_t *regs = &local_regs;
#endif

  fb_base += y0*frame_pitch + x0*sizeof(rgba);
  regs->x0 = x0;
  regs->x1 = x1;
  regs->y0 = y0;
  regs->y1 = y1;

  regs->base = fb_base;
  regs->xstride = x0 < x1 ? VIDEO_FRAMEBUFFER_DEPTH/8 : -VIDEO_FRAMEBUFFER_DEPTH/8;
  regs->ystride = y0 < y1 ? frame_pitch : -frame_pitch;

  regs->rgba = rgba;
  
#ifndef DISABLE_HARDWARE_ACCEL
  regs->run = 1; //start
  while(!regs->done); //wait until done
#else
  line32(BUSMASTER_ARG, regs->x0, regs->x1, regs->y0, regs->y1, rgba, fb_base, regs->xstride, regs->ystride);
#endif  
  return 1; //executed
}

