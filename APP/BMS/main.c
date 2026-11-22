/* --COPYRIGHT-- 
 * Copyright (c) 2024, Space Concordia
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * --/COPYRIGHT--*/

//******************************************************************************
//  BMS Firmware
//******************************************************************************

#include "BMS.h"
#include <msp430.h>

int main(void)
{
    bms_init();
    while (1) {
        // Checking work and setting CPUOFF are atomic with respect to ISRs.
        // CDH I2C ISRs handle messages and return directly to LPM0; the
        // sampling ISR clears CPUOFF on exit when main has acquisition work.
        __disable_interrupt();
        if (!bms_isr_triggered()) {
            __bis_SR_register(LPM0_bits | GIE);
        } else {
            __enable_interrupt();
        }
        bms_collectdata();
    }
}
