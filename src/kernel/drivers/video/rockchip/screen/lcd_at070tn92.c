#ifndef _LCD_AT070NT_H_
#define _LCD_AT070NT_H_

/* Base */
#define SCREEN_TYPE		SCREEN_RGB
#define LVDS_FORMAT		LVDS_8BIT_1
#define OUT_FACE		OUT_P888

/* DCLK Frequency fclk */
#define DCLK			33300000
#define LCDC_ACLK       500000000

/* Timing horizontal */
#define H_PW			1
#define H_BP			46
#define H_VD			800
#define H_FP			210
/* H_PW + H_BP + H_VD + H_FP = 1076 (One Horizontal Line)*/

/* Timing vertical */
#define V_PW			1
#define V_BP			23
#define V_VD			480
#define V_FP			7
/* V_PW + V_BP + V_VD + V_FP = 540 (VS period time) */

#define LCD_WIDTH      	154
#define LCD_HEIGHT     	86

/* Other */
#define DCLK_POL		0
#define DEN_POL			0
#define VSYNC_POL		0
#define HSYNC_POL		0

#define SWAP_RB			0
#define SWAP_RG			0
#define SWAP_GB			0

#endif
