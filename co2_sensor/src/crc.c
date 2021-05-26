#include <stdint.h>
#include <stdio.h>

#define CRC8_POLYNOMIAL 0x31
#define CRC8_INIT 0xFF

// Function:    generate_crc()
// Params:      
//          - (uint16_t *) data --> every new byte at new index
//          - (uint16_t) number of bytes (# of uint16_t's in array)
// Returns:     
//          - (uint8_t) crc
// Desription:  Generates a CRC that can be sent too the SCD41
uint8_t generate_crc(const uint16_t* data, uint16_t count) {
    uint16_t current_byte;
    uint8_t crc = CRC8_INIT;
    uint8_t crc_bit;

    /* calculates 8-Bit checksum with given polynomial */
    for (current_byte = 0; current_byte < count; ++current_byte) 
    {
        crc ^= (data[current_byte]);
        for (crc_bit = 8; crc_bit > 0; --crc_bit) 
        {
            if (crc & 0x80)     crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            else                crc = (crc << 1);
        }
    }
    return crc;
}
