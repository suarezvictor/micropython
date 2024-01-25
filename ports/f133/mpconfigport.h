// This file is Copyright (c) 2017-2021 MicroPython Developers
// This file is Copyright (c) 2021, 2024 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause

#include <stdint.h>

// Python internal features.
#define MICROPY_ENABLE_COMPILER     (1)
#define MICROPY_ENABLE_GC           (1)

#define MICROPY_HELPER_REPL         (1)
#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_DOUBLE)

// Python builtins, classes, modules, etc. features.
#define MICROPY_PY_MATH             (1)
#define MICROPY_PY_CMATH            (1)
#define MICROPY_PY_SYS              (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_KBD_EXCEPTION       (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)

// extended modules
#define MICROPY_PY_MACHINE_SPI              (0)
#define MICROPY_PY_MACHINE_SPI_MSB          (0)
#define MICROPY_PY_MACHINE_SPI_LSB          (0)
#define MICROPY_PY_MACHINE_I2C              (0)
#define MICROPY_PY_UTIME_MP_HAL             (1)
#if !MICROPY_ENABLE_SCHEDULER
#define MICROPY_ENABLE_SCHEDULER                (0)
#endif

// Type definitions for the specific machine

typedef intptr_t  mp_int_t;
typedef uintptr_t mp_uint_t;
typedef long      mp_off_t;


#define MICROPY_PY_FRAMEBUF (0)
#define MICROPY_VFS_FAT (0)


#define mp_hal_delay_us(us) delay_us(us)

extern const struct _mp_obj_module_t mp_module_machine;
extern const struct _mp_obj_module_t mp_module_f133;
extern const struct _mp_obj_module_t mp_module_utime;
#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_umachine), MP_ROM_PTR(&mp_module_machine) }, \
    { MP_ROM_QSTR(MP_QSTR_f133),    MP_ROM_PTR(&mp_module_f133)   }, \
    { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&mp_module_utime) }, \

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MICROPY_HW_BOARD_NAME "F133 board"

#define MICROPY_HW_MCU_NAME "RISCV CPU"

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8];
