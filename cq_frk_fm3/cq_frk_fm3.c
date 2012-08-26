#include <kernel.h>
#include "kernel_cfg.h"
#include "cq_frk_fm3.h"
#include "cq_frk_fm3_tsk.h"
#include "cq_frk_fm3_loc.h"


void cq_frk_fm3_led_ctrl(CqFrkFm3Led ctrl, uint16_t period, RELTIM span)
{
	ER ercd;
	T_RTSK rtsk;
	
	ercd = ref_tsk(CQFRKFM3_TASK, &rtsk);
	if(ercd != E_OK) {
		return;
	}
	
	
	switch(ctrl) {
	case CQFRKFM3_LED_OFF:
		cqfrkfm3_alm_hdr(0);
		if(rtsk.tskstat == TTS_RUN) {
			ter_tsk(CQFRKFM3_TASK);
		}
		CQFRKFM3_GPIO_LED_OFF();
		break;

	case CQFRKFM3_LED_ON:
		cqfrkfm3_alm_hdr(0);
		if(rtsk.tskstat == TTS_RUN) {
			ter_tsk(CQFRKFM3_TASK);
		}
		CQFRKFM3_GPIO_LED_ON();
		break;

	case CQFRKFM3_LED_BLINK:
		cqfrkfm3_alm_start(period, span);
		if(rtsk.tskstat != TTS_RUN) {
			act_tsk(CQFRKFM3_TASK);
		}
		break;
	}
}
