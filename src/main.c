/* SPDX-License-Identifier: GPL-2.0-or-later */

/* Copyright (c) 1883 Thomas Edison - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the BSD 3 clause license, which unfortunately
 * won't be written for another century.
 */

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

#include "hardware/structs/scb.h"
#include "hardware/watchdog.h"

#include "pico/bootrom.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/sync.h"

#include "cc.h"
#include "io.h"
#include "readln.h"
#include "tar.h"
#include "vi.h"
#include "shell.h"
#include "common/base_def.h"
#include "xmodem.h"
#include "ymodem.h"


#include "FreeRTOS.h"
#include "task.h"


#if !defined(NDEBUG) || defined(PSHELL_TESTS)
#include "tests.h"
#endif

typedef int (*GPIO_CALLBACK)(uint gpio, uint32_t events);
static GPIO_CALLBACK gpio_callback_hook = NULL;


int sel_0 = 12;
int sel_1 = 13;
int sel_2 = 14;


int btn_left  = 21;
int btn_right = 23;
int btn_enter = 22;
int btn_leave = 24;



int led_pin = PICO_DEFAULT_LED_PIN;

int led_a = 16;
int led_b = 17;
int led_c = 18;
int led_d = 19;

int enalbe_oled_pwr_pin = 27;


static int act_port = 0;
int en_oled_pwr     = 0;
uint64_t ab_time    = 0;

#define LED_GPIO_MASK (1<<led_a | 1<<led_b | 1<<led_c | 1<<led_d)
#define CTR_GPIO_MASK (1<<sel_0 | 1<<sel_1 | 1<<sel_2 )
#define GPIO_MASK  (LED_GPIO_MASK | CTR_GPIO_MASK)



void active_port(int port){
//    gpio_put(sel_2, 0);
//    gpio_put(sel_1, 0);
//    gpio_put(sel_0, 1);
//
//    gpio_put(led_a, 1);
//    gpio_put(led_b, 0);
//    gpio_put(led_c, 0);
//    gpio_put(led_d, 0);
    gpio_put_masked(LED_GPIO_MASK|CTR_GPIO_MASK,1<<(port+led_a) | (port==3?6:(port+1))<<sel_0);

}




void gpio_callback(uint gpio, uint32_t events) {
    static int tic      = 0;
    
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    uint64_t curr_tm = time_us_64();
    if(curr_tm - ab_time < 300*1000) { // 200ms
        return;
    }

    ab_time = curr_tm;

    if(gpio_callback_hook) {
        int ret = gpio_callback_hook(gpio,events);
        if(ret) return ;
    }
    
    tic ^= 1;
    gpio_put(led_pin, tic);
    gpio_put(enalbe_oled_pwr_pin, tic);

    if(btn_left == gpio){
        act_port --;
    }else if(btn_right == gpio){
        act_port ++;
    }

    if(act_port<0) act_port +=4;
    act_port %=4;

    active_port(act_port);
    
    return;
}


void init_sys_gpio(){

    int gpio_mask = LED_GPIO_MASK|CTR_GPIO_MASK|(1<<enalbe_oled_pwr_pin|1<<led_pin);

    gpio_init_mask(gpio_mask);
    gpio_set_dir_masked(gpio_mask,gpio_mask);

    gpio_set_irq_enabled_with_callback(btn_left,   GPIO_IRQ_EDGE_RISE |GPIO_IRQ_EDGE_FALL, true, &gpio_callback);//GPIO_IRQ_EDGE_RISE |
    gpio_set_irq_enabled_with_callback(btn_right,  GPIO_IRQ_EDGE_RISE |GPIO_IRQ_EDGE_FALL, true, &gpio_callback);//GPIO_IRQ_EDGE_RISE |
    gpio_set_irq_enabled_with_callback(btn_enter,  GPIO_IRQ_EDGE_RISE |GPIO_IRQ_EDGE_FALL, true, &gpio_callback);//GPIO_IRQ_EDGE_RISE |
    gpio_set_irq_enabled_with_callback(btn_leave,  GPIO_IRQ_EDGE_RISE |GPIO_IRQ_EDGE_FALL, true, &gpio_callback);//GPIO_IRQ_EDGE_RISE |

    gpio_put_masked(gpio_mask,0);
    gpio_pull_up(btn_leave);
    active_port(act_port);
    // gpio_put(led_pin, 1);
    
}


void reg_gpio_hook(GPIO_CALLBACK callback){
    gpio_callback_hook = callback?callback:NULL;
}

void unreg_gpio_hook(){
    gpio_callback_hook = NULL;
}


extern bool check_mount(bool need);




static void HardFault_Handler(void) {
    static const char* clear = "\n\n"  "*** " VT_BLINK "CRASH" VT_NORMAL VT_BOLD
                               " - Rebooting in 5 seconds ***" VT_NORMAL "\r\n\n";
    for (const char* cp = clear; *cp; cp++)
        putchar(*cp);
#ifndef NDEBUG
    for (;;);
#endif
    watchdog_reboot(0, 0, 5000);
    for (;;)
        __wfi();
}




#include <pico/time.h>




extern int main_SSH1107();
extern void km_tty_init();
extern void LCD_Init(void);



// application entry point
int main(void) {
    // initialize the pico SDK
    stdio_init_all();
    km_tty_init();
    init_sys_gpio();
    bool uart = true;

    // LCD_Init();
    // probe_main();

#if LIB_PICO_STDIO_USB
    if(1) {
        while (!stdio_usb_connected())
            sleep_ms(1000);
    }
    uart = false;

#endif

//    main_SSH1107();
    ((int*)scb_hw->vtor)[3] = (int)HardFault_Handler;
    getchar_timeout_us(500);

    run_shell();

    /*void shell_thread(void *ptr)
        {
           run_shell();
        }  
        #define SHELL_TASK_PRIO (tskIDLE_PRIORITY+1)
        static TaskHandle_t shell_taskhandle;
        xTaskCreate(shell_thread, "SHELL", configMINIMAL_STACK_SIZE, NULL, SHELL_TASK_PRIO, &shell_taskhandle);
        vTaskStartScheduler(); 
    */
  
    printf("\ndone\n");
    sleep_ms(1000);
}




