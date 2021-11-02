// AUTHOR: GUILLERMO PEREZ GUILLEN  
  
  
#include "cy_pdl.h"  
#include "cy_retarget_io.h"  
#include "cyhal.h"  
#include "cybsp.h"  
  
  
/****************************************************************************** 
 * Macros 
 *****************************************************************************/  
#define DELAY_SHORT_MS          (250)   /* milliseconds */  
#define DELAY_LONG_MS           (300)   /* milliseconds */  
#define LED_BLINK_COUNT         (4)  
#define GPIO_INTERRUPT_PRIORITY (7u)  
  
  
/* PWM Frequency */  
#define PWM_FREQUENCY (50u)  
  
  
/* PWM Duty-cycle */  
#define PWM_DUTY_CYCLE_1 (4.58f) //  30 degrees  
#define PWM_DUTY_CYCLE_2 (7.75f) //  90 degrees  
#define PWM_DUTY_CYCLE_3 (12.5f) //   0 degrees  
#define PWM_DUTY_CYCLE_4 (10.92f) // 150 degrees  
#define PWM_DUTY_CYCLE_5 (7.75f) //  90 degrees  
#define PWM_DUTY_CYCLE_6 (4.50f) //  30 degrees  
  
  
/******************************************************************************* 
* Function Prototypes 
********************************************************************************/  
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event);  
  
  
/******************************************************************************* 
* Global Variables 
********************************************************************************/  
volatile bool gpio_intr_flag = false;  
  
  
/******************************************************************************* 
* Function Name: main 
*******************************************************************************/  
int main(void)  
{  
    cy_rslt_t result;  
    uint32_t count = 0;  
    uint32_t delay_led_blink = DELAY_LONG_MS;  
  
  
    /* PWM object */  
    cyhal_pwm_t servo_1, servo_2, servo_3, servo_4, servo_5, servo_6;  
  
  
    /* Initialize the device and board peripherals */  
    result = cybsp_init();  
      
    /* Board init failed. Stop program execution */  
    if (result != CY_RSLT_SUCCESS)  
    {  
        CY_ASSERT(0);  
    }  
  
  
    /* Initialize retarget-io to use the debug UART port */  
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,  
                                 CY_RETARGET_IO_BAUDRATE);  
  
  
    /* Initialize the user LED */  
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,  
                    CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);  
  
  
    /* Initialize the user button */  
    result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT,  
                    CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);  
  
  
    /* Initialize the PWMs */  
    result = cyhal_pwm_init(&servo_1, P7_5, NULL);  
    result = cyhal_pwm_init(&servo_2, P7_6, NULL);  
    result = cyhal_pwm_init(&servo_3, P12_3, NULL);  
    result = cyhal_pwm_init(&servo_4, P12_0, NULL);  
    result = cyhal_pwm_init(&servo_5, P12_1, NULL);  
    result = cyhal_pwm_init(&servo_6, P12_2, NULL);  
  
  
    /* Configure GPIO interrupt */  
    cyhal_gpio_register_callback(CYBSP_USER_BTN,  
                                 gpio_interrupt_handler, NULL);  
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL,  
                                 GPIO_INTERRUPT_PRIORITY, true);  
  
  
    /* Enable global interrupts */  
    __enable_irq();  
  
  
    /* Set the PWM output frequency and duty cycle */  
    result = cyhal_pwm_set_duty_cycle(&servo_1, PWM_DUTY_CYCLE_1, PWM_FREQUENCY);  
    result = cyhal_pwm_set_duty_cycle(&servo_2, PWM_DUTY_CYCLE_2, PWM_FREQUENCY);  
    result = cyhal_pwm_set_duty_cycle(&servo_3, PWM_DUTY_CYCLE_3, PWM_FREQUENCY);  
    result = cyhal_pwm_set_duty_cycle(&servo_4, PWM_DUTY_CYCLE_4, PWM_FREQUENCY);  
    result = cyhal_pwm_set_duty_cycle(&servo_5, PWM_DUTY_CYCLE_5, PWM_FREQUENCY);  
    result = cyhal_pwm_set_duty_cycle(&servo_6, PWM_DUTY_CYCLE_6, PWM_FREQUENCY);  
  
  
    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */  
    printf("\x1b[2J\x1b[;H");  
    printf("**************** PSoC 6 MCU: GPIO Interrupt *****************\r\n");  
  
  
    for (;;)  
    {  
        /* Start the PWM */  
        result = cyhal_pwm_start(&servo_1);  
        cyhal_system_delay_ms(10);  
        result = cyhal_pwm_start(&servo_2);  
        cyhal_system_delay_ms(10);  
        result = cyhal_pwm_start(&servo_3);  
        cyhal_system_delay_ms(10);  
        result = cyhal_pwm_start(&servo_4);  
        cyhal_system_delay_ms(10);  
        result = cyhal_pwm_start(&servo_5);  
        cyhal_system_delay_ms(10);  
        result = cyhal_pwm_start(&servo_6);  
        cyhal_system_delay_ms(10);  
  
  
        /* Check the interrupt status */  
        if (true == gpio_intr_flag)  
        {  
            gpio_intr_flag = false;  
  
  
            /* Update LED toggle delay */  
            if (DELAY_LONG_MS == delay_led_blink)  
            {  
                delay_led_blink = DELAY_SHORT_MS;  
            }  
            else  
            {  
                delay_led_blink = DELAY_LONG_MS;  
            }  
        }  
  
  
        /* Blink LED four times */  
        for (count = 0; count < LED_BLINK_COUNT; count++)  
        {  
            cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);  
            cyhal_system_delay_ms(delay_led_blink);  
            cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);  
            cyhal_system_delay_ms(delay_led_blink);  
        }  
        cyhal_system_delay_ms(10);  
  
  
        for (int i = 180; i >= 90; i--){ // servo_3  
        float PWM_DUTY_CYCLE_C = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_3, PWM_DUTY_CYCLE_C, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_3);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 30; i <= 180; i++){ // servo_1 ***  
        float PWM_DUTY_CYCLE_A = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_1, PWM_DUTY_CYCLE_A, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_1);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 30; i <= 110; i++){ // servo_6  
        float PWM_DUTY_CYCLE_F = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_6, PWM_DUTY_CYCLE_F, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_6);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 90; i >= 75; i--){ // servo_2  
        float PWM_DUTY_CYCLE_B = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_2, PWM_DUTY_CYCLE_B, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_2);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 90; i <= 120; i++){ // servo_3  
        float PWM_DUTY_CYCLE_C = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_3, PWM_DUTY_CYCLE_C, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_3);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 110; i >= 60; i--){ // servo_6  
        float PWM_DUTY_CYCLE_F = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_6, PWM_DUTY_CYCLE_F, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_6);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 150; i >= 90; i--){ // servo_4  
        float PWM_DUTY_CYCLE_D = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_4, PWM_DUTY_CYCLE_D, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_4);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 180; i >= 90; i--){ // servo_1 ***  
        float PWM_DUTY_CYCLE_A = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_1, PWM_DUTY_CYCLE_A, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_1);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 90; i <= 120; i++){ // servo_4  
        float PWM_DUTY_CYCLE_D = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_4, PWM_DUTY_CYCLE_D, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_4);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 60; i <= 110; i++){ // servo_6  
        float PWM_DUTY_CYCLE_F = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_6, PWM_DUTY_CYCLE_F, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_6);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 90; i >= 60; i--){ // servo_1 ***  
        float PWM_DUTY_CYCLE_A = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_1, PWM_DUTY_CYCLE_A, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_1);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 120; i <= 150; i++){ // servo_4  
        float PWM_DUTY_CYCLE_D = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_4, PWM_DUTY_CYCLE_D, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_4);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 120; i >= 90; i--){ // servo_3  
        float PWM_DUTY_CYCLE_C = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_3, PWM_DUTY_CYCLE_C, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_3);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 75; i <= 90; i++){ // servo_2  
        float PWM_DUTY_CYCLE_B = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_2, PWM_DUTY_CYCLE_B, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_2);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 110; i >= 30; i--){ // servo_6  
        float PWM_DUTY_CYCLE_F = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_6, PWM_DUTY_CYCLE_F, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_6);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 60; i >= 30; i--){ // servo_1 ***  
        float PWM_DUTY_CYCLE_A = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_1, PWM_DUTY_CYCLE_A, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_1);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(100);  
  
  
        for (int i = 90; i <= 180; i++){ // servo_3  
        float PWM_DUTY_CYCLE_C = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&servo_3, PWM_DUTY_CYCLE_C, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&servo_3);  
            cyhal_system_delay_ms(20);  
        }  
        cyhal_system_delay_ms(10);  
  
  
        result = cyhal_pwm_stop(&servo_1);  
        result = cyhal_pwm_stop(&servo_2);  
        result = cyhal_pwm_stop(&servo_3);  
        result = cyhal_pwm_stop(&servo_4);  
        result = cyhal_pwm_stop(&servo_5);  
        result = cyhal_pwm_stop(&servo_6);  
        cyhal_system_delay_ms(10);  
  
  
        /* Enter deep sleep mode */  
        cyhal_system_deepsleep();  
    }  
}  
  
  
/******************************************************************************* 
* Function Name: gpio_interrupt_handler 
*******************************************************************************/  
  
  
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_irq_event_t event)  
{  
    gpio_intr_flag = true;  
}