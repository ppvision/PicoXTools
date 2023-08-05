#include <ti/drivers/UART2.h>
#include "ti_drivers_config.h"

static UART2_Handle uart = NULL;

void init_tty_uart(){
    UART2_Params params;
    UART2_Params_init(&params);
    params.baudRate  = 115200;
    params.readMode  = UART2_Mode_BLOCKING; //UART2_Mode_BLOCKING
    params.writeMode = UART2_Mode_BLOCKING; //UART2_Mode_NONBLOCKING

        // Open the UART
    uart = UART2_open(CONFIG_UART2_0, &params);
    // Enable receiver, inhibit low power mode
    UART2_rxEnable(uart);
}


#if 0

static int unget_have = 0;
static int unget_char = -1;

int tty_putchar(int c){
    size_t ret_len = 0;
    UART2_write(uart, (void *)&c, 1, &ret_len);
    return (int)ret_len;
}

void tty_fflush(){
    fflush(stdout);
}

int tty_puts(char* buff){
    while(*buff){
        tty_putchar(*buff++);
    }
}

int tty_getchar(){
    int c = 0;
    size_t ret_len = 0;
    if(unget_have) {
        unget_have = 0;
        return unget_char;
    }
    int_fast16_t ret = UART2_read(uart, (void *)&c, 1, &ret_len);
    if(ret_len ==1) return c;
    printf("UART2_read return:%d\r\n",ret);
    return -1;
}

void tty_ungetc(int c,FILE *stream){
    (void)stream;
    unget_have = 1;
    unget_char = c;
}


int getchar_timeout_us(uint32_t timeout){
    int c = 0;
    if(uart) {
        size_t ret_len = 0;
        size_t b_len = UART2_getRxCount(uart);
        if(b_len){

        }
        int_fast16_t ret = UART2_readTimeout(uart, (void *)&c, 1, &ret_len,timeout);//,microsecondsToSysTicks(timeout)
        if(ret == UART2_STATUS_SUCCESS)  return ret_len?c:-1;
    }
    return -1;
}

#endif

ssize_t _write(int fd, const void *buf, size_t count)
{
    (void) fd;  /* Not used, avoid warning */
    size_t ret_len  = 0;
    size_t sent_len = 0;

    if (fd == 1) {//STDOUT_FILENO  fileno(stdout)
        if(!uart) return -1;
        int_fast16_t ret = UART2_STATUS_SUCCESS;
        while(sent_len < count){
            ret = UART2_write(uart, buf + sent_len, count - sent_len, &ret_len);
            if((UART2_STATUS_EINUSE == ret || UART2_STATUS_EAGAIN == ret || UART2_STATUS_ETIMEOUT==ret) && ret_len == 0) continue;
            if(ret_len == 0 && ret != UART2_STATUS_SUCCESS ){
                break;
            }
            if(ret_len>0){
                sent_len += ret_len;
            }else{
                break;
            }
        }
    }
    return sent_len;
}

ssize_t _read(int fd, void *buf, size_t count) 
{
    ssize_t ret_len = 0;
    if (fd == 0){//STDIN_FILENO fileno(stdin)
        if(!uart) return -1;
        int_fast16_t ret = UART2_read(uart, buf, count, &ret_len);
        return (ret_len > 0)? ret_len:-1;
    } 
    return -1;
}