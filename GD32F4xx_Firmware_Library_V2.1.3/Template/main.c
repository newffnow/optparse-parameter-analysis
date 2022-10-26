/*!
    \file    main.c
    \brief   led spark with systick
    
    \version 2016-08-15, V1.0.0, firmware for GD32F4xx
    \version 2018-12-12, V2.0.0, firmware for GD32F4xx
    \version 2020-09-30, V2.1.0, firmware for GD32F4xx
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

#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32f450i_eval.h"

#include "drv_uart.h"
#include "shell_ext.h"

#if SHELL_AUTO_PRASE==1
int add(int i, int j)
{
    printf("%d + %d = : %d\r\n", i, j, i+j);
    return i+j;
}
SHELL_EXPORT_CMD(add, add, add num);
#else
void func(int argc, char *argv[])
{
    printf("%dparameter(s)\r\n", argc);
    for (char i = 1; i < argc; i++)
    {
        printf("%s\r\n", argv[i]);
    }
}
SHELL_EXPORT_CMD(func, func, test);
#endif

#define OPTPARSE_IMPLEMENTATION
#include "../../Utilities/Third_Party/optparse-master/optparse.h"

void optparse_test(int argc, char *argv[])
{
    int option;
    struct optparse options;
    if(argc<=1) {
        return;
    }
    optparse_init(&options, argv);
    while ((option = optparse(&options, "ab:C::d")) != -1) {
        switch (option) {
        case 'a':
            printf("get -a\r\n");
            break;
        case 'b':
            printf("get -b, parse is %s\r\n", options.optarg);
            break;
        case 'C':
            if(options.optarg){
                printf("get -C, parse is %s\r\n", options.optarg);
            } else {
                printf("get -C without parse\r\n");
            }
            break;
        case 'd':
            printf("get -d\r\n");
            break;
        case '?':
            printf("get ?\r\n");
            break;
        }
    }
}
SHELL_EXPORT_CMD(optparse_test, optparse_test, test optparse short);

void long_test(int argc, char *argv[])
{
    int option;
    struct optparse options;

    struct optparse_long longopts[] = {
        {"help", 'h', OPTPARSE_NONE},
        {"count", 'c', OPTPARSE_REQUIRED},
        {"time", 't', OPTPARSE_OPTIONAL},
        {0}
    };
    if(argc<=1) {
        return;
    }
    optparse_init(&options, argv);
    while ((option = optparse_long(&options, longopts, NULL)) != -1) {
        switch (option) {
        case 'h':
            printf("get -h or --help\r\n");
            break;
        case 'c':
            printf("get -c or --count, parse is %s\r\n", options.optarg);
            break;
        case 't':
            if(options.optarg){
                printf("get -t or --time, parse is %s\r\n", options.optarg);
            } else {
                printf("get -t or --time without parse\r\n");
            }
            break;
        case '?':
            printf("get ?\r\n");
            break;
        }
    }
}
SHELL_EXPORT_CMD(long_test, long_test, test optparse long);

/*!
    \brief    main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);

    com_init();
    systick_config();

    while (1){
    }
}
