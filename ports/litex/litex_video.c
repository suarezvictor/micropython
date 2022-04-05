// This file is Copyright (c) 2021 Victor Suarez Rovere <suarezvictor@gmail.com>
// License: BSD-2-Clause

#include <string.h> //for memcpy
#include <stdio.h> //for printf

#include <generated/csr.h>

#ifdef CSR_VIDEO_FRAMEBUFFER_BASE

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/obj.h"
#include "py/objarray.h"


#define video_flush() flush_l2_cache() //TODO: to optimize, should flush just written regions
#define vsync_signal() (video_framebuffer_dma_offset_read() == 0) //it seems offset pass most time at 0: assumed vsync //TODO: use a better signal

#ifndef VIDEO_FRAMEBUFFER_DEPTH
#error VIDEO_FRAMEBUFFER_DEPTH should be specified at SoC generation
#endif

#define VIDEO_ALIGN 0x1000

typedef struct _litex_video_type_t {
    mp_obj_base_t base;
    void *video_addr[2]; //two possible buffers
#ifndef vsync_signal
    uint32_t dma_threshold; //for frame syncronization
#endif
    int xres, yres;
    size_t stride;
    uint8_t bitdepth;
    int current_buffer; //0 or 1, for double buffer support
    uint8_t num;
} litex_video_obj_t;


#ifdef LEARNFPGA_LITEX

#define VIDEO_FRAMEBUFFER_STRIDE (VIDEO_FRAMEBUFFER_HRES*VIDEO_FRAMEBUFFER_DEPTH/8)

#include <lite_fb.h>

void video_clear32(void *video_addr, uint32_t color)
{
    uint8_t *p = (uint8_t * ) video_addr;
    for(int y = 0; y < VIDEO_FRAMEBUFFER_VRES; ++y, p += VIDEO_FRAMEBUFFER_STRIDE)
    {
        uint32_t *px = (uint32_t *) p;
        for(int x = 0; x < VIDEO_FRAMEBUFFER_HRES; ++x)
            *px++ = color;
    }
}
/*
static inline void fb_setpixel_RGB(uint32_t x, uint32_t y, uint8_t R, uint8_t G, uint8_t B);
void fb_clear(void);
void fb_set_cliprect(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
void fb_fillrect(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t RGB);
void fb_line(int x1, int y1, int x2, int y2, uint32_t RGB);
void fb_set_poly_mode(PolyMode mode); //FB_POLY_LINES, FB_POLY_FILL
void fb_set_poly_culling(PolyCulling culling); //FB_POLY_CW, FB_POLY_CCW, FB_POLY_NO_CULLING
void fb_fill_poly(uint32_t nb_pts, int* points, uint32_t RGB);
*/
void video_demo(litex_video_obj_t *self)
{
#ifdef LEARNFPGA_LITEX
    static uint32_t color = 0xFF;

    fb_clear();

    fb_fillrect(50, 50, 200, 200, color);
    color <<= 8; color |= color >> 24; //cycle R, G, B

    fb_line(50, 50, 200, 200, color);
    color <<= 8; color |= color >> 24; //cycle R, G, B

    int points[]={10, 10, 30, 100, 40, 20, 70, 50 };
    fb_fill_poly(sizeof(points)/(2*sizeof(*points)), points, color);
#endif
}

#endif


const mp_obj_type_t litex_video_type;

STATIC mp_obj_t litex_video_make_new(const mp_obj_type_t *type_in,
		size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false); //TODO: allow first argument to be ommited

    mp_uint_t video_num = mp_obj_get_int(args[0]);

    if (video_num != 0) {
         nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
			"not a valid video number: %d", video_num));
    }
    litex_video_obj_t *self = m_new_obj(litex_video_obj_t);
    self->base.type = &litex_video_type;
    self->num = video_num;
    self->xres = video_framebuffer_hres_read(); //VIDEO_FRAMEBUFFER_HRES
    self->yres = video_framebuffer_vres_read(); //VIDEO_FRAMEBUFFER_VRES
    self->bitdepth = VIDEO_FRAMEBUFFER_DEPTH;
    self->stride = self->xres*VIDEO_FRAMEBUFFER_DEPTH/8;

    bool doublebuf = false;
    self->current_buffer = 0;
    if(n_args > 1)
    {
      doublebuf = mp_obj_get_int(args[1]);
      //printf("Doublebuffer enabled: %d, buffer address 1: %p, buffer address 2: %p\n", doublebuf, self->video_addr[0], self->video_addr[1]);
    }

    self->video_addr[0] = (void *) VIDEO_FRAMEBUFFER_BASE; //at reset corresponds to video_framebuffer_base_read();
    self->video_addr[1] = doublebuf ? ((uint8_t*)self->video_addr[0]) + ((self->stride*self->yres + (VIDEO_ALIGN-1)) & (~(VIDEO_ALIGN-1))) : NULL; //align to 1024-bytes

#ifndef vsync_signal
	//sets dma threshold at first part of screen, for frame syncrhonization
    //(the value should be low enough to avoid waiting too long, but not too low so the position is not missed at polling
    //max offset is yres*xres/4
    self->dma_threshold = (self->yres/16)*self->xres/sizeof(uint32_t); 
#endif

#ifdef LEARNFPGA_LITEX
    fb_init();
#endif

    memset(self->video_addr[0], 0, self->yres*self->stride);
	if(doublebuf)
    {
       memset(self->video_addr[1], 0, self->yres*self->stride);
       self->current_buffer = 1; //user should start writing on 2nd framebuffer
    }


    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_int_t video_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    (void)flags;
    litex_video_obj_t *self = MP_OBJ_TO_PTR(self_in);
    bufinfo->buf = self->video_addr[self->current_buffer];
    bufinfo->len = self->stride * self->yres;
    bufinfo->typecode = 'B'; // view buffer as bytes
    return 0;
}

void litex_video_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    litex_video_obj_t *self = (litex_video_obj_t *) self_in;
    mp_printf(print, "video(%u): %dx%d %dbpp, stride=%d bytes, buffers base address=(0x%p, 0x%p)",
      self->num, self->xres, self->yres, self->bitdepth, self->stride, self->video_addr[0], self->video_addr[1]);
}

STATIC mp_obj_t litex_video_width(mp_obj_t self_in)
{
    litex_video_obj_t *self = (litex_video_obj_t *) self_in;
    return MP_OBJ_NEW_SMALL_INT(self->xres);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_video_width_obj, litex_video_width);

STATIC mp_obj_t litex_video_height(mp_obj_t self_in)
{
    litex_video_obj_t *self = (litex_video_obj_t *) self_in;
    return MP_OBJ_NEW_SMALL_INT(self->yres);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_video_height_obj, litex_video_height);

STATIC mp_obj_t litex_video_stride(mp_obj_t self_in)
{
    litex_video_obj_t *self = (litex_video_obj_t *) self_in;
    int stride = self->stride/(self->bitdepth/8);
    return MP_OBJ_NEW_SMALL_INT(stride); //expected result is in pixels
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_video_stride_obj, litex_video_stride);

STATIC mp_obj_t litex_video_bpp(mp_obj_t self_in)
{
    litex_video_obj_t *self = (litex_video_obj_t *) self_in;
    return MP_OBJ_NEW_SMALL_INT(self->bitdepth);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_video_bpp_obj, litex_video_bpp);


STATIC mp_obj_t litex_video_blitbuf(mp_obj_t self_in, mp_obj_t mem_buf)
{
    //TODO: test this function
    litex_video_obj_t *self = (litex_video_obj_t *) self_in;

    mp_buffer_info_t src;
    mp_get_buffer_raise(mem_buf, &src, MP_BUFFER_READ);

    size_t len = self->yres*self->stride;
    if (len > src.len)
      len = src.len;
    //memcpy(self->video_addr, src.buf, len);
    uint16_t pix = *(uint16_t*) src.buf;
    for(uint16_t *p = (uint16_t*) self->video_addr[self->current_buffer]; p < ((uint16_t*) self->video_addr[self->current_buffer])+(self->yres*self->stride); ++p)
     *p = pix;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(litex_video_blitbuf_obj, litex_video_blitbuf);

static void wait_vsync(litex_video_obj_t *self)
{
#ifndef vsync_signal
      uint32_t off = 0;
      //printf("initial offset is: %d\n", video_framebuffer_dma_offset_read());
      while(off < self->dma_threshold) //wait while in upper part of screen
        off = video_framebuffer_dma_offset_read();
      //printf("other offset is: %d\n", off);

      while(!(off < self->dma_threshold)) //wait until frame restarts (nearly offset 0), in practice it's always 0
        off = video_framebuffer_dma_offset_read();
      //printf("offset near zero is: %d\n", off);
#else
    while(vsync_signal());
    while(!vsync_signal());
#endif
}

STATIC mp_obj_t litex_video_flip(mp_obj_t self_in)
{
    litex_video_obj_t *self = (litex_video_obj_t *) self_in;

    //printf("initial offset is: %d\n", video_framebuffer_dma_offset_read());
    video_flush(); //write any pending pixels in current buffer

    wait_vsync(self);

    int next_buf = self->current_buffer;

    if(self->video_addr[1] != NULL) //if double buffer not enabled, skip change
    {
      video_framebuffer_dma_base_write((uintptr_t)self->video_addr[self->current_buffer]); //switch to the buffer that was written to
      self->current_buffer = !self->current_buffer; //switch active buffer
    }


    return MP_OBJ_NEW_SMALL_INT(next_buf);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_video_flip_obj, litex_video_flip);

STATIC mp_obj_t litex_video_flush(mp_obj_t self_in)
{
    video_flush();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_video_flush_obj, litex_video_flush);

#ifdef LEARNFPGA_LITEX
STATIC mp_obj_t litex_video_demo(mp_obj_t self_in)
{
    litex_video_obj_t *self = (litex_video_obj_t *) self_in;
    video_demo(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_video_demo_obj, litex_video_demo);
#endif

STATIC const mp_map_elem_t litex_video_locals_dict_table[] = {
	{ MP_OBJ_NEW_QSTR(MP_QSTR_width), (mp_obj_t) &litex_video_width_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_height), (mp_obj_t) &litex_video_height_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_bpp), (mp_obj_t) &litex_video_bpp_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_stride), (mp_obj_t) &litex_video_stride_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_blitbuf), (mp_obj_t) &litex_video_blitbuf_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_flush), (mp_obj_t) &litex_video_flush_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_flip), (mp_obj_t) &litex_video_flip_obj },
#ifdef LEARNFPGA_LITEX
	{ MP_OBJ_NEW_QSTR(MP_QSTR_demo), (mp_obj_t) &litex_video_demo_obj },
#endif
};

STATIC MP_DEFINE_CONST_DICT(litex_video_locals_dict, litex_video_locals_dict_table);

const mp_obj_type_t litex_video_type = {
	{ &mp_type_type },
	.name = MP_QSTR_Video,
	.print = litex_video_print,
	.make_new = litex_video_make_new,
	.buffer_p = { .get_buffer = video_get_buffer },
	.locals_dict = (mp_obj_t)&litex_video_locals_dict,
};

#endif //CSR_VIDEO_FRAMEBUFFER_BASE

