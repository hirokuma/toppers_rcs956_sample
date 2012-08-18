#include <kernel.h>
#include "kernel_cfg.h"
#include "fm3_mb9bxxx.h"

#include "cq_frk_fm3.h"
#include "rcs620s_sample.h"
#include "st7032i.h"

#include "HkNfcRw.h"


void main_task(intptr_t exinf)
{
	HkNfcType type;

	st7032i_task_init();

	bool b = HkNfcRw_Open();
	if(!b) {
		CQFM3_LED_ON();
		ext_tsk();
	}

	st7032i_write_string("Please card.");

	while(1) {
		type = HkNfcRw_Detect(true, true, true);
		st7032i_move_pos(1, 1);
		switch(type) {
		case HKNFCTYPE_A:
			CQFM3_LED_ON();
			st7032i_write_string("NFC-A card !");
			break;
		case HKNFCTYPE_B:
			CQFM3_LED_ON();
			st7032i_write_string("NFC-B card !");
			break;
		case HKNFCTYPE_F:
			CQFM3_LED_ON();
			st7032i_write_string("NFC-F card !");
			break;
		default:
			CQFM3_LED_OFF();
			st7032i_write_string("            ");
		}
		HkNfcRw_RfOff();
		dly_tsk(1000);
	} 

	HkNfcRw_Close();
	ext_tsk();
}
