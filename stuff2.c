#include "stm32f0xx.h"

/*
 * Task 2 - Better Blinky
 *
 * MCU:
 *   STM32F030xC
 *
 * LEDs:
 *   LED_0  -> PA15
 *   LED_1  -> PB5
 *   LED_2  -> PB6
 *   LED_IR -> PB7
 *
 * Joystick:
 *   UP     -> PC13
 *   LEFT   -> PC14
 *   DOWN   -> PC15
 *   RIGHT  -> PF0
 *   CENTER -> PF1
 *
 * Joystick inputs are active LOW:
 *   released = 1
 *   pressed  = 0
 *
 * Clock:
 *   HSI = 8 MHz
 *   PLL = HSI/2 * 12 = 48 MHz
 *
 * TIM3:
 *   Timer clock = 48 MHz
 *   Prescaler = 479
 *   Counter clock = 100 kHz
 *
 * LED toggle period:
 *   Initial = 500 ms
 *   UP      = decrease by 50 ms
 *   DOWN    = increase by 50 ms
 *
 * LED selection:
 *   LEFT  = previous LED
 *   RIGHT = next LED
 */

/* ----------------------------------------------------------
 * LED configuration
 * ---------------------------------------------------------- */

typedef struct
{
    GPIO_TypeDef *port;
    uint32_t pin;
} LED_t;

static const LED_t leds[] =
{
    { GPIOA, 15U },   /* LED_0 */
    { GPIOB, 5U  },   /* LED_1 */
    { GPIOB, 6U  },   /* LED_2 */
    { GPIOB, 7U  }    /* LED_IR */
};

#define LED_COUNT 4U

static volatile uint32_t current_led = 1U;

/*
 * Timer period in milliseconds.
 */
static volatile uint32_t led_period_ms = 500U;

/*
 * Reasonable limits for the joystick-controlled period.
 */
#define LED_PERIOD_MIN_MS 50U
#define LED_PERIOD_MAX_MS 1000U


/* ----------------------------------------------------------
 * Clock initialization
 * ---------------------------------------------------------- */

static void clock_init(void)
{
    /*
     * Enable HSI.
     * HSI = 8 MHz.
     */
    RCC->CR |= RCC_CR_HSION;

    /*
     * Wait until HSI is ready.
     */
    while ((RCC->CR & RCC_CR_HSIRDY) == 0U)
    {
    }

    /*
     * Flash latency:
     * one wait state for 48 MHz operation.
     */
    FLASH->ACR |= FLASH_ACR_LATENCY;

    /*
     * PLL source = HSI/2
     * PLL multiplier = x12
     *
     * 8 MHz / 2 * 12 = 48 MHz
     */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL);

    RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_DIV2;
    RCC->CFGR |= RCC_CFGR_PLLMUL12;

    /*
     * Enable PLL.
     */
    RCC->CR |= RCC_CR_PLLON;

    /*
     * Wait for PLL ready.
     */
    while ((RCC->CR & RCC_CR_PLLRDY) == 0U)
    {
    }

    /*
     * Select PLL as system clock.
     */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /*
     * Wait until PLL is actually used.
     */
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    {
    }
}


/* ----------------------------------------------------------
 * LED initialization
 * ---------------------------------------------------------- */

static void led_init(void)
{
    /*
     * Enable GPIOA and GPIOB clocks.
     */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    /*
     * Configure PA15, PB5, PB6 and PB7 as outputs.
     */
    GPIOA->MODER &= ~(3U << (15U * 2U));
    GPIOA->MODER |=  (1U << (15U * 2U));

    GPIOB->MODER &= ~(3U << (5U * 2U));
    GPIOB->MODER |=  (1U << (5U * 2U));

    GPIOB->MODER &= ~(3U << (6U * 2U));
    GPIOB->MODER |=  (1U << (6U * 2U));

    GPIOB->MODER &= ~(3U << (7U * 2U));
    GPIOB->MODER |=  (1U << (7U * 2U));

    /*
     * Push-pull outputs.
     */
    GPIOA->OTYPER &= ~(1U << 15);
    GPIOB->OTYPER &= ~(1U << 5);
    GPIOB->OTYPER &= ~(1U << 6);
    GPIOB->OTYPER &= ~(1U << 7);

    /*
     * No pull-up / pull-down.
     */
    GPIOA->PUPDR &= ~(3U << (15U * 2U));

    GPIOB->PUPDR &= ~(3U << (5U * 2U));
    GPIOB->PUPDR &= ~(3U << (6U * 2U));
    GPIOB->PUPDR &= ~(3U << (7U * 2U));

    /*
     * All LEDs OFF.
     *
     * LEDs are active HIGH.
     */
    GPIOA->BSRR = (1U << (15U + 16U));

    GPIOB->BSRR = (1U << (5U + 16U));
    GPIOB->BSRR = (1U << (6U + 16U));
    GPIOB->BSRR = (1U << (7U + 16U));
}


/* ----------------------------------------------------------
 * LED helper functions
 * ---------------------------------------------------------- */

static void leds_off(void)
{
    GPIOA->BSRR = (1U << (15U + 16U));

    GPIOB->BSRR = (1U << (5U + 16U));
    GPIOB->BSRR = (1U << (6U + 16U));
    GPIOB->BSRR = (1U << (7U + 16U));
}


static void led_toggle(void)
{
    GPIO_TypeDef *port = leds[current_led].port;
    uint32_t pin = leds[current_led].pin;

    port->ODR ^= (1U << pin);
}


/* ----------------------------------------------------------
 * Joystick initialization
 * ---------------------------------------------------------- */

static void joystick_init(void)
{
    /*
     * Enable GPIOC and GPIOF clocks.
     */
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOFEN;

    /*
     * PC13, PC14, PC15 -> input mode (00).
     */
    GPIOC->MODER &= ~(3U << (13U * 2U));
    GPIOC->MODER &= ~(3U << (14U * 2U));
    GPIOC->MODER &= ~(3U << (15U * 2U));

    /*
     * PF0, PF1 -> input mode (00).
     */
    GPIOF->MODER &= ~(3U << (0U * 2U));
    GPIOF->MODER &= ~(3U << (1U * 2U));

    /*
     * Internal pull-ups.
     *
     * Released joystick = HIGH.
     * Pressed joystick = LOW.
     */
    GPIOC->PUPDR &= ~(3U << (13U * 2U));
    GPIOC->PUPDR |=  (1U << (13U * 2U));

    GPIOC->PUPDR &= ~(3U << (14U * 2U));
    GPIOC->PUPDR |=  (1U << (14U * 2U));

    GPIOC->PUPDR &= ~(3U << (15U * 2U));
    GPIOC->PUPDR |=  (1U << (15U * 2U));

    GPIOF->PUPDR &= ~(3U << (0U * 2U));
    GPIOF->PUPDR |=  (1U << (0U * 2U));

    GPIOF->PUPDR &= ~(3U << (1U * 2U));
    GPIOF->PUPDR |=  (1U << (1U * 2U));
}


/* ----------------------------------------------------------
 * Timer initialization
 * ---------------------------------------------------------- */

static void timer_init(void)
{
    /*
     * Enable TIM3 clock.
     */
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    /*
     * Timer clock:
     *
     * 48 MHz / (479 + 1)
     * = 100 kHz
     */
    TIM3->PSC = 479U;

    /*
     * Initial period:
     *
     * 500 ms
     *
     * 100 kHz * 0.5 s = 50000 counts
     *
     * ARR = 49999
     */
    TIM3->ARR = 49999U;

    /*
     * Up-counting mode.
     */
    TIM3->CR1 &= ~TIM_CR1_DIR;

    /*
     * Generate update event.
     */
    TIM3->EGR = TIM_EGR_UG;

    /*
     * Clear update flag.
     */
    TIM3->SR &= ~TIM_SR_UIF;

    /*
     * Enable TIM3 update interrupt.
     */
    TIM3->DIER |= TIM_DIER_UIE;

    /*
     * Enable TIM3 interrupt in the NVIC.
     */
    NVIC_EnableIRQ(TIM3_IRQn);

    /*
     * Start timer.
     */
    TIM3->CR1 |= TIM_CR1_CEN;
}


/* ----------------------------------------------------------
 * Update timer period
 * ---------------------------------------------------------- */

static void timer_set_period(uint32_t period_ms)
{
    uint32_t arr;

    /*
     * 100 kHz timer:
     *
     * 100 ticks = 1 ms
     *
     * period_ms * 100 ticks
     */
    arr = (period_ms * 100U) - 1U;

    TIM3->ARR = arr;

    /*
     * Force update so the new ARR is applied.
     */
    TIM3->EGR = TIM_EGR_UG;

    /*
     * The update event sets UIF.
     * Clear it again.
     */
    TIM3->SR &= ~TIM_SR_UIF;
}


/* ----------------------------------------------------------
 * TIM3 interrupt service routine
 * ---------------------------------------------------------- */

void TIM3_IRQHandler(void)
{
    /*
     * Check update interrupt flag.
     */
    if (TIM3->SR & TIM_SR_UIF)
    {
        /*
         * Clear update flag.
         */
        TIM3->SR &= ~TIM_SR_UIF;

        /*
         * Toggle selected LED.
         */
        led_toggle();
    }
}


/* ----------------------------------------------------------
 * Joystick polling
 * ---------------------------------------------------------- */

static void joystick_poll(void)
{
    /*
     * Static state variables prevent one long button press
     * from changing the setting continuously.
     */
    static uint8_t left_old  = 1U;
    static uint8_t right_old = 1U;
    static uint8_t up_old    = 1U;
    static uint8_t down_old  = 1U;

    uint8_t left;
    uint8_t right;
    uint8_t up;
    uint8_t down;

    /*
     * Read joystick.
     *
     * Active LOW.
     */
    up    = (GPIOC->IDR & (1U << 13)) ? 1U : 0U;
    left  = (GPIOC->IDR & (1U << 14)) ? 1U : 0U;
    down  = (GPIOC->IDR & (1U << 15)) ? 1U : 0U;
    right = (GPIOF->IDR & (1U << 0))  ? 1U : 0U;

    /*
     * LEFT:
     * previous LED.
     */
    if ((left == 0U) && (left_old == 1U))
    {
        if (current_led == 0U)
        {
            current_led = LED_COUNT - 1U;
        }
        else
        {
            current_led--;
        }

        leds_off();
    }

    /*
     * RIGHT:
     * next LED.
     */
    if ((right == 0U) && (right_old == 1U))
    {
        current_led++;

        if (current_led >= LED_COUNT)
        {
            current_led = 0U;
        }

        leds_off();
    }

    /*
     * UP:
     * decrease toggle period by 50 ms.
     */
    if ((up == 0U) && (up_old == 1U))
    {
        if (led_period_ms > LED_PERIOD_MIN_MS)
        {
            led_period_ms -= 50U;

            if (led_period_ms < LED_PERIOD_MIN_MS)
            {
                led_period_ms = LED_PERIOD_MIN_MS;
            }

            timer_set_period(led_period_ms);
        }
    }

    /*
     * DOWN:
     * increase toggle period by 50 ms.
     */
    if ((down == 0U) && (down_old == 1U))
    {
        if (led_period_ms < LED_PERIOD_MAX_MS)
        {
            led_period_ms += 50U;

            if (led_period_ms > LED_PERIOD_MAX_MS)
            {
                led_period_ms = LED_PERIOD_MAX_MS;
            }

            timer_set_period(led_period_ms);
        }
    }

    /*
     * Save current button states.
     */
    left_old  = left;
    right_old = right;
    up_old    = up;
    down_old  = down;
}


/* ----------------------------------------------------------
 * Main
 * ---------------------------------------------------------- */

int main(void)
{
    clock_init();

    led_init();

    joystick_init();

    timer_init();

    /*
     * Main loop only polls the joystick.
     *
     * Timer timing is handled independently
     * by the TIM3 interrupt.
     */
    while (1)
    {
        joystick_poll();
    }
}
