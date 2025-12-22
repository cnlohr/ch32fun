// MIT License
// Copyright (c) 2025 UniTheCat

#include "font_5x8.h"

#define ST7735_W    160

//###########################################
//# INTERFACES
//###########################################

void TFT_CS_HIGH();
void TFT_CS_LOW();

void TFT_SET_WINDOW(int x0, int y0, int x1, int y1);
void TFT_SEND_BUFF8(const u8* buffer, int len);
void TFT_SEND_BUFF16(u16* buffer, int len);
void TFT_SEND_PIXEL(u16 color);
void TFT_SELECT_SPI_DEV(void *spi_dev);


//###########################################
//# BASE METHODS
//###########################################

typedef struct {
    unsigned char* data;
    int width;
    int height;
    int spacing;
} TFT_font_t;

TFT_font_t default_font = { font_5x8, 5, 8, 1 };

u16 _tft_frame[ST7735_W] = {0};

//# draw text
void tft_print(
    TFT_font_t *font,const char *str, int x, int y,
    u16 color, u16 bg_color
) {
    int current_x = x, current_y = y;
    
    TFT_CS_LOW();

    while (*str) {
        char c = *str++;
        int len = 0;
        const char* glyph = &font->data[(c-32) * font->width];

        for (int y = font->height - 1; y >= 0; y--) {
            u8 mask = 0x01 << y;
            
            for (int x = 0; x < font->width; x++) {
                _tft_frame[len++] = (glyph[x] & mask) ? color : bg_color;
            }
        }

        TFT_SET_WINDOW(
            current_x,
            current_y,
            current_x + font->width - 1,
            current_y + font->height - 1
        );
        TFT_SEND_BUFF16(_tft_frame, len);
        current_x += font->width + font->spacing;
    }

    TFT_CS_HIGH();
}

//# draw filled_rect
void tft_fill_rect(
    int x, int y,
    int width, int height, int color
) {
    TFT_CS_LOW();
    TFT_SET_WINDOW(x, y, x + width - 1, y + height - 1);

    for (int i = 0; i < width; i++) _tft_frame[i] = color;

    while(height-- > 0) {
        TFT_SEND_BUFF16(_tft_frame, width);
    }
    
    TFT_CS_HIGH();
}


// render vertical line - draw with CS controls
static void _render_vertical_line(
    int x, int y,
    int len, u16 color
) {
    for (int i = 0; i < len; i++) _tft_frame[i] = color;

    TFT_SET_WINDOW(x, y, x, y + len - 1);
    TFT_SEND_BUFF16(_tft_frame, len);
}


// render horizontal line - draw with CS controls
static void _render_horizontal_line(
    int x, int y,
    int len, u16 color
) {
    for (int i = 0; i < len; i++) _tft_frame[i] = color;

    TFT_SET_WINDOW(x, y, x + len - 1, y);
    TFT_SEND_BUFF16(_tft_frame, len);
}

// render pixel - draw with CS controls
void _render_pixel(int x, int y, u16 color) {
    TFT_SET_WINDOW(x, y, x, y);
    TFT_SEND_PIXEL(color);
}

//# draw pixel
void tft_draw_pixel(int x, int y, u16 color) {
    TFT_CS_LOW();
    _render_pixel(x, y, color);
    TFT_CS_HIGH();
}