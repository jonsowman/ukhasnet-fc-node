/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "hal.h"
#include "nil.h"

static uint8_t htu_tx;
static uint8_t htu_buf[3];
static uint8_t i2cerr;

/*
 *  * I2C2 config. See p643 of F0x0 refman.
 *   */
static const I2CConfig i2c_config = { 
    STM32_TIMINGR_PRESC(1U) |
        STM32_TIMINGR_SCLDEL(4U) | STM32_TIMINGR_SDADEL(2U) |
        STM32_TIMINGR_SCLH(15U)  | STM32_TIMINGR_SCLL(19U),
    0,  
    0
};

/*
 * Thread 1.
 */
THD_WORKING_AREA(waThread1, 256);
THD_FUNCTION(Thread1, arg) {
    (void)arg;

    // Wait for hardware to start
    chThdSleepMilliseconds(100);

    // Configure I2C
    i2cStart(&I2CD1, &i2c_config);

    while(true)
    {
        htu_tx = 0xe3;
        i2cMasterTransmitTimeout(&I2CD1, 0x40, &htu_tx, 1,
                htu_buf, 3, TIME_INFINITE);
        i2cerr = i2c_lld_get_errors(&I2CD1);
        chThdSleepMilliseconds(500);
    }
}

/*
 * Threads static table, one entry per thread. The number of entries must
 * match NIL_CFG_NUM_THREADS.
 */
THD_TABLE_BEGIN
  THD_TABLE_ENTRY(waThread1, "thd1", Thread1, NULL)
THD_TABLE_END

/*
 * Application entry point.
 */
int main(void) {

    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();

    /* This is now the idle thread loop, you may perform here a low priority
       task but you must never try to sleep or wait in this loop. Note that
       this tasks runs at the lowest priority level so any instruction added
       here will be executed after all other tasks have been started.*/
    while (true) {
    }
}
