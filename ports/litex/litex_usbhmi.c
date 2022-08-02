// This file is Copyright (c) 2022 Victor Suarez Rovere <suarezvictor@gmail.com>
// License: BSD-2-Clause


#include <stdio.h> //for printf
#include <generated/csr.h>
#include <generated/soc.h> //for getting framebuffer resolution

#ifdef LITEX_SOFTUSB_HOST

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


typedef struct _litex_usbhmi_type_t {
    mp_obj_base_t base;
    int pins[4];
    int mousex, mousey, dx, dy, buttons, wheel;
    uint64_t t0;
} litex_usbhmi_obj_t;


#define MP_GETTER_INT_IMPL(cls, typ, member) \
STATIC mp_obj_t cls##_get_##member(mp_obj_t self_in) { \
    return typ(((cls##_obj_t *) self_in)->member); } \
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cls##_##member##_obj, cls##_get_##member);

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


const mp_obj_type_t litex_usbhmi_type;


void usbhmi_start(litex_usbhmi_obj_t *self)
{
    printf("pins %d, %d, %d, %d!\n", self->pins[0], self->pins[1], self->pins[2], self->pins[3]);
    usbh_pins_init(self->pins[0], self->pins[1], self->pins[2], self->pins[3], USBH_QUEUE_SIZE);

    self->mousex = self->mousey = self->dx = self->dy = self->buttons = self->wheel = 0;
#ifdef VIDEO_FRAMEBUFFER_HRES
	self->mousex = VIDEO_FRAMEBUFFER_HRES/2;
#endif
#ifdef VIDEO_FRAMEBUFFER_VRES
	self->mousey = VIDEO_FRAMEBUFFER_VRES/2;
#endif

#warning this should be handled inside the API
    //waits a bit to detect already plugged devices
    self->t0 = micros();
    uint64_t tout = self->t0 + 200*1000;
    while((int64_t)(self->t0 - tout) < 0)
      self->t0 = micros();
}

int mousex, mousey;
hid_event_mouse mouse_event;

//mouse event (called by the usbh_hid_poll function)
int usbh_on_hidevent_mouse(int dx, int dy, int buttons, int wheel)
{
  mousex += dx;
#ifdef VIDEO_FRAMEBUFFER_HRES
  if(mousex < 0)
  {
    dx -= mousex; 
    mousex = 0;
  }
  if(mousex > VIDEO_FRAMEBUFFER_HRES-1)
  {
    dx -= mousex - (VIDEO_FRAMEBUFFER_HRES-1);
    mousex = VIDEO_FRAMEBUFFER_HRES-1;
  }
#endif
  mousey += dy;
#ifdef VIDEO_FRAMEBUFFER_VRES
  if(mousey < 0)
  {
    dy -= mousey; 
    mousey = 0;
  }
  if(mousey > VIDEO_FRAMEBUFFER_VRES-1)
  {
    dy -= mousey - (VIDEO_FRAMEBUFFER_VRES-1);
    mousey = VIDEO_FRAMEBUFFER_VRES-1;
  }
#endif
  mouse_event.x = dx;
  mouse_event.y = dy;
  mouse_event.buttons = buttons;
  mouse_event.wheel = wheel;
  
  //printf("MOUSE event: x %d (%+d), y %d (%+d), buttons 0x%02X wheel %d\n", mousex, dx, mousey, dy, buttons, wheel);
  return true; //process all mouse events
}

int usbh_on_hidevent_keyboard(uint8_t modifiers, uint8_t key, int pressed, char inputchar)
{
  printf("KEYBOARD event: %s event, key 0x%02x, char '%c', modifiers 0x%02x\n", pressed ? "PRESSED":"RELEASED", key, inputchar, modifiers);
  return true;
}

hid_protocol_t usbhmi_process(litex_usbhmi_obj_t *self)
{
    //printf("Activity count %d\n", activity_count);
	uint64_t t1 = micros();
	float dt = (int64_t)(t1-self->t0)*1.e-6;
	self->t0 = t1;

#warning the FPS limits should be internal
    if(dt < 1./60)
      dt = 1./60; //limit FPS for not distorting acelleration calcuations

    mousex = self->mousex;
    mousey = self->mousey;
    hid_protocol_t proto = usbh_hid_poll(dt); //may call events
    switch(proto)
    {
		case USB_HID_PROTO_MOUSE:
		    self->mousex = mousex;
		    self->mousey = mousey;
		    self->dx = mouse_event.x;
		    self->dy = mouse_event.y;
		    self->buttons = mouse_event.buttons;
		    self->wheel = mouse_event.wheel;
		    break;
		case USB_HID_PROTO_KEYBOARD:
			break;
		case USB_HID_PROTO_NONE:
			break;
	}
	return proto;
}

STATIC mp_obj_t litex_usbhmi_make_new(const mp_obj_type_t *type_in,
		size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 4, false);


    if (n_args & 1) {
         nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
			"pin numbers should be set in pairs, but an odd number of arguments was passed"));
    }
    litex_usbhmi_obj_t *self = m_new_obj(litex_usbhmi_obj_t);
    self->base.type = &litex_usbhmi_type;

	for(int i= 0; i < n_args; ++i) //set pins from argument pairs
      self->pins[i] = mp_obj_get_int(args[i]);

    return MP_OBJ_FROM_PTR(self);
}

void litex_usbhmi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    litex_usbhmi_obj_t *self = (litex_usbhmi_obj_t *) self_in;
    mp_printf(print, "USB_HMI, pins D0+:%d,D0-:%d,D1+:%d,D1-:%d", self->pins[0], self->pins[1], self->pins[2], self->pins[3]);
}

STATIC mp_obj_t litex_usbhmi_start(mp_obj_t self_in)
{
    litex_usbhmi_obj_t *self = (litex_usbhmi_obj_t *) self_in;
    usbhmi_start(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_usbhmi_start_obj, litex_usbhmi_start);

STATIC mp_obj_t litex_usbhmi_process(mp_obj_t self_in)
{
    litex_usbhmi_obj_t *self = (litex_usbhmi_obj_t *) self_in;
    return MP_OBJ_NEW_SMALL_INT(usbhmi_process(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_usbhmi_process_obj, litex_usbhmi_process);

MP_GETTER_INT_IMPL(litex_usbhmi, MP_OBJ_NEW_SMALL_INT, mousex)
MP_GETTER_INT_IMPL(litex_usbhmi, MP_OBJ_NEW_SMALL_INT, mousey)
MP_GETTER_INT_IMPL(litex_usbhmi, MP_OBJ_NEW_SMALL_INT, dx)
MP_GETTER_INT_IMPL(litex_usbhmi, MP_OBJ_NEW_SMALL_INT, dy)
MP_GETTER_INT_IMPL(litex_usbhmi, MP_OBJ_NEW_SMALL_INT, buttons)
MP_GETTER_INT_IMPL(litex_usbhmi, MP_OBJ_NEW_SMALL_INT, wheel)

STATIC const mp_map_elem_t litex_usbhmi_locals_dict_table[] = {
	{ MP_OBJ_NEW_QSTR(MP_QSTR_start),	(mp_obj_t) &litex_usbhmi_start_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_process),		(mp_obj_t) &litex_usbhmi_process_obj },
	//getters
	{ MP_OBJ_NEW_QSTR(MP_QSTR_mousex),	(mp_obj_t) &litex_usbhmi_mousex_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_mousey),	(mp_obj_t) &litex_usbhmi_mousey_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_dx),      (mp_obj_t) &litex_usbhmi_dx_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_dy),		(mp_obj_t) &litex_usbhmi_dy_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_buttons),	(mp_obj_t) &litex_usbhmi_buttons_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_wheel),	(mp_obj_t) &litex_usbhmi_wheel_obj },
};

STATIC MP_DEFINE_CONST_DICT(litex_usbhmi_locals_dict, litex_usbhmi_locals_dict_table);

const mp_obj_type_t litex_usbhmi_type = {
	{ &mp_type_type },
	.name = MP_QSTR_USB_HMI,
	.print = litex_usbhmi_print,
	.make_new = litex_usbhmi_make_new,
	.locals_dict = (mp_obj_t)&litex_usbhmi_locals_dict,
};

#endif // LITEX_SOFTUSB_HOST

