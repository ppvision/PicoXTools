//
// Created by Bright on 2023/5/17.
//
/* SDK GPIO interface test. Blink the default LED. */

int main() {
    int led_pin = PICO_DEFAULT_LED_PIN;

    int led_mask = 34537472;

    gpio_init_mask(led_mask);
    gpio_set_dir_masked(led_mask,led_mask);


    uint32_t value = led_mask;
    gpio_put_masked(led_mask,value);


    while (1) {
        if (getchar_timeout_us(0) == 3)
            break;
        sleep_ms(500);
        gpio_put_masked(led_mask,value^=0xffffffff);
    }
    return 0;
}