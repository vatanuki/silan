#ifndef __SL_RESET_H
#define __SL_RESET_H

typedef enum {
	SILAN_SR_START = 0,
	SILAN_SR_DVE = SILAN_SR_START,
	SILAN_SR_DDR_PHY,
	SILAN_SR_I2S3A = 7,
	SILAN_SR_SPDIFA,
	SILAN_SR_APWMA = 10,
	SILAN_SR_I2S2A,
	SILAN_SR_I2S0A,
	SILAN_SR_SATA = 14,
	SILAN_SR_USB  = 16,

	SILAN_SR_C = 32,
	SILAN_SR_G,
	SILAN_SR_DX = SILAN_SR_C+9,
	SILAN_SR_SR,
	SILAN_SR_CFG,
	SILAN_SR_DDR,
	SILAN_SR_LSP,
	SILAN_SR_BOOT,
	SILAN_SR_GPU = SILAN_SR_C+16,
	SILAN_SR_JPU,
	SILAN_SR_VPU,
	SILAN_SR_VPPC,
	SILAN_SR_DSP0,
	SILAN_SR_DSP1,

	SILAN_SR_THREE = 64,
	SILAN_SR_VIDIN = SILAN_SR_THREE+3,
	SILAN_SR_GMAC = SILAN_SR_THREE+5,
	SILAN_SR_DMAC0,
	SILAN_SR_DMAC1,
	SILAN_SR_SPDIF,
	SILAN_SR_I2S0,
	SILAN_SR_I2S1,
	SILAN_SR_I2S2,
	SILAN_SR_DMA,
	SILAN_SR_PX,
	SILAN_SR_HUART,
	SILAN_SR_I2S3,
	SILAN_SR_SD,
	SILAN_SR_EMMC,
	SILAN_SR_SDIO,
	SILAN_SR_HDMI,

	SILAN_SR_TIMER = 96,
	SILAN_SR_UART,
	SILAN_SR_SPI = SILAN_SR_TIMER+3,
	SILAN_SR_GPIO,
	SILAN_SR_I2C = SILAN_SR_TIMER+6,
	SILAN_SR_SSP,
	SILAN_SR_CAN,
	SILAN_SR_SCI,
	SILAN_SR_PWM,
	SILAN_SR_RTC,
	SILAN_SR_APWM,
	SILAN_SR_END,
}RSTMOD;

int silan_module_rst(RSTMOD module);

#endif