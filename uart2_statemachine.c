#include "uart2_statemachine.h"

uint32_t power_consumption;

void UART2_initialise_logic(void)
{    
    UART2_TX_EN();
}

dynamic_funtion_t UART2_request_state(void)
{
    // Frame: SlaveId, FunctionId, Address Hight, Address Low, Register number High, register number Low, CRC Low!, CRC High
    // In case of CRC we use Little Endian order
    static uint8_t req2004[] = {0x02, 0x03, 0x20, 0x04, 0x00, 0x02, 0x8e, 0x39};
    static uint8_t byte_cnt = 0;
    
    while(byte_cnt<sizeof(req2004))
    {
        //Master mode we initiate the communication 
        UART2_TX_EN();

        //sending
        if(UART2_is_tx_ready() && UART2_Write_NonBlocking(req2004[byte_cnt]))
        {
            byte_cnt++;
        }
        else
        {
            break;
        }
    }
    if(byte_cnt == sizeof(req2004) && UART2_is_tx_done())
    {
        byte_cnt = 0;
        //after the last bit, the frame is ended, we change the mode
        UART2_RX_EN();
        return FUNC_RESP;
    }
    else
    {
        return FUNC_REQ;
    }
    
}

dynamic_funtion_t UART2_response_state(void)
{
    static uint8_t received = 0;
    static uint8_t byte_cntr = 0;
    
    uint8_t rx_data;
    
    //receive response
    while(UART2_is_rx_ready() && UART2_Read_NonBlocking(&rx_data))
    {
        switch(byte_cntr)
        {
            case 0: //SlaveID
                power_consumption = 0;
                break;
            case 1: //FunctionID
                break;
            case 2: //Length
                break;
            case 3: //0x2004 HIGH
                power_consumption = power_consumption | ((uint32_t)rx_data << 24);
                break;
            case 4: //0x2004 LOW
                power_consumption = power_consumption | ((uint32_t)rx_data << 16);
                break;
            case 5: //0x2005 HIGH
                power_consumption = power_consumption | ((uint32_t)rx_data << 8);
                break;
            case 6: //0x2005 LOW
                power_consumption = power_consumption | rx_data;
                break;
            case 7: //CRC LOW            
                break;
            case 8: //CRC HIGH
                received = 1;
                break;
        }
        byte_cntr++;
    }   

    if(received)
    {
        received = 0;
        byte_cntr = 0;
        return FUNC_REQ;
    }
    else
    {
        return FUNC_RESP;
    }
}

