#include <stdint.h>
#include <stdbool.h>
#include "./mcc_generated_files/uart1.h"

// ----------------------------------------------------
// A szimulált bemeneti puffer beállításai (Modbus kérés)
// Teszteljük a 0x0164-es címet, ami a calc164-et hívja!
// SlaveID: 254 (0xFE), FunctionID: 3 (Read Holding), 
// Start Address: 0x0164, Quantity: 0x0002, CRC: 0xAC50
// ----------------------------------------------------
uint8_t SIM_RX_DATA_BUF[8] = { 254, 3, 0x01, 0x64, 0x00, 0x02, 0xAC, 0x50 }; 
uint8_t SIM_RX_INDEX = 0;

// Globális változó, amit a Modbus logika használ (a main_test-ben állítjuk be)
extern uint32_t power_consumption; 

// A valós idej? függvény hívások szimulálása

void UART1_Initialize(void) { 
    //TODO:
}

bool UART1_Write_NonBlocking(uint8_t data) {
    //TODO:
    return true; 
}

bool UART1_is_tx_done(void) {
    //TODO
    return true;
}

// A kritikus rész: az adatok beolvasásának szimulációja
bool UART1_Read_NonBlocking(uint8_t* rx_data) {
    if (SIM_RX_INDEX < sizeof(SIM_RX_DATA_BUF)) {
        *rx_data = SIM_RX_DATA_BUF[SIM_RX_INDEX];
        SIM_RX_INDEX++;
        return true; // Van adat
    }
    return false; // Nincs több adat
}