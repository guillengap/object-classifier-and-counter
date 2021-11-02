// AUTHOR: GUILLERMO PEREZ GUILLEN  
  
  
#include "cy_pdl.h"  
#include "cy_retarget_io.h"  
#include "cyhal.h"  
#include "cybsp.h"  
  
  
/****************************************************************************** 
 * Macros 
 *****************************************************************************/  
#define DELAY_SHORT_MS          (250)   /* milliseconds */  
#define DELAY_LONG_MS           (500)   /* milliseconds */  
#define LED_BLINK_COUNT         (4)  
#define GPIO_INTERRUPT_PRIORITY (7u)  
  
  
/* PWM Frequency */  
#define PWM_FREQUENCY (50u)  
  
  
/* PWM Duty-cycle */  
#define PWM_DUTY_CYCLE (12.5f)  
  
  
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
    cyhal_pwm_t pwm_led_control;  
  
  
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
  
  
    /* Initialize the PWM */  
    result = cyhal_pwm_init(&pwm_led_control, P7_5, NULL);  
  
  
    /* Configure GPIO interrupt */  
    cyhal_gpio_register_callback(CYBSP_USER_BTN,  
                                 gpio_interrupt_handler, NULL);  
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL,  
                                 GPIO_INTERRUPT_PRIORITY, true);  
  
  
    /* Enable global interrupts */  
    __enable_irq();  
  
  
    /* Set the PWM output frequency and duty cycle */  
    result = cyhal_pwm_set_duty_cycle(&pwm_led_control, PWM_DUTY_CYCLE, PWM_FREQUENCY);  
  
  
    /* Start the PWM */  
    result = cyhal_pwm_start(&pwm_led_control);  
    result = cyhal_pwm_stop(&pwm_led_control);  
    cyhal_system_delay_ms(10);  
  
  
    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */  
    printf("\x1b[2J\x1b[;H");  
    printf("**************** PSoC 6 MCU: GPIO Interrupt *****************\r\n");  
  
  
    for (;;)  
    {  
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
  
  
        for (int i = 180; i >= 0; i--){  
        float PWM_DUTY_CYCLE_3 = ((i*9.5)/180)+3; // 7.5  
        result = cyhal_pwm_set_duty_cycle(&pwm_led_control, PWM_DUTY_CYCLE_3, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&pwm_led_control);  
            cyhal_system_delay_ms(10);  
        }  
  
  
    result = cyhal_pwm_stop(&pwm_led_control);  
        cyhal_system_delay_ms(10);  
  
  
        for (int i = 0; i <= 180; i++){  
        float PWM_DUTY_CYCLE_3 = ((i*9.5)/180)+3;  
        result = cyhal_pwm_set_duty_cycle(&pwm_led_control, PWM_DUTY_CYCLE_3, PWM_FREQUENCY);  
        result = cyhal_pwm_start(&pwm_led_control);  
            cyhal_system_delay_ms(10);  
        }  
  
  
    result = cyhal_pwm_stop(&pwm_led_control);  
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
 