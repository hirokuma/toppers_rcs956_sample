#ifndef TARGET_I2C_H
#define TARGET_I2C_H

#define INTNO_I2C	IRQ_VECTOR_MFS1TX
//#define INTPRI_I2C	-1			/* 割込み優先度 */
//#define INTATR_I2C	TA_NULL		/* 割込み属性 */

#ifndef TOPPERS_MACRO_ONLY

#include <kernel.h>


enum { I2C_WRITE, I2C_READ };	/* Data directiron flag (I2CCTRL.dir) */

/* I2C transaction work area */
typedef struct {
	uint8_t status;			/* Status of I2C transaction (0:busy, !0:terminated) */
	uint8_t phase;			/* Current state of the transaction */
	uint8_t dir;			/* Read or write for secondary data (I2C_WRITE, I2C_READ) */
	uint8_t sla;			/* 7-bit slave address (0..127) */
	uint16_t retry;			/* Number of retries of SLA+W when slave not responded (0..) */
	uint8_t ncmd;			/* Number of bytes of cmd[] (1..4) */
	uint8_t icmd;			/* Index of command byte */
	uint8_t cmd[4];			/* Command bytes follows SLA+W */
	uint16_t ndata;			/* Number of bytes of secondary data (0:no secondary data transfer) */
	uint8_t *data;			/* Pointer to secondary data buffer */
	void (*eotfunc)(int);	/* Call-back function to notify end of transaction (0:not used) */
} I2CCTRL;


/* Status code (I2CCTRL.status) */
enum {
	I2C_BUSY = 0,	/* An I2C transaction is in progress. Must not discard this structure */
	I2C_SUCCEEDED,	/* Transaction succeeded */
	I2C_TIMEOUT,	/* Failed due to slave not responded to addressing */
	I2C_ABORTED,	/* Failed due to slave not responded to sent data */
	I2C_ERROR		/* Failed due to bus error, arbitration failed or unknown error */
};

void i2c_hw_init(intptr_t exinf);
void i2c_task_init(void);

ER i2c_start(I2CCTRL *ctrl);
void i2c_abort(void);

void i2c_isr(intptr_t exinf);

#endif	/* TOPPERS_MACRO_ONLY */



#endif /* TARGET_I2C_H */
