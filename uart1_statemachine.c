#include "uart1_statemachine.h"
#include "checksum.h"

dynamic_funtion_t send(uint8_t* vector, uint8_t length);
void calc164(uint8_t* vector, uint32_t powerconsumption);

typedef union {
    uint32_t i; // 32-bites egész szám
    float    f; // 32-bites lebeg?pontos szám
} float_converter_t;

typedef enum {
    SEND_STATE_IDLE,          // Alaphelyzet, nincs küldés
    SEND_STATE_TRANSMITTING,  // Küldés folyamatban van (bájtok a pufferben)
    SEND_STATE_WAIT_TX_DONE,  // Utolsó bájt elküldve, várás a Shift Register kiürülésére
    SEND_STATE_T3_5_DELAY,    // T3.5 késleltetés, ami a keret végét jelöli
    SEND_STATE_COMPLETE       // Küldés kész, visszaváltás megtörtént
} send_states_t;

//comming from uart2_statemachine
extern uint32_t power_consumption;

void UART1_initialise_logic(void)
{
    UART1_RX_EN();
}

//látható kell legyen mindkét függvény számára
static uint16_t address = 0;

dynamic_funtion_t UART1_request_state(void)
{
    //TODO: megnézni hogy gyorsabb-e ha nem static függvény alatti szkópú
    static uint8_t byte_cntr;
    static uint8_t rx_buffer[8];
    uint16_t crc;
    uint8_t rx_data;
    
    // Az aszinkron olvasás ciklusban történik
    while(UART1_Read_NonBlocking(&rx_data))
    {
        // Központi helyen eltároljuk a bájtot (byte_cntr: 0-tól 7-ig)
        if (byte_cntr < sizeof(rx_buffer))
        {
            rx_buffer[byte_cntr] = rx_data;
        } 
        else 
        {
            // Hibakezelés: túl sok bájt (elfelejtettük nullázni a számlálót)
            byte_cntr = 0;
            continue; 
        }

        switch(byte_cntr)
        {
            case 0: // SlaveID (SlaveID-t a 0. indexen tároltuk)
                //if(rx_buffer[0] != 254) // Ha nem a mi címünk
                //{
                    //byte_cntr = 0; // Maradunk a 0. bájtnál, ha jön a jó cím
                    //continue;      // <-- JAVÍTÁS: Kihagyjuk a byte_cntr növelését
                //}
                break;
            
            case 1: // FunctionID (FunctionID-t az 1. indexen tároltuk)
                //if(rx_buffer[1] != 3) // Modbus: Read Holding Registers
                //{
                    //byte_cntr = 0; // Újra a SlaveID-re vár
                    //received = 0;
                    //continue;      // <-- JAVÍTÁS: Kihagyjuk a byte_cntr növelését
                //}
                break;

            case 7: // CRC HIGH (Utolsó bájt: a 8. bájt a 7. indexen)
                
                // 1. 16 bites adatok összef?zése (Address, CRC)
                address = ((uint16_t)rx_buffer[2] << 8) | rx_buffer[3]; // ADDR_H | ADDR_L
                // length  = ((uint16_t)rx_buffer[4] << 8) | rx_buffer[5]; // LENGTH_H | LENGTH_L
                crc     = ((uint16_t)rx_buffer[7] << 8) | rx_buffer[6]; // CRC_H | CRC_L (feltételezve, hogy a Modbus RTU-hoz a CRC LSB el?bb jön)
                
                // TODO: Ide jöhetne a crc ellen?rzés a rx_buffer-en
                
                // 2. Visszatérés a válasz állapotba
                byte_cntr = 0; // El?készítés a következ? keret vételére
                
                // AZONNALI VISSZATÉRÉS, ha megvan az üzenet
                return FUNC_RESP;
            
            // A többi bájt (2, 3, 4, 5, 6) egyszer? adatgy?jtés.
            // A "break" átdobja a kód végére, ahol a byte_cntr++ van.
            default: 
                break; 
        }
        
        byte_cntr++; // Lépés a következ? állapotba (csak a default és 0, 1 sikeres ágban)

    }

    // Ha a while ciklus lefutott (nincs több adat), de még nincs kész az üzenet,
    // maradjunk a kérés állapotban.
    return FUNC_REQ;
}

dynamic_funtion_t UART1_response_state(void)
{
    static uint8_t resp003f[] = {0xfe, 0x03, 0x02, 0x00, 0x00, 0xac, 0x50};
    static uint8_t resp0164[] = {0xfe, 0x03, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xac, 0x50};
    static uint8_t resp000a[] = {0xfe, 0x03, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F, 0x1F};
    static uint8_t resp0061[] = {0xfe, 0x03, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x81};
    static uint8_t resp0077[] = {0xfe, 0x03, 0x02, 0x00, 0x00, 0xac, 0x50};
    dynamic_funtion_t result;

    switch(address)
    {
        case 0x003f:
            result = send(resp003f, sizeof(resp003f));
        break;
        case 0x0164:
            calc164(resp0164, power_consumption);
            result = send(resp0164, sizeof(resp0164));
        break;
        case 0x000a:
            result = send(resp000a, sizeof(resp000a));
        break;
        case 0x0061:
            result = send(resp0061, sizeof(resp0061));
        break;
        case 0x0077:
            result = send(resp0077, sizeof(resp0077));
        break;
        default:
            result = send(resp003f, sizeof(resp003f));
        break;
    }
    return result;
}

void calc164(uint8_t* vector, uint32_t powerconsumption)
{
    uint8_t i;
    uint16_t crc = 0;
    float_converter_t raw_value;
    uint32_t converted_value;
    
    raw_value.i = powerconsumption;
    converted_value = (uint32_t)(raw_value.f*20000);
    
    vector[7]=(uint8_t)(converted_value >> 24); 
    vector[8]=(uint8_t)(converted_value >> 16);
    vector[9]=(uint8_t)(converted_value >> 8);
    vector[10]=(uint8_t)(converted_value);
    vector[11]=(uint8_t)(converted_value >> 24); 
    vector[12]=(uint8_t)(converted_value >> 16);
    vector[13]=(uint8_t)(converted_value >> 8);
    vector[14]=(uint8_t)(converted_value);
    for(i=0;i<19;i++)
    {
        crc = update_crc_16(crc, vector[i]);
    }
    //CRC LOW
    vector[20]=(uint8_t)crc; 
    //CRC HIGH
    vector[21]=(uint8_t)(crc >> 8);
}

dynamic_funtion_t send(uint8_t* vector, uint8_t length)
{
    //TODO: lehet ha nem függvény alatti szkópú akkor gyorsabb?
    //megnézni
    static send_states_t tx_state = SEND_STATE_IDLE;
    static uint8_t byte_cnt = 0;

    // A logikának el?ször az ADÓT (TX) kell engedélyeznie, ha új keret indul
    if (tx_state == SEND_STATE_IDLE) {
        UART1_TX_EN();
        tx_state = SEND_STATE_TRANSMITTING;
        byte_cnt = 0; // Biztonsági nullázás
    }

    switch (tx_state) {
        case SEND_STATE_TRANSMITTING:
            // Folytatjuk a bájtok betöltését, amíg van hely és van adat
            while (byte_cnt < length) {
                if (UART1_Write_NonBlocking(vector[byte_cnt])) {
                    byte_cnt++;
                } else {
                    // UART puffer megtelt. Kés?bb visszajövünk.
                    return FUNC_RESP;
                }
            }

            // Ha a ciklus végére értünk, az utolsó bájt is bekerült a pufferbe
            if (byte_cnt == length) {
                tx_state = SEND_STATE_WAIT_TX_DONE;
            }
            // NEM törünk ki, megyünk a következ? állapotra (a switch-en belül)!
            
        case SEND_STATE_WAIT_TX_DONE:
            // Várjuk meg, amíg a hardware Shift Register is kiürül
            if (UART1_is_tx_done()) {
                //TIMER_T3_5_Start(); // T3.5 id?zítés indítása
                tx_state = SEND_STATE_T3_5_DELAY;
            } else {
                // Még küldés közben a hardver
                return FUNC_RESP;
            }
            // NEM törünk ki, megyünk a következ? állapotra (a switch-en belül)!

        case SEND_STATE_T3_5_DELAY:
            // Várjuk meg, amíg a T3.5 id?zít? lejár
            //if (TIMER_T3_5_IsExpired()) {
                // *** EZ A KRITIKUS PONT: ÁTKAPCSOLÁS RX-RE ***
                __delay_ms(4);
                UART1_RX_EN(); 
                tx_state = SEND_STATE_IDLE; // Vissza az alaphelyzetbe
                return FUNC_REQ; // SIKERES KÜLDÉS VÉGE
            //} else {
                // A T3.5 késleltetés folyamatban van
              //  return false;
            //}

        case SEND_STATE_IDLE:
        case SEND_STATE_COMPLETE:
            // Ezekbe az állapotokba nem jutunk el a normál futás során (IDLE-t a tetején kezeljük)
            // A COMPLETE állapotot lehetne használni a tiszta kilépésre, de IDLE-re váltunk.
            return FUNC_RESP; // Küldés nincs folyamatban

        default:
            // Hibás állapot
            tx_state = SEND_STATE_IDLE;
            return FUNC_RESP;
    }
    
    // Alapértelmezett visszatérés: még folyamatban van a küldés (az állapotokon belül nem érte el a 'true'-t)
    return FUNC_RESP; 
}

