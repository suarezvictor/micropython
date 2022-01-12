// This file is Copyright (c) 2017-2021 Fupy/LiteX-MicroPython Developers
// This file is Copyright (c) 2021 Victor Suarez Rovere <suarezvictor@gmail.com>
// License: BSD-2-Clause

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

// extended modules
#define MICROPY_PY_MACHINE_SPI              (1)
#define MICROPY_PY_MACHINE_SPI_MSB          (1)
#define MICROPY_PY_MACHINE_SPI_LSB          (0)
#define MICROPY_PY_MACHINE_I2C              (1)
#define MICROPY_PY_UTIME_MP_HAL             (1)

// Type definitions for the specific machine

typedef intptr_t  mp_int_t;
typedef uintptr_t mp_uint_t;
typedef long      mp_off_t;


#include <generated/csr.h>

// C-level pin HAL
#define MP_HAL_PIN_FMT "%u"
#define mp_hal_pin_name(p)      (p)

#ifdef CSR_GPIO_BASE
#if CSR_GPIO_OE_SIZE > 1
typedef uint64_t csr_gpio_t; //up to 64 pins supported as defined in software/include/generated/csr.h (uint64_t type)
#else
typedef uint32_t csr_gpio_t;
#endif
#define csr_1 ((csr_gpio_t)1)
#define csr_pin_set(v, p) ((v) | (csr_1 << (p)))
#define csr_pin_clear(v, p) ((v) & ~(csr_1 << (p)))
#define csr_pin_read(v, p) (((v) & (csr_1 << (p))) != 0)

#define mp_hal_pin_obj_t uint8_t

#define mp_hal_pin_input(p)     gpio_oe_write(csr_pin_clear(gpio_oe_read(), p))
#define mp_hal_pin_output(p)    gpio_oe_write(csr_pin_set(gpio_oe_read(), p))
#define mp_hal_pin_low(p)       gpio_out_write(csr_pin_clear(gpio_out_read(), p))
#define mp_hal_pin_high(p)      gpio_out_write(csr_pin_set(gpio_out_read(), p))
#define mp_hal_pin_read(p)      csr_pin_read(gpio_in_read(), p)
#define mp_hal_pin_write(p, v)  ((v) ? mp_hal_pin_high(p) : mp_hal_pin_low(p)) //TODO: write a more time-deterministic implementation:
//#define csr_pin_write(v, p, bit) (csr_pin_clear(v, p) | (((csr_gpio_t)bit) << (p)))

#define mp_hal_pin_open_drain(p) (mp_hal_pin_input(p), mp_hal_pin_low(p)) //assumes hardware pullup (external resistor or FPGA pin configuration)
#define mp_hal_pin_od_low(p)    mp_hal_pin_output(p)
#define mp_hal_pin_od_high(p)   mp_hal_pin_input(p)

mp_hal_pin_obj_t pin_find(const void *pin_in);
#define mp_hal_get_pin_obj(o) pin_find(o)
#define machine_pin_get_id(o) mp_hal_get_pin_obj(o)

#else
//no GPIO enabled in SoC definition
#define mp_hal_pin_input(p)
#define mp_hal_pin_output(p)
#define mp_hal_pin_open_drain(p)
#define mp_hal_pin_od_low(p)
#define mp_hal_pin_od_high(p)
#define machine_pin_get_id(o) 0
#endif

#define TIMER0_POLLING //interrupt handing not enabled yet
#ifdef CSR_TIMER0_UPTIME_LATCH_ADDR
//TODO: use SDK
static inline uint64_t litex_uptime() {  timer0_uptime_latch_write(1); return timer0_uptime_cycles_read(); }
#else
//calibrated for sleep_ms / sleep
static inline uint64_t litex_uptime() { static uint64_t uptime = 0; return uptime+=250; }
#endif

#ifdef CSR_TIMER0_UPTIME_CYCLES_ADDR
void litex_delay_cycles(uint64_t c); //TODO: maybe a faster implementation can be limited to 32 bits
static inline void mp_hal_delay_us_fast(mp_uint_t us) {  uint64_t c = us; c *= CONFIG_CLOCK_FREQUENCY; c /= 1000000; litex_delay_cycles(c); }
#else
static inline void mp_hal_delay_us_fast(mp_uint_t us) { us*=4; volatile static uint8_t t; while(us--) ++t; }
#endif
#define mp_hal_delay_us(us)   mp_hal_delay_us_fast(us)


// Extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_open), (mp_obj_t)&mp_builtin_open_obj },

extern const struct _mp_obj_module_t mp_module_machine;
extern const struct _mp_obj_module_t mp_module_litex;
extern const struct _mp_obj_module_t mp_module_utime;
#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_umachine), MP_ROM_PTR(&mp_module_machine) }, \
    { MP_ROM_QSTR(MP_QSTR_litex),    MP_ROM_PTR(&mp_module_litex)   }, \
    { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&mp_module_utime) }, \

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MICROPY_HW_BOARD_NAME "LiteX SoC"

#ifdef __lm32__
#define MICROPY_HW_MCU_NAME "LM32 CPU"
#elif __or1k__
#define MICROPY_HW_MCU_NAME "Mork1x CPU"
#elif __vexriscv__
#define MICROPY_HW_MCU_NAME "VexRiscv CPU"
#else
#error "Unknown MCU."
#endif

#define MP_STATE_PORT MP_STATE_VM

#ifdef CSR_TIMER0_BASE
#define MICROPY_ENABLE_SCHEDULER                (1)

#define MICROPY_PORT_ROOT_POINTERS \
    struct _machine_timer_obj_t *machine_timer_obj_head; \
    const char *readline_hist[8];
#else
#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8];
#endif //CSR_TIMER0_BASE
