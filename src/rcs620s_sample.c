#include <kernel.h>
#include "kernel_cfg.h"
#include "fm3_mb9bxxx.h"

#include "cq_frk_fm3.h"
#include "rcs620s_sample.h"
#include "st7032i.h"

#include "HkNfcRw.h"
#include "HkNfcA.h"
#include "HkNfcF.h"
#include "HkNfcNdef.h"
#include "HkNfcSnep.h"

static void _exit(const char *pStr);


void main_task(intptr_t exinf)
{
	HkNfcType type;
	bool bLoop = true;

	st7032i_task_init();

	bool b = HkNfcRw_Open();
	if(!b) {
		CQFM3_LED_ON();
		ext_tsk();
	}

	//最初は(0, 0)にカーソルがある
	st7032i_write_string("Please card.");

	while(bLoop) {
		const char *pStr;
		
		type = HkNfcRw_Detect(true, true, true);
		HkNfcRw_RfOff();
		st7032i_move_pos(1, 1);
		switch(type) {
		case HKNFCTYPE_A:
			CQFM3_LED_ON();
			pStr = "NFC-A";
			{
				HkNfcASelRes selres = HkNfcA_GetSelRes();
				if(HKNFCA_IS_SELRES_TPE(selres)) {
					bLoop = false;
				} else {
					TOPPERS_reset();
				}
			}
			break;
		case HKNFCTYPE_B:
			CQFM3_LED_ON();
			pStr = "NFC-B";
			break;
		case HKNFCTYPE_F:
			CQFM3_LED_ON();
			pStr = "NFC-F";
			{
				uint8_t idm[NFCID2_LEN];
				if(HkNfcRw_GetNfcId(idm) != 0) {
					if(HKNFCF_IS_NFCID_TPE(idm)) {
						bLoop = false;
					}
				}
			}
			break;
		default:
			CQFM3_LED_OFF();
			pStr = "     ";
			break;
		}
		st7032i_write_string(pStr);
		dly_tsk(1000);
	}

	st7032i_write_string(":TPE");
	st7032i_move_pos(0, 0);
	st7032i_write_string("            ");

	HkNfcNdefMsg msg;
	b = HkNfcNdef_CreateText(&msg, "hiro99ma", 8, LANGCODE_EN);
	if(!b) {
		_exit("fail:NDEF create");
	}

	b = HkNfcSnep_PutStart(HKNFCSNEP_MD_INITIATOR, &msg);
	if(!b) {
		_exit("fail:PutStart");
	}
	
	while(HkNfcSnep_Poll()) {
		;
	}
	
	if(HkNfcSnep_GetResult() != HKNFCSNEP_SUCCESS) {
		_exit("fail:Put");
	}

	CQFM3_LED_OFF();
	st7032i_move_pos(0, 0);
	st7032i_write_string("SNEP success!");

	HkNfcRw_Close();
	ext_tsk();
}


static void _exit(const char *pStr)
{
	st7032i_move_pos(0, 0);
	st7032i_write_string(pStr);

	HkNfcRw_Close();
	ext_tsk();
}
