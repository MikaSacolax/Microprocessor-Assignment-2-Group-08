#ifndef WS2812_H
#define WS2812_H

#include <stdint.h>

void put_pixel(uint32_t pixel_grb);

uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);

#define GREEN urgb_u32(0x00, 0x7F, 0x00)
#define RED urgb_u32(0x7F, 0x00, 0x00)
#define YELLOW urgb_u32(0x7F, 0x7F, 0x00)
#define OFF urgb_u32(0x00, 0x00, 0x00)
#define BLUE urgb_u32(0x00, 0x00, 0x7F)
#define ORANGE urgb_u32(0xFF, 0xA5, 0x00)

#endif