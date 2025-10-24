#include "uart1_statemachine.h"
#include "checksum.h"

dynamic_funtion_t send(uint8_t* vector, uint8_t length);
void calc164(uint8_t* vector, uint32_t powerconsumption);

//bej?n az uart2_statemachine-b?l
uint32_t power_consumption;

void UART1_initialise_logic(void)
{
    UART1_RX_EN();
}

//l?that? kell legyen mindk?t f?ggv?ny sz?m?ra
static uint16_t address = 0;

dynamic_funtion_t UART1_request_state(void)
{
    static uint8_t byte_cntr;
    static uint8_t rx_buffer[8];
    uint16_t crc;
    uint8_t rx_data;
    
    // Az aszinkron olvas?s ciklusban t?rt?nik
    while(UART1_Read_NonBlocking(&rx_data))
    {
        // K?zponti helyen elt?roljuk a b?jtot (byte_cntr: 0-t?l 7-ig)
        if (byte_cntr < sizeof(rx_buffer))
        {
            rx_buffer[byte_cntr] = rx_data;
        } 
        else 
        {
            // Hibakezel?s: t?l sok b?jt (elfelejtett?k null?zni a sz?ml?l?t)
            byte_cntr = 0;
            continue; 
        }

        switch(byte_cntr)
        {
            case 0: // SlaveID (SlaveID-t a 0. indexen t?roltuk)
                if(rx_buffer[0] != 254) // Ha nem a mi c?m?nk
                {
                    byte_cntr = 0; // Maradunk a 0. b?jtn?l, ha j?n a j? c?m
                    continue;      // <-- JAV?T?S: Kihagyjuk a byte_cntr n?vel?s?t
                }
                break;
            
            case 1: // FunctionID (FunctionID-t az 1. indexen t?roltuk)
                if(rx_buffer[1] != 3) // Modbus: Read Holding Registers
                {
                    byte_cntr = 0; // ?jra a SlaveID-re v?r
                    continue;      // <-- JAV?T?S: Kihagyjuk a byte_cntr n?vel?s?t
                }
                break;

            case 7: // CRC HIGH (Utols? b?jt: a 8. b?jt a 7. indexen)
                
                // 1. 16 bites adatok ?sszef?z?se (Address, CRC)
                address = ((uint16_t)rx_buffer[2] << 8) | rx_buffer[3]; // ADDR_H | ADDR_L
                // length  = ((uint16_t)rx_buffer[4] << 8) | rx_buffer[5]; // LENGTH_H | LENGTH_L
                crc     = ((uint16_t)rx_buffer[7] << 8) | rx_buffer[6]; // CRC_H | CRC_L (felt?telezve, hogy a Modbus RTU-hoz a CRC LSB el?bb j?n)
                
                // TODO: Ide j?hetne a crc ellen?rz?s a rx_buffer-en
                
                // 2. Visszat?r?s a v?lasz ?llapotba
                byte_cntr = 0; // El?k?sz?t?s a k?vetkez? keret v?tel?re
                
                // AZONNALI VISSZAT?R?S, ha megvan az ?zenet
                return FUNC_RESP;
            
            // A t?bbi b?jt (2, 3, 4, 5, 6) egyszer? adatgy?jt?s.
            // A "break" ?tdobja a k?d v?g?re, ahol a byte_cntr++ van.
            default: 
                break; 
        }
        
        byte_cntr++; // L?p?s a k?vetkez? ?llapotba (csak a default ?s 0, 1 sikeres ?gban)

    }

    // Ha a while ciklus lefutott (nincs t?bb adat), de m?g nincs k?sz az ?zenet,
    // maradjunk a k?r?s ?llapotban.
    return FUNC_REQ;
}

dynamic_funtion_t UART1_response_state(void)
{
    //TODO: ezek legyenek static-ok vazgy const ?s ker?lj?n a program mem?ri?ba???
    //ha static akkor a RAMba ker?lne
    static uint8_t resp003f[] = {0xfe, 0x03, 0x02, 0x00, 0x00, 0xac, 0x50};
    static uint8_t resp0164[] = {0xfe, 0x03, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xac, 0x50};
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

// UNION a 32 bites ?rt?k b?jtokra bont?s?ra
typedef union {
    uint32_t u32;
    uint8_t bytes[4];
} u32_to_bytes_t;

// UNION a float ?s az integer azonos mem?riahelyen val? el?r?s?re
typedef union {
    uint32_t i; // 32-bites eg?sz sz?m
    float    f; // 32-bites lebeg?pontos sz?m
} float_converter_t;

void calc164(uint8_t* vector, uint32_t powerconsumption)
{
    // A register kulcssz? a PIC-en seg?ti a gyorsabb hozz?f?r?st a 8 bites regiszterekhez.
    register uint16_t crc = 0xffff; 
    uint8_t i;
    
    // Union a bemenet kezel?s?re ?s a manipul?ci?ra
    float_converter_t val_to_scale;
    
    // Union a kimeneti b?jtok k?nnyebb ?r?s?hoz
    u32_to_bytes_t val_alias;
    
    // A 32 bit bej?n, mint IEEE 754
    val_to_scale.i = powerconsumption;
    
    // ===============================================================
    // 1. A KRITIKUS GYORS?T? L?P?S: F * 2^14 (Bitmanipul?ci?)
    // ===============================================================
    
    // A kitev? a bit 23-t?l 30-ig van (8 bit). A 32 bites sz?n bel?l a 23. bit hely?n kezd?dik.
    // Ezzel a 32 bites v?ltoz?val (val_to_scale.u32) oper?lunk.
    
    // A C k?d szimpl?n ?gy n?z ki:
    // val_to_scale.u32 += (14 << 23); 
    // VAGY egy maszkol?ssal biztons?gosabb:

    #define EXPONENT_MASK_SHIFT (8) // A kitev? 8 bit
    #define EXPONENT_POSITION   (23) // A kitev? kezd? bitje
    #define EXPONENT_INCREMENT  (14) // 2^14-gyel szorz?s

    // Hozz?adjuk a 14-et a 8 bites kitev?h?z.
    // Mivel a 32 bites int-en van, ez a 32 bites hozz?ad?s (ha XC8 j?l csin?lja), 
    // de csak 8 biten m?dos?t, ?s elker?li a lass? float szorz?st.
    val_to_scale.i += (uint32_t)EXPONENT_INCREMENT << EXPONENT_POSITION;

    // A lebeg?pontos ?rt?k most $F \times 2^{14}$
    
    // 2. INTEGER KONVERZI? (Elker?lhetetlen, lass? l?p?s)
    // Az eg?sz r?szt vessz?k: (uint32_t) (F * 16384.0f)
    val_alias.u32 = (uint32_t)val_to_scale.f; 
    //val_alias.u32 = powerconsumption;
    
    // ===============================================================
    // 3. ADAT?R?S ?S CRC (Optimaliz?lva)
    // ===============================================================

    // B?jtok ?r?sa (Union-al)
    vector[7] = val_alias.bytes[3]; // MSB
    vector[8] = val_alias.bytes[2];
    vector[9] = val_alias.bytes[1];
    vector[10] = val_alias.bytes[0]; // LSB

    // M?sodik p?ld?ny
    vector[11] = val_alias.bytes[3]; 
    vector[12] = val_alias.bytes[2];
    vector[13] = val_alias.bytes[1];
    vector[14] = val_alias.bytes[0];
    
    // CRC sz?m?t?s (Ciklus Unroll, ahogy kor?bban javasoltuk)
    // Felt?telezve 19 b?jtos CRC (index 0-t?l 18-ig)
    // A ciklus a Modbus keret adattartalm?n fut v?gig, 
    // a 0. indext?l (SlaveID) a 18. indexig (az utols? adatb?jt)
    for (i = 0; i < 19; i++) {
        // update_crc_16_simple val?sz?n?leg egy header f?jlban van defini?lva.
        crc = update_crc_16_simple(crc, vector[i]); 
    }
    
    // CRC LOW
    vector[19] = (uint8_t)crc; 
    // CRC HIGH
    vector[20] = (uint8_t)(crc >> 8);
}


typedef enum {
    SEND_STATE_IDLE,          // Alaphelyzet, nincs k?ld?s
    SEND_STATE_TRANSMITTING,  // K?ld?s folyamatban van (b?jtok a pufferben)
    SEND_STATE_WAIT_TX_DONE,  // Utols? b?jt elk?ldve, v?r?s a Shift Register ki?r?l?s?re
    SEND_STATE_T3_5_DELAY,    // T3.5 k?sleltet?s, ami a keret v?g?t jel?li
    SEND_STATE_COMPLETE       // K?ld?s k?sz, visszav?lt?s megt?rt?nt
} send_states_t;

dynamic_funtion_t send(uint8_t* vector, uint8_t length)
{
    static send_states_t tx_state = SEND_STATE_IDLE;
    static uint8_t byte_cnt = 0;

    // A logik?nak el?sz?r az AD?T (TX) kell enged?lyeznie, ha ?j keret indul
    if (tx_state == SEND_STATE_IDLE) {
        UART1_TX_EN();
        tx_state = SEND_STATE_TRANSMITTING;
        byte_cnt = 0; // Biztons?gi null?z?s
    }

    switch (tx_state) {
        case SEND_STATE_TRANSMITTING:
            // Folytatjuk a b?jtok bet?lt?s?t, am?g van hely ?s van adat
            while (byte_cnt < length) {
                if (UART1_Write_NonBlocking(vector[byte_cnt])) {
                    byte_cnt++;
                } else {
                    // UART puffer megtelt. K?s?bb visszaj?v?nk.
                    return FUNC_RESP;
                }
            }

            // Ha a ciklus v?g?re ?rt?nk, az utols? b?jt is beker?lt a pufferbe
            if (byte_cnt == length) {
                tx_state = SEND_STATE_WAIT_TX_DONE;
            }
            // NEM t?r?nk ki, megy?nk a k?vetkez? ?llapotra (a switch-en bel?l)!
            
        case SEND_STATE_WAIT_TX_DONE:
            // V?rjuk meg, am?g a hardware Shift Register is ki?r?l
            if (UART1_is_tx_done()) {
                //TIMER_T3_5_Start(); // T3.5 id?z?t?s ind?t?sa
                tx_state = SEND_STATE_T3_5_DELAY;
            } else {
                // M?g k?ld?s k?zben a hardver
                return FUNC_RESP;
            }
            // NEM t?r?nk ki, megy?nk a k?vetkez? ?llapotra (a switch-en bel?l)!

        case SEND_STATE_T3_5_DELAY:
            // V?rjuk meg, am?g a T3.5 id?z?t? lej?r
            //if (TIMER_T3_5_IsExpired()) {
                // *** EZ A KRITIKUS PONT: ?TKAPCSOL?S RX-RE ***
                //__delay_ms(3);
                UART1_RX_EN(); 
                tx_state = SEND_STATE_IDLE; // Vissza az alaphelyzetbe
                return FUNC_REQ; // SIKERES K?LD?S V?GE
            //} else {
                // A T3.5 k?sleltet?s folyamatban van
              //  return false;
            //}

        case SEND_STATE_IDLE:
        case SEND_STATE_COMPLETE:
            // Ezekbe az ?llapotokba nem jutunk el a norm?l fut?s sor?n (IDLE-t a tetej?n kezelj?k)
            // A COMPLETE ?llapotot lehetne haszn?lni a tiszta kil?p?sre, de IDLE-re v?ltunk.
            return FUNC_RESP; // K?ld?s nincs folyamatban

        default:
            // Hib?s ?llapot
            tx_state = SEND_STATE_IDLE;
            return FUNC_RESP;
    }
    
    // Alap?rtelmezett visszat?r?s: m?g folyamatban van a k?ld?s (az ?llapotokon bel?l nem ?rte el a 'true'-t)
    return FUNC_RESP; 
}
