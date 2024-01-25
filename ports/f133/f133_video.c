// This file is Copyright (c) 2021 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: AGPL-3.0-only

#include <string.h> //for memcpy
#include <stdio.h> //for printf

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/obj.h"
#include "py/objarray.h"

#ifdef MICROPY_PY_FRAMEBUF

#warning implement cache flushing
#define video_flush()

#warning implement vsync
#define vsync_signal() 0

#define VIDEO_FRAMEBUFFER_DEPTH 32
#define FRAMEBUFFER_WIDTH 640
#define FRAMEBUFFER_HEIGHT 480

#define VIDEO_ALIGN 0x1000

typedef struct _f133_video_type_t {
    mp_obj_base_t base;
    void *video_addr[2]; //two possible buffers
    int xres, yres;
    size_t stride;
    uint8_t bitdepth;
    int current_buffer; //0 or 1, for double buffer support
    uint8_t num;
} f133_video_obj_t;


void video_clear32(void *video_addr, f133_video_obj_t *self, uint32_t color)
{
    memset(video_addr, color, self->yres*self->stride);
}
/*
void video_demo(f133_video_obj_t *self)
{
    uint8_t *buf = (uint8_t *) self->video_addr[self->current_buffer];
    for(int y = 0; y < self->yres/2; ++y)
    {
      memset(buf, 0xFF, self->stride/2);
      buf += self->stride;
    }
}
*/

const mp_obj_type_t f133_video_type;

extern uintptr_t framebuffer_address;
extern unsigned framebuffer_pitch;

STATIC mp_obj_t f133_video_make_new(const mp_obj_type_t *type_in,
		size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false); //TODO: allow first argument to be ommited

    mp_uint_t video_num = mp_obj_get_int(args[0]);

    if (video_num != 0) {
         nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
			"not a valid video number: %d", video_num));
    }

    framebuffer_pitch = FRAMEBUFFER_WIDTH*VIDEO_FRAMEBUFFER_DEPTH/8;
    void fb_probe(void);
    fb_probe();

    f133_video_obj_t *self = m_new_obj(f133_video_obj_t);
    self->base.type = &f133_video_type;
    self->num = video_num;
    self->xres = FRAMEBUFFER_WIDTH;
    self->yres = FRAMEBUFFER_HEIGHT;
    self->bitdepth = VIDEO_FRAMEBUFFER_DEPTH;
    self->stride = framebuffer_pitch;

    bool doublebuf = false;
    self->current_buffer = 0;
    if(n_args > 1)
      doublebuf = mp_obj_get_int(args[1]);

    self->video_addr[0] = framebuffer_address;
    video_clear32(self->video_addr[0], self, 0);
    
    framebuffer_pitch = self->stride;
    framebuffer_address = self->video_addr[0];

    self->video_addr[1] = NULL;
    if(doublebuf)
    {
      self->video_addr[1] = ((uint8_t*)self->video_addr[0]) + ((self->stride*self->yres + (VIDEO_ALIGN-1)) & (~(VIDEO_ALIGN-1))); //align to 1024-bytes
      video_clear32(self->video_addr[1], self, 0);
      self->current_buffer = 1; //user should start writing on 2nd framebuffer
#ifdef _DEBUG
      printf("Doublebuffer enabled: %d, buffer address 1: 0x%p, buffer address 2: 0x%p\n", doublebuf, self->video_addr[0], self->video_addr[1]);
#endif
    }

#ifndef vsync_signal
	//sets dma threshold at first part of screen, for frame syncrhonization
    //(the value should be low enough to avoid waiting too long, but not too low so the position is not missed at polling
    //max offset is yres*xres/4
    //self->dma_threshold = (self->yres/16)*self->xres/sizeof(uint32_t); 
#endif

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_int_t video_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    (void)flags;
    f133_video_obj_t *self = MP_OBJ_TO_PTR(self_in);
    bufinfo->buf = self->video_addr[self->current_buffer];
    bufinfo->len = self->stride * self->yres;
    bufinfo->typecode = 'B'; // view buffer as bytes
    return 0;
}

void f133_video_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    f133_video_obj_t *self = (f133_video_obj_t *) self_in;
    mp_printf(print, "video(%u): %dx%d %dbpp, stride=%d bytes, buffers base address=(0x%p, 0x%p)",
      self->num, self->xres, self->yres, self->bitdepth, self->stride, self->video_addr[0], self->video_addr[1]);
}

STATIC mp_obj_t f133_video_width(mp_obj_t self_in)
{
    f133_video_obj_t *self = (f133_video_obj_t *) self_in;
    return MP_OBJ_NEW_SMALL_INT(self->xres);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(f133_video_width_obj, f133_video_width);

STATIC mp_obj_t f133_video_height(mp_obj_t self_in)
{
    f133_video_obj_t *self = (f133_video_obj_t *) self_in;
    return MP_OBJ_NEW_SMALL_INT(self->yres);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(f133_video_height_obj, f133_video_height);

STATIC mp_obj_t f133_video_stride(mp_obj_t self_in)
{
    f133_video_obj_t *self = (f133_video_obj_t *) self_in;
    int stride = self->stride/(self->bitdepth/8);
    return MP_OBJ_NEW_SMALL_INT(stride); //expected result is in pixels
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(f133_video_stride_obj, f133_video_stride);

STATIC mp_obj_t f133_video_bpp(mp_obj_t self_in)
{
    f133_video_obj_t *self = (f133_video_obj_t *) self_in;
    return MP_OBJ_NEW_SMALL_INT(self->bitdepth);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(f133_video_bpp_obj, f133_video_bpp);


STATIC mp_obj_t f133_video_blitbuf(mp_obj_t self_in, mp_obj_t mem_buf)
{
    //TODO: test this function
    f133_video_obj_t *self = (f133_video_obj_t *) self_in;

    mp_buffer_info_t src;
    mp_get_buffer_raise(mem_buf, &src, MP_BUFFER_READ);

    size_t len = self->yres*self->stride;
    if (len > src.len)
      len = src.len;
    //memcpy(self->video_addr, src.buf, len);
#warning: optimize    
    uint16_t pix = *(uint16_t*) src.buf;
    for(uint16_t *p = (uint16_t*) self->video_addr[self->current_buffer]; p < ((uint16_t*) self->video_addr[self->current_buffer])+(self->yres*self->stride); ++p)
     *p = pix;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(f133_video_blitbuf_obj, f133_video_blitbuf);

static void wait_vsync(f133_video_obj_t *self)
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
    //while(vsync_signal());
    //while(!vsync_signal());
#endif
}

STATIC mp_obj_t f133_video_flip(mp_obj_t self_in)
{
    f133_video_obj_t *self = (f133_video_obj_t *) self_in;

    //printf("initial offset is: %d\n", video_framebuffer_dma_offset_read());
    video_flush(); //write any pending pixels in current buffer

    wait_vsync(self);

    int next_buf = self->current_buffer;

    if(self->video_addr[1] != NULL) //if double buffer not enabled, skip change
    {
      //video_framebuffer_dma_base_write((uintptr_t)self->video_addr[self->current_buffer]); //switch to the buffer that was written to
      self->current_buffer = !self->current_buffer; //switch active buffer
    }


    return MP_OBJ_NEW_SMALL_INT(next_buf);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(f133_video_flip_obj, f133_video_flip);

STATIC mp_obj_t f133_video_flush(mp_obj_t self_in)
{
    video_flush();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(f133_video_flush_obj, f133_video_flush);

/*
STATIC mp_obj_t f133_video_demo(mp_obj_t self_in)
{
    f133_video_obj_t *self = (f133_video_obj_t *) self_in;
    video_demo(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(f133_video_demo_obj, f133_video_demo);
*/

STATIC const mp_map_elem_t f133_video_locals_dict_table[] = {
	{ MP_OBJ_NEW_QSTR(MP_QSTR_width), (mp_obj_t) &f133_video_width_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_height), (mp_obj_t) &f133_video_height_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_bpp), (mp_obj_t) &f133_video_bpp_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_stride), (mp_obj_t) &f133_video_stride_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_blitbuf), (mp_obj_t) &f133_video_blitbuf_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_flush), (mp_obj_t) &f133_video_flush_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_flip), (mp_obj_t) &f133_video_flip_obj },
	//{ MP_OBJ_NEW_QSTR(MP_QSTR_demo), (mp_obj_t) &f133_video_demo_obj },
};

STATIC MP_DEFINE_CONST_DICT(f133_video_locals_dict, f133_video_locals_dict_table);

const mp_obj_type_t f133_video_type = {
	{ &mp_type_type },
	.name = MP_QSTR_Video,
	.print = f133_video_print,
	.make_new = f133_video_make_new,
	.buffer_p = { .get_buffer = video_get_buffer },
	.locals_dict = (mp_obj_t)&f133_video_locals_dict,
};

#endif //MICROPY_PY_FRAMEBUF

