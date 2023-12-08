/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
 * Copyright (c) 2022 Victor Suarez Rovere <suarezvictor@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#define MICROPY_ENABLE_FRAMEBUFFER_ACCEL //TODO: move to config file (mphalport.h or mpconfigport.h)

#include <stdio.h>
#include <string.h>
#include <csr_helpers.h>

#include "py/runtime.h"

#if MICROPY_PY_FRAMEBUF

#ifdef MICROPY_ENABLE_FRAMEBUFFER_ACCEL

//FIXME: move to a new source
#define ACCEL_STATIC_ASSERT(cond, msg) STATIC_ASSERT(cond, msg);
#include <../../../accel_cores.h>

//TODO: accelerator functions can return a boolean to indicate if it was run or not, and resort to software version if not
//(i.e checking the target buffer is within allowable region)
static void _rectangle_fill32(accel_rectangle_fill32_layout_t *regs, uintptr_t fb_base, int x0, int y0, int x1, int y1, uint32_t rgba, unsigned frame_pitch)
{
#ifndef ACCEL_RECTANGLE_FILL32_CSR_PAGE_OFFSET
#error MICROPY_ENABLE_FRAMEBUFFER_ACCEL is defined but ACCEL_RECTANGLE_FILL32_CSR_PAGE_OFFSET is not defined
#endif

#if defined(VRAM_ORIGIN_ACCEL_RECTANGLE_FILL32) && defined(VIDEO_FRAMEBUFFER_BASE) && VRAM_ORIGIN_ACCEL_RECTANGLE_FILL32 != VIDEO_FRAMEBUFFER_BASE
#warning VRAM_ORIGIN_ACCEL_RECTANGLE_FILL32 should be tested
  fb_base += VRAM_ORIGIN_ACCEL_RECTANGLE_FILL32 - VIDEO_FRAMEBUFFER_BASE;
#endif

  regs->run = 0; //stps
  while(regs->done); //wait data latch

  fb_base += (y0<y1?y0:y1)*frame_pitch + (x0<x1?x0:x1)*sizeof(rgba);
  regs->x0 = x0 < x1 ? x0 : x1;
  regs->x1 = 1 + (x1 > x0 ? x1 : x0);
  regs->y0 = y0 < y1 ? y0 : y1;
  regs->y1 = 1 + (y1 > y0 ? y1 : y0);
  regs->base = fb_base;
  regs->xstride = VIDEO_FRAMEBUFFER_DEPTH/8;
  regs->ystride = frame_pitch;
  regs->rgba = rgba;

  regs->run = 1; //start
  while(!regs->done); //wait until done
}

static void _ellipse_fill32(accel_ellipse_fill32_layout_t *regs, uintptr_t fb_base, int x0, int y0, int x1, int y1, uint32_t rgba, unsigned frame_pitch)
{
#ifndef ACCEL_ELLIPSE_FILL32_CSR_PAGE_OFFSET
#error MICROPY_ENABLE_FRAMEBUFFER_ACCEL is defined but ACCEL_ELLIPSE_FILL32_CSR_PAGE_OFFSET is not defined
#endif

#if defined(VRAM_ORIGIN_ACCEL_ELLIPSE_FILL32) && defined(VIDEO_FRAMEBUFFER_BASE) && VRAM_ORIGIN_ACCEL_ELLIPSE_FILL32 != VIDEO_FRAMEBUFFER_BASE
#warning VRAM_ORIGIN_ACCEL_ELLIPSE_FILL32 should be tested
  fb_base += VRAM_ORIGIN_ACCEL_ELLIPSE_FILL32 - VIDEO_FRAMEBUFFER_BASE;
#endif

  regs->run = 0; //stops
  while(regs->done); //wait data latch

  fb_base += (y0<y1?y0:y1)*frame_pitch + (x0<x1?x0:x1)*sizeof(rgba);

  regs->x0 = x0 < x1 ? x0 : x1;
  regs->x1 = 1 + (x1 > x0 ? x1 : x0);
  regs->y0 = y0 < y1 ? y0 : y1;
  regs->y1 = 1 + (y1 > y0 ? y1 : y0);
  regs->base = fb_base;
  regs->xstride = VIDEO_FRAMEBUFFER_DEPTH/8;
  regs->ystride = frame_pitch;
  regs->rgba = rgba;
  
  regs->run = 1; //start
  while(!regs->done); //wait until done
}

static void _line32(accel_line32_layout_t *regs, uintptr_t fb_base, int x0, int y0, int x1, int y1, uint32_t rgba, unsigned frame_pitch)
{
#ifndef ACCEL_LINE32_CSR_PAGE_OFFSET
#error MICROPY_ENABLE_FRAMEBUFFER_ACCEL is defined but ACCEL_LINE32_CSR_PAGE_OFFSET is not defined
#endif

#if defined(VRAM_ORIGIN_ACCEL_LINE32) && defined(VIDEO_FRAMEBUFFER_BASE) && VRAM_ORIGIN_ACCEL_LINE32 != VIDEO_FRAMEBUFFER_BASE
#warning VRAM_ORIGIN_ACCEL_LINE32 should be tested
  fb_base += VRAM_ORIGIN_ACCEL_LINE32 - VIDEO_FRAMEBUFFER_BASE;
#endif

  regs->run = 0; //stops
  while(regs->done); //wait data latch

  fb_base += y0*frame_pitch + x0*sizeof(rgba);
  regs->x0 = x0;
  regs->x1 = x1;
  regs->y0 = y0;
  regs->y1 = y1;

  regs->base = fb_base;
  regs->xstride = x0 < x1 ? VIDEO_FRAMEBUFFER_DEPTH/8 : -VIDEO_FRAMEBUFFER_DEPTH/8;
  regs->ystride = y0 < y1 ? frame_pitch : -frame_pitch;

  regs->rgba = rgba;
  
  regs->run = 1; //start
  while(!regs->done); //wait until done
}

#endif //MICROPY_ENABLE_FRAMEBUFFER_ACCEL


#ifndef VIDEO_FRAMEBUFFER_BASE
#warning MICROPY_ENABLE_FRAMEBUFFER_ACCEL defined but VIDEO_FRAMEBUFFER_BASE not defined
#undef MICROPY_ENABLE_FRAMEBUFFER_ACCEL
#endif



#if defined(_DEBUG) && defined(__linux__)
#define FRAMEBUFFER_DEBUG
#endif

#ifdef FRAMEBUFFER_DEBUG
#ifndef __linux__
#error FRAMEBUFFER_DEBUG only supported on Linux
#endif

#include <SDL2/SDL.h>
typedef struct
{
    SDL_Window* win;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
} debug_handle_t;

bool fb_init(unsigned width, unsigned height, int format, debug_handle_t *handle);
void fb_update(debug_handle_t *handle, const void *buf, size_t stride_bytes);
void fb_deinit(debug_handle_t *handle);
bool fb_should_quit(void);  
#define fb_debug_update(s)  fb_update(&(s)->debug_handle, (s)->buf, (s)->stride*format_bpp((s)->format));
#endif

#include "ports/stm32/font_petme128_8x8.h"

typedef struct _mp_obj_framebuf_t {
    mp_obj_base_t base;
    mp_obj_t buf_obj; // need to store this to prevent GC from reclaiming buf
    void *buf;
    uint16_t width, height, stride;
    uint8_t format;
#ifdef FRAMEBUFFER_DEBUG
    debug_handle_t debug_handle;
#endif
} mp_obj_framebuf_t;

#if !MICROPY_ENABLE_DYNRUNTIME
STATIC const mp_obj_type_t mp_type_framebuf;
#endif

typedef void (*setpixel_t)(const mp_obj_framebuf_t *, unsigned int, unsigned int, uint32_t);
typedef uint32_t (*getpixel_t)(const mp_obj_framebuf_t *, unsigned int, unsigned int);
typedef void (*fill_rect_t)(const mp_obj_framebuf_t *, unsigned int, unsigned int, unsigned int, unsigned int, uint32_t);

typedef struct _mp_framebuf_p_t {
    setpixel_t setpixel;
    getpixel_t getpixel;
    fill_rect_t fill_rect;
} mp_framebuf_p_t;

// constants for formats
#define FRAMEBUF_MVLSB    (0)
#define FRAMEBUF_RGB565   (1)
#define FRAMEBUF_GS2_HMSB (5)
#define FRAMEBUF_GS4_HMSB (2)
#define FRAMEBUF_GS8      (6)
#define FRAMEBUF_MHLSB    (3)
#define FRAMEBUF_MHMSB    (4)
#define FRAMEBUF_RGB32    (7) //this is an addition to the standard, that doesn't support RGB32

// Functions for MHLSB and MHMSB

STATIC void mono_horiz_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    size_t index = (x + y * fb->stride) >> 3;
    unsigned int offset = fb->format == FRAMEBUF_MHMSB ? x & 0x07 : 7 - (x & 0x07);
    ((uint8_t *)fb->buf)[index] = (((uint8_t *)fb->buf)[index] & ~(0x01 << offset)) | ((col != 0) << offset);
}

STATIC uint32_t mono_horiz_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    size_t index = (x + y * fb->stride) >> 3;
    unsigned int offset = fb->format == FRAMEBUF_MHMSB ? x & 0x07 : 7 - (x & 0x07);
    return (((uint8_t *)fb->buf)[index] >> (offset)) & 0x01;
}

STATIC void mono_horiz_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    unsigned int reverse = fb->format == FRAMEBUF_MHMSB;
    unsigned int advance = fb->stride >> 3;
    while (w--) {
        uint8_t *b = &((uint8_t *)fb->buf)[(x >> 3) + y * advance];
        unsigned int offset = reverse ?  x & 7 : 7 - (x & 7);
        for (unsigned int hh = h; hh; --hh) {
            *b = (*b & ~(0x01 << offset)) | ((col != 0) << offset);
            b += advance;
        }
        ++x;
    }
}

// Functions for MVLSB format

STATIC void mvlsb_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    size_t index = (y >> 3) * fb->stride + x;
    uint8_t offset = y & 0x07;
    ((uint8_t *)fb->buf)[index] = (((uint8_t *)fb->buf)[index] & ~(0x01 << offset)) | ((col != 0) << offset);
}

STATIC uint32_t mvlsb_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    return (((uint8_t *)fb->buf)[(y >> 3) * fb->stride + x] >> (y & 0x07)) & 0x01;
}

STATIC void mvlsb_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    while (h--) {
        uint8_t *b = &((uint8_t *)fb->buf)[(y >> 3) * fb->stride + x];
        uint8_t offset = y & 0x07;
        for (unsigned int ww = w; ww; --ww) {
            *b = (*b & ~(0x01 << offset)) | ((col != 0) << offset);
            ++b;
        }
        ++y;
    }
}

// Functions for RGB565 format

STATIC void rgb565_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    ((uint16_t *)fb->buf)[x + y * fb->stride] = col;
}

STATIC uint32_t rgb565_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    return ((uint16_t *)fb->buf)[x + y * fb->stride];
}

STATIC void rgb565_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    uint16_t *b = &((uint16_t *)fb->buf)[x + y * fb->stride];
    while (h--) {
        for (unsigned int ww = w; ww; --ww) {
            *b++ = col;
        }
        b += fb->stride - w;
    }
}

// Functions for RGB32 format

STATIC void rgb32_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
#ifdef MICROPY_ENABLE_FRAMEBUFFER_ACCEL
#warning accelerator should check that target memory is within accesible video ram, or resort to software rendering
    _rectangle_fill32(accel_rectangle_fill32_regs, (uintptr_t) fb->buf, x, y, x, y, col, fb->stride*sizeof(col));
#else
    ((uint32_t *)fb->buf)[x + y * fb->stride] = col;
#endif
}

STATIC uint32_t rgb32_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    return ((uint32_t *)fb->buf)[x + y * fb->stride];
}

STATIC void rgb32_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
#ifdef MICROPY_ENABLE_FRAMEBUFFER_ACCEL
#warning accelerator should check that target memory is within accesible video ram, or resort to software rendering
    _rectangle_fill32(accel_rectangle_fill32_regs, (uintptr_t) fb->buf, x, y, x+w-1, y+h-1, col, fb->stride*sizeof(col));
#else
    uint32_t *b = &((uint32_t *)fb->buf)[x + y * fb->stride];
    while (h--) {
        for (unsigned int ww = w; ww; --ww) {
            *b++ = col;
        }
        b += fb->stride - w;
    }
#endif
}
// Functions for GS2_HMSB format

STATIC void gs2_hmsb_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    uint8_t *pixel = &((uint8_t *)fb->buf)[(x + y * fb->stride) >> 2];
    uint8_t shift = (x & 0x3) << 1;
    uint8_t mask = 0x3 << shift;
    uint8_t color = (col & 0x3) << shift;
    *pixel = color | (*pixel & (~mask));
}

STATIC uint32_t gs2_hmsb_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    uint8_t pixel = ((uint8_t *)fb->buf)[(x + y * fb->stride) >> 2];
    uint8_t shift = (x & 0x3) << 1;
    return (pixel >> shift) & 0x3;
}

STATIC void gs2_hmsb_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    for (unsigned int xx = x; xx < x + w; xx++) {
        for (unsigned int yy = y; yy < y + h; yy++) {
            gs2_hmsb_setpixel(fb, xx, yy, col);
        }
    }
}

// Functions for GS4_HMSB format

STATIC void gs4_hmsb_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    uint8_t *pixel = &((uint8_t *)fb->buf)[(x + y * fb->stride) >> 1];

    if (x % 2) {
        *pixel = ((uint8_t)col & 0x0f) | (*pixel & 0xf0);
    } else {
        *pixel = ((uint8_t)col << 4) | (*pixel & 0x0f);
    }
}

STATIC uint32_t gs4_hmsb_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    if (x % 2) {
        return ((uint8_t *)fb->buf)[(x + y * fb->stride) >> 1] & 0x0f;
    }

    return ((uint8_t *)fb->buf)[(x + y * fb->stride) >> 1] >> 4;
}

STATIC void gs4_hmsb_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    col &= 0x0f;
    uint8_t *pixel_pair = &((uint8_t *)fb->buf)[(x + y * fb->stride) >> 1];
    uint8_t col_shifted_left = col << 4;
    uint8_t col_pixel_pair = col_shifted_left | col;
    unsigned int pixel_count_till_next_line = (fb->stride - w) >> 1;
    bool odd_x = (x % 2 == 1);

    while (h--) {
        unsigned int ww = w;

        if (odd_x && ww > 0) {
            *pixel_pair = (*pixel_pair & 0xf0) | col;
            pixel_pair++;
            ww--;
        }

        memset(pixel_pair, col_pixel_pair, ww >> 1);
        pixel_pair += ww >> 1;

        if (ww % 2) {
            *pixel_pair = col_shifted_left | (*pixel_pair & 0x0f);
            if (!odd_x) {
                pixel_pair++;
            }
        }

        pixel_pair += pixel_count_till_next_line;
    }
}

// Functions for GS8 format

STATIC void gs8_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    uint8_t *pixel = &((uint8_t *)fb->buf)[(x + y * fb->stride)];
    *pixel = col & 0xff;
}

STATIC uint32_t gs8_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    return ((uint8_t *)fb->buf)[(x + y * fb->stride)];
}

STATIC void gs8_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    uint8_t *pixel = &((uint8_t *)fb->buf)[(x + y * fb->stride)];
    while (h--) {
        memset(pixel, col, w);
        pixel += fb->stride;
    }
}

STATIC mp_framebuf_p_t formats[] = {
    [FRAMEBUF_MVLSB] = {mvlsb_setpixel, mvlsb_getpixel, mvlsb_fill_rect},
    [FRAMEBUF_RGB565] = {rgb565_setpixel, rgb565_getpixel, rgb565_fill_rect},
    [FRAMEBUF_GS2_HMSB] = {gs2_hmsb_setpixel, gs2_hmsb_getpixel, gs2_hmsb_fill_rect},
    [FRAMEBUF_GS4_HMSB] = {gs4_hmsb_setpixel, gs4_hmsb_getpixel, gs4_hmsb_fill_rect},
    [FRAMEBUF_GS8] = {gs8_setpixel, gs8_getpixel, gs8_fill_rect},
    [FRAMEBUF_MHLSB] = {mono_horiz_setpixel, mono_horiz_getpixel, mono_horiz_fill_rect},
    [FRAMEBUF_MHMSB] = {mono_horiz_setpixel, mono_horiz_getpixel, mono_horiz_fill_rect},
    [FRAMEBUF_RGB32] = {rgb32_setpixel, rgb32_getpixel, rgb32_fill_rect},
};

static inline void setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    formats[fb->format].setpixel(fb, x, y, col);
}

static inline uint32_t getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    return formats[fb->format].getpixel(fb, x, y);
}

STATIC void fill_rect(const mp_obj_framebuf_t *fb, int x, int y, int w, int h, uint32_t col) {
    if (h < 1 || w < 1 || x + w <= 0 || y + h <= 0 || y >= fb->height || x >= fb->width) {
        // No operation needed.
        return;
    }

    // clip to the framebuffer
    int xend = MIN(fb->width, x + w);
    int yend = MIN(fb->height, y + h);
    x = MAX(x, 0);
    y = MAX(y, 0);

    formats[fb->format].fill_rect(fb, x, y, xend - x, yend - y, col);
}

STATIC unsigned format_bpp(int format)
{
    switch(format)
    {
      case FRAMEBUF_RGB32: return 4;
      case FRAMEBUF_RGB565: return 2;
    }
    return 1;
}

STATIC mp_obj_t framebuf_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 4, 5, false);

    mp_obj_framebuf_t *o = m_new_obj(mp_obj_framebuf_t);
    o->base.type = type;
    o->buf_obj = args[0];

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_WRITE);
    o->buf = bufinfo.buf;

    o->width = mp_obj_get_int(args[1]);
    o->height = mp_obj_get_int(args[2]);
    o->format = mp_obj_get_int(args[3]);
    if (n_args >= 5) {
        o->stride = mp_obj_get_int(args[4]);
    } else {
        o->stride = o->width;
    }

    switch (o->format) {
        case FRAMEBUF_MVLSB:
        case FRAMEBUF_RGB565:
        case FRAMEBUF_RGB32:
            break;
        case FRAMEBUF_MHLSB:
        case FRAMEBUF_MHMSB:
            o->stride = (o->stride + 7) & ~7;
            break;
        case FRAMEBUF_GS2_HMSB:
            o->stride = (o->stride + 3) & ~3;
            break;
        case FRAMEBUF_GS4_HMSB:
            o->stride = (o->stride + 1) & ~1;
            break;
        case FRAMEBUF_GS8:
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid format"));
    }

#ifdef FRAMEBUFFER_DEBUG
    memset(&o->debug_handle, 0, sizeof(o->debug_handle));
    if(!fb_init(o->width, o->height, o->format, &o->debug_handle))
       mp_raise_ValueError(MP_ERROR_TEXT("Cannot create framebuffer debug window"));
    printf("Created debug framebuffer %dx%d, %dbpp\n", o->width, o->height, 8*format_bpp(o->format));
#endif

    return MP_OBJ_FROM_PTR(o);
}

STATIC void framebuf_args(const mp_obj_t *args_in, mp_int_t *args_out, int n) {
    for (int i = 0; i < n; ++i) {
        args_out[i] = mp_obj_get_int(args_in[i + 1]);
    }
}

STATIC mp_int_t framebuf_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    (void)flags;
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    bufinfo->buf = self->buf;
    bufinfo->len = self->stride * self->height * format_bpp(self->format);
    bufinfo->typecode = 'B'; // view framebuf as bytes
    return 0;
}

STATIC mp_obj_t framebuf_fill(mp_obj_t self_in, mp_obj_t col_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t col = mp_obj_get_int(col_in);
    formats[self->format].fill_rect(self, 0, 0, self->width, self->height, col);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(framebuf_fill_obj, framebuf_fill);

STATIC mp_obj_t framebuf_fill_rect(size_t n_args, const mp_obj_t *args) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t width = mp_obj_get_int(args[3]);
    mp_int_t height = mp_obj_get_int(args[4]);
    mp_int_t col = mp_obj_get_int(args[5]);

    fill_rect(self, x, y, width, height, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_fill_rect_obj, 6, 6, framebuf_fill_rect);

STATIC mp_obj_t framebuf_pixel(size_t n_args, const mp_obj_t *args) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    if (0 <= x && x < self->width && 0 <= y && y < self->height) {
        if (n_args == 3) {
            // get
            return MP_OBJ_NEW_SMALL_INT(getpixel(self, x, y));
        } else {
            // set
            setpixel(self, x, y, mp_obj_get_int(args[3]));
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_pixel_obj, 3, 4, framebuf_pixel);

STATIC mp_obj_t framebuf_hline(size_t n_args, const mp_obj_t *args) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t w = mp_obj_get_int(args[3]);
    mp_int_t col = mp_obj_get_int(args[4]);

    fill_rect(self, x, y, w, 1, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_hline_obj, 5, 5, framebuf_hline);

STATIC mp_obj_t framebuf_vline(size_t n_args, const mp_obj_t *args) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t h = mp_obj_get_int(args[3]);
    mp_int_t col = mp_obj_get_int(args[4]);

    fill_rect(self, x, y, 1, h, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_vline_obj, 5, 5, framebuf_vline);

STATIC mp_obj_t framebuf_rect(size_t n_args, const mp_obj_t *args) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t w = mp_obj_get_int(args[3]);
    mp_int_t h = mp_obj_get_int(args[4]);
    mp_int_t col = mp_obj_get_int(args[5]);

    fill_rect(self, x, y, w, 1, col);
    fill_rect(self, x, y + h - 1, w, 1, col);
    fill_rect(self, x, y, 1, h, col);
    fill_rect(self, x + w - 1, y, 1, h, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_rect_obj, 6, 6, framebuf_rect);

STATIC mp_obj_t framebuf_line(size_t n_args, const mp_obj_t *args) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x1 = mp_obj_get_int(args[1]);
    mp_int_t y1 = mp_obj_get_int(args[2]);
    mp_int_t x2 = mp_obj_get_int(args[3]);
    mp_int_t y2 = mp_obj_get_int(args[4]);

#ifdef MICROPY_ENABLE_FRAMEBUFFER_ACCEL
    if(self->format != FRAMEBUF_RGB32)
      mp_raise_ValueError(MP_ERROR_TEXT("Accelerated line requires 32-bit color"));

#warning accelerator should check that target memory is within accesible video ram, or resort to software rendering

    uint32_t col = mp_obj_get_int(args[5]);
    _line32(accel_line32_regs, (uintptr_t) self->buf, x1, y1, x2, y2, col, self->stride*sizeof(col));

#else
    mp_int_t col = mp_obj_get_int(args[5]);
    mp_int_t dx = x2 - x1;
    mp_int_t sx;
    if (dx > 0) {
        sx = 1;
    } else {
        dx = -dx;
        sx = -1;
    }

    mp_int_t dy = y2 - y1;
    mp_int_t sy;
    if (dy > 0) {
        sy = 1;
    } else {
        dy = -dy;
        sy = -1;
    }

    bool steep;
    if (dy > dx) {
        mp_int_t temp;
        temp = x1;
        x1 = y1;
        y1 = temp;
        temp = dx;
        dx = dy;
        dy = temp;
        temp = sx;
        sx = sy;
        sy = temp;
        steep = true;
    } else {
        steep = false;
    }

    mp_int_t e = 2 * dy - dx;
    for (mp_int_t i = 0; i < dx; ++i) {
        if (steep) {
            if (0 <= y1 && y1 < self->width && 0 <= x1 && x1 < self->height) {
                setpixel(self, y1, x1, col);
            }
        } else {
            if (0 <= x1 && x1 < self->width && 0 <= y1 && y1 < self->height) {
                setpixel(self, x1, y1, col);
            }
        }
        while (e >= 0) {
            y1 += sy;
            e -= 2 * dx;
        }
        x1 += sx;
        e += 2 * dy;
    }

    if (0 <= x2 && x2 < self->width && 0 <= y2 && y2 < self->height) {
        setpixel(self, x2, y2, col);
    }
#endif

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_line_obj, 6, 6, framebuf_line);

#ifdef MICROPY_ENABLE_FRAMEBUFFER_ACCEL

// Q2 Q1
// Q3 Q4
#define ELLIPSE_MASK_FILL (0x10)
#define ELLIPSE_MASK_ALL (0x0f)
#define ELLIPSE_MASK_Q1 (0x01)
#define ELLIPSE_MASK_Q2 (0x02)
#define ELLIPSE_MASK_Q3 (0x04)
#define ELLIPSE_MASK_Q4 (0x08)

STATIC mp_obj_t framebuf_ellipse(size_t n_args, const mp_obj_t *args_in) {
    mp_obj_framebuf_t *fb = MP_OBJ_TO_PTR(args_in[0]);

    if(fb->format != FRAMEBUF_RGB32)
      mp_raise_ValueError(MP_ERROR_TEXT("Accelerated ellipse requires 32-bit color"));
    
    mp_int_t args[5];
    framebuf_args(args_in, args, 5); // cx, cy, xradius, yradius, col
    mp_int_t mask = (n_args > 6 && mp_obj_is_true(args_in[6])) ? ELLIPSE_MASK_FILL : 0;
    if (n_args > 7) {
        mask |= mp_obj_get_int(args_in[7]) & ELLIPSE_MASK_ALL;
    } else {
        mask |= ELLIPSE_MASK_ALL;
    }

    if(mask != (ELLIPSE_MASK_FILL | ELLIPSE_MASK_ALL))
      mp_raise_ValueError(MP_ERROR_TEXT("Accelerated ellipse should be filled and on all its quadrants"));

    mp_int_t cx = args[0], cy = args[1], xr = args[2], yr = args[3];
    uint32_t col = args[4];
    //printf("framebuf_ellipse: buf 0x%p, cx = %d, cy = %d, xr = %d, yr = %d, col = 0x%08lX\n", fb->buf, cx, cy, xr, yr, col);
#warning accelerator should check that target memory is within accesible video ram, or resort to software rendering
    _ellipse_fill32(accel_ellipse_fill32_regs, (uintptr_t) fb->buf, cx - xr, cy - yr, cx + xr, cy + yr, col, fb->stride*sizeof(col));

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_ellipse_obj, 6, 8, framebuf_ellipse);
#endif

STATIC mp_obj_t framebuf_blit(size_t n_args, const mp_obj_t *args) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_t source_in = mp_obj_cast_to_native_base(args[1], MP_OBJ_FROM_PTR(&mp_type_framebuf));
    if (source_in == MP_OBJ_NULL) {
        mp_raise_TypeError(NULL);
    }
    mp_obj_framebuf_t *source = MP_OBJ_TO_PTR(source_in);

    mp_int_t x = mp_obj_get_int(args[2]);
    mp_int_t y = mp_obj_get_int(args[3]);
    mp_int_t key = -1;
    if (n_args > 4) {
        key = mp_obj_get_int(args[4]);
    }

    if (
        (x >= self->width) ||
        (y >= self->height) ||
        (-x >= source->width) ||
        (-y >= source->height)
        ) {
        // Out of bounds, no-op.
        return mp_const_none;
    }

    // Clip.
    int x0 = MAX(0, x);
    int y0 = MAX(0, y);
    int x1 = MAX(0, -x);
    int y1 = MAX(0, -y);
    int x0end = MIN(self->width, x + source->width);
    int y0end = MIN(self->height, y + source->height);

    for (; y0 < y0end; ++y0) {
        int cx1 = x1;
        for (int cx0 = x0; cx0 < x0end; ++cx0) {
            uint32_t col = getpixel(source, cx1, y1);
            if (col != (uint32_t)key) {
                setpixel(self, cx0, y0, col);
            }
            ++cx1;
        }
        ++y1;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_blit_obj, 4, 5, framebuf_blit);

STATIC mp_obj_t framebuf_scroll(mp_obj_t self_in, mp_obj_t xstep_in, mp_obj_t ystep_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t xstep = mp_obj_get_int(xstep_in);
    mp_int_t ystep = mp_obj_get_int(ystep_in);
    int sx, y, xend, yend, dx, dy;
    if (xstep < 0) {
        sx = 0;
        xend = self->width + xstep;
        dx = 1;
    } else {
        sx = self->width - 1;
        xend = xstep - 1;
        dx = -1;
    }
    if (ystep < 0) {
        y = 0;
        yend = self->height + ystep;
        dy = 1;
    } else {
        y = self->height - 1;
        yend = ystep - 1;
        dy = -1;
    }
    for (; y != yend; y += dy) {
        for (int x = sx; x != xend; x += dx) {
            setpixel(self, x, y, getpixel(self, x - xstep, y - ystep));
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(framebuf_scroll_obj, framebuf_scroll);

STATIC mp_obj_t framebuf_text(size_t n_args, const mp_obj_t *args) {
    // extract arguments
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    const char *str = mp_obj_str_get_str(args[1]);
    mp_int_t x0 = mp_obj_get_int(args[2]);
    mp_int_t y0 = mp_obj_get_int(args[3]);
    mp_int_t col = 1;
    if (n_args >= 5) {
        col = mp_obj_get_int(args[4]);
    }

    // loop over chars
    for (; *str; ++str) {
        // get char and make sure its in range of font
        int chr = *(uint8_t *)str;
        if (chr < 32 || chr > 127) {
            chr = 127;
        }
        // get char data
        const uint8_t *chr_data = &font_petme128_8x8[(chr - 32) * 8];
        // loop over char data
        for (int j = 0; j < 8; j++, x0++) {
            if (0 <= x0 && x0 < self->width) { // clip x
                uint vline_data = chr_data[j]; // each byte is a column of 8 pixels, LSB at top
                for (int y = y0; vline_data; vline_data >>= 1, y++) { // scan over vertical column
                    if (vline_data & 1) { // only draw if pixel set
                        if (0 <= y && y < self->height) { // clip y
                            setpixel(self, x0, y, col);
                        }
                    }
                }
            }
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_text_obj, 4, 5, framebuf_text);

#ifdef FRAMEBUFFER_DEBUG
STATIC mp_obj_t framebuf_debug_update(mp_obj_t self_in)
{
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);

    if(fb_should_quit())
      return MP_OBJ_NEW_SMALL_INT(0);

    fb_debug_update(self);
    return MP_OBJ_NEW_SMALL_INT(1);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(framebuf_debug_update_obj, framebuf_debug_update);

STATIC mp_obj_t framebuf_del(mp_obj_t self_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    fb_deinit(&self->debug_handle);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(framebuf_del_obj, framebuf_del);

bool fb_init(unsigned width, unsigned height, int format, debug_handle_t *handle)
{
    int texture_format;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
     return false;

    handle->win = SDL_CreateWindow("Micropython debug framebuffer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!handle->win)
      return false;

    handle->renderer = SDL_CreateRenderer(handle->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
    if (!handle->renderer)
      return false;

    switch(format)
    {
        case FRAMEBUF_RGB32: texture_format = SDL_PIXELFORMAT_BGRA32; break;
        case FRAMEBUF_RGB565: texture_format = SDL_PIXELFORMAT_RGB565; break;
        default:
            return false; //FIXME: raise error
    }

    handle->texture = SDL_CreateTexture(handle->renderer, texture_format, SDL_TEXTUREACCESS_TARGET, width, height);
    if (!handle->texture)
      return false;

    return true;
}


bool fb_should_quit(void)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
          case SDL_QUIT:
            return true;
          case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE)
               return true;
        }
    }
    return false;
}

void fb_update(debug_handle_t *handle, const void *buf, size_t stride_bytes)
{
    SDL_UpdateTexture(handle->texture, NULL, buf, stride_bytes);
    SDL_RenderCopy(handle->renderer, handle->texture, NULL, NULL);
    SDL_RenderPresent(handle->renderer);
}

void fb_deinit(debug_handle_t *handle)
{
    SDL_DestroyTexture(handle->texture);
    SDL_DestroyRenderer(handle->renderer);
    SDL_DestroyWindow(handle->win);
    handle->win = NULL;
}
#endif

#if !MICROPY_ENABLE_DYNRUNTIME
STATIC const mp_rom_map_elem_t framebuf_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&framebuf_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_rect), MP_ROM_PTR(&framebuf_fill_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&framebuf_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_hline), MP_ROM_PTR(&framebuf_hline_obj) },
    { MP_ROM_QSTR(MP_QSTR_vline), MP_ROM_PTR(&framebuf_vline_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&framebuf_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&framebuf_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_blit), MP_ROM_PTR(&framebuf_blit_obj) },
    { MP_ROM_QSTR(MP_QSTR_scroll), MP_ROM_PTR(&framebuf_scroll_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&framebuf_text_obj) },
#ifdef MICROPY_ENABLE_FRAMEBUFFER_ACCEL
    { MP_ROM_QSTR(MP_QSTR_ellipse), MP_ROM_PTR(&framebuf_ellipse_obj) },
#endif
#ifdef FRAMEBUFFER_DEBUG
    { MP_ROM_QSTR(MP_QSTR_debug_update), MP_ROM_PTR(&framebuf_debug_update_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&framebuf_del_obj) },
#endif
};
STATIC MP_DEFINE_CONST_DICT(framebuf_locals_dict, framebuf_locals_dict_table);

STATIC const mp_obj_type_t mp_type_framebuf = {
    { &mp_type_type },
    .name = MP_QSTR_FrameBuffer,
    .make_new = framebuf_make_new,
    .buffer_p = { .get_buffer = framebuf_get_buffer },
    .locals_dict = (mp_obj_dict_t *)&framebuf_locals_dict,
};
#endif

// this factory function is provided for backwards compatibility with old FrameBuffer1 class
STATIC mp_obj_t legacy_framebuffer1(size_t n_args, const mp_obj_t *args) {
    mp_obj_framebuf_t *o = m_new_obj(mp_obj_framebuf_t);
    o->base.type = &mp_type_framebuf;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_WRITE);
    o->buf = bufinfo.buf;

    o->width = mp_obj_get_int(args[1]);
    o->height = mp_obj_get_int(args[2]);
    o->format = FRAMEBUF_MVLSB;
    if (n_args >= 4) {
        o->stride = mp_obj_get_int(args[3]);
    } else {
        o->stride = o->width;
    }

    return MP_OBJ_FROM_PTR(o);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(legacy_framebuffer1_obj, 3, 4, legacy_framebuffer1);

#if !MICROPY_ENABLE_DYNRUNTIME
STATIC const mp_rom_map_elem_t framebuf_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_framebuf) },
    { MP_ROM_QSTR(MP_QSTR_FrameBuffer), MP_ROM_PTR(&mp_type_framebuf) },
    { MP_ROM_QSTR(MP_QSTR_FrameBuffer1), MP_ROM_PTR(&legacy_framebuffer1_obj) },
    { MP_ROM_QSTR(MP_QSTR_MVLSB), MP_ROM_INT(FRAMEBUF_MVLSB) },
    { MP_ROM_QSTR(MP_QSTR_MONO_VLSB), MP_ROM_INT(FRAMEBUF_MVLSB) },
    { MP_ROM_QSTR(MP_QSTR_RGB565), MP_ROM_INT(FRAMEBUF_RGB565) },
    { MP_ROM_QSTR(MP_QSTR_GS2_HMSB), MP_ROM_INT(FRAMEBUF_GS2_HMSB) },
    { MP_ROM_QSTR(MP_QSTR_GS4_HMSB), MP_ROM_INT(FRAMEBUF_GS4_HMSB) },
    { MP_ROM_QSTR(MP_QSTR_GS8), MP_ROM_INT(FRAMEBUF_GS8) },
    { MP_ROM_QSTR(MP_QSTR_MONO_HLSB), MP_ROM_INT(FRAMEBUF_MHLSB) },
    { MP_ROM_QSTR(MP_QSTR_MONO_HMSB), MP_ROM_INT(FRAMEBUF_MHMSB) },
    { MP_ROM_QSTR(MP_QSTR_RGB32), MP_ROM_INT(FRAMEBUF_RGB32) },
};

STATIC MP_DEFINE_CONST_DICT(framebuf_module_globals, framebuf_module_globals_table);

const mp_obj_module_t mp_module_framebuf = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&framebuf_module_globals,
};
#endif

#endif // MICROPY_PY_FRAMEBUF
