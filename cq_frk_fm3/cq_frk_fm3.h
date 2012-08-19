#ifndef CQ_FRK_FM3_H
#define CQ_FRK_FM3_H

#include <kernel.h>
#include "fm3_mb9bxxx.h"

#define CQFM3_LED_ON()	{ *(uint32_t *)FM3_GPIO_PDORF &= ~(0x08); }
#define CQFM3_LED_OFF()	{ *(uint32_t *)FM3_GPIO_PDORF |=  (0x08); }

void cqfrkfm3_hw_init(intptr_t exinf);

#endif /* CQ_FRK_FM3_H */
