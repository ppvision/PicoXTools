// -------------------------------------------------- //
// 文件名 : pio_asm_def.h
// 说  明 : 用C语言宏直接构建PIO指令二进制代码。安富莱电子
// 作  者 : 安富莱电子, 可免费试用
// -------------------------------------------------- //
 
#ifndef _PIO_ASM_DEF_H
#define _PIO_ASM_DEF_H
 
#define SIDEONE 12
#define HIGH 1
#define LOW 0
#define DELAY 8
#define C_JMP   (0 << 13)
    enum IFEnum
    {
        IF_X_EQU_0       = (1 << 5),
        IF_X_DEC_EQU_1   = (2 << 5),
        IF_Y_EQU_0       = (3 << 5),
        IF_Y_DEC_EQU_1   = (4 << 5),
        IF_X_NOT_EQU_Y   = (5 << 5),
        IF_PIN           = (6 << 5),
        IF_OSR_NOT_EMPTY = (7 << 5)
    };
    enum TAREnum
    {
        TAR_0,  TAR_1,  TAR_2,  TAR_3,  TAR_4,  TAR_5,  TAR_6,  TAR_7,
        TAR_8,  TAR_9,  TAR_10, TAR_11, TAR_12, TAR_13, TAR_14, TAR_15,
        TAR_16, TAR_17, TAR_18, TAR_19, TAR_20, TAR_21, TAR_22, TAR_23,
        TAR_24, TAR_25, TAR_26, TAR_27, TAR_28, TAR_29, TAR_30, TAR_31   
    };
 
#define C_WAIT  (1 << 13)
    enum WaitForEnum
    {
        WAIT_FOR_0 = (0 << 7),
        WAIT_FOR_1 = (1 << 7)          
    };
    enum WaitSrcEnum
    {
        WAIT_SRC_GPIO = (0 << 5),
        WAIT_SRC_PIN  = (1 << 5),
        WAIT_SRC_IRQ  = (2 << 5),
    };
 
#define C_IN    (2 << 13)
    enum InSrcEnum
    {
        IN_SRC_PINS   = (0 << 5),
        IN_SRC_X      = (1 << 5),
        IN_SRC_Y      = (2 << 5),
        IN_SRC_NULL   = (3 << 5),
        IN_SRC_ISR    = (6 << 5),
        IN_SRC_OSR    = (7 << 5),    
    };
 
#define C_OUT   (3 << 13)
    enum OutSrcEnum
    {
        OUT_SRC_PINS = (0 << 5),
        OUT_SRC_X    = (1 << 5),
        OUT_SRC_Y    = (2 << 5),
        OUT_SRC_NULL = (3 << 5),
        OUT_SRC_PINDIRS = (4 << 5),
        OUT_SRC_PC   = (5 << 5),    
        OUT_SRC_ISR  = (6 << 5),
        OUT_SRC_EXEC = (7 << 5),    
    };
 
    enum
    {
        BIT_COUNT_32 = 0, 
        BIT_COUNT_1, BIT_COUNT_2, BIT_COUNT_3, BIT_COUNT_4, 
        BIT_COUNT_5, BIT_COUNT_6, BIT_COUNT_7, BIT_COUNT_8, 
        BIT_COUNT_9, BIT_COUNT_10, BIT_COUNT_11, BIT_COUNT_12, 
        BIT_COUNT_13, BIT_COUNT_14, BIT_COUNT_15, BIT_COUNT_16, 
        BIT_COUNT_17, BIT_COUNT_18, BIT_COUNT_19, BIT_COUNT_20, 
        BIT_COUNT_21, BIT_COUNT_22, BIT_COUNT_23, BIT_COUNT_24,
        BIT_COUNT_25, BIT_COUNT_26, BIT_COUNT_27, BIT_COUNT_28, 
        BIT_COUNT_29, BIT_COUNT_30, BIT_COUNT_31,
    };
     
#define C_PUSH  (4 << 13)
    enum
    {
        IF_FULL      = (1 << 6),
        PUSH_BLOCK   = (1 << 5),
    };
#define C_PULL  ((4 << 13) | (1 << 7))
    enum
    {
        IF_EMPTY     = (1 << 6),
        PULL_BLOCK   = (1 << 5),
    };
#define C_MOV   (5 << 13)
    enum
    {
        MOV_DEST_PINS = (0 << 5),
        MOV_DEST_X    = (1 << 5),
        MOV_DEST_Y    = (2 << 5),
        MOV_DEST_EXEC = (4 << 5),
        MOV_DEST_PC   = (5 << 5),
        MOV_DEST_ISR  = (6 << 5),
        MOV_DEST_OSR  = (7 << 5),
    };
    enum
    {
        MOV_OP_NONE      = (0 << 3),
        MOV_OP_INVERT    = (1 << 3),
    };
    enum
    {
        MOV_SRC_PINS   = (0 << 0),
        MOV_SRC_X      = (1 << 0),
        MOV_SRC_Y      = (2 << 0),
        MOV_SRC_NULL   = (3 << 0),
        MOV_SRC_STATUS = (5 << 0),
        MOV_SRC_ISR    = (6 << 0),
        MOV_SRC_OSR    = (7 << 0),    
    };
 
#define C_IRQ   (6 << 13)
    enum
    {
        IRQ_CLEAR      = (1 << 6),
        IRQ_WAIT       = (1 << 5), 
    };
 
#define C_SET   (7 << 13)
    enum
    {
        SET_DEST_PINS = (0<<5),
        SET_DEST_X = (1<<5),
        SET_DEST_Y = (2<<5),
        SET_DEST_PINDIRS = (4<<5),
    };
#endif
 
/****************************** END ***************************************************/


    #include "pio_asm_def.h"
 
#define blink_wrap_target 2
#define blink_wrap 7
 
 
static const uint16_t blink_program_instructions[] = {
    // 0x80a0, 0: pull   block
    C_PULL | PULL_BLOCK,
    
    //  0x6040, 1: out    y, 32
    C_OUT | OUT_SRC_Y | BIT_COUNT_32,
    
    //     .wrap_target
    // 0xa022,  2: mov    x, y
    C_MOV | MOV_DEST_X | MOV_SRC_Y,
    
    // 0xe001,  3: set    pins, 1
    C_SET | SET_DEST_PINS | 1,
    
    //  0x0044, 4: jmp    x--, 4      
    C_JMP | IF_X_DEC_EQU_1 | 4,
    
    //  0xa022, 5: mov    x, y        
    C_MOV | MOV_DEST_X | MOV_SRC_Y,
         
    // 0xe000,  6: set    pins, 0
    C_SET | SET_DEST_PINS | 0,
         
    //  0x0047, 7: jmp    x--, 7                     
    C_JMP | IF_X_DEC_EQU_1 | 7
    
    //     .wrap
};
 
 
static const struct pio_program blink_program = {
    .instructions = blink_program_instructions,
    .length = 8,
    .origin = -1,
};
 
 
static inline pio_sm_config blink_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + blink_wrap_target, offset + blink_wrap);
    return c;
}
 
 
// this is a raw helper function for use by the user which sets up the GPIO output, and configures the SM to output on a particular pin
void blink_program_init(PIO pio, uint sm, uint offset, uint pin) {
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
    pio_sm_config c = blink_program_get_default_config(offset);
    sm_config_set_set_pins(&c, pin, 1);
    pio_sm_init(pio, sm, offset, &c);
}
 
 
void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq);
 
 
int TestPioMain() {
    setup_default_uart();
 
 
    // todo get free sm
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &blink_program);
    printf("Loaded program at %d\n", offset);
 
 
    blink_pin_forever(pio, 0, offset, 0, 3);
    blink_pin_forever(pio, 1, offset, 6, 4);
    blink_pin_forever(pio, 2, offset, 11, 1);
}
 
 
void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq) {
    blink_program_init(pio, sm, offset, pin);
    pio_sm_set_enabled(pio, sm, true);
    
    pio->txf[sm] = clock_get_hz(clk_sys) / (2 * freq);
}