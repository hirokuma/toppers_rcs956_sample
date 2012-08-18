#include <kernel.h>
#include "kernel_cfg.h"
#include "fm3_mb9bxxx.h"

#include "rcs620s_sample.h"
#include "st7032i.h"

#include "HkNfcRw.h"

#define LED_ON()	{ *(uint32_t *)FM3_GPIO_PDORF &= ~(0x08); }
#define LED_OFF()	{ *(uint32_t *)FM3_GPIO_PDORF |=  (0x08); }


void main_task(intptr_t exinf)
{
	HkNfcType type;

	*(uint32_t *)FM3_GPIO_PFRF &= ~(0x08);	//gpio
	*(uint32_t *)FM3_GPIO_PZRF |=  (0x08);	//open-drain
	*(uint32_t *)FM3_GPIO_DDRF |=  (0x08);	//output
	LED_OFF();

	st7032i_task_init();

	bool b = HkNfcRw_Open();
	if(!b) {
		LED_ON();
		ext_tsk();
	}

	st7032i_write_string("Please card.");

	while(1) {
		type = HkNfcRw_Detect(true, true, true);
		st7032i_move_pos(1, 1);
		switch(type) {
		case HKNFCTYPE_A:
			st7032i_write_string("NFC-A card !");
			break;
		case HKNFCTYPE_B:
			st7032i_write_string("NFC-B card !");
			break;
		case HKNFCTYPE_F:
			st7032i_write_string("NFC-F card !");
			break;
		default:
			st7032i_write_string("            ");
		}
		dly_tsk(500);
	} 

	HkNfcRw_Close();
	ext_tsk();
}
