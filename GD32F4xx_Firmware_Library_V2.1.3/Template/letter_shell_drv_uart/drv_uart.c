#include "gd32f4xx.h"
#include "shell.h"
#include "stdio.h"

#define SHELL_COM       UART4

static void usart_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_UART4);

    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_12);

    /* connect port to USARTx_Rx */
    gpio_af_set(GPIOD, GPIO_AF_8, GPIO_PIN_2);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_12);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    /* USART configure */
    usart_deinit(SHELL_COM);
    usart_baudrate_set(SHELL_COM, 115200U);
    usart_transmit_config(SHELL_COM, USART_TRANSMIT_ENABLE);
    usart_receive_config(SHELL_COM, USART_RECEIVE_ENABLE);
    
    usart_interrupt_enable(SHELL_COM, USART_INT_RBNE);  /* enable recive not empty interrupt */
//    usart_interrupt_enable(SHELL_COM, USART_INTEN_IDLEIE);  /* enable idle interrupt */
    nvic_irq_enable(UART4_IRQn, 2, 0);
    
    usart_enable(SHELL_COM);
}


static void uart_send_string(const char strbuf)
{
    usart_data_transmit(SHELL_COM, (uint8_t)strbuf);
    while(RESET == usart_flag_get(SHELL_COM, USART_FLAG_TBE));
}

SHELL_TypeDef shell;
void com_init(void)
{
    usart_init();
    shell.write = uart_send_string;
    shellInit(&shell);
}

void UART4_IRQHandler()
{
    char getValue;
    if (usart_flag_get(SHELL_COM, USART_FLAG_RBNE) != RESET){
        getValue = usart_data_receive(SHELL_COM);
        shellInput(&shell, getValue);
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    uart_send_string(ch);
    return ch;
}
