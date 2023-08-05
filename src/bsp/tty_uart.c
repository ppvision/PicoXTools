#include "bsp.h"

#include "tty_uart.h"

#if defined(MCU_CC2652P) && MCU_CC2652P
	#include "tty_uart_cc265x.h"
#endif




/*
#pragma printflike isr_printf
//int snprintf(char *str, size_t, const char *format, ...)
int isr_printf(const char * format, ...){
    va_list args;
    #define LOCAL_BUFF_LEN 128
    char buffer[LOCAL_BUFF_LEN];
    va_start(args,format);
    int done = vsnprintf (buffer,LOCAL_BUFF_LEN, format, args) ;
    va_end(args);

    return _write(1,buffer,done>LOCAL_BUFF_LEN?LOCAL_BUFF_LEN:done);
}
*/

