//
// Created by Bright on 2023/5/17.
//
/* SDK GPIO interface test. Blink the default LED. */

    int sel_0 = 12;
    int sel_1 = 13;
    int sel_2 = 14;


int btn_left  = 21;
int btn_right = 23;
int btn_enter = 22;
int btn_leave = 24;




    int led_a  = 16;
    int led_b = 17;
    int led_c = 18;
    int led_d = 19;

//#define uint32_t int

int active_port =  0;


void gpio_callback(int  gpio, int events) {
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    if(btn_left == gpio){
        active_port --;
    }else if(btn_right == gpio){
        active_port ++;
    }




    if(active_port<0) active_port *=-1;
    active_port %=4;

    switch(active_port){
    case 0:// 1
        gpio_put(sel_2, 0);
        gpio_put(sel_1, 0);
        gpio_put(sel_0, 1);

                gpio_put(led_a, 1);
                gpio_put(led_b, 0);
                gpio_put(led_c, 0);
                gpio_put(led_d, 0);



        break;
    case 1: //2
        gpio_put(sel_2, 0);
        gpio_put(sel_1, 1);
        gpio_put(sel_0, 0);

                gpio_put(led_a, 0);
                gpio_put(led_b, 1);
                gpio_put(led_c, 0);
                gpio_put(led_d, 0);

        break;

    case 2://3
        gpio_put(sel_2, 0);
        gpio_put(sel_1, 1);
        gpio_put(sel_0, 1);

                gpio_put(led_a, 0);
                gpio_put(led_b, 0);
                gpio_put(led_c, 1);
                gpio_put(led_d, 0);
        break;
    case 3://6
        gpio_put(sel_2, 1);
        gpio_put(sel_1, 1);
        gpio_put(sel_0, 0);

                gpio_put(led_a, 0);
                gpio_put(led_b, 0);
                gpio_put(led_c, 0);
                gpio_put(led_d, 1);
        break;
    }

}


int main() {
    int led_pin = PICO_DEFAULT_LED_PIN;
    int tic = 1;



    gpio_init(led_pin);
    gpio_init(sel_0);
    gpio_init(sel_1);
    gpio_init(sel_2);

    gpio_init(led_a);
    gpio_init(led_b);
    gpio_init(led_c);
    gpio_init(led_d);


    gpio_init(btn_left);
    gpio_init(btn_right);
    gpio_init(btn_enter);
    gpio_init(btn_leave);
    gpio_set_dir(btn_left, GPIO_IN);
    gpio_set_dir(btn_right, GPIO_IN);
    gpio_set_dir(btn_enter, GPIO_IN);
    gpio_set_dir(btn_leave, GPIO_IN);


    gpio_set_irq_enabled_with_callback(btn_left,  GPIO_IRQ_EDGE_FALL, true, gpio_callback);//GPIO_IRQ_EDGE_RISE |
    gpio_set_irq_enabled_with_callback(btn_right,  GPIO_IRQ_EDGE_FALL, true,gpio_callback);//GPIO_IRQ_EDGE_RISE |
    gpio_set_irq_enabled_with_callback(btn_enter,  GPIO_IRQ_EDGE_FALL, true,gpio_callback);//GPIO_IRQ_EDGE_RISE |
    gpio_set_irq_enabled_with_callback(btn_leave,  GPIO_IRQ_EDGE_FALL, true,gpio_callback);//GPIO_IRQ_EDGE_RISE |

    gpio_set_dir(led_pin, GPIO_OUT);
    gpio_set_dir(sel_0, GPIO_OUT);
    gpio_set_dir(sel_1, GPIO_OUT);
    gpio_set_dir(sel_2, GPIO_OUT);

    gpio_set_dir(led_a, GPIO_OUT);
    gpio_set_dir(led_b, GPIO_OUT);
    gpio_set_dir(led_c, GPIO_OUT);
    gpio_set_dir(led_d, GPIO_OUT);

    gpio_put(sel_0, 0);
    gpio_put(sel_1, 0);
    gpio_put(sel_2, 0);

    while (1) {
        gpio_put(led_pin, tic ^= 1);
        //gpio_put(led_a, tic == 1);
        //gpio_put(led_b, tic == 1);
        //gpio_put(led_c, tic == 1);
        //gpio_put(led_d, tic == 1);

        if (getchar_timeout_us(0) == 3)
            break;
        sleep_ms(500);
    }
    return 0;
}