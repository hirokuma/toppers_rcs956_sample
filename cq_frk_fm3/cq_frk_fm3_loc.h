#ifndef CQ_FRK_FM3_LOC_H
#define CQ_FRK_FM3_LOC_H

#include "fm3_mb9bxxx.h"

#define CQFRKFM3_GPIO_LED_ON()	{ *(uint32_t *)FM3_GPIO_PDORF &= ~(0x08); }
#define CQFRKFM3_GPIO_LED_OFF()	{ *(uint32_t *)FM3_GPIO_PDORF |=  (0x08); }

void cqfrkfm3_alm_start(uint16_t period, RELTIM span);

#endif /* CQ_FRK_FM3_LOC_H */
