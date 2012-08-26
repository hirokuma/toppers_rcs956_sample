#include <kernel.h>
#include "kernel_cfg.h"
#include "cq_frk_fm3.h"
#include "cq_frk_fm3_tsk.h"
#include "cq_frk_fm3_loc.h"

static RELTIM		m_Period = 0;
static uint8_t 		m_Timeout = 0;


void cqfrkfm3_hw_init(intptr_t exinf)
{
	*(uint32_t *)FM3_GPIO_PFRF &= ~(0x08);	//gpio
	*(uint32_t *)FM3_GPIO_PZRF |=  (0x08);	//open-drain
	*(uint32_t *)FM3_GPIO_DDRF |=  (0x08);	//output
	CQFRKFM3_GPIO_LED_OFF();
}


void cqfrkfm3_task(intptr_t exinf)
{
	while(m_Timeout == 0) {
		CQFRKFM3_GPIO_LED_ON();
		dly_tsk(m_Period);
		CQFRKFM3_GPIO_LED_OFF();
		dly_tsk(m_Period);
	}
	
	ext_tsk();
}


void cqfrkfm3_alm_start(uint16_t period, RELTIM span)
{
	m_Period = period;
	m_Timeout = 0;

	if(span != 0) {
		/* ëOÇÃÇ™ìÆÇ¢ÇƒÇ¢ÇΩÇÁÅAâÑí∑ */
		sta_alm(CQFRKFM3_ALM, span);
	} else {
		/* é~ÇﬂÇÈ */
		stp_alm(CQFRKFM3_ALM);
	}
}


void cqfrkfm3_alm_hdr(intptr_t exinf)
{
	m_Timeout = 1;
}
