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
//  PDS Firmware
//******************************************************************************

#include "BMS.h"
#include "i2c.h"
#include "tinyprotocol.h"

int main(void)
{
    BMS_init();

    __bis_SR_register(GIE);
    while (1) {
        if (BMS_ISRTriggered()) {
            BMS_collectData();
        }

        if (I2C_CLOCK_HANG) {
            I2C_CLOCK_HANG = 0;
            BMS_init();
            // __delay_cycles(10);
        }

        if (I2C_RECEIVED_BYTE_FLAG) {
            I2C_RECEIVED_BYTE_FLAG = 0;
            if (!TINYPROTOCOL_ParseByte(&protocolConfig, last_received_byte())) {
                // FIXME;
                BMS_init();
                __delay_cycles(10);
            }
        }
    }
    return 0;
}
