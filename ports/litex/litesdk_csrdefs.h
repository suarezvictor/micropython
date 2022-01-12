////////////////////////////////////////////////////////////
//FIXME: this source should be autogenerated (i.e. in csr.h)
////////////////////////////////////////////////////////////
#ifndef __LITESDK_CSR_DEFS__H_
#define __LITESDK_CSR_DEFS__H_

#include "csr_helpers.h"

////////////////////////////////////////////////////////////
//supported peripherals
////////////////////////////////////////////////////////////

//IMPORTANT NOTES:
// 1) register should be 'volatile' (since memory-mapped IO should never be cached by the compiler as sometimes are regular variables)
// macro LITEX_OFFSET_ASSERT can be used to check at compile time if the offset for the registers matches the definition in csr.h
// 2) structures should be *packed* at 32-bit boundaries (since 64-bit registers sometimes are not aligned to 8-byte boundaries)
// macro LITEX_PACKED32 should be used in structure definitions (only GCC support for now)


///////////////////////////////
// Timer
///////////////////////////////

#ifdef CSR_TIMER0_BASE
#define LITETIMER_ENABLED
#define LITETIMER_READ(s, field)		LITECSR_READ(CSR_TIMER0, s, field)
#define LITETIMER_WRITE(s, field, v)		LITECSR_WRITE(CSR_TIMER0, s, field, v)
typedef LITECSR(CSR_TIMER0_LOAD_SIZE) 		litetimer_load_t;
typedef LITECSR(CSR_TIMER0_RELOAD_SIZE)		litetimer_reload_t;
typedef LITECSR(CSR_TIMER0_EN_SIZE)		litetimer_en_t;
typedef LITECSR(CSR_TIMER0_UPDATE_VALUE_SIZE)	litetimer_update_value_t;
typedef LITECSR(CSR_TIMER0_VALUE_SIZE)		litetimer_value_t;
typedef LITECSR(CSR_TIMER0_EV_STATUS_SIZE)	litetimer_ev_status_t;
typedef LITECSR(CSR_TIMER0_EV_PENDING_SIZE)	litetimer_ev_pending_t;
typedef LITECSR(CSR_TIMER0_EV_ENABLE_SIZE)	litetimer_ev_enable_t;
#ifdef CSR_TIMER0_UPTIME_LATCH_ADDR
typedef LITECSR(CSR_TIMER0_UPTIME_LATCH_SIZE)   litetimer_uptime_latch_t;
typedef LITECSR(CSR_TIMER0_UPTIME_CYCLES_SIZE)  litetimer_uptime_cycles_t; //2 words
#endif


typedef struct LITEX_PACKED32 //fields arrangement must mach order to get exact peripheral layout
{
  litetimer_load_t		LOAD;
  litetimer_reload_t		RELOAD;
  litetimer_en_t		EN;
  litetimer_update_value_t	UPDATE_VALUE;
  litetimer_value_t		VALUE;
  litetimer_ev_status_t		EV_STATUS;
  litetimer_ev_pending_t	EV_PENDING;
  litetimer_ev_enable_t		EV_ENABLE;
#ifdef CSR_TIMER0_UPTIME_LATCH_ADDR
  litetimer_uptime_latch_t	UPTIME_LATCH;
  litetimer_uptime_cycles_t	UPTIME_CYCLES;
#endif
} litetimer_t;


//offset checking
LITEX_OFFSET_ASSERT(CSR_TIMER0, litetimer_t, LOAD);
LITEX_OFFSET_ASSERT(CSR_TIMER0, litetimer_t, RELOAD);
LITEX_OFFSET_ASSERT(CSR_TIMER0, litetimer_t, UPDATE_VALUE);
LITEX_OFFSET_ASSERT(CSR_TIMER0, litetimer_t, VALUE);
LITEX_OFFSET_ASSERT(CSR_TIMER0, litetimer_t, EV_STATUS);
LITEX_OFFSET_ASSERT(CSR_TIMER0, litetimer_t, EV_PENDING);
LITEX_OFFSET_ASSERT(CSR_TIMER0, litetimer_t, EV_ENABLE);
#ifdef CSR_TIMER0_UPTIME_LATCH_ADDR
LITEX_OFFSET_ASSERT(CSR_TIMER0, litetimer_t, UPTIME_LATCH);
LITEX_OFFSET_ASSERT(CSR_TIMER0, litetimer_t, UPTIME_CYCLES);
#endif
#endif //CSR_TIMER0_BASE


///////////////////////////////
// GPIO
///////////////////////////////

#ifdef CSR_GPIO_BASE
#define LITEGPIO_ENABLED
#define LITEGPIO_READ(s, field)		LITECSR_READ(CSR_GPIO, s, field)
#define LITEGPIO_WRITE(s, field, v)	LITECSR_WRITE(CSR_GPIO, s, field, v)
typedef LITECSR(CSR_GPIO_OE_SIZE)	litegpio_oe_t;
typedef LITECSR(CSR_GPIO_IN_SIZE)	litegpio_in_t;
typedef LITECSR(CSR_GPIO_OUT_SIZE)	litegpio_out_t;

typedef struct LITEX_PACKED32 //fields arrangement must mach order to get exact peripheral layout
{
  litegpio_oe_t		OE;
  litegpio_in_t		IN;
  litegpio_out_t	OUT;
} litegpio_t;

//offset checking
LITEX_OFFSET_ASSERT(CSR_GPIO, litegpio_t, OE);
LITEX_OFFSET_ASSERT(CSR_GPIO, litegpio_t, IN);
LITEX_OFFSET_ASSERT(CSR_GPIO, litegpio_t, OUT);
#endif //CSR_GPIO_BASE



#endif //__LITESDK_CSR_DEFS__H_
