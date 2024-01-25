// This file is Copyright (c) 2017-2021 MicroPython Developers
// This file is Copyright (c) 2021, 2024 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/stream.h"
#include "lib/utils/pyexec.h"

#include "mphalport.h"


static char *stack_top;
int upython_main(int argc, char **argv, char *stack_top_arg)
{
    stack_top = stack_top_arg;
    {
#ifdef VIDEO_FRAMEBUFFER_BASE
        void *heap_start = &_end, *heap_end = &_emain_ram; //TODO: move this logic to the C SDK

        #warning A ram region for the video framebuffer should be allocated in linker scripts
        void *video_base = (void *) VIDEO_FRAMEBUFFER_BASE; //TODO: move framebuffer logic to the C SDK
        if(heap_start <= video_base && heap_end > video_base) //check if framebuffer overlaps
        {
            //heap_end = video_base; //WARNING: this will collide with malloc, since based on sbrk that used linker script's _end
            //it's better instead to use the RAM after the framebuffer up to the end of the SDRAM
            heap_start = ((char *)video_base) + (VIDEO_FRAMEBUFFER_HRES*VIDEO_FRAMEBUFFER_VRES*VIDEO_FRAMEBUFFER_DEPTH/8)*4; //supports 4 framebuffers
        }


        #ifdef _DEBUG        
        printf("RAM base at 0x%p, heap at 0x%p, end=0x%p (%d KiB)\n", (void *)MICROPY_HW_SDRAM_BASE, heap_start, heap_end, ((char*)heap_end-(char*)heap_start)/1024);
        #endif
        gc_init(heap_start, heap_end);
#else
        static uint8_t heap[4096];
        gc_init(heap, heap + sizeof(heap));
#endif
    }

    mp_init();

#if MICROPY_ENABLE_COMPILER
    #if !MICROPY_REPL_EVENT_DRIVEN
    for (;;)
    {
        int resp;
        if(pyexec_mode_kind == PYEXEC_MODE_RAW_REPL)
            resp = pyexec_raw_repl();
        else
            resp = pyexec_friendly_repl();

        switch(resp)
        {
            case 0: continue;
            case PYEXEC_FORCED_EXIT: //CTRL-D, do soft reset
                break;
            default:
                return -1; //do hard reset
        }
        break;
    }
    #else
    #error MICROPY_REPL_EVENT_DRIVEN is not tested!
    pyexec_event_repl_init();
    for (;;)
    {
        int c = mp_hal_stdin_rx_chr();
        if (pyexec_event_repl_process_char(c)) {
            break;
        }
    }
    #endif
#else
#error not enabling MICROPY_ENABLE_COMPILER is not tested!
#endif


#if MICROPY_ENABLE_GC
    gc_sweep_all();
#endif

    mp_hal_stdout_tx_strn("MPY: soft reboot\r\n", 18);
    mp_hal_delay_us(10000); // allow UART to flush output
    mp_deinit();
    #if MICROPY_REPL_EVENT_DRIVEN
    pyexec_event_repl_init();
    #endif

    return 0;
}

#if MICROPY_ENABLE_GC
void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    //gc_dump_info();
}
#else
#warning MICROPY_ENABLE_GC should be enabled
#endif

void start_micropython(int argc, char **argv)
{
	int dummy; //it doesn't matter what references since only address is taken
    if(upython_main(argc, argv, (char*)&dummy) == -1)
      /*do_hard_reset()*/;
}

#if !MICROPY_READER_VFS
mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    //defined in case of not using the implementation in extmod/vfs_reader.c
    mp_raise_OSError(MP_ENOENT);
}
#endif

#if !MICROPY_VFS
mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);
#endif

void nlr_jump_fail(void *val) {
    while (1) {
        ;
    }
}

void NORETURN __fatal_error(const char *msg) {
    while (1) {
        ;
    }
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    __fatal_error("mp_hal_stdio_poll not implemented");
//    if ((poll_flags & MP_STREAM_POLL_RD) && stdin_ringbuf.iget != stdin_ringbuf.iput)
//        ret |= MP_STREAM_POLL_RD;
    return ret;
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif

