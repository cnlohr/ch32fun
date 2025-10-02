#include "ch32fun.h"
#include <stdio.h>

// CRC-16-CCITT (common in communications)
#define CRC16_CCITT_POLY 0x1021

uint16_t _crc16_ccitt(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;  // Initial value
    
    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ CRC16_CCITT_POLY;
            } else {
                crc = crc << 1;
            }
        }
    }
    
    return crc;
}

// Calculate CRC for first 6 bytes of uint64_t (excluding last 2 bytes)
uint16_t crc16_ccitt_compute(uint64_t data) {
    // Use only first 6 bytes for CRC calculation (mask out last 2 bytes)
    uint64_t data_for_crc = data & 0xFFFFFFFFFFFF0000;
    
    uint8_t bytes[6];
    for (int i = 0; i < 6; i++) {
        bytes[i] = (data_for_crc >> (56 - i * 8)) & 0xFF;
    }
    
    return _crc16_ccitt(bytes, 6);
}

// Pack data with CRC into uint64_t
uint64_t crc16_ccitt_make64(uint64_t data_without_crc) {
    // Clear the last 2 bytes to ensure clean space for CRC
    uint64_t clean_data = data_without_crc & 0xFFFFFFFFFFFF0000;
    uint16_t crc = crc16_ccitt_compute(clean_data);
    
    // Combine: keep first 6 bytes, replace last 2 bytes with CRC
    return (clean_data & 0xFFFFFFFFFFFF0000) | crc;
}

// Verify and extract data from packed uint64_t
int crc16_ccitt_check64(uint64_t packed_data, uint64_t *extracted_data) {
    // Extract the CRC from last 2 bytes
    uint16_t received_crc = packed_data & 0xFFFF;
    uint16_t calculated_crc = crc16_ccitt_compute(packed_data);
    
    printf("received_crc: 0x%04X\r\n", received_crc);
    printf("calculated_crc: 0x%04X\r\n", calculated_crc);
    
    if (received_crc == calculated_crc) {
        // Return the original data (with last 2 bytes cleared)
        *extracted_data = packed_data & 0xFFFFFFFFFFFF0000;
        return 1; // Success
    }
    
    return 0; // CRC error
}


u64 combine_64(u16 part0, u16 part1, u16 part2, u16 part3) {
    return ((u64)part0 << 48) | ((u64)part1 << 32) | 
            ((u64)part2 << 16) | ((u64)part3 << 0);
}