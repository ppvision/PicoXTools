#pragma once
#include "bsp.h"




#if defined(USE_STD_IN) && USE_STD_IN
    #define tty_fflush(stdout)    fflush(stdout)
    #define tty_puts(str)         puts(str),fflush(stdout)
    #define tty_getchar()         getchar()
    #define tty_putchar(c)        putchar(c),fflush(stdout)
    #define tty_ungetc(c,stream)  ungetc(c,stdin)
#else
    void tty_fflush();
    int  tty_putchar(int c);
    int  tty_puts(char* buff);
    int  tty_getchar();
    void tty_ungetc(int c,FILE *stream);   

    uint32_t tty_available();
    void tty_read (uint8_t *buf, size_t len, size_t *ret_len);   
    void tty_write(uint8_t *buf, size_t len, size_t *ret_len);   
#endif

// typedef void * uart_fd;
// int uart_read(uart_fd fd,void *buff, size_t len, size_t *ret_len);
// int uart_write(uart_fd fd,void *buff, size_t len, size_t *ret_len);
// int uart_write_timeout(uart_fd fd,void *buff, size_t len, size_t *ret_len,uint32_t ms);
// int uart_read_timeout(uart_fd fd,void *buff, size_t len, size_t *ret_len,uint32_t ms);
