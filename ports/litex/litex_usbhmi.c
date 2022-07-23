// This file is Copyright (c) 2022 Victor Suarez Rovere <suarezvictor@gmail.com>
// License: BSD-2-Clause


#include <stdio.h> //for printf
#include <generated/csr.h>

#ifndef CSR_GPIO_BASE
#error The SOC needs a GPIO module (use --with_pmod_gpio in SoC generator)
#endif

#include "irq.h"

#include <litesdk_timer.h>
typedef void (*timer_isr_t)(void); //FIXME: redefinition
typedef int timer_idx_t; //FIXME: redefinition
timer_isr_t timer_handler = NULL;
int timer_isr_count = 0;
void timer0_isr(void)
{
  timer0_ev_pending_write(timer0_ev_pending_read()); //FIXME: add to SDK
  if(timer_handler)
    timer_handler();
  ++timer_isr_count;
}

//FIXME: move
void hal_timer_setup(timer_idx_t timer_num, uint32_t usec, timer_isr_t handler)
{
   timer_handler = handler;
   litetimer_t *tim = litetimer_instance(timer_num);
     
   litetimer_set_periodic_cycles(tim, litetimer_us_to_cycles(tim, usec));
   irq_setmask(irq_getmask() | (1 << TIMER0_INTERRUPT));

   timer0_ev_pending_write(timer0_ev_pending_read()); //FIXME: move to SDK
   timer0_ev_enable_write(1); //FIXME: move to SDK
   litetimer_start(tim);
}

#include <usbhost/usb_host.h>
#include "py/runtime.h"
#include "py/obj.h"


static unsigned activity_count = 0;
void usbh_on_activitystatus(int on_off)
{
#ifdef BLINK_GPIO
  hal_gpio_set_level(BLINK_GPIO, on_off);
#endif

  ++activity_count;
}

void usbh_on_hiddata_log(uint8_t usbNum, uint8_t byte_depth, uint8_t* data, uint8_t data_len)
{
  // if( myListenUSBPort != usbNum ) return;
  printf("USB %d in (HID type %d): ", usbNum, hid_types[usbNum]);
  for(int k=0;k<data_len;k++) {
    printf("0x%02x ", data[k] );
  }
  printf("\n");
}


void usbh_on_detect( uint8_t usbNum, void * dev )
{
  sDevDesc *device = (sDevDesc*)dev;
  printf("New device detected on USB#%d\n", usbNum);
  printf("desc.bcdUSB             = 0x%04x\n", device->bcdUSB);
  printf("desc.bDeviceClass       = 0x%02x\n", device->bDeviceClass);
  printf("desc.bDeviceSubClass    = 0x%02x\n", device->bDeviceSubClass);
  printf("desc.bDeviceProtocol    = 0x%02x\n", device->bDeviceProtocol);
  printf("desc.bMaxPacketSize0    = 0x%02x\n", device->bMaxPacketSize0);
  printf("desc.idVendor           = 0x%04x\n", device->idVendor);
  printf("desc.idProduct          = 0x%04x\n", device->idProduct);
  printf("desc.bcdDevice          = 0x%04x\n", device->bcdDevice);
  printf("desc.iManufacturer      = 0x%02x\n", device->iManufacturer);
  printf("desc.iProduct           = 0x%02x\n", device->iProduct);
  printf("desc.iSerialNumber      = 0x%02x\n", device->iSerialNumber);
  printf("desc.bNumConfigurations = 0x%02x\n", device->bNumConfigurations);
  // if( device->iProduct == mySupportedIdProduct && device->iManufacturer == mySupportedManufacturer ) {
  //   myListenUSBPort = usbNum;
  // }
  
  if(usbNum < NUM_USB)
  {
    hid_protocol_t hid_protocol = hid_types[usbNum];
    if(hid_protocol == USB_HID_PROTO_KEYBOARD)
      printf("HID KEYBOARD DETECTED\n");
    if(hid_protocol == USB_HID_PROTO_MOUSE)
      printf("HID MOUSE DETECTED\n");
  }
}


typedef struct _litex_usbhmi_type_t {
    mp_obj_base_t base;
    int pins[4];
    int mousex, mousey, dx, dy, buttons, wheel;
    uint64_t t0;
    mp_uint_t num;
} litex_usbhmi_obj_t;

const mp_obj_type_t litex_usbhmi_type;


void usbhmi_initialize(litex_usbhmi_obj_t *self)
{
    printf("pins %d, %d, %d, %d!\n", self->pins[0], self->pins[1], self->pins[2], self->pins[3]);
    usbh_pins_init(self->pins[0], self->pins[1], self->pins[2], self->pins[3], USBH_QUEUE_SIZE);

    self->mousex = self->mousey = self->dx = self->dy = self->buttons = self->wheel = 0;

    self->t0 = micros();
    
}

static int mousex = 320, mousey = 240;
static int mouse_dx = 0, mouse_dy = 0;
uint8_t mousebuttons = 0;
int mousewheel = 0;

//mouse event (called by the usbh_hid_poll function)
int usbh_on_hidevent_mouse(int dx, int dy, int buttons, int wheel)
{
  mousex += dx;
  mousey += dy;
  mouse_dx = dx;
  mouse_dy = dy;
  mousebuttons = buttons;
  mousewheel = wheel;
  
  printf("MOUSE event: x %d (%+d), y %d (%+d), buttons 0x%02X wheel %d\n", mousex, dx, mousey, dy, buttons, wheel);
  return true; //process all mouse events
}

int usbh_on_hidevent_keyboard(uint8_t modifiers, uint8_t key, int pressed, char inputchar)
{
  printf("KEYBOARD event: %s event, key 0x%02x, char '%c', modifiers 0x%02x\n", pressed ? "PRESSED":"RELEASED", key, inputchar, modifiers);
  return true;
}

void usbhmi_process(litex_usbhmi_obj_t *self)
{
    //printf("Activity count %d\n", activity_count);
	uint64_t t1 = micros();
	float dt = (int64_t)(t1-self->t0)*1.e-6;
	self->t0 = t1;

    usbh_hid_poll(dt); //may call events
    self->mousex = mousex;
    self->mousey = mousey;
    self->dx = mouse_dx;
    self->dy = mouse_dy;
    self->buttons = mousebuttons;
    self->wheel = mousewheel;
}

STATIC mp_obj_t litex_usbhmi_make_new(const mp_obj_type_t *type_in,
		size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false); //TODO: allow first argument to be ommited

    mp_uint_t num = mp_obj_get_int(args[0]);

    if (num != 0) {
         nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
			"not a argument number: %d", num));
    }
    litex_usbhmi_obj_t *self = m_new_obj(litex_usbhmi_obj_t);
    self->base.type = &litex_usbhmi_type;
    self->num = num;

	static const int DP_P0 = 13; //D+
	static const int DM_P0 = 12; //D-
	static const int DP_P1 = 14; //D+
	static const int DM_P1 = 15; //D-
    self->pins[0] = DP_P0;
    self->pins[1] = DM_P0;
    self->pins[2] = DP_P1;
    self->pins[3] = DM_P1;

    return MP_OBJ_FROM_PTR(self);
}

void litex_usbhmi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    litex_usbhmi_obj_t *self = (litex_usbhmi_obj_t *) self_in;
    mp_printf(print, "usbhmi(%u):", self->num);
}

STATIC mp_obj_t litex_usbhmi_initialize(mp_obj_t self_in)
{
    litex_usbhmi_obj_t *self = (litex_usbhmi_obj_t *) self_in;
    usbhmi_initialize(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_usbhmi_initialize_obj, litex_usbhmi_initialize);

STATIC mp_obj_t litex_usbhmi_process(mp_obj_t self_in)
{
    litex_usbhmi_obj_t *self = (litex_usbhmi_obj_t *) self_in;
    usbhmi_process(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_usbhmi_process_obj, litex_usbhmi_process);


STATIC const mp_map_elem_t litex_usbhmi_locals_dict_table[] = {
	{ MP_OBJ_NEW_QSTR(MP_QSTR_initialize), (mp_obj_t) &litex_usbhmi_initialize_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_process), (mp_obj_t) &litex_usbhmi_process_obj },
};

STATIC MP_DEFINE_CONST_DICT(litex_usbhmi_locals_dict, litex_usbhmi_locals_dict_table);

const mp_obj_type_t litex_usbhmi_type = {
	{ &mp_type_type },
	.name = MP_QSTR_USBHMI,
	.print = litex_usbhmi_print,
	.make_new = litex_usbhmi_make_new,
	.locals_dict = (mp_obj_t)&litex_usbhmi_locals_dict,
};


