#include <stdint.h>
#include <stdbool.h>

#define CONVERTER_MONITOR_DATA_LEN  ( 10 )
typedef union sVoltageConvData{
    struct {
        uint16_t Int_5v_vs_data;
        uint16_t Reg_5v_vs_data;
        uint16_t A_5v_vs_data;
        uint16_t B_5v_vs_data;
        uint16_t Temp_Sense_data;
    };
    uint8_t bytes[CONVERTER_MONITOR_DATA_LEN];
} sVoltageConvData_t;

extern bool heartbeat_msgs_recieved;

void RoutineCycle_Process(void);

void RoutineCycle_Reboot(void);

void RoutineCycle_Fault(void);

uint8_t* RoutineCycle_GetVoltageConvData(void);
