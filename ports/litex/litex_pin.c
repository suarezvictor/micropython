
#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/objexcept.h"

#include <generated/csr.h>

#if !defined(CSR_GPIO_BASE)
static inline unsigned char gpio_out_read(void) {
	return 0;
}

static inline void gpio_out_write(unsigned char value) {
}
#endif

const mp_obj_type_t litex_pin_type;

typedef struct _litex_pin_obj_t {
    mp_obj_base_t base;
    int num;
} litex_pin_obj_t;

STATIC litex_pin_obj_t litex_pin[8] = {
	{{&litex_pin_type}, 0},
	{{&litex_pin_type}, 1},
	{{&litex_pin_type}, 2},
	{{&litex_pin_type}, 3},
	{{&litex_pin_type}, 4},
	{{&litex_pin_type}, 5},
	{{&litex_pin_type}, 6},
	{{&litex_pin_type}, 7}
};

STATIC mp_obj_t litex_pin_make_new(const mp_obj_type_t *type_in,
		size_t n_args, size_t n_kw, const mp_obj_t *args) {
	mp_arg_check_num(n_args, n_kw, 1, 1, false);

	mp_uint_t pin_num = mp_obj_get_int(args[0]);

	switch (pin_num) {
	case 0 ... 7:
		return &litex_pin[pin_num];
	default:
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
			"not a valid PIN number: %d", pin_num));
	}
}

void litex_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
	litex_pin_obj_t *self = self_in;
	mp_printf(print, "PIN(%u)", self->num);
}

STATIC mp_obj_t litex_pin_value(mp_obj_t self_in) {
	litex_pin_obj_t *pin = self_in;
	bool state = gpio_out_read() & (1 << pin->num);

	return mp_obj_new_bool(state);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_pin_value_obj, litex_pin_value);

STATIC mp_obj_t litex_pin_high(mp_obj_t self_in) {
	litex_pin_obj_t *pin = self_in;
	char oe    = gpio_oe_read();
	char value = gpio_out_read();

	gpio_oe_write( oe    | (1 << pin->num));
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

const mp_obj_type_t litex_pin_type = {
	{ &mp_type_type },
	.name = MP_QSTR_pin,
	.print = litex_pin_print,
	.make_new = litex_pin_make_new,
	.locals_dict = (mp_obj_t)&litex_pin_locals_dict,
};
