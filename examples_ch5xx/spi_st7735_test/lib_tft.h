// MIT License
// Copyright (c) 2025 UniTheCat

#include "font6x8.h"

#define ST7735_W    160

//###########################################
//# INTERFACES
//###########################################

void INT_TFT_CS_HIGH();
void INT_TFT_CS_LOW();

void INTF_TFT_SET_WINDOW(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void INTF_TFT_SEND_BUFF8(const uint8_t* buffer, uint16_t len);
void INTF_TFT_SEND_BUFF16(uint16_t* buffer, uint16_t len);
void INTF_TFT_SEND_PIXEL(uint16_t color);


//###########################################
//# BASE METHODS
//###########################################

uint16_t _tft_frame[ST7735_W] = {0};

//# draw text
void tft_print(const char* str, uint8_t x, uint8_t y, uint16_t color, uint16_t bg_color) {
    uint8_t font_width = 6, font_height = 8;
    uint16_t current_x = x, current_y = y;
    
    INT_TFT_CS_LOW();

    while (*str) {
        char c = *str++;
        uint16_t len = 0;
        const char* glyph = &font6x8[(c-32) * font_width];

        for (uint8_t y = font_height - 1; y < font_height; y--) {
            uint8_t mask = 0x01 << y;
            
            for (uint8_t x = 0; x < font_width; x++) {
                _tft_frame[len++] = (glyph[x] & mask) ? color : bg_color;
            }
        }

        INTF_TFT_SET_WINDOW(
            current_x, current_y,
            current_x + font_width - 1, current_y + font_height - 1
        );
        INTF_TFT_SEND_BUFF16(_tft_frame, len);
        current_x += font_width;
    }

    INT_TFT_CS_HIGH();
}

//# draw filled_rect
void tft_fill_rect(
    uint16_t x, uint16_t y,
    uint16_t width, uint16_t height, uint16_t color
) {
    INT_TFT_CS_LOW();
    INTF_TFT_SET_WINDOW(x, y, x + width - 1, y + height - 1);

    for (uint16_t i = 0; i < width; i++) _tft_frame[i] = color;

    while(height-- > 0) {
        INTF_TFT_SEND_BUFF16(_tft_frame, width);
    }
    
    INT_TFT_CS_HIGH();
}


// render vertical line - draw with CS controls
static void _render_vertical_line(
    int16_t x, int16_t y, int16_t len, uint16_t color
) {
    for (int16_t i = 0; i < len; i++) _tft_frame[i] = color;

    INTF_TFT_SET_WINDOW(x, y, x, y + len - 1);
    INTF_TFT_SEND_BUFF16(_tft_frame, len);
}


// render horizontal line - draw with CS controls
static void _render_horizontal_line(
    int16_t x, int16_t y, int16_t len, uint16_t color
) {
    for (int16_t i = 0; i < len; i++) _tft_frame[i] = color;

    INTF_TFT_SET_WINDOW(x, y, x + len - 1, y);
    INTF_TFT_SEND_BUFF16(_tft_frame, len);
}

// render pixel - draw with CS controls
void _render_pixel(uint16_t x, uint16_t y, uint16_t color) {
    INTF_TFT_SET_WINDOW(x, y, x, y);
    INTF_TFT_SEND_PIXEL(color);
}

//# draw pixel
void tft_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    INT_TFT_CS_LOW();
    _render_pixel(x, y, color);
    INT_TFT_CS_HIGH();
}