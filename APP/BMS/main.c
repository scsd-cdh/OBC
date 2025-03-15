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
#include <sys/types.h>


#define SLAVE_ADDR 0x08

volatile uint8_t buffer[8];
const uint8_t outBuffer[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

static const SystemStatusResp_t SystemStatusOut = {
    .runtime = 0x12,
    .fw_version = 0xA, 
};

void I2C_Proc_RX_Data(uint8_t data);

uint16_t SendTelemetryResponse()
{
    uint8_t bytes[TINYPROTOCOL_MAX_PACKET_SIZE];
    uint8_t count = 0;
    uint8_t* pbyte = bytes;

    while(TINYPROTOCOL_TelemetryBytesLeft() > 0) {
        int16_t result = TINYPROTOCOL_ReadNextTelemetryByte(pbyte);
        if (result == ETINYPROTOCOL_SUCCESS) {
            pbyte++;
            count++;
        } else {
            return result;
        }
    }
    
    transmitI2C(bytes, count);
    return ETINYPROTOCOL_SUCCESS;
} 

static int16_t ProcessTelemetryRequest(uint8_t request)
{
    return SendTelemetryResponse();
}

static int16_t ProcessTelecommand(uint8_t command, const uint8_t* buffer, uint8_t size)
{
    return 0;
}

const struct TINYPROTOCOL_Config protocolConfig =
{
    .TINYPROTOCOL_ProcessTelecommand = ProcessTelecommand,
    .TINYPROTOCOL_ProcessTelemetryRequest = ProcessTelemetryRequest,
    .TINYPROTOCOL_WriteBuffer = transmitI2C
};

void I2C_Proc_RX_Data(uint8_t data)
{
    TINYPROTOCOL_ParseByte(&protocolConfig, data);
}

int main(void)
{
    sI2cConfigCb_t i2cConfig = {
        .Rx_Proc_Data = I2C_Proc_RX_Data,
        .slave_addr = SLAVE_ADDR
    };

    initBSP();
    initI2C(&i2cConfig);

    TINYPROTOCOL_Initialize();
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_SYSTEM_STATUS_ID, SystemStatusOut.buffer , sizeof(SystemStatusOut.buffer));
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_POWER_STATUS_ID, sPowerStatusBattery1Out.buffer, sizeof(sPowerStatusBattery1Out.buffer));
    
    __bis_SR_register(LPM0_bits + GIE);
}
