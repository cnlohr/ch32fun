
#include "ch32fun.h"
#include "NecIrDecoder.h"

static NecIrDecoder *g_nec_ir_decoder_instance = nullptr;

// --- Helper function to reverse 8 bits ---
static uint8_t bit_reverse_8(uint8_t in)
{
    uint8_t out = 0;
    for (int i = 0; i < 8; i++)
    {
        if (in & (1 << i))
        {
            out |= (1 << (7 - i));
        }
    }
    return out;
}

extern "C" void EXTI7_0_IRQHandler(void) __attribute__((interrupt));
extern "C" void EXTI7_0_IRQHandler(void)
{
    if (g_nec_ir_decoder_instance)
    {
        g_nec_ir_decoder_instance->isr();
    }
    EXTI->INTFR = (1 << 6);   // limpia el flag de EXTI6 (write-1-to-clear)
}

NecIrDecoder::NecIrDecoder(uint8_t pin)
{
    this->ir_pin = pin;
    reset();
    data_ready = false;
    last_event_time = 0;
}

void NecIrDecoder::begin()
{
    g_nec_ir_decoder_instance = this;

    // --- Reloj para GPIOD, AFIO y TIM2 ---
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO;
    RCC->APB1PCENR |= RCC_APB1Periph_TIM2;

    // --- PD6 como entrada con pull-up ---
    funPinMode(this->ir_pin, GPIO_CNF_IN_PUPD);
    funDigitalWrite(this->ir_pin, FUN_HIGH);

    // --- EXTI6 -> puerto D ---
    AFIO->EXTICR = (AFIO->EXTICR & ~AFIO_EXTICR_EXTI6) | AFIO_EXTICR_EXTI6_PD;

    // NEC necesita medir tanto pulsos bajos como espacios altos -> ambos flancos
    EXTI->FTENR |= (1 << 6);
    EXTI->RTENR |= (1 << 6);
    EXTI->INTENR |= (1 << 6);

    NVIC_EnableIRQ(EXTI7_0_IRQn);

    // --- TIM2 en microsegundos ---
    TIM2->PSC = (FUNCONF_SYSTEM_CORE_CLOCK / 1000000) - 1;
    TIM2->ATRLR = 0xFFFF;
    TIM2->CNT = 0;
    TIM2->CTLR1 |= TIM_CEN;

    last_event_time = TIM2->CNT;
}

bool NecIrDecoder::available()
{
    return data_ready;
}

NecDecodedData NecIrDecoder::read()
{
    if (!data_ready)
    {
        return {0, 0, 0, false, false};
    }

    __disable_irq();
    NecDecodedData data_copy;
    data_copy.address = result.address;
    data_copy.command = result.command;
    data_copy.raw_data = result.raw_data;
    data_copy.is_repeat = result.is_repeat;
    data_copy.valid = result.valid;
    data_ready = false;
    __enable_irq();

    return data_copy;
}

void NecIrDecoder::reset()
{
    state = STATE_IDLE;
    bit_count = 0;
    data = 0;
}

void NecIrDecoder::process_data()
{
    // The data variable is a 32-bit mirror of the logical packet.
    // Logical Packet = (Addr) (~Addr) (Cmd) (~Cmd)
    // `data` variable = (Reversed ~Cmd) (Reversed Cmd) (Reversed ~Addr) (Reversed Addr)

    uint8_t addr_rev = (0xFF000000 & data) >> 24;
    uint8_t addr_inv_rev = (0x00FF0000 & data) >> 16;
    uint8_t cmd_rev = (0x0000FF00 & data) >> 8;
    uint8_t cmd_inv_rev = 0x000000FF & data;


    // Un-reverse them to get the logical values
    uint8_t addr = bit_reverse_8(addr_rev);
    uint8_t addr_inv = bit_reverse_8(addr_inv_rev);
    uint8_t cmd = bit_reverse_8(cmd_rev);
    uint8_t cmd_inv = bit_reverse_8(cmd_inv_rev);

    // Build the "logical" 32-bit packet
    uint32_t logical_packet = ((uint32_t)addr << 24) |
                              ((uint32_t)addr_inv << 16) |
                              ((uint32_t)cmd << 8) |
                              ((uint32_t)cmd_inv);

    // Validate
    if ((uint8_t)~addr == addr_inv && (uint8_t)~cmd == cmd_inv)
    {
        result.address = addr;
        result.command = cmd;
        result.raw_data = logical_packet;
        result.is_repeat = false;
        result.valid = true;
        data_ready = true;
    }

    reset();
}

void NecIrDecoder::isr()
{
    uint16_t now = TIM2->CNT;
    uint16_t elapsed = now - last_event_time;
    last_event_time = now;

    switch (state)
    {
    case STATE_IDLE:
        // Only start on a FALLING edge (pin goes LOW)
        if (funDigitalRead(this->ir_pin) == FUN_LOW)
        {
            state = STATE_START_PULSE;
        }
        break;

    case STATE_START_PULSE:
        if (in_range(elapsed, NEC_START_PULSE))
        {
            state = STATE_START_SPACE;
        }
        else
        {
            reset();
        }
        break;

    case STATE_START_SPACE:
        if (in_range(elapsed, NEC_START_SPACE))
        {
            state = STATE_DATA_PULSE;
            data = 0;
            bit_count = 0;
        }
        else if (in_range(elapsed, NEC_REPEAT_SPACE))
        {
            state = STATE_REPEAT_PULSE;
        }
        else
        {
            reset();
        }
        break;

    case STATE_DATA_PULSE:
        if (in_range(elapsed, NEC_PULSE))
        {
            state = STATE_DATA_SPACE;
        }
        else
        {
            reset();
        }
        break;

    case STATE_DATA_SPACE:
        data <<= 1;
        if (in_range(elapsed, NEC_ONE_SPACE))
        {
            data |= 1;
        }
        else if (!in_range(elapsed, NEC_ZERO_SPACE))
        {
            reset();
            break;
        }

        bit_count++;
        if (bit_count == 32)
        {
            process_data();
        }
        else
        {
            state = STATE_DATA_PULSE;
        }
        break;

    case STATE_REPEAT_PULSE:
        if (in_range(elapsed, NEC_PULSE))
        {
            if (result.valid)
            {
                result.is_repeat = true;
                data_ready = true;
            }
        }
        reset();
        break;

    default:
        reset();
        break;
    }
}
