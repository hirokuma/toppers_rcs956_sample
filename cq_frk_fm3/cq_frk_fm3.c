#include "cq_frk_fm3.h"

void cqfrkfm3_hw_init(intptr_t exinf)
{
	*(uint32_t *)FM3_GPIO_PFRF &= ~(0x08);	//gpio
	*(uint32_t *)FM3_GPIO_PZRF |=  (0x08);	//open-drain
	*(uint32_t *)FM3_GPIO_DDRF |=  (0x08);	//output
	CQFM3_LED_OFF();
}
