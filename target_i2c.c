/*------------------------------------------------------------------------/
/  MB9BF616/617/618 I2C master control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2012, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

/*
 * TOPPERS/ASP用に改造 : hiro99ma
 */

#include "fm3_mb9bxxx.h"
#include "target_i2c.h"

#include <kernel.h>
#include "kernel_cfg.h"

#define I2C_BPS 	400000		/* I2C bit rate */
#define F_PCLK		SysFrePCLK2	/* Bus clock for the MFS module */

//SDA=SOT1_0
//SCL=SCK1_0
//#define I2C_MFS		1			/* MFS channel to be used as I2C (0-7) */
//#define I2C_LOC		0			/* Pin relocation number (0-2) */

//SDA=SOT1_2 / PF1
//SCL=SCK1_2 / PF2
#define I2C_MFS		1			/* MFS channel to be used as I2C (0-7) */
#define I2C_LOC		2			/* Pin relocation number (0-2) */

////SDA=SOT6_1 / P32
////SCL=SCK6_1 / P31
//#define I2C_MFS		6			/* MFS channel to be used as I2C (0-7) */
//#define I2C_LOC		1			/* Pin relocation number (0-2) */


/* MFS ch0-3 */
#define MFS0_SMR   (*(volatile uint8_t *)0x40038000)
#define MFS0_SCR   (*(volatile uint8_t *)0x40038001)
#define MFS0_IBCR  (*(volatile uint8_t *)0x40038001)
#define MFS0_ESCR  (*(volatile uint8_t *)0x40038004)
#define MFS0_IBSR  (*(volatile uint8_t *)0x40038004)
#define MFS0_SSR   (*(volatile uint8_t *)0x40038005)
#define MFS0_TDR   (*(volatile uint16_t*)0x40038008)
#define MFS0_RDR   (*(volatile uint16_t*)0x40038008)
#define MFS0_BGR   (*(volatile uint16_t*)0x4003800C)
#define MFS0_ISBA  (*(volatile uint8_t *)0x40038010)
#define MFS0_ISMK  (*(volatile uint8_t *)0x40038011)

#define MFS1_SMR   (*(volatile uint8_t *)0x40038100)
#define MFS1_SCR   (*(volatile uint8_t *)0x40038101)
#define MFS1_IBCR  (*(volatile uint8_t *)0x40038101)
#define MFS1_ESCR  (*(volatile uint8_t *)0x40038104)
#define MFS1_IBSR  (*(volatile uint8_t *)0x40038104)
#define MFS1_SSR   (*(volatile uint8_t *)0x40038105)
#define MFS1_TDR   (*(volatile uint16_t*)0x40038108)
#define MFS1_RDR   (*(volatile uint16_t*)0x40038108)
#define MFS1_BGR   (*(volatile uint16_t*)0x4003810C)
#define MFS1_ISBA  (*(volatile uint8_t *)0x40038110)
#define MFS1_ISMK  (*(volatile uint8_t *)0x40038111)

#define MFS2_SMR   (*(volatile uint8_t *)0x40038200)
#define MFS2_SCR   (*(volatile uint8_t *)0x40038201)
#define MFS2_IBCR  (*(volatile uint8_t *)0x40038201)
#define MFS2_ESCR  (*(volatile uint8_t *)0x40038204)
#define MFS2_IBSR  (*(volatile uint8_t *)0x40038204)
#define MFS2_SSR   (*(volatile uint8_t *)0x40038205)
#define MFS2_TDR   (*(volatile uint16_t*)0x40038208)
#define MFS2_RDR   (*(volatile uint16_t*)0x40038208)
#define MFS2_BGR   (*(volatile uint16_t*)0x4003820C)
#define MFS2_ISBA  (*(volatile uint8_t *)0x40038210)
#define MFS2_ISMK  (*(volatile uint8_t *)0x40038211)

#define MFS3_SMR    (*(volatile uint8_t *)0x40038300)
#define MFS3_SCR    (*(volatile uint8_t *)0x40038301)
#define MFS3_IBCR   (*(volatile uint8_t *)0x40038301)
#define MFS3_ESCR   (*(volatile uint8_t *)0x40038304)
#define MFS3_IBSR   (*(volatile uint8_t *)0x40038304)
#define MFS3_SSR    (*(volatile uint8_t *)0x40038305)
#define MFS3_TDR    (*(volatile uint16_t*)0x40038308)
#define MFS3_RDR    (*(volatile uint16_t*)0x40038308)
#define MFS3_BGR    (*(volatile uint16_t*)0x4003830C)
#define MFS3_ISBA   (*(volatile uint8_t *)0x40038310)
#define MFS3_ISMK   (*(volatile uint8_t *)0x40038311)


/* MFS ch4-7 */
#define MFS4_SMR    (*(volatile uint8_t *)0x40038400)
#define MFS4_SCR    (*(volatile uint8_t *)0x40038401)
#define MFS4_IBCR   (*(volatile uint8_t *)0x40038401)
#define MFS4_ESCR   (*(volatile uint8_t *)0x40038404)
#define MFS4_IBSR   (*(volatile uint8_t *)0x40038404)
#define MFS4_SSR    (*(volatile uint8_t *)0x40038405)
#define MFS4_TDR    (*(volatile uint16_t*)0x40038408)
#define MFS4_RDR    (*(volatile uint16_t*)0x40038408)
#define MFS4_BGR    (*(volatile uint16_t*)0x4003840C)
#define MFS4_ISBA   (*(volatile uint8_t *)0x40038410)
#define MFS4_ISMK   (*(volatile uint8_t *)0x40038411)
#define MFS4_FCR    (*(volatile uint16_t*)0x40038414)
#define MFS4_FCR0   (*(volatile uint8_t *)0x40038414)
#define MFS4_FCR1   (*(volatile uint8_t *)0x40038415)
#define MFS4_FBYTE  (*(volatile uint16_t*)0x40038418)
#define MFS4_FBYTE1 (*(volatile uint8_t *)0x40038418)
#define MFS4_FBYTE2 (*(volatile uint8_t *)0x40038419)

#define MFS5_SMR    (*(volatile uint8_t *)0x40038500)
#define MFS5_SCR    (*(volatile uint8_t *)0x40038501)
#define MFS5_IBCR   (*(volatile uint8_t *)0x40038501)
#define MFS5_ESCR   (*(volatile uint8_t *)0x40038504)
#define MFS5_IBSR   (*(volatile uint8_t *)0x40038504)
#define MFS5_SSR    (*(volatile uint8_t *)0x40038505)
#define MFS5_TDR    (*(volatile uint16_t*)0x40038508)
#define MFS5_RDR    (*(volatile uint16_t*)0x40038508)
#define MFS5_BGR    (*(volatile uint16_t*)0x4003850C)
#define MFS5_ISBA   (*(volatile uint8_t *)0x40038510)
#define MFS5_ISMK   (*(volatile uint8_t *)0x40038511)
#define MFS5_FCR    (*(volatile uint16_t*)0x40038514)
#define MFS5_FCR0   (*(volatile uint8_t *)0x40038514)
#define MFS5_FCR1   (*(volatile uint8_t *)0x40038515)
#define MFS5_FBYTE  (*(volatile uint16_t*)0x40038518)
#define MFS5_FBYTE1 (*(volatile uint8_t *)0x40038518)
#define MFS5_FBYTE2 (*(volatile uint8_t *)0x40038519)

#define MFS6_SMR    (*(volatile uint8_t *)0x40038600)
#define MFS6_SCR    (*(volatile uint8_t *)0x40038601)
#define MFS6_IBCR   (*(volatile uint8_t *)0x40038601)
#define MFS6_ESCR   (*(volatile uint8_t *)0x40038604)
#define MFS6_IBSR   (*(volatile uint8_t *)0x40038604)
#define MFS6_SSR    (*(volatile uint8_t *)0x40038605)
#define MFS6_TDR    (*(volatile uint16_t*)0x40038608)
#define MFS6_RDR    (*(volatile uint16_t*)0x40038608)
#define MFS6_BGR    (*(volatile uint16_t*)0x4003860C)
#define MFS6_ISBA   (*(volatile uint8_t *)0x40038610)
#define MFS6_ISMK   (*(volatile uint8_t *)0x40038611)
#define MFS6_FCR    (*(volatile uint16_t*)0x40038614)
#define MFS6_FCR0   (*(volatile uint8_t *)0x40038614)
#define MFS6_FCR1   (*(volatile uint8_t *)0x40038615)
#define MFS6_FBYTE  (*(volatile uint16_t*)0x40038618)
#define MFS6_FBYTE1 (*(volatile uint8_t *)0x40038618)
#define MFS6_FBYTE2 (*(volatile uint8_t *)0x40038619)

#define MFS7_SMR    (*(volatile uint8_t *)0x40038700)
#define MFS7_SCR    (*(volatile uint8_t *)0x40038701)
#define MFS7_IBCR   (*(volatile uint8_t *)0x40038701)
#define MFS7_ESCR   (*(volatile uint8_t *)0x40038704)
#define MFS7_IBSR   (*(volatile uint8_t *)0x40038704)
#define MFS7_SSR    (*(volatile uint8_t *)0x40038705)
#define MFS7_TDR    (*(volatile uint16_t*)0x40038708)
#define MFS7_RDR    (*(volatile uint16_t*)0x40038708)
#define MFS7_BGR    (*(volatile uint16_t*)0x4003870C)
#define MFS7_ISBA   (*(volatile uint8_t *)0x40038710)
#define MFS7_ISMK   (*(volatile uint8_t *)0x40038711)
#define MFS7_FCR    (*(volatile uint16_t*)0x40038714)
#define MFS7_FCR0   (*(volatile uint8_t *)0x40038714)
#define MFS7_FCR1   (*(volatile uint8_t *)0x40038715)
#define MFS7_FBYTE  (*(volatile uint16_t*)0x40038718)
#define MFS7_FBYTE1 (*(volatile uint8_t *)0x40038718)
#define MFS7_FBYTE2 (*(volatile uint8_t *)0x40038719)

#define MFS_I2CDNF  (*(volatile uint16_t*)0x40038800)

/*--------------------------------------------------------------------------

   Module Private Functions and Macros

---------------------------------------------------------------------------*/
#define REG(addr)	(*((volatile uint32_t*)addr))

#if   I2C_MFS == 0
#define MFS_TX_IRQHandler MFS0_TX_IRQHandler
#define MFS_TX_IRQn MFS0_TX_IRQn
#define MFS_IBCR  MFS0_IBCR
#define MFS_SMR   MFS0_SMR
#define MFS_SSR   MFS0_SSR
#define MFS_TDR   MFS0_TDR
#define MFS_RDR   MFS0_RDR
#define MFS_BGR   MFS0_BGR
#define MFS_ISBA  MFS0_ISBA
#define MFS_ISMK  MFS0_ISMK
#if   I2C_LOC == 0
#define ATTACH_MFS() { FM3_GPIO_EPFR07 = (FM3_GPIO_EPFR07 & ~(15 << 6)) | (5 << 6); FM3_GPIO_PFR2 |= 3 << 2; }
#elif I2C_LOC == 1
#define ATTACH_MFS() { FM3_GPIO_EPFR07 = (FM3_GPIO_EPFR07 & ~(15 << 6)) | (10 << 6); FM3_GPIO_PFR1 |= 3 << 5; }
#elif I2C_LOC == 2
#define ATTACH_MFS() { FM3_GPIO_EPFR07 = (FM3_GPIO_EPFR07 & ~(15 << 6)) | (15 << 6); FM3_GPIO_PFRB |= 3 << 5; }
#endif

#elif I2C_MFS == 1
#define MFS_TX_IRQHandler MFS1_TX_IRQHandler
#define MFS_TX_IRQn MFS1_TX_IRQn
#define MFS_IBCR  MFS1_IBCR
#define MFS_IBSR  MFS1_IBSR
#define MFS_SMR   MFS1_SMR
#define MFS_SSR   MFS1_SSR
#define MFS_TDR   MFS1_TDR
#define MFS_RDR   MFS1_RDR
#define MFS_BGR   MFS1_BGR
#define MFS_ISBA  MFS1_ISBA
#define MFS_ISMK  MFS1_ISMK
#if   I2C_LOC == 0
#define ATTACH_MFS() { REG(FM3_GPIO_EPFR07) = (REG(FM3_GPIO_EPFR07) & ~(0x0f << 12)) | (0x05 << 12); REG(FM3_GPIO_PFR5) |= 0x03 << 7; }
#elif I2C_LOC == 1
#define ATTACH_MFS() { REG(FM3_GPIO_EPFR07) = (REG(FM3_GPIO_EPFR07) & ~(0x0f << 12)) | (0x0a << 12); REG(FM3_GPIO_PFR1) |= 0x03 << 21; }
#elif I2C_LOC == 2
#define ATTACH_MFS() { REG(FM3_GPIO_EPFR07) = (REG(FM3_GPIO_EPFR07) & ~(15 << 12)) | (15 << 12); REG(FM3_GPIO_PFRF) |= 3 << 1; }
#endif

#elif I2C_MFS == 2
#define MFS_TX_IRQHandler MFS2_TX_IRQHandler
#define MFS_TX_IRQn MFS2_TX_IRQn
#define MFS_IBCR  MFS2_IBCR
#define MFS_IBSR  MFS2_IBSR
#define MFS_SMR   MFS2_SMR
#define MFS_SSR   MFS2_SSR
#define MFS_TDR   MFS2_TDR
#define MFS_RDR   MFS2_RDR
#define MFS_BGR   MFS2_BGR
#define MFS_ISBA  MFS2_ISBA
#define MFS_ISMK  MFS2_ISMK
#if   I2C_LOC == 0
#define ATTACH_MFS() { FM3_GPIO_EPFR07 = (FM3_GPIO_EPFR07 & ~(15 << 18)) | (5 << 18); FM3_GPIO_PFR7 |= 3 << 3; }
#elif I2C_LOC == 1
#define ATTACH_MFS() { FM3_GPIO_EPFR07 = (FM3_GPIO_EPFR07 & ~(15 << 18)) | (10 << 18); FM3_GPIO_PFR2 |= 3 << 5; }
#elif I2C_LOC == 2
#define ATTACH_MFS() { FM3_GPIO_EPFR07 = (FM3_GPIO_EPFR07 & ~(15 << 18)) | (15 << 18); FM3_GPIO_PFR1 |= 3 << 8; }
#endif

#elif I2C_MFS == 3
#define MFS_TX_IRQHandler MFS3_TX_IRQHandler
#define MFS_TX_IRQn MFS3_TX_IRQn
#define MFS_IBCR  MFS3_IBCR
#define MFS_IBSR  MFS3_IBSR
#define MFS_SMR   MFS3_SMR
#define MFS_SSR   MFS3_SSR
#define MFS_TDR   MFS3_TDR
#define MFS_RDR   MFS3_RDR
#define MFS_BGR   MFS3_BGR
#define MFS_ISBA  MFS3_ISBA
#define MFS_ISMK  MFS3_ISMK
#if   I2C_LOC == 0
#define ATTACH_MFS() { FM3_GPIO_EPFR07 = (FM3_GPIO_EPFR07 & ~(15 << 24)) | (5 << 24); FM3_GPIO_PFR7 |= 3 << 6; }
#elif I2C_LOC == 1
#define ATTACH_MFS() { FM3_GPIO_EPFR07 = (FM3_GPIO_EPFR07 & ~(15 << 24)) | (10 << 24); FM3_GPIO_PFR5 |= 3 << 1; }
#elif I2C_LOC == 2
#define ATTACH_MFS() { FM3_GPIO_EPFR07 = (FM3_GPIO_EPFR07 & ~(15 << 24)) | (15 << 24); FM3_GPIO_PFR4 |= 3 << 9; }
#endif

#elif I2C_MFS == 4
#define MFS_TX_IRQHandler MFS4_TX_IRQHandler
#define MFS_TX_IRQn MFS4_TX_IRQn
#define MFS_IBCR  MFS4_IBCR
#define MFS_IBSR  MFS4_IBSR
#define MFS_SMR   MFS4_SMR
#define MFS_SSR   MFS4_SSR
#define MFS_TDR   MFS4_TDR
#define MFS_RDR   MFS4_RDR
#define MFS_BGR   MFS4_BGR
#define MFS_ISBA  MFS4_ISBA
#define MFS_ISMK  MFS4_ISMK
#if   I2C_LOC == 0
#define ATTACH_MFS() { FM3_GPIO_EPFR08 = (FM3_GPIO_EPFR08 & ~(15 << 6)) | (5 << 6); FM3_GPIO_PFRD |= 3 << 0; }
#elif I2C_LOC == 1
#define ATTACH_MFS() { FM3_GPIO_EPFR08 = (FM3_GPIO_EPFR08 & ~(15 << 6)) | (10 << 6); FM3_GPIO_PFR1 |= 3 << 11; }
#elif I2C_LOC == 2
#define ATTACH_MFS() { FM3_GPIO_EPFR08 = (FM3_GPIO_EPFR08 & ~(15 << 6)) | (15 << 6); FM3_GPIO_PFR0 |= 3 << 6; }
#endif

#elif I2C_MFS == 5
#define MFS_TX_IRQHandler MFS5_TX_IRQHandler
#define MFS_TX_IRQn MFS5_TX_IRQn
#define MFS_IBCR  MFS5_IBCR
#define MFS_IBSR  MFS5_IBSR
#define MFS_SMR   MFS5_SMR
#define MFS_SSR   MFS5_SSR
#define MFS_TDR   MFS5_TDR
#define MFS_RDR   MFS5_RDR
#define MFS_BGR   MFS5_BGR
#define MFS_ISBA  MFS5_ISBA
#define MFS_ISMK  MFS5_ISMK
#if   I2C_LOC == 0
#define ATTACH_MFS() { FM3_GPIO_EPFR08 = (FM3_GPIO_EPFR08 & ~(15 << 12)) | (5 << 12); FM3_GPIO_PFR6 |= 3 << 1; }
#elif I2C_LOC == 1
#define ATTACH_MFS() { FM3_GPIO_EPFR08 = (FM3_GPIO_EPFR08 & ~(15 << 12)) | (10 << 12); FM3_GPIO_PFR9 |= 3 << 3; }
#elif I2C_LOC == 2
#define ATTACH_MFS() { FM3_GPIO_EPFR08 = (FM3_GPIO_EPFR08 & ~(15 << 12)) | (15 << 12); FM3_GPIO_PFR3 |= 3 << 7; }
#endif

#elif I2C_MFS == 6
#define MFS_TX_IRQHandler MFS6_TX_IRQHandler
#define MFS_TX_IRQn MFS6_TX_IRQn
#define MFS_IBCR  MFS6_IBCR
#define MFS_IBSR  MFS6_IBSR
#define MFS_SMR   MFS6_SMR
#define MFS_SSR   MFS6_SSR
#define MFS_TDR   MFS6_TDR
#define MFS_RDR   MFS6_RDR
#define MFS_BGR   MFS6_BGR
#define MFS_ISBA  MFS6_ISBA
#define MFS_ISMK  MFS6_ISMK
#if   I2C_LOC == 0
#define ATTACH_MFS() { FM3_GPIO_EPFR08 = (FM3_GPIO_EPFR08 & ~(15 << 18)) | (5 << 18); FM3_GPIO_PFR5 |= 3 << 4; }
#elif I2C_LOC == 1
#define ATTACH_MFS() {	\
		REG(FM3_GPIO_EPFR08) = (REG(FM3_GPIO_EPFR08) & ~(15 << 18)) | (10 << 18);	\
		REG(FM3_GPIO_PFR3) |= 0x06;	\
		REG(FM3_GPIO_PZR3) |= 0x06;	\
	}
#elif I2C_LOC == 2
#define ATTACH_MFS() { FM3_GPIO_EPFR08 = (FM3_GPIO_EPFR08 & ~(15 << 18)) | (15 << 18); FM3_GPIO_PFRF |= 3 << 4; }
#endif

#elif I2C_MFS == 7
#define MFS_TX_IRQHandler MFS7_TX_IRQHandler
#define MFS_TX_IRQn MFS7_TX_IRQn
#define MFS_IBCR  MFS7_IBCR
#define MFS_IBSR  MFS7_IBSR
#define MFS_SMR   MFS7_SMR
#define MFS_SSR   MFS7_SSR
#define MFS_TDR   MFS7_TDR
#define MFS_RDR   MFS7_RDR
#define MFS_BGR   MFS7_BGR
#define MFS_ISBA  MFS7_ISBA
#define MFS_ISMK  MFS7_ISMK
#if   I2C_LOC == 0
#define ATTACH_MFS() { FM3_GPIO_EPFR08 = (FM3_GPIO_EPFR08 & ~(15 << 24)) | (5 << 24); FM3_GPIO_PFR5 |= 3 << 10; }
#elif I2C_LOC == 1
#define ATTACH_MFS() { FM3_GPIO_EPFR08 = (FM3_GPIO_EPFR08 & ~(15 << 24)) | (10 << 24); FM3_GPIO_PFR4 |= 3 << 12; }
#elif I2C_LOC == 2
#define ATTACH_MFS() { FM3_GPIO_EPFR08 = (FM3_GPIO_EPFR08 & ~(15 << 24)) | (15 << 24); FM3_GPIO_PFRB |= 3 << 1; }
#endif

#endif


#define TSKID_INVALID	((ID)-1)


enum { S_SLA_W, S_CMD_W, S_DAT_W, S_SLA_R, S_DAT_R };	/* State of transaction m_pCtrl0->phase */


static volatile I2CCTRL *m_pCtrl0 = 0;	/* Current I2C control structure */
static ID m_TskId = TSKID_INVALID;


/**
 * @brief	ハードウェア初期化
 * 
 * マルチファンクションシリアルの初期化.
 * ATT_INIで呼び出される.
 */
void i2c_hw_init(intptr_t exinf)
{
	MFS_ISMK = 0;		/* Disable I2C function */
	
	/* Bus reset sequence may be required here to make all slaves inactive */
	MFS_IBCR = 0x00;

	/* Initialize MFS in I2C mode */
	MFS_SMR = 0x80;		/* Set I2C mode */
	MFS_SSR = 0;
	MFS_BGR = F_PCLK / I2C_BPS - 1;
	MFS_ISBA = 0;		/* No address detection */

#if (60000000 < SysFrePCLK1) && (SysFrePCLK1 <= 80000000)
	/* noise filter */
//	MFS_I2CDNF |= (0x02) << 2;
#endif

	/* Attach MFS module to I/O pads */
	ATTACH_MFS();
}


/**
 * @brief	タスク初期化
 *
 * TOPPERSのタスクが動作してからでないと動作しないAPIを使っているため.
 */
void i2c_task_init(void)
{
	MFS_ISMK = 0xFF;	/* Enable I2C function */

	/* Enable MFS status interrupt */
	ena_int(INTNO_I2C);
}


/**
 * @brief	I2C処理開始
 * 
 * I2C処理を開始する.
 *
 * @param[in,out]	pCtrl	処理内容
 * @return			TOPPERSエラーコード
 */
ER i2c_start(I2CCTRL *pCtrl)
{
	ER ercd;

	if (m_pCtrl0) {
		/* Reject if an I2C transaction is in progress */
		return E_SYS;
	}
	if((MFS_SSR & 0x02) == 0x00) {
		/* TDRE=0 */
		return E_SYS;
	}

	/* 処理完了待ち用 */
	ercd = get_tid(&m_TskId);
	if(ercd != E_OK) {
		return ercd;
	}

	m_pCtrl0 = pCtrl;				/* Register the I2C control strucrure as current transaction */
	pCtrl->status = I2C_BUSY;	/* An I2C transaction is in progress */
	MFS_TDR = pCtrl->sla << 1;	/* Data to be sent as 1st byte = SLA+W */
								/* TDR write before MSS set */
	pCtrl->phase = S_SLA_W;
	MFS_IBCR = 0x85;			/* Wait for bus-free and then generate start condition */

	/* 処理完了待ち */
	ercd = slp_tsk();
	m_TskId = TSKID_INVALID;
	if(ercd == E_OK) {
		if(pCtrl->status != I2C_SUCCEEDED) {
			ercd = E_SYS;
		}
	}
	return ercd;
}


/**
 * I2Cを使用禁止にする。
 * 再開するにはi2c_task_init()を呼び出すこと。
 */
void i2c_abort(void)
{
	MFS_IBCR = 0;		/* Disable I2C function */
	MFS_ISMK = 0;		/* Disable I2C function */
	m_pCtrl0 = 0;		/* Discard I2C control structure */

	/* Disable MFS status interrupt */
	dis_int(INTNO_I2C);
	
	/* タスク起こし */
	if(m_TskId != TSKID_INVALID) {
		(void)iwup_tsk(m_TskId);
	}
}


/**
 * I2C割り込みハンドラ
 */
void i2c_isr(intptr_t exinf)
{
	volatile I2CCTRL *pCtrl;
	uint8_t ibcr, ibsr, eot = 0;
	uint16_t n;

	pCtrl = m_pCtrl0;
	if (!pCtrl) {		/* Spurious Interrupt */
		MFS_IBCR = 0;	/* Disable I2C function */
		MFS_ISMK = 0;	/* Disable I2C function */
		dis_int(INTNO_I2C);
		return;
	}

	ibcr = MFS_IBCR;
	ibsr = MFS_IBSR;
	if (!(ibcr & 0x80) || (ibsr & 0x20)) {	/* Bus error, Arbitration lost or Reserved address */
		eot = I2C_ERROR;

	} else {
		switch (pCtrl->phase) {
		case S_SLA_W:	/* S+SLA+W sent */
			if (ibsr & 0x40) {	/* ACK not received (slave not responded) */
				n = pCtrl->retry;
				if (n) {
					pCtrl->retry = n - 1;
					MFS_TDR = pCtrl->sla << 1;	/* Send Sr+SLA+W */
					MFS_IBCR = 0xC5;
				} else {
					eot = I2C_TIMEOUT;
				}
			} else {			/* ACK received (slave responded) */
				pCtrl->ncmd--;
				MFS_TDR = pCtrl->cmd[0];	/* Sent command byte */
				pCtrl->icmd = 1;
				MFS_IBCR = 0x84;
				pCtrl->phase = S_CMD_W;
			}
			break;

		case S_CMD_W:	/* Command byte sent */
			if (ibsr & 0x40) {	/* ACK not received */
				eot = I2C_ABORTED;
			} else {			/* ACK received */
				n = pCtrl->ncmd;
				if (n) {	/* There is command byte to be sent */
					pCtrl->ncmd = n - 1;
					MFS_TDR = pCtrl->cmd[pCtrl->icmd++];	/* Send next command byte */
					MFS_IBCR = 0x84;
				} else {	/* All command byte has been sent */
					n = pCtrl->ndata;
					if (n) {
						if (pCtrl->dir == I2C_WRITE) {	/* There is data to be written */
							pCtrl->ndata = n - 1;
							pCtrl->phase = S_DAT_W;
							MFS_TDR = *pCtrl->data++;	/* Send data */
							MFS_IBCR = 0x84;
							pCtrl->phase = S_DAT_W;
						} else {						/* There is data to be read */
							MFS_TDR = (pCtrl->sla << 1) + 1;	/* Send Sr+SLA+R */
							MFS_IBCR = 0xC5;
							pCtrl->phase = S_SLA_R;
						}
					} else {	/* No data to be written/read */
						eot = I2C_SUCCEEDED;
					}
				}
			}
			break;

		case S_DAT_W:	/* Data sent */
			if (ibsr & 0x40) {	/* ACK not received */
				eot = I2C_ABORTED;
			} else {			/* ACK received */
				n = pCtrl->ndata;
				if (n) {		/* There is any data to be written */
					pCtrl->ndata = n - 1;
					MFS_TDR = *pCtrl->data++;	/* Send data */
					MFS_IBCR = 0x84;
				} else {		/* All data has been sent */
					eot = I2C_SUCCEEDED;
				}
			}
			break;

		case S_SLA_R:	/* Sr+SLA+R sent */
			if (ibsr & 0x40) {	/* ACK not received (slave not responded) */
				eot = I2C_ABORTED;
			} else {			/* ACK received (slave responded) */
				MFS_IBCR = (pCtrl->ndata > 1) ? 0xA4 : 0x84;
				pCtrl->phase = S_DAT_R;
			}
			break;

		case S_DAT_R:	/* Data received */
			*pCtrl->data++ = MFS_RDR;	/* Store read data */
			n = pCtrl->ndata - 1;
			if (n) {	/* There is any data to be read */
				pCtrl->ndata = n;
				MFS_IBCR = (n > 1) ? 0xA4 : 0x84;
			} else {	/* All data has been read */
				eot = I2C_SUCCEEDED;
			}
			break;

		default:
			eot = I2C_ERROR;
		}
	}

	if (eot) {	/* End of I2C transaction? */
		MFS_IBCR = 0x00;
		m_pCtrl0 = 0;			/* Release I2C control structure */
		pCtrl->status = eot;	/* Set result code */
		if (pCtrl->eotfunc) {
			/* Notify EOT if call-back function is specified */
			pCtrl->eotfunc(eot);
		}

		/* タスク起こし */
		if(m_TskId != TSKID_INVALID) {
			(void)iwup_tsk(m_TskId);
		}
	}
}
