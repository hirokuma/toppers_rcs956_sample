/*
 * Copyright (c) 2012-2012, hiro99ma(uenokuma@gmail.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *         this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *         this list of conditions and the following disclaimer
 *         in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */
#include <kernel.h>
#include "kernel_cfg.h"

#include "hk_misc.h"
#include "hk_toppers.h"

#include <stdbool.h>

#define TM_NONE		(0)
#define TM_START	(1)
#define TM_TIMEOUT	(2)

static uint8_t m_Timeout = TM_NONE;

/**
 *  @brief	ミリ秒スリープ
 *
 * @param	msec	待ち時間[msec]
 */
void hk_msleep(uint16_t msec)
{
	(void)dly_tsk(msec);
}


/**
 * タイマ開始
 *
 * @param[in]	tmval	タイムアウト時間[msec]
 */
void hk_start_timer(uint16_t tmval)
{
	if(m_Timeout == TM_START) {
		(void)stp_alm(ALM_HKMISC);
	}
	(void)sta_alm(ALM_HKMISC, tmval);
	
	m_Timeout = TM_START;
}


/**
 *  タイムアウト監視
 * 
 * @retval	true	タイムアウト発生
 */
bool hk_is_timeout(void)
{
	return (m_Timeout == TM_TIMEOUT);
}


/**
 * メモリ比較
 *
 * @param[in]	s1		比較元1
 * @param[in]	s2		比較元2
 * @param[in]	n		比較サイズ
 * @retval		0			一致
 * @retval		上記以外	不一致
 */
int hk_memcmp(const void *s1, const void *s2, uint16_t n)
{
	const uint8_t* p1 = (const uint8_t *)s1;
	const uint8_t* p2 = (const uint8_t *)s2;

	while(n--) {
		if(*p1++ != *p2++) {
			return 1;
		}
	}
	
	return 0;
}


/**
 * メモリコピー
 *
 * @param[out]	dst		コピー先
 * @param[in]	src		コピー元
 * @param[in]	len		コピーサイズ
 * @return				コピー先アドレス(dst)
 */
void* hk_memcpy(void* dst, const void* src, uint16_t len)
{
	uint8_t * pdst = (uint8_t *)dst;
	const uint8_t* psrc = (const uint8_t *)src;
	
	while(len--) {
		*pdst++ = *psrc++;
	}
	
	return dst;
}


/**
 * メモリ書き込み
 *
 * @param[out]	dst		書き込み先
 * @param[in]	dat		書き込みデータ
 * @param[in]	len		書き込みサイズ
 * @return				書き込み先アドレス(dst)
 */
void* hk_memset(void* dst, uint8_t dat, uint16_t len)
{
	uint8_t * pdst = (uint8_t *)dst;
	
	while(len--) {
		*pdst++ = dat;
	}
	
	return dst;
}


/////////////////////////////////////////////////////////////

/**
 * TOPPERSからのタイムアウトコールバック
 */
void hk_alm_misc_timeout(intptr_t exinf)
{
	m_Timeout = TM_TIMEOUT;
}
