#include "stm32f0xx.h"

/*
 * Task 1 - Blinky
 *
 * MCU:       STM32F030xC
 * LED:       LED_1 -> PB5
 *
 * Clock:
 *   HSI      = 8 MHz
 *   PLL      = HSI x 6
 *   SYSCLK   = 48 MHz
 *
 * TIM3:
 *   Timer clock = 48 MHz
 *   Prescaler   = 479
 *   Counter     = 100 kHz
 *   ARR         = 49999
 *
 *   100 kHz / 50000 counts = 2 Hz
 *   -> overflow every 500 ms
 *
 * LED is active HIGH:
 *   PB5 = 1 -> LED ON
 *   PB5 = 0 -> LED OFF
 */

static void clock_init(void)
{
    /*
     * Enable HSI.
     * HSI is the internal 8 MHz oscillator.
     */
    RCC->CR |= RCC_CR_HSION;

    /*
     * Wait until HSI is ready.
     */
    while ((RCC->CR & RCC_CR_HSIRDY) == 0)
    {
    }

    /*
     * Flash latency must be increased before running
     * the CPU at 48 MHz.
     *
     * STM32F0 FLASH ACR:
     * LATENCY = 1 wait state.
     */
    FLASH->ACR |= FLASH_ACR_LATENCY;

    /*
     * Configure PLL:
     *
     * PLL source = HSI/2
     * PLL multiplier = x12
     *
     * HSI = 8 MHz
     * HSI/2 = 4 MHz
     * 4 MHz x 12 = 48 MHz
     *
     * STM32F030xC uses these PLL settings to obtain 48 MHz.
     */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL);
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_DIV2;
    RCC->CFGR |= RCC_CFGR_PLLMUL12;

    /*
     * Enable PLL.
     */
    RCC->CR |= RCC_CR_PLLON;

    /*
     * Wait until PLL is ready.
     */
    while ((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /*
     * Select PLL as system clock.
     */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /*
     * Wait until PLL is actually used as SYSCLK.
     */
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    {
    }
}

static void led_init(void)
{
    /*
     * Enable GPIOB peripheral clock.
     */
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    /*
     * PB5 -> General purpose output mode
     *
     * MODER5 = 01
     */
    GPIOB->MODER &= ~(3U << (5U * 2U));
    GPIOB->MODER |=  (1U << (5U * 2U));

    /*
     * Push-pull output.
     */
    GPIOB->OTYPER &= ~(1U << 5);

    /*
     * Low speed is sufficient for an LED.
     */
    GPIOB->OSPEEDR &= ~(3U << (5U * 2U));

    /*
     * No pull-up / pull-down.
     */
    GPIOB->PUPDR &= ~(3U << (5U * 2U));

    /*
     * Start with LED OFF.
     */
    GPIOB->BSRR = (1U << (5U + 16U));
}

static void timer_init(void)
{
    /*
     * Enable TIM3 clock.
     *
     * TIM3 is located on APB1.
     */
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    /*
     * Timer clock = 48 MHz
     *
     * Prescaler:
     *
     * 48 MHz / (479 + 1)
     * = 100 kHz
     */
    TIM3->PSC = 479U;

    /*
     * Auto-reload:
     *
     * 50000 timer ticks
     *
     * 50000 / 100000 Hz
     * = 0.5 s
     */
    TIM3->ARR = 49999U;

    /*
     * Up-counting mode.
     *
     * DIR = 0
     */
    TIM3->CR1 &= ~TIM_CR1_DIR;

    /*
     * Generate an update event so the prescaler
     * value is loaded immediately.
     */
    TIM3->EGR = TIM_EGR_UG;

    /*
     * Clear the update flag generated above.
     */
    TIM3->SR &= ~TIM_SR_UIF;

    /*
     * Start TIM3.
     */
    TIM3->CR1 |= TIM_CR1_CEN;
}

int main(void)
{
    clock_init();
    led_init();
    timer_init();

    while (1)
    {
        /*
         * Busy-wait until TIM3 overflows.
         */
        if (TIM3->SR & TIM_SR_UIF)
        {
            /*
             * Clear update flag.
             */
            TIM3->SR &= ~TIM_SR_UIF;

            /*
             * Toggle PB5.
             *
             * XOR changes:
             *   0 -> 1
             *   1 -> 0
             */
            GPIOB->ODR ^= (1U << 5);
        }
    }
}
