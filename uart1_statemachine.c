#include "uart1_statemachine.h"
#include "checksum.h"

void send(uint8_t* vector, uint8_t length);
void calc164(uint8_t* vector, uint32_t powerconsumption);

extern uint32_t power_consumption;

void UART1_initialise_logic(void)
{
    UART1_RX_EN();
}

static uint16_t address = 0;
static uint16_t crc = 0;

void* UART1_request_state(void)
{
    static uint8_t received = 0;    
    static uint8_t byte_cntr = 0;

    uint8_t rx_data;

    //receive response
    while(UART1_is_rx_ready())
    {
        rx_data = UART1_Read();
        switch(byte_cntr)
        {
            case 0: //SlaveID
                if(rx_data != 254)
                {
                    byte_cntr = 0; //if not this device, wait until it is
                }
                break;
            case 1: //FunctionID
                if(rx_data != 3)
                {
                    byte_cntr = 0;
                }
                break;
            case 2: //ADDRESS HIGH
                address = address + rx_data;
                address = address << 8;
                break;
            case 3: //ADDRESS LOW
                address = address + rx_data;
                break;
            case 4: //LENGTH HIGH
                break;
            case 5: //LENGTH LOW
                break;
            case 6: //CRC LOW
                crc = crc + rx_data;
                crc = crc << 8;
                break;
            case 7: //CRC HIGH
                crc = crc + rx_data;
                //TODO: put here crc check
                received = 1;
                break;
        }
        byte_cntr++;
    }   

    if(received)
    {
        received = 0;
        byte_cntr = 0;
        return UART1_response_state;
    }
    else
    {
        return UART1_request_state;
    }
}

void* UART1_response_state(void)
{
    static uint8_t* resp003f = {0xfe, 0x03, 0x02, 0x00, 0x00, 0xac, 0x50};
    static uint8_t* resp0164[] = {0xfe, 0x03, 0x02, 0x00, 0x00, 0xac, 0x50};
    static uint8_t* resp000a[] = {0xfe, 0x03, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F, 0x1F};
    static uint8_t* resp0061[] = {0xfe, 0x03, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x81};
    static uint8_t* resp0077[] = {0xfe, 0x03, 0x02, 0x00, 0x00, 0xac, 0x50};

    switch(address)
    {
        case 0x003f:
            send(resp003f, sizeof(resp003f));
        break;
        case 0x0164:
            calc164(resp0164, power_consumption);
            send(resp0164, sizeof(resp0164));
        break;
        case 0x000a:
            send(resp000a, sizeof(resp000a));
        break;
        case 0x0061:
            send(resp0061, sizeof(resp0061));
        break;
        case 0x0077:
            send(resp0077, sizeof(resp0077));
        break;
        default:
        break;
    }

    return UART1_request_state;
}

void calc164(uint8_t* vector, uint32_t powerconsumption)
{
    static uint16_t crc = 0;
    vector[3]=powerconsumption >> 8 & 0xff; 
    vector[4]=powerconsumption & 0xff;
    for(int i=0;i<5;i++)
    {
        crc = update_crc_16(crc, vector[i]);
    }
    vector[5]=crc >> 8 & 0xff;
    vector[6]=crc & 0xff;
}

void send(uint8_t* vector, uint8_t length)
{
    static uint8_t i;
    //send request
    UART1_TX_EN();
    for(i=0;i<length;i++)
    {
        //sending
        if(UART1_is_tx_ready())
        {
            UART1_Write(vector[i]);
            while(!UART1_is_tx_done());
        }
    }
    UART1_RX_EN();
}


