// modified from https://github.com/adafruit/Adafruit-GFX-Library

#include "ch32fun.h"
#include "font_5x8.h"

#define ST7735_W    160

//###########################################
//# INTERFACES
//###########################################

void INT_TFT_CS_HIGH();
void INT_TFT_CS_LOW();

void INTF_TFT_SET_WINDOW(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void INTF_TFT_SEND_BUFF8(const uint8_t* buffer, uint16_t len);
void INTF_TFT_SEND_BUFF16(const uint16_t* buffer, uint16_t len);
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
        const char* glyph = &font_5x8[(c-32) * font_width];

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

//###########################################
//# OTHER METHODS
//###########################################

#define _diff(a, b)         ((a > b) ? (a - b) : (b - a))
#define _swap_int16(a, b)   { int16_t temp = a; a = b; b = temp; }

// draw line diagonal (use bresenham algorithm) - draw with CS controls
static void _render_diagonal_line(
    int16_t x0, int16_t y0,
    int16_t x1, int16_t y1, uint16_t color, uint8_t width
) {
    uint8_t steep = _diff(y1, y0) > _diff(x1, x0);
    if (steep) {
        _swap_int16(x0, y0);
        _swap_int16(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16(x0, x1);
        _swap_int16(y0, y1);
    }

    int16_t dx   = x1 - x0;
    int16_t dy   = _diff(y1, y0);
    int16_t err  = dx >> 1;
    int16_t step = (y0 < y1) ? 1 : -1;

    for (; x0 <= x1; x0++) {
        for (int16_t w = -(width / 2); w <= width / 2; w++) {
            if (steep) {
                _render_pixel(y0 + w, x0, color); // Draw perpendicular pixels for width
            } else {
                _render_pixel(x0, y0 + w, color); // Draw perpendicular pixels for width
            }
        }
        err -= dy;
        if (err < 0) {
            err += dx;
            y0 += step;
        }
    }
}

// render line - draw with CS controls
static void _render_line(
    int16_t x0, int16_t y0, int16_t x1, int16_t y1,
    uint16_t color, uint8_t width
) {
    INT_TFT_CS_LOW();
    if (x0 == x1) {
        if (y0 > y1) _swap_int16(y0, y1);
        _render_vertical_line(x0, y0, y1 - y0 + 1, color);
    }
    else if (y0 == y1) {
        if (x0 > x1) _swap_int16(x0, x1);
        _render_horizontal_line(x0, y0, x1 - x0 + 1, color);
    }
    else {
        _render_diagonal_line(x0, y0, x1, y1, color, width);
    }
    INT_TFT_CS_HIGH();
}

//# draw line
void tft_draw_line(
    int16_t x0, int16_t y0, int16_t x1, int16_t y1,
    uint16_t color, uint8_t width
) {
    INT_TFT_CS_LOW();
    _render_line(x0, y0, x1, y1, color, width);
    INT_TFT_CS_HIGH();
}


//# draw rectangle
void tft_draw_rect(
    uint16_t x, uint16_t y,
    uint16_t width, uint16_t height, uint16_t color
) {
    INT_TFT_CS_LOW();
    _render_horizontal_line(x, y, width, color);
    _render_horizontal_line(x, y + height - 1, width, color);
    _render_vertical_line(x, y, height, color);
    _render_vertical_line(x + width - 1, y, height, color);
    INT_TFT_CS_HIGH();
}


typedef struct {
    int16_t x; // X-coordinate
    int16_t y; // Y-coordinate
} Point16_t;

//# draw polygon
static void tft_draw_poly(
    const int16_t* vertices_x, // Array of x-coordinates of vertices
    const int16_t* vertices_y, // Array of y-coordinates of vertices
    uint16_t num_vertices,     // Number of vertices in the polygon
    uint16_t color,            // Color of the polygon edges
    uint8_t width              // Width of the edges
) {
    if (num_vertices < 3) return; // A polygon must have at least 3 vertices

    for (uint16_t i = 0; i < num_vertices; i++) {
        int16_t x0 = vertices_x[i];
        int16_t y0 = vertices_y[i];
        int16_t x1 = vertices_x[(i + 1) % num_vertices]; // Wrap around to connect last vertex to first
        int16_t y1 = vertices_y[(i + 1) % num_vertices];

        tft_draw_line(x0, y0, x1, y1, color, width); // Draw edge with specified width
    }
}

//# draw polygon
static void tft_draw_poly2(
    const Point16_t* vertices, uint16_t num_vertices,
    uint16_t color, uint8_t width
) {
    if (num_vertices < 3) return; // A polygon must have at least 3 vertices

    for (uint16_t i = 0; i < num_vertices; i++) {
        Point16_t p0 = vertices[i];
        Point16_t p1 = vertices[(i + 1) % num_vertices]; // Wrap around to connect last vertex to first

        tft_draw_line(p0.x, p0.y, p1.x, p1.y, color, width); // Draw edge with specified width
    }
}

//# draw solid polygon
static void tft_draw_solid_poly(
    const Point16_t* vertices, uint16_t num_vertices,
    uint16_t fill_color, uint16_t edge_color, uint8_t edge_width
) {
    if (num_vertices < 3) return;  // A polygon must have at least 3 vertices

    // Find the bounding box of the polygon
    int16_t min_y = vertices[0].y, max_y = vertices[0].y;
    
    for (uint16_t i = 1; i < num_vertices; i++) {
        if (vertices[i].y < min_y) min_y = vertices[i].y;
        if (vertices[i].y > max_y) max_y = vertices[i].y;
    }

    // Scan through each row of the polygon
    for (int16_t y = min_y; y <= max_y; y++) {
        // Find all intersections with the polygon edges
        int16_t intersections[20]; // Adjust size as needed
        uint8_t num_intersections = 0;

        for (uint16_t i = 0; i < num_vertices; i++) {
            Point16_t p0 = vertices[i];
            Point16_t p1 = vertices[(i + 1) % num_vertices];

            // Skip horizontal edges
            if (p0.y == p1.y) continue;

            // Check if y is between the edge's y range
            if ((y >= p0.y && y < p1.y) || (y >= p1.y && y < p0.y)) {
                // Calculate intersection x coordinate
                int16_t x = p0.x + ((int32_t)(y - p0.y) * (p1.x - p0.x)) / (p1.y - p0.y);
                intersections[num_intersections++] = x;
            }
        }

        // OPTIMIZATION #1: Replace bubble sort with insertion sort
        for (uint8_t i = 1; i < num_intersections; i++) {
            int16_t key = intersections[i];
            int8_t j = i - 1;
            while (j >= 0 && intersections[j] > key) {
                intersections[j + 1] = intersections[j];
                j--;
            }
            intersections[j + 1] = key;
        }

        // Fill between pairs of intersections
        for (uint8_t i = 0; i < num_intersections; i += 2) {
            if (i + 1 >= num_intersections) break;
            int16_t x0 = intersections[i];
            int16_t x1 = intersections[i + 1];
            
            // Draw horizontal line between intersections
            tft_draw_line(x0, y, x1, y, fill_color, 1);
        }
    }
    
    // Optionally draw the edges
    if (edge_width > 0) {
        tft_draw_poly2(vertices, num_vertices, edge_color, edge_width); // Draw edges with specified width
    }
}


//# optimized draw solid polygon
static void tft_draw_solid_poly2(
    const Point16_t* vertices, uint16_t num_vertices,
    uint16_t fill_color, uint16_t edge_color, uint8_t edge_width
) {
    if (num_vertices < 3) return;  // A polygon must have at least 3 vertices

    // Find the bounding box of the polygon
    int16_t min_y = vertices[0].y, max_y = vertices[0].y;
    
    for (uint16_t i = 1; i < num_vertices; i++) {
        if (vertices[i].y < min_y) min_y = vertices[i].y;
        if (vertices[i].y > max_y) max_y = vertices[i].y;
    }

    // OPTIMIZATION #2: Precompute edge information
    typedef struct {
        int16_t y_min, y_max;
        int32_t x_step;  // Fixed-point slope (dx/dy)
        int32_t x_curr;  // Fixed-point current x
    } EdgeInfo;
    
    EdgeInfo edges[num_vertices];
    uint8_t valid_edges = 0;
    
    for (uint16_t i = 0; i < num_vertices; i++) {
        const Point16_t* p0 = &vertices[i];
        const Point16_t* p1 = &vertices[(i + 1) % num_vertices];
        
        if (p0->y == p1->y) continue; // Skip horizontal edges
        
        // Order vertices top to bottom
        int16_t y_min, y_max, x_start;
        if (p0->y < p1->y) {
            y_min = p0->y;
            y_max = p1->y;
            x_start = p0->x;
            edges[valid_edges].x_curr = x_start << 16; // Fixed-point init
            edges[valid_edges].x_step = ((int32_t)(p1->x - p0->x) << 16) / (p1->y - p0->y);
        } else {
            y_min = p1->y;
            y_max = p0->y;
            x_start = p1->x;
            edges[valid_edges].x_curr = x_start << 16;
            edges[valid_edges].x_step = ((int32_t)(p0->x - p1->x) << 16) / (p0->y - p1->y);
        }
        
        edges[valid_edges].y_min = y_min;
        edges[valid_edges].y_max = y_max;
        valid_edges++;
    }

    // Scan through each row of the polygon
    for (int16_t y = min_y; y <= max_y; y++) {
        int16_t intersections[20];
        uint8_t num_intersections = 0;

        // Find active edges
        for (uint8_t i = 0; i < valid_edges; i++) {
            if (y >= edges[i].y_min && y < edges[i].y_max) {
                // Calculate x intersection (with rounding)
                intersections[num_intersections++] = (edges[i].x_curr + (1 << 15)) >> 16;
                // Update x for next scanline
                edges[i].x_curr += edges[i].x_step;
            }
        }

        // Insertion sort (from previous optimization)
        for (uint8_t i = 1; i < num_intersections; i++) {
            int16_t key = intersections[i];
            int8_t j = i - 1;
            while (j >= 0 && intersections[j] > key) {
                intersections[j + 1] = intersections[j];
                j--;
            }
            intersections[j + 1] = key;
        }

        // Fill between pairs
        for (uint8_t i = 0; i < num_intersections; i += 2) {
            if (i + 1 >= num_intersections) break;
            int16_t x0 = intersections[i];
            int16_t x1 = intersections[i + 1];
            if (x1 > x0) {
                tft_draw_line(x0, y, x1, y, fill_color, 1);
            }
        }
    }
    
    // Optionally draw the edges
    if (edge_width > 0) {
        tft_draw_poly2(vertices, num_vertices, edge_color, edge_width); // Draw edges with specified width
    }
}

//# draw circle
static void tft_draw_circle(
    Point16_t center, int16_t radius, uint16_t color
) {
    int16_t x = 0;
    int16_t y = radius;
    int16_t err = 1 - radius; // Initial error term

    INT_TFT_CS_LOW();
    
    while (x <= y) {
        // Draw symmetric points in all octants
        _render_pixel(center.x + x, center.y + y, color); // Octant 1
        _render_pixel(center.x - x, center.y + y, color); // Octant 2
        _render_pixel(center.x + x, center.y - y, color); // Octant 3
        _render_pixel(center.x - x, center.y - y, color); // Octant 4
        _render_pixel(center.x + y, center.y + x, color); // Octant 5
        _render_pixel(center.x - y, center.y + x, color); // Octant 6
        _render_pixel(center.x + y, center.y - x, color); // Octant 7
        _render_pixel(center.x - y, center.y - x, color); // Octant 8

        if (err < 0) {
            err += 2 * x + 3;
        } else {
            err += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }

    INT_TFT_CS_HIGH();
}


//# draw filled circle
static void tft_draw_filled_circle(
    Point16_t p0, int16_t radius, uint16_t color
) {
    int16_t x = radius;
    int16_t y = 0;
    int16_t err = 0;

    INT_TFT_CS_LOW();

    //# optimize for radius <= 4
    if (radius <= 4) {
        _render_horizontal_line(p0.x - radius, p0.y, radius + radius + 1, color);
        for (int16_t i = 1; i <= radius; i++) {
            int16_t w = (radius - i) + (radius - i) + 1;
            _render_horizontal_line(p0.x - (radius - i), p0.y + i, w, color);
            _render_horizontal_line(p0.x - (radius - i), p0.y - i, w, color);
        }
        return;
    }

    while (x >= y) {
        // Draw horizontal spans for each octant
        _render_horizontal_line(p0.x - x, p0.y + y, 2 * x + 1, color);  // Bottom span
        _render_horizontal_line(p0.x - x, p0.y - y, 2 * x + 1, color);  // Top span
        _render_horizontal_line(p0.x - y, p0.y + x, 2 * y + 1, color);  // Right span
        _render_horizontal_line(p0.x - y, p0.y - x, 2 * y + 1, color);  // Left span

        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }

    INT_TFT_CS_HIGH();
}

//# draw ring
static void tft_draw_ring(
    Point16_t center, int16_t radius, uint16_t color, uint8_t width
) {
    tft_draw_filled_circle(center, radius, color); // Draw outer circle
    // tft_draw_filled_circle(center, radius - width, PURPLE); // Draw inner circle
}