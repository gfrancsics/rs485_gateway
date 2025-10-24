#include "uart1_statemachine.h"
#include "checksum.h"

dynamic_funtion_t send(uint8_t* vector, uint8_t length);
void calc164(uint8_t* vector, uint32_t powerconsumption);

//bejön az uart2_statemachine-b?l
uint32_t power_consumption;

void UART1_initialise_logic(void)
{
    UART1_RX_EN();
}

//látható kell legyen mindkét függvény számára
static uint16_t address = 0;

dynamic_funtion_t UART1_request_state(void)
{
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
                if(rx_buffer[0] != 254) // Ha nem a mi címünk
                {
                    byte_cntr = 0; // Maradunk a 0. bájtnál, ha jön a jó cím
                    continue;      // <-- JAVÍTÁS: Kihagyjuk a byte_cntr növelését
                }
                break;
            
            case 1: // FunctionID (FunctionID-t az 1. indexen tároltuk)
                if(rx_buffer[1] != 3) // Modbus: Read Holding Registers
                {
                    byte_cntr = 0; // Újra a SlaveID-re vár
                    continue;      // <-- JAVÍTÁS: Kihagyjuk a byte_cntr növelését
                }
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
    //TODO: ezek legyenek static-ok vazgy const és kerüljön a program memóriába???
    //ha static akkor a RAMba kerülne
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

// UNION a 32 bites érték bájtokra bontására
typedef union {
    uint32_t u32;
    uint8_t bytes[4];
} u32_to_bytes_t;

// UNION a float és az integer azonos memóriahelyen való elérésére
typedef union {
    uint32_t i; // 32-bites egész szám
    float    f; // 32-bites lebeg?pontos szám
} float_converter_t;

void calc164(uint8_t* vector, uint32_t powerconsumption)
{
    // A register kulcsszó a PIC-en segíti a gyorsabb hozzáférést a 8 bites regiszterekhez.
    register uint16_t crc = 0; 
    uint8_t i;
    
    // Union a bemenet kezelésére és a manipulációra
    float_converter_t val_to_scale;
    
    // Union a kimeneti bájtok könnyebb írásához
    u32_to_bytes_t val_alias;
    
    // A 32 bit bejön, mint IEEE 754
    val_to_scale.i = powerconsumption;
    
    // ===============================================================
    // 1. A KRITIKUS GYORSÍTÓ LÉPÉS: F * 2^14 (Bitmanipuláció)
    // ===============================================================
    
    // A kitev? a bit 23-tól 30-ig van (8 bit). A 32 bites szón belül a 23. bit helyén kezd?dik.
    // Ezzel a 32 bites változóval (val_to_scale.u32) operálunk.
    
    // A C kód szimplán így néz ki:
    // val_to_scale.u32 += (14 << 23); 
    // VAGY egy maszkolással biztonságosabb:

    #define EXPONENT_MASK_SHIFT (8) // A kitev? 8 bit
    #define EXPONENT_POSITION   (23) // A kitev? kezd? bitje
    #define EXPONENT_INCREMENT  (14) // 2^14-gyel szorzás

    // Hozzáadjuk a 14-et a 8 bites kitev?höz.
    // Mivel a 32 bites int-en van, ez a 32 bites hozzáadás (ha XC8 jól csinálja), 
    // de csak 8 biten módosít, és elkerüli a lassú float szorzást.
    val_to_scale.i += (uint32_t)EXPONENT_INCREMENT << EXPONENT_POSITION;

    // A lebeg?pontos érték most $F \times 2^{14}$
    
    // 2. INTEGER KONVERZIÓ (Elkerülhetetlen, lassú lépés)
    // Az egész részt vesszük: (uint32_t) (F * 16384.0f)
    val_alias.u32 = (uint32_t)val_to_scale.f; 
    //val_alias.u32 = powerconsumption;
    
    // ===============================================================
    // 3. ADATÍRÁS ÉS CRC (Optimalizálva)
    // ===============================================================

    // Bájtok írása (Union-al)
    vector[7] = val_alias.bytes[3]; // MSB
    vector[8] = val_alias.bytes[2];
    vector[9] = val_alias.bytes[1];
    vector[10] = val_alias.bytes[0]; // LSB

    // Második példány
    vector[11] = val_alias.bytes[3]; 
    vector[12] = val_alias.bytes[2];
    vector[13] = val_alias.bytes[1];
    vector[14] = val_alias.bytes[0];
    
    // CRC számítás (Ciklus Unroll, ahogy korábban javasoltuk)
    // Feltételezve 19 bájtos CRC (index 0-tól 18-ig)
    // A ciklus a Modbus keret adattartalmán fut végig, 
    // a 0. indext?l (SlaveID) a 18. indexig (az utolsó adatbájt)
    for (i = 0; i < 19; i++) {
        // update_crc_16_simple valószín?leg egy header fájlban van definiálva.
        crc = update_crc_16_simple(crc, vector[i]); 
    }
    
    // CRC LOW
    vector[19] = (uint8_t)crc; 
    // CRC HIGH
    vector[20] = (uint8_t)(crc >> 8);
}


typedef enum {
    SEND_STATE_IDLE,          // Alaphelyzet, nincs küldés
    SEND_STATE_TRANSMITTING,  // Küldés folyamatban van (bájtok a pufferben)
    SEND_STATE_WAIT_TX_DONE,  // Utolsó bájt elküldve, várás a Shift Register kiürülésére
    SEND_STATE_T3_5_DELAY,    // T3.5 késleltetés, ami a keret végét jelöli
    SEND_STATE_COMPLETE       // Küldés kész, visszaváltás megtörtént
} send_states_t;

dynamic_funtion_t send(uint8_t* vector, uint8_t length)
{
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
                //__delay_ms(3);
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

