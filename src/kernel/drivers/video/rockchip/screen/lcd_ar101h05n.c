#ifndef _LCD_AR101H05N_H_
#define _LCD_AR101H05N_H_

/* Base */
#define SCREEN_TYPE		SCREEN_RGB
#define LVDS_FORMAT		LVDS_8BIT_1
#define OUT_FACE		OUT_D888_P666
#define DCLK			51200000		// 51.2MHz
#define LCDC_ACLK       500000000           //29 lcdc axi DMA Ƶ��

/* Timing */
#define H_PW			60
#define H_BP			100
#define H_VD			1024
#define H_FP			160
/* H_PW + H_BP + H_VD + H_FP = 60 + 100 + 1024 + 160 = 1344 */

#define V_PW			10
#define V_BP			13
#define V_VD			600
#define V_FP			12
/* V_PW + V_BP + V_VD + V_FP = 10 + 13 + 600 + 12 = 635 */

#define LCD_WIDTH       	223
#define LCD_HEIGHT      	125
/* Other */
#define DCLK_POL		0
#define DEN_POL			0
#define VSYNC_POL		0
#define HSYNC_POL		0

#define SWAP_RB			0
#define SWAP_RG			0
#define SWAP_GB			0

#endif
