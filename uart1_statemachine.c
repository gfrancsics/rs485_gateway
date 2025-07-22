# include "uart1_statemachine.h"

static state_t* p_request_state;
static state_t* p_response_state;

state_t* func_request_state(input_st input, unsigned char failure);
state_t* func_response_state(input_st input, unsigned char failure);

void UART1_initialise_logic(void)
{
    p_request_state = register_state(1, 0, func_request_state, 0);
    p_response_state = register_state(2, 0, func_response_state, 0);
    
    initialize_state_machine(p_request_state);

    UART1_RX_EN();
}

state_t* func_request_state(input_st input, unsigned char failure)
{
    static uint8_t received = 0;
    uint8_t rx_data;
    static uint8_t byte_cntr = 0;
    static uint16_t address = 0;
    static uint8_t is_good_id = 0;
    static uint8_t is_good_function = 0;

    //receive response
    while(UART1_is_rx_ready())
    {
        rx_data = UART1_Read();
        switch(byte_cntr)
        {
            case 0: //SlaveID
                if(rx_data == 254)
                {
                    is_good_id = 1;
                }
                else
                {
                    is_good_id = 0;
                }
                break;
            case 1: //FunctionID
                if(rx_data == 3)
                {
                    is_good_function = 1;
                }
                else{
                    is_good_function = 0;
                }
                break;
            case 2: //ADDRESS HIGH
                if(is_good_id && is_good_function)
                {
                    address = address + rx_data;
                    address = address << 8;
                }
                break;
            case 3: //ADDRESS LOW
                if(is_good_id && is_good_function)
                {
                    address = address + rx_data;
                }
                break;
            case 4: //LENGTH HIGH
                break;
            case 5: //LENGTH LOW
                break;
            case 6: //CRC LOW            
                break;
            case 7: //CRC HIGH
                received = 1;
                break;
        }
        byte_cntr++;
    }   

    if(received)
    {
        received = 0;
        byte_cntr = 0;
        return p_response_state;
    }
    else
    {
        return p_request_state;
    }
}

state_t* func_response_state(input_st input, unsigned char failure)
{
    static uint8_t resp003f[] = {0xfe, 0x03, 0x02, 0x00, 0x00, 0xac, 0x50};
    static uint8_t resp0164[] = {0xfe, 0x03, 0x02, 0x00, 0x00, 0xac, 0x50};
    static uint8_t resp000a[] = {0xfe, 0x03, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F, 0x1F};
    static uint8_t resp0061[] = {0xfe, 0x03, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x81};
    static uint8_t resp0077[] = {0xfe, 0x03, 0x02, 0x00, 0x00, 0xac, 0x50};

    switch(address)
    {
        case 0x003f:
        send(resp003f, 8);
        break;
        case 0x0164:
        calc_resp0164();
        send();
        break;
        case 0x000a:
        send(resp000a, 8);
        break;
        case 0x0061:
        send(resp0061, 29);
        break;
        case 0077:
        send(resp0077, 8);
        break;
        default:
        break;
    }

    return p_request_state;
}

void calc_resp0164()
{

}

void send(uint8_t* vektor, uint8_t length)
{
    static uint8_t i;
    //send request
    UART1_TX_EN();
    for(i=0;i<length;i++)
    {
        //sending
        if(UART1_is_tx_ready())
        {
            UART1_Write(vektor[i]);
            while(!UART1_is_tx_done());
        }
    }
    UART1_RX_EN();
}


