#include <stdio.h>

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/objexcept.h"
#include "py/mphal.h"

#include <generated/csr.h>

#define GPIO_MODE_IN  (0)
#define GPIO_MODE_OUT (1)

#if !defined(CSR_GPIO_BASE)
static inline unsigned char gpio_out_read(void) {
    return 0;
}

static inline void gpio_out_write(unsigned char value) {
}
#endif

mp_obj_type_t litex_pin_type;

typedef struct _litex_pin_obj_t {
    mp_obj_base_t base;
    uint8_t mode;
    uint32_t num;
} litex_pin_obj_t;

/* FIXME : Start simple with 8 fixed IOs, we'll generalize/expand it later */

STATIC litex_pin_obj_t litex_pin_obj[8] = {
    {{&litex_pin_type}, 0},
    {{&litex_pin_type}, 1},
    {{&litex_pin_type}, 2},
    {{&litex_pin_type}, 3},
    {{&litex_pin_type}, 4},
    {{&litex_pin_type}, 5},
    {{&litex_pin_type}, 6},
    {{&litex_pin_type}, 7}
};

void litex_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    litex_pin_obj_t *self = self_in;
    qstr mode_qst;
    if (self->mode == GPIO_MODE_IN)
        mode_qst =  MP_QSTR_IN;
    else
        mode_qst =  MP_QSTR_OUT;
    mp_printf(print, "PIN(%u, mode=%q)", self->num, mode_qst);
}

STATIC mp_obj_t litex_pin_obj_init_helper(litex_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value, ARG_alt };

    char oe    = gpio_oe_read();

    /* Init API: pin.init(mode=0) */
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,  MP_ARG_INT, {.u_rom_obj = MP_ROM_INT(0)}},
    };

    /* Parse args */
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    /* Configure mode */
    oe = gpio_oe_read();
    oe = (oe & ~(1 << self->num));
    if (args[ARG_mode].u_int == GPIO_MODE_OUT) {
        oe |= (1 << self->num);
    }
    self->mode = args[ARG_mode].u_int;
    gpio_oe_write(oe);

    return mp_const_none;
}

STATIC mp_obj_t litex_pin_make_new(const mp_obj_type_t *type_in,
        size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    litex_pin_obj_t *self;


    /* Get PIN number and create PIN */
    mp_uint_t pin_num = mp_obj_get_int(args[0]);
    switch (pin_num) {
    case 0 ... 7:
        self = &litex_pin_obj[pin_num];
        break;
    default:
        mp_raise_ValueError("Invalid PIN");
    }

    /* Configure PIN */
    if (n_args > 1 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        litex_pin_obj_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t litex_pin_value(mp_obj_t self_in) {
    litex_pin_obj_t *pin = self_in;
    bool state = gpio_in_read() & (1 << pin->num);

    return mp_obj_new_bool(state);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_pin_value_obj, litex_pin_value);

STATIC mp_obj_t litex_pin_high(mp_obj_t self_in) {
    litex_pin_obj_t *pin = self_in;
    char value = gpio_out_read();

    gpio_out_write(value | (1 << pin->num));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_pin_high_obj, litex_pin_high);

STATIC mp_obj_t litex_pin_low(mp_obj_t self_in) {
    litex_pin_obj_t *pin = self_in;
    char value = gpio_out_read();

    gpio_out_write(value & ~(1 << pin->num));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_pin_low_obj, litex_pin_low);


STATIC const mp_map_elem_t litex_pin_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_value), (mp_obj_t)&litex_pin_value_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_high),  (mp_obj_t)&litex_pin_high_obj   },
    { MP_OBJ_NEW_QSTR(MP_QSTR_low),   (mp_obj_t)&litex_pin_low_obj  },
};
STATIC MP_DEFINE_CONST_DICT(litex_pin_locals_dict, litex_pin_locals_dict_table);

mp_obj_type_t litex_pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_pin,
    .print = litex_pin_print,
    .make_new = litex_pin_make_new,
    .locals_dict = (mp_obj_t)&litex_pin_locals_dict,
};
