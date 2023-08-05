//
// Created by Bright on 2023/5/17.
//
/* SDK GPIO interface test. Blink the default LED. */

int main() {
    int led_pin = PICO_DEFAULT_LED_PIN;
    int tic = 1;
    int sel_0 = 10;
    int sel_1 = 11;
    int sel_2 = 12;
    int led_a  = 16;
    int led_b = 17;
    int led_c = 18;
    int led_d = 19;


    gpio_init_mask(1<<led_pin|1<<sel_0|1<<sel_1||1<<sel_2);
    gpio_init_mask(1<<led_a|1<<led_b|1<<led_c||1<<led_d);


    /*gpio_init(led_pin);
    gpio_init(sel_0);
    gpio_init(sel_1);
    gpio_init(sel_2);

    gpio_init(led_a);
    gpio_init(led_b);
    gpio_init(led_c);
    gpio_init(led_d);*/

    gpio_set_dir_masked(1<<led_pin|1<<led_pin|1<<sel_0|1<<sel_1||1<<sel_2,
        1<<led_pin|1<<led_pin|1<<sel_0|1<<sel_1||1<<sel_2);

    gpio_set_dir_masked(1<<led_a|1<<led_b|1<<led_c||1<<led_d,
        1<<led_a|1<<led_b|1<<led_c||1<<led_d);


    /*gpio_set_dir(led_pin, GPIO_OUT);
    gpio_set_dir(sel_0, GPIO_OUT);
    gpio_set_dir(sel_1, GPIO_OUT);
    gpio_set_dir(sel_2, GPIO_OUT);

    gpio_set_dir(led_a, GPIO_OUT);
    gpio_set_dir(led_b, GPIO_OUT);
    gpio_set_dir(led_c, GPIO_OUT);
    gpio_set_dir(led_d, GPIO_OUT);*/

    uint32_t value = 1<<sel_0;

    gpio_put_masked(1<<led_pin|1<<sel_0|1<<sel_1|1<<sel_2,value);


    // gpio_put(sel_0, 1);
    // gpio_put(sel_1, 0);
    // gpio_put(sel_2, 0);

    while (1) {
        // gpio_put(led_pin, tic ^= 1);
        // gpio_put(led_a, tic == 1);
        // gpio_put(led_b, tic == 1);
        // gpio_put(led_c, tic == 1);
        // gpio_put(led_d, tic == 1);

        if (getchar_timeout_us(0) == 3)
            break;
        sleep_ms(500);
        gpio_put_masked(1<<led_pin|1<<sel_0|1<<sel_1|1<<sel_2,value^=0xffffffff);
    }
    return 0;
}