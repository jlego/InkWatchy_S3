#pragma once

#include "defines.h"

void initCnFont();
void cnFontClose();
void cnFontReopen();
void cnPrint(Adafruit_GFX *g, String str, uint16_t color = 0);
uint16_t cnGetTextWidth(String str);
uint16_t cnGetTextHeight(String str);

struct CjkFontData {
    uint8_t *rawBuf;
    uint32_t bitmapSize;
    uint8_t *bitmap;
    uint16_t glyphCount;
    uint8_t *glyphData;
    uint8_t *codepoints;
    uint8_t yAdvance;
};

extern CjkFontData cjkFont;