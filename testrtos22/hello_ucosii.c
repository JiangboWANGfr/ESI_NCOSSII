/*************************************************************************
 * Copyright (c) 2004 Altera Corporation, San Jose, California, USA.      *
 * All rights reserved. All use of this software and documentation is     *
 * subject to the License Agreement located at the end of this file below.*
 **************************************************************************
 * Description:                                                           *
 * The following is a simple hello world program running MicroC/OS-II.The *
 * purpose of the design is to be a very simple application that just     *
 * demonstrates MicroC/OS-II running on NIOS II.The design doesn't account*
 * for issues such as checking system call return codes. etc.             *
 *                                                                        *
 * Requirements:                                                          *
 *   -Supported Example Hardware Platforms                                *
 *     Standard                                                           *
 *     Full Featured                                                      *
 *     Low Cost                                                           *
 *   -Supported Development Boards                                        *
 *     Nios II Development Board, Stratix II Edition                      *
 *     Nios Development Board, Stratix Professional Edition               *
 *     Nios Development Board, Stratix Edition                            *
 *     Nios Development Board, Cyclone Edition                            *
 *   -System Library Settings                                             *
 *     RTOS Type - MicroC/OS-II                                           *
 *     Periodic System Timer                                              *
 *   -Know Issues                                                         *
 *     If this design is run on the ISS, terminal output will take several*
 *     minutes per iteration.                                             *
 **************************************************************************/

#include <stdio.h>
#include "includes.h"
#include "altera_avalon_pio_regs.h"
/* Definition of Task Stacks */
#define TASK_STACKSIZE 2048
OS_STK task1_stk[TASK_STACKSIZE];
OS_STK task2_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK1_PRIORITY 1
#define TASK2_PRIORITY 2

/*Global variables*/
INT32U time_sec = 0;
#define SEVEN_SEG_PIO_BASE 0x81020
#define SWITCH_PIO_BASE 0x81040
#define KEY_PIO_BASE 0x81050
/*Senven segment display*/
static void foursevensegDisplay(int afficheurNumber1, int afficheurNumber2, int afficheurNumber3, int afficheurNumber4)
{
    static alt_u8 segments[16] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0e};
    alt_u32 data = segments[afficheurNumber1 & 0xff] | (segments[afficheurNumber2 & 0xff] << 8) | (segments[afficheurNumber3 & 0xff] << 16) | (segments[afficheurNumber4 & 15] << 24);
    IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_PIO_BASE, data);
}
static void foursevensegDisplayAnyNumber(int number)
{
    int afficheurNumber1 = (number / 10) % 10;
    int afficheurNumber2 = number % 10;
    int afficheurNumber3 = 0;
    int afficheurNumber4 = 0;
    foursevensegDisplay(afficheurNumber1, afficheurNumber2, afficheurNumber3, afficheurNumber4);
}

/*Read the 4 switches value*/
static int readSwitchValue()
{
    int switchValue = IORD_ALTERA_AVALON_PIO_DATA(SWITCH_PIO_BASE);
    return switchValue;
}

/* read key value */
static int readKeyValue()
{
    int keyValue = IORD_ALTERA_AVALON_PIO_DATA(KEY_PIO_BASE);
    return keyValue;
}

/* Prints "Hello World" and sleeps for three seconds */
void task1(void *pdata)
{
    INT8U err;
    OS_STK_DATA taskstkcheckdata;

    // check the task satck check option
    err = OSTaskStkChk(TASK1_PRIORITY, &taskstkcheckdata);
    if (err == OS_ERR_NONE)
    {
        printf("Task1 free stack size is %ld\n", taskstkcheckdata.OSFree);
        printf("Task1 used stack size is %ld\n", taskstkcheckdata.OSUsed);
    }
    else
    {
        printf("error code for task1 stack check is %d\n", err);
    }
    while (1)
    {
        printf("Hello from task1\n");
        foursevensegDisplayAnyNumber(time_sec);
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}
/* Prints "Hello World" and sleeps for three seconds */
void task2(void *pdata)
{
    INT8U err;
    OS_STK_DATA taskstkcheckdata2;
    err = OSTaskStkChk(TASK2_PRIORITY, &taskstkcheckdata2);
    if (err == OS_ERR_NONE)
    {
        printf("Task2 free stack size is %ld\n", taskstkcheckdata2.OSFree);
        printf("Task2 used stack size is %ld\n", taskstkcheckdata2.OSUsed);
    }
    else
    {
        printf("error code for task2 stack check is %d\n", err);
    }
    while (1)
    {
        INT8U switchValue = readSwitchValue();
        switchValue = switchValue << 4 | 0x0f;
        printf("Switch value is %d\n", switchValue);
        OSTimeDlyHMSM(0, 0, 3, 0);
    }
}
/* The main function creates two task and starts multi-tasking */
int main(void)
{
    printf("Hello from Nios II!\n");
    OSTaskStkInit(task1, NULL, (void *)&task1_stk[TASK_STACKSIZE - 1], 0);
    OSTaskCreateExt(task1,
                    NULL,
                    (void *)&task1_stk[TASK_STACKSIZE - 1],
                    TASK1_PRIORITY,
                    TASK1_PRIORITY,
                    task1_stk,
                    TASK_STACKSIZE,
                    NULL,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskStkInit(task2, NULL, (void *)&task2_stk[TASK_STACKSIZE - 1], 0);
    OSTaskCreateExt(task2,
                    NULL,
                    (void *)&task2_stk[TASK_STACKSIZE - 1],
                    TASK2_PRIORITY,
                    TASK2_PRIORITY,
                    task2_stk,
                    TASK_STACKSIZE,
                    NULL,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSStart();
    return 0;
}
void OSTimeTickHook(void)
{
    static INT32U compteur = 0;
#if OS_TMR_EN > 0
    OSTmrCtr++;
    if (OSTmrCtr >= (OS_TICKS_PER_SEC / OS_TMR_CFG_TICKS_PER_SEC))
    {
        OSTmrCtr = 0;
        OSTmrSignal();
    }
#endif
    if (compteur % OS_TICKS_PER_SEC == 0)
    {
        time_sec++;
    }
    compteur++;
#ifdef ALT_INICHE
    /* Service the Interniche timer */
    cticks_hook();
#endif
}

void OSTaskIdleHook(void)
{

}
/******************************************************************************
 *                                                                             *
 * License Agreement                                                           *
 *                                                                             *
 * Copyright (c) 2004 Altera Corporation, San Jose, California, USA.           *
 * All rights reserved.                                                        *
 *                                                                             *
 * Permission is hereby granted, free of charge, to any person obtaining a     *
 * copy of this software and associated documentation files (the "Software"),  *
 * to deal in the Software without restriction, including without limitation   *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
 * and/or sell copies of the Software, and to permit persons to whom the       *
 * Software is furnished to do so, subject to the following conditions:        *
 *                                                                             *
 * The above copyright notice and this permission notice shall be included in  *
 * all copies or substantial portions of the Software.                         *
 *                                                                             *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
 * DEALINGS IN THE SOFTWARE.                                                   *
 *                                                                             *
 * This agreement shall be governed in all respects by the laws of the State   *
 * of California and by the laws of the United States of America.              *
 * Altera does not recommend, suggest or require that this reference design    *
 * file be used in conjunction or combination with any other product.          *
 ******************************************************************************/
