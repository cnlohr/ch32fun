#ifndef NEC_IR_DECODER_H
#define NEC_IR_DECODER_H

#include <cstdint>

struct NecDecodedData
{
    uint8_t address;
    uint8_t command;
    uint32_t raw_data; // The "logical" 32-bit packet (e.g., 0xE916FB04)
    bool is_repeat;
    bool valid;
};

class NecIrDecoder
{
public:
    NecIrDecoder(uint8_t pin);
    void begin();
    bool available();
    NecDecodedData read();
    void isr();

private:
    void reset();
    void process_data();

    static const uint16_t NEC_TOLERANCE = 250;
    static const uint16_t NEC_START_PULSE = 9000;
    static const uint16_t NEC_START_SPACE = 4500;
    static const uint16_t NEC_REPEAT_SPACE = 2250;
    static const uint16_t NEC_PULSE = 562;
    static const uint16_t NEC_ZERO_SPACE = 562;
    static const uint16_t NEC_ONE_SPACE = 1687;

    enum NecState
    {
        STATE_IDLE,
        STATE_START_PULSE,
        STATE_START_SPACE,
        STATE_DATA_PULSE,
        STATE_DATA_SPACE,
        STATE_REPEAT_PULSE
    };

    bool in_range(uint16_t duration, uint16_t target) {
        return (duration > (target - NEC_TOLERANCE)) && (duration < (target + NEC_TOLERANCE));
    }

    uint8_t ir_pin;

    volatile NecState state;
    volatile uint16_t last_event_time;
    volatile uint32_t data;
    volatile uint8_t bit_count;
    volatile bool data_ready;
    volatile NecDecodedData result;
};

#endif
