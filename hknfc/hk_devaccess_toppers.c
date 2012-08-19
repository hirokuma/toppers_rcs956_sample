#include "hk_devaccess.h"

#include "fm3_mb9bxxx.h"
#include "kernel_cfg.h"

#include "hk_toppers.h"

#include <syssvc/serial.h>

/**
* @brief		RC-S620/S用のI/F
*/
#define UART_NUM	(1)
static bool m_bOpening = false;
static uint16_t m_Timeout = 0;
static ID m_TskId;

/**
 * ポートオープン
 *
 * @retval	true	オープン成功
 */
bool hk_nfcrw_open()
{
	ER ret;
	
	if(m_bOpening) {
		return true;
	}

	//シリアルオープン
	ret = serial_opn_por(UART_NUM);
	if(ret == E_OK) {
		ret = serial_ctl_por(1, IOCTL_NULL);
	}
	m_bOpening = (ret == E_OK);

	return m_bOpening;
}


/**
 * ポートクローズ
 */
void hk_nfcrw_close()
{
	serial_cls_por(UART_NUM);
	m_bOpening = false;
}


/**
 * ポート送信
 *
 * @param[in]	data		送信データ
 * @param[in]	len			dataの長さ
 * @return					送信したサイズ
 */
uint16_t hk_nfcrw_write(const uint8_t* data, uint16_t len)
{
	ER_UINT ret;
	T_SERIAL_RPOR pror;

	ret = serial_wri_dat(UART_NUM, data, len);
	
#if 0
	//同期で送信
	do {
		serial_ref_por(UART_NUM, &pror);
	} while(pror.wricnt != 0);
#endif

	return (ret == len) ? len : 0;
}

/**
 * 受信
 *
 * @param[out]	data		受信バッファ
 * @param[in]	len			受信サイズ
 *
 * @return					受信したサイズ
 *
 * @attention	- len分のデータを受信するか失敗するまで処理がブロックされる。
 */
uint16_t hk_nfcrw_read(uint8_t* data, uint16_t len)
{
	ER_UINT ret;

	if(m_Timeout) {
		/* タイムアウト計測開始 */
		(void)sta_alm(ALM_HKREAD, m_Timeout);
		(void)get_tid(&m_TskId);
	}

	ret = serial_rea_dat(UART_NUM, data, len);

	return (ret == len) ? len : 0;
}


/**
 * ポート受信タイムアウト時間設定
 * 
 * タイムアウト処理が可能な場合、受信タイムアウト時間を設定する。
 * タイムアウトがない場合は、何も処理しないし、#hk_nfcrw_read()にも影響はない。
 *
 * @param[in]	msec		タイムアウト時間(ミリ秒)。0のときはタイムアウト解除。
 */
void hk_nfcrw_read_timeout(uint16_t msec)
{
	m_Timeout = msec;

	if(msec == 0) {
		/* タイムアウト解除 */
		(void)stp_alm(ALM_HKREAD);
	}
}


/**
 * TOPPERSからのタイムアウトコールバック
 */
void hk_alm_read_timeout(intptr_t exinf)
{
	(void)irel_wai(m_TskId);
}
