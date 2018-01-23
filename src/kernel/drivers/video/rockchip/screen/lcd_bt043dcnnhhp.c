#ifndef _LCD_BT043DCNNHHP_H_
#define _LCD_BT043DCNNHHP_H_

/* Base */
#define SCREEN_TYPE		SCREEN_RGB
#define LVDS_FORMAT		LVDS_8BIT_1
#define OUT_FACE		OUT_P888  
#define DCLK			9000000		// 9MHz
#define LCDC_ACLK       500000000           //29 lcdc axi DMA Ƶ��

/* Timing */
#define H_PW			20
#define H_BP			2
#define H_VD			480
#define H_FP			23
/* H_PW + H_BP + H_VD + H_FP = 20 + 2 + 480 + 23 = 525 */

#define V_PW			10
#define V_BP			2
#define V_VD			272
#define V_FP			4
/* V_PW + V_BP + V_VD + V_FP = 10 + 2 + 272 + 4 = 288 */

#define LCD_WIDTH       	95
#define LCD_HEIGHT      	54
/* Other */
#define DCLK_POL		0
#define DEN_POL			0
#define VSYNC_POL		0
#define HSYNC_POL		0

#define SWAP_RB			0
#define SWAP_RG			0
#define SWAP_GB			0

#endif
