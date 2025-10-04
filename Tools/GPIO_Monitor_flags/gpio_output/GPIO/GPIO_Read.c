#include "GPIO_Read.h"

void GPIO_Initialize_Pin(GPIO_Port port, GPIO_pin pin, uint8_t purpose){
    // Configure GPIO Pin
    switch(purpose){
        case 0: // Set as Output
            GPIO_setAsInputPin(port,pin);
           break;
        case 1: // Set as Input
            GPIO_setAsOutputPin(port,pin);
            break;
        default:
            break;
    }
}


uint8_t GPIO_Read(GPIO_Port port,GPIO_pin pin){
    // Determine the direction of the Pin (Output or Input)
    uint8_t purpose = 0xFF;
    uint8_t output = 0xFF;
    switch(port){
        case PORT1:
            purpose = (P1DIR & pin);
            output = (P1OUT & pin);
            break;
        case PORT2:
            purpose = (P2DIR & pin);
            output = (P2OUT & pin);
            break;
        case PORT4:
            purpose = (P4DIR & pin);
            output = (P4OUT & pin);
            break;
        case PORT3:
            purpose = (P3DIR & pin);
            output = (P3OUT & pin);
            break;
        default:
            purpose = 0xFF;
            return 0xFF;
    }

    // Read Output
    if (!purpose){
        return GPIO_getInputPinValue(port, pin);
    }
    else{
        return (!output)?0:1;
    }
}

void GPIO_Write(GPIO_Port port,GPIO_pin pin, uint8_t value){
    // Determine the direction of the Pin (Output or Input)
    uint8_t purpose = 0xFF;
    switch(port){
        case PORT1:
            purpose = (P1DIR & pin);
            break;
        case PORT2:
            purpose = (P2DIR & pin);
            break;
        case PORT4:
            purpose = (P4DIR & pin);
            break;
        case PORT3:
            purpose = (P3DIR & pin);
            break;
        default:
            purpose = 0xFF;
            return;
    }

    // Write pin values
    if(purpose){
        if(!value){
            GPIO_setOutputLowOnPin(port,pin);
        }
        else{
            GPIO_setOutputHighOnPin(port,pin);
        }
    }
}
