/*!
    \file    gd32f4xx_hw.c
    \brief   USB hardware configuration for GD32F4xx

    \version 2020-08-01, V3.0.0, firmware for GD32F4xx
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "drv_usb_hw.h"
#include "gd32f4xx_fmc.h"

#define TIM_MSEC_DELAY                          0x01U
#define TIM_USEC_DELAY                          0x02U

__IO uint32_t delay_time = 0U;
__IO uint16_t timer_prescaler = 5U;

/* function declarations */
static void hw_time_set (uint8_t unit);
static void hw_delay    (uint32_t ntime, uint8_t unit);

/*!
    \brief      configure USB clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_rcu_config(void)
{
#ifdef USE_USB_FS

    #ifndef USE_IRC48M
        rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);

        rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);
    #else
        /* enable IRC48M clock */
        rcu_osci_on(RCU_IRC48M);

        /* wait till IRC48M is ready */
        while (SUCCESS != rcu_osci_stab_wait(RCU_IRC48M)) {
        }

        rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);
    #endif /* USE_IRC48M */

    rcu_periph_clock_enable(RCU_USBFS);

#elif defined(USE_USB_HS)

    #ifdef USE_EMBEDDED_PHY

        #ifndef USE_IRC48M
            rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);

            rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);
        #else
            /* enable IRC48M clock */
            rcu_osci_on(RCU_IRC48M);

            /* wait till IRC48M is ready */
            while (SUCCESS != rcu_osci_stab_wait(RCU_IRC48M)) {
            }

            rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);
        #endif /* USE_IRC48M */

    #elif defined(USE_ULPI_PHY)
        rcu_periph_clock_enable(RCU_USBHSULPI);
    #endif /* USE_EMBEDDED_PHY */

    rcu_periph_clock_enable(RCU_USBHS);
#endif /* USB_USBFS */
}

/*!
    \brief      configure USB data line gpio
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_SYSCFG);

#ifdef USE_USB_FS

    rcu_periph_clock_enable(RCU_GPIOA);

    /* USBFS_DM(PA11) and USBFS_DP(PA12) GPIO pin configuration */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11 | GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_11 | GPIO_PIN_12);

    gpio_af_set(GPIOA, GPIO_AF_10, GPIO_PIN_11 | GPIO_PIN_12);

#elif defined(USE_USB_HS)

    #ifdef USE_ULPI_PHY
        rcu_periph_clock_enable(RCU_GPIOA);
        rcu_periph_clock_enable(RCU_GPIOB);
        rcu_periph_clock_enable(RCU_GPIOC);
        rcu_periph_clock_enable(RCU_GPIOH);
        rcu_periph_clock_enable(RCU_GPIOI);

        /* ULPI_STP(PC0) GPIO pin configuration */
        gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
        gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_0);

        /* ULPI_CK(PA5) GPIO pin configuration */
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_5);

        /* ULPI_NXT(PH4) GPIO pin configuration */
        gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
        gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_4);

        /* ULPI_DIR(PI11) GPIO pin configuration */
        gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
        gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_11);

        /* ULPI_D1(PB0), ULPI_D2(PB1), ULPI_D3(PB10), ULPI_D4(PB11) \
           ULPI_D5(PB12), ULPI_D6(PB13) and ULPI_D7(PB5) GPIO pin configuration */
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, \
                        GPIO_PIN_5 | GPIO_PIN_13 | GPIO_PIN_12 |\
                        GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_1 | GPIO_PIN_0);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, \
                        GPIO_PIN_5 | GPIO_PIN_13 | GPIO_PIN_12 |\
                        GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_1 | GPIO_PIN_0);

        /* ULPI_D0(PA3) GPIO pin configuration */
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_200MHZ, GPIO_PIN_3);

        gpio_af_set(GPIOC, GPIO_AF_10, GPIO_PIN_0);
        gpio_af_set(GPIOH, GPIO_AF_10, GPIO_PIN_4);
        gpio_af_set(GPIOI, GPIO_AF_10, GPIO_PIN_11);
        gpio_af_set(GPIOA, GPIO_AF_10, GPIO_PIN_5 | GPIO_PIN_3);
        gpio_af_set(GPIOB, GPIO_AF_10, GPIO_PIN_5 | GPIO_PIN_13 | GPIO_PIN_12 |\
                                       GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_1 | GPIO_PIN_0);
    #endif /* USE_ULPI_PHY */

#endif /* USE_USBFS */
}

/*!
    \brief      configure USB interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_intr_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

#ifdef USE_USB_FS
    nvic_irq_enable((uint8_t)USBFS_IRQn, 2U, 0U);

    #if USB_LOW_POWER
        /* enable the power module clock */
        rcu_periph_clock_enable(RCU_PMU);

        /* USB wakeup EXTI line configuration */
        exti_interrupt_flag_clear(EXTI_18);
        exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
        exti_interrupt_enable(EXTI_18);

        nvic_irq_enable((uint8_t)USBFS_WKUP_IRQn, 1U, 0U);
    #endif

#elif defined(USE_USB_HS)
    nvic_irq_enable((uint8_t)USBHS_IRQn, 2U, 0U);

    #if USB_LOW_POWER
        /* enable the power module clock */
        rcu_periph_clock_enable(RCU_PMU);

        /* USB wakeup EXTI line configuration */
        exti_interrupt_flag_clear(EXTI_20);
        exti_init(EXTI_20, EXTI_INTERRUPT, EXTI_TRIG_RISING);
        exti_interrupt_enable(EXTI_20);

        nvic_irq_enable((uint8_t)USBHS_WKUP_IRQn, 1U, 0U);
    #endif

#endif /* USE_USB_FS */

#ifdef USB_HS_DEDICATED_EP1_ENABLED
    nvic_irq_enable(USBHS_EP1_Out_IRQn, 1U, 0U);
    nvic_irq_enable(USBHS_EP1_In_IRQn, 1U, 0U);
#endif /* USB_HS_DEDICATED_EP1_ENABLED */
}

/*!
    \brief      initializes delay unit using Timer2
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_timer_init (void)
{
    /* set the vector table base address at 0x08000000 */
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x00U);

    /* configure the priority group to 2 bits */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    /* enable the TIM2 global interrupt */
    nvic_irq_enable((uint8_t)TIMER2_IRQn, 1U, 0U);

    rcu_periph_clock_enable(RCU_TIMER2);
}

/*!
    \brief      delay in micro seconds
    \param[in]  usec: value of delay required in micro seconds
    \param[out] none
    \retval     none
*/
void usb_udelay (const uint32_t usec)
{
    hw_delay(usec, TIM_USEC_DELAY);
}

/*!
    \brief      delay in milliseconds
    \param[in]  msec: value of delay required in milliseconds
    \param[out] none
    \retval     none
*/
void usb_mdelay (const uint32_t msec)
{
    hw_delay(msec, TIM_MSEC_DELAY);
}

/*!
    \brief      time base IRQ
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_timer_irq (void)
{
    if (timer_interrupt_flag_get(TIMER2, TIMER_INT_UP) != RESET){
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP);

        if (delay_time > 0x00U){
            delay_time--;
        } else {
            timer_disable(TIMER2);
        }
    }
}

/*!
    \brief      delay routine based on TIMER2
    \param[in]  nTime: delay Time 
    \param[in]  unit: delay Time unit = milliseconds / microseconds
    \param[out] none
    \retval     none
*/
static void hw_delay(uint32_t ntime, uint8_t unit)
{
    delay_time = ntime;

    hw_time_set(unit);

    while (0U != delay_time) {
    }

    timer_disable(TIMER2);
}

/*!
    \brief      configures TIM2 for delay routine based on TIMER2
    \param[in]  unit: msec /usec
    \param[out] none
    \retval     none
*/
static void hw_time_set(uint8_t unit)
{
    timer_parameter_struct  timer_basestructure;

    timer_disable(TIMER2);
    timer_interrupt_disable(TIMER2, TIMER_INT_UP);

    if (unit == TIM_USEC_DELAY) {
        timer_basestructure.period = 13U;
    } else if(unit == TIM_MSEC_DELAY) {
        timer_basestructure.period = 13999U;
    } else {
        /* no operation */
    }

    timer_basestructure.prescaler         = timer_prescaler;
    timer_basestructure.alignedmode       = TIMER_COUNTER_EDGE;
    timer_basestructure.counterdirection  = TIMER_COUNTER_UP;
    timer_basestructure.clockdivision     = TIMER_CKDIV_DIV1;
    timer_basestructure.repetitioncounter = 0U;

    timer_init(TIMER2, &timer_basestructure);

    timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP);

    timer_auto_reload_shadow_enable(TIMER2);

    /* TIMER2 IT enable */
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);

    /* TIMER2 enable counter */ 
    timer_enable(TIMER2);
}

#ifdef USE_IRC48M

/*!
    \brief      configure the CTC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_config(void)
{
    /* configure CTC reference signal source prescaler */
    ctc_refsource_prescaler_config(CTC_REFSOURCE_PSC_OFF);
    /* select reference signal source */
    ctc_refsource_signal_select(CTC_REFSOURCE_USBSOF);
    /* select reference signal source polarity */
    ctc_refsource_polarity_config(CTC_REFSOURCE_POLARITY_RISING);
    /* configure hardware automatically trim mode */
    ctc_hardware_trim_mode_config(CTC_HARDWARE_TRIM_MODE_ENABLE);

    /* configure CTC counter reload value, Fclock/Fref-1 */
    ctc_counter_reload_value_config(0xBB7FU);
    /* configure clock trim base limit value, Fclock/Fref*0.0012/2 */
    ctc_clock_limit_value_config(0x1DU);

    /* CTC counter enable */
    ctc_counter_enable();
}

#endif