#ifndef CQ_FRK_FM3_H
#define CQ_FRK_FM3_H

#include <stdint.h>

typedef uint8_t	CqFrkFm3Led;

#define CQFRKFM3_LED_OFF		((CqFrkFm3Led)0)
#define CQFRKFM3_LED_ON			((CqFrkFm3Led)1)
#define CQFRKFM3_LED_BLINK		((CqFrkFm3Led)2)

void cq_frk_fm3_led_ctrl(CqFrkFm3Led ctrl, uint16_t period, RELTIM span);

#endif /* CQ_FRK_FM3_H */
