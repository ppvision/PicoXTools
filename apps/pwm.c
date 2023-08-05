
#define PWM_PIN_A 8
#define PWM_PIN_B 9
int main() {
    gpio_set_function(PWM_PIN_A, GPIO_FUNC_PWM);
    gpio_set_function(PWM_PIN_B, GPIO_FUNC_PWM);
    // Find out which PWM slice is connected to GPIO 0 (it's slice 0)
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN_A);

    float div = 4.0;

    // Set period of 4 cycles (0 to 3 inclusive)
    pwm_set_wrap(slice_num, 3);
    // Set channel A output high for one cycle before dropping
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 1);
    // Set initial B output high for three cycles before dropping
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 3);
    // Set the PWM running
    pwm_set_enabled(slice_num, true);
    
    while (1) {
        int char_v = getchar_timeout_us(0);
        if (char_v == 3) break;
        if(char_v =='a') {div +=1.0;pwm_set_clkdiv(slice_num,div);
        if(char_v =='b') {div -=1.0;pwm_set_clkdiv(slice_num,div);
        sleep_ms(500);
    }
}