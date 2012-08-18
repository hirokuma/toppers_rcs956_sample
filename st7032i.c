/**
 * @file	st7032i_fm3.c
 * @brief	ST7032iアクセス実装(for FM3)
 * 
 * Resetポートとして、P39を使っている。
 * 同期APIとして実装しているので、完了までタスクが待たされる。
 * TOPPERSのタイマ(ミリ秒)を使っているので、全体的に無駄な時間が多い。
 * 
 * http://strawberry-linux.com/catalog/items?code=27001
 */
#include "st7032i.h"
#include "target_i2c.h"
#include "fm3_mb9bxxx.h"

#define REG(addr)	(*((volatile uint32_t*)addr))

#define ST7032_CMD_COMMAND		((uint8_t)0x00)
#define ST7032_CMD_DATA			((uint8_t)0x40)

/* Extension Mode(RS=0) */
#define ST7032_DATA_CLEARDISPLAY	((uint8_t)0x01)
#define ST7032_DATA_RETURNHOME		((uint8_t)0x02)
#define ST7032_DATA_ENTRYMODESET(id,s)	((uint8_t)(0x04|((id)<<1)|(s)))
#define ST7032_DATA_DISPLAY(d,c,b)	((uint8_t)(0x08|((d)<<2)|((c)<<1)|(b)))
#define ST7032_DATA_FUNCTIONSET(is)	((uint8_t)(0x38|((0)<<2)|(is)))
#define ST7032_DATA_SETDDRAMADDR(addr)	((uint8_t)(0x80|(addr)))

#define ST7032_DATA_ENTRYMODESET_NORMAL	ST7032_DATA_ENTRYMODESET(1,0)
#define ST7032_DATA_DISPLAY_ON		ST7032_DATA_DISPLAY(1,0,0)
#define ST7032_DATA_DISPLAY_OFF		ST7032_DATA_DISPLAY(0,0,0)
#define ST7032_DATA_FUNCTIONSET_IS0	ST7032_DATA_FUNCTIONSET(0)
#define ST7032_DATA_FUNCTIONSET_IS1	ST7032_DATA_FUNCTIONSET(1)

#define VAL_CONTRAST	(0x28)

#define FLG_CLEAR		(1 << 0)
#define FLG_MOVE		(1 << 1)
#define FLG_DISP		(1 << 2)
#define FLG_ABORT		(1 << 3)


static ER _write_lcd(uint8_t cmd, uint8_t data, uint16_t wait);

static I2CCTRL m_I2c;


/**
 * ハードウェア初期化
 * 
 * タスク起動前に呼び出される想定
 */
void st7032i_hw_init(intptr_t exinf)
{
	//FM3 GPIO設定
	//LCD Reset
	REG(FM3_GPIO_PFR3) &= ~(0x200);		//gpio
	REG(FM3_GPIO_DDR3) |=  (0x200);		//output

	//リセット(Active Low)
	//解除はst7032i_task_init()
	REG(FM3_GPIO_PDOR3) &= ~(0x200);
}


/**
 * タスク初期化
 * 
 * TOPPERSのタスクが動作してからでないと動作しないAPIを使っているため。
 * 計測していないが、結構時間がかかる。
 */
void st7032i_task_init(void)
{
	i2c_task_init();

	/* 変更しないパラメータ */
	m_I2c.sla = 0x3e;						/* 7-bit slave address */
	m_I2c.eotfunc = 0;						/* コールバック関数なし */
	m_I2c.dir = I2C_WRITE;

	//リセット解除
	dly_tsk(2);				/* なんとなく2ms */
	REG(FM3_GPIO_PDOR3) |=  (0x200);

	dly_tsk(40);
	
	//Function Set(IS=0)
	_write_lcd(ST7032_CMD_COMMAND, ST7032_DATA_FUNCTIONSET_IS0, 1);

	//Function Set(IS=1)
	_write_lcd(ST7032_CMD_COMMAND, ST7032_DATA_FUNCTIONSET_IS1, 1);
	
	/*
	 * (Instruction table 1)internal OSC frequency
	 * 		BS=1(1/4bias)
	 * 		F2-0=0
	 */
	_write_lcd(ST7032_CMD_COMMAND, 0x14, 1);
	
	/*
	 * (Instruction table 1)コントラスト調整など
	 * 		Constrast Set:
	 * 			C3-0=8
	 * 		Power/ICON/Contrast Set
	 * 			Ion=1
	 * 			Bon=1
	 * 			C5-4=2
	 */
	_write_lcd(ST7032_CMD_COMMAND, (0x70 | (VAL_CONTRAST & 0x0f)), 1);
	_write_lcd(ST7032_CMD_COMMAND, (0x5c | (VAL_CONTRAST >> 4)), 1);
	
	/*
	 * (Instruction table 1)Follower control
	 *		Fon=1
	 * 		Rab=4
	 */
	_write_lcd(ST7032_CMD_COMMAND, 0x6c, 300);

	//Function Set(IS=0)
	_write_lcd(ST7032_CMD_COMMAND, ST7032_DATA_FUNCTIONSET_IS0, 1);

	//display on
	_write_lcd(ST7032_CMD_COMMAND, ST7032_DATA_DISPLAY_ON, 1);
	//clear display
	_write_lcd(ST7032_CMD_COMMAND, ST7032_DATA_CLEARDISPLAY, 2);
	//entry mode set
	_write_lcd(ST7032_CMD_COMMAND, ST7032_DATA_ENTRYMODESET_NORMAL, 2);
}


/**
 * 画面クリア
 */
void st7032i_clear(void)
{
	_write_lcd(ST7032_CMD_COMMAND, ST7032_DATA_CLEARDISPLAY, 2);
}


/**
 * カーソル移動
 * 
 * @param[in]	x	X座標(0～15)
 * @param[in]	y	Y座標(0 or else)
 * 
 * Yが0以外なら、強制的に1として扱う.
 */
void st7032i_move_pos(int x, int y)
{
	uint8_t addr;

	if(y == 0) {
		addr = 0;
	} else {
		addr = 0x40;
	}
	
	addr += x;
	
	_write_lcd(ST7032_CMD_COMMAND, ST7032_DATA_SETDDRAMADDR(addr), 1);
}


/**
 * 現在のカーソル位置から文字列出力
 * 
 * @param[in]	pStr	文字列
 */
void st7032i_write_string(const char* pStr)
{
	while(*pStr) {
		_write_lcd(ST7032_CMD_DATA, *pStr++, 1);
	}
}


/**
 * ST7032iへの出力
 * 
 * @param[in]	cmd		コマンド
 * @param[in]	data	データ
 * @param[in]	wait	待ち時間[msec]
 */
static ER _write_lcd(uint8_t cmd, uint8_t data, uint16_t wait)
{
	ER ercd;

	m_I2c.cmd[0] = cmd;						/* control byte */
	m_I2c.ncmd = 1;							/* command size */
	m_I2c.data = &data;						/* data byte */
	m_I2c.ndata = 1;						/* data size */
	ercd = i2c_start(&m_I2c);				/* Start the I2C transaction */
	if(ercd != E_OK) {
		return ercd;
	}
	dly_tsk(wait);
	
	return ercd;
}
