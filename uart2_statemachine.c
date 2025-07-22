# include "uart2_statemachine.h"

static uint32_t power_consumption;

static state_t* p_request_state;
static state_t* p_response_state;

state_t* func_request_state(input_st input, unsigned char failure);
state_t* func_response_state(input_st input, unsigned char failure);

void UART1_initialise_logic(void)
{
    p_request_state = register_state(1, 0, func_request_state, 0);
    p_response_state = register_state(2, 0, func_response_state, 0);
    
    initialize_state_machine(p_request_state);
    
    UART2_TX_EN();
}

state_t* func_request_state(input_st input, unsigned char failure)
{
    static uint8_t req2004[] = {0x02, 0x03, 0x20, 0x04, 0x00, 0x02, 0x8e, 0x39};
    static uint8_t i;
    //send request
    UART2_TX_EN();
    for(i=0;i<8;i++)
    {
        //sending
        if(UART2_is_tx_ready())
        {
            UART2_Write(req2004[i]);
            while(!UART2_is_tx_done());
        }
    }
    UART2_RX_EN();
    return p_response_state;
}

state_t* func_response_state(input_st input, unsigned char failure)
{
    static uint8_t received = 0;
    uint8_t rx_data;
    static uint8_t byte_cntr = 0;

    //receive response
    while(UART2_is_rx_ready())
    {
        rx_data = UART2_Read();
        switch(byte_cntr)
        {
            case 0: //SlaveID
                break;
            case 1: //FunctionID
                break;
            case 2: //Length
                break;
            case 3: //0x2004 HIGH
                power_consumption = power_consumption + rx_data << 24;
                break;
            case 4: //0x2004 LOW
                power_consumption = power_consumption + rx_data << 16;
                break;
            case 5: //0x2005 HIGH
                power_consumption = power_consumption + rx_data << 8;
                break;
            case 6: //0x2005 LOW
                power_consumption = power_consumption + rx_data;
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
        return p_request_state;
    }
    else
    {
        return p_response_state;
    }
}

