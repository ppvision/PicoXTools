/*
extern void LCD_Clear(u16 Color);
extern void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);
extern void LCD_DrawPoint(u16 x,u16 y,u16 color);
extern void LCD_DrawPoint_big(u16 x,u16 y,u16 color);
extern void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);
extern void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);
extern void LCD_Init(void);
extern void LCD_SetRotation(u8 rot);
extern void LCD_Scroll_ShowString(u16 x, u16 y, u16 x_min, u16 x_max, u8 *p, 
        u8 mode, u16 color, u16 *sft_val, u32 tick);
*/
//
#define WHITE            0xFFFF
#define BLACK            0x0000   
#define BLUE             0x001F  
#define BRED             0XF81F
#define GRED                     0XFFE0
#define GBLUE                    0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN                    0XBC40 //
#define BRRED                    0XFC07 //
#define GRAY                     0X8430 //

int main(){

printf("1\r\n");
        LCD_Init();
printf("1-1\r\n");
        LCD_Init();
printf("2\r\n");
//      LCD_SetRotation(2);
printf("3\r\n");
//      LCD_Clear(0);
printf("4\r\n");
    
    LCD_ShowStringLn(0, 0*16, 0*8, 17*8, "Raspberry Pi Pico", 1, BLACK);
sleep_ms(500);

LCD_Clear(RED);
        return 0;

}
