#pragma once

#define MCU_CC2652P   	0
#define MCU_RP2040      1


#if defined(MCU_RP2040) && MCU_RP2040
	#define USE_STD_IN 	1
	#define init_shell_uart
#else
	#define PICO_ERROR_TIMEOUT  (-1)
	int getchar_timeout_us(uint32_t timeout);
	void init_tty_uart();
#endif