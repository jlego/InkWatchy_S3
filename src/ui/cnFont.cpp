#include "cnFont.h"
#include "ui/functionsUi.h"
#include "ui/functionsUi.h"

CjkFontData cjkFont = {0};

#define GLYPH_SIZE 10

static File cjkFontFile;
static uint8_t *cjkGlyphBuf = nullptr;
static uint8_t *cjkCpBuf = nullptr;

static inline uint32_t glyphBitmapOffset(int16_t idx) {
    uint8_t *p = cjkGlyphBuf + idx * GLYPH_SIZE;
    return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}
static inline uint8_t glyphWidth(int16_t idx) {
    return cjkGlyphBuf[idx * GLYPH_SIZE + 4];
}
static inline uint8_t glyphHeight(int16_t idx) {
    return cjkGlyphBuf[idx * GLYPH_SIZE + 5];
}
static inline uint8_t glyphXAdvance(int16_t idx) {
    return cjkGlyphBuf[idx * GLYPH_SIZE + 6];
}
static inline int8_t glyphXOffset(int16_t idx) {
    return (int8_t)cjkGlyphBuf[idx * GLYPH_SIZE + 7];
}
static inline int8_t glyphYOffset(int16_t idx) {
    return (int8_t)cjkGlyphBuf[idx * GLYPH_SIZE + 8];
}

static inline uint16_t readCodepoint(int16_t idx) {
    uint8_t *p = cjkCpBuf + idx * 2;
    return p[0] | (p[1] << 8);
}

static int16_t findCjkGlyph(uint16_t codepoint)
{
    if (cjkFont.glyphCount == 0) return -1;
    int16_t lo = 0, hi = cjkFont.glyphCount - 1;
    while (lo <= hi)
    {
        int16_t mid = (lo + hi) / 2;
        uint16_t cp = readCodepoint(mid);
        if (cp == codepoint) return mid;
        if (cp < codepoint) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

void cnFontClose()
{
    if (cjkFontFile)
    {
        cjkFontFile.close();
        debugLog("CJK font file closed");
    }
}

void cnFontReopen()
{
    if (cjkFont.glyphCount == 0) return;
    if (cjkFontFile)
    {
        cjkFontFile.close();
    }
    cjkFontFile = LittleFS.open("/font/NotoSansSC16");
    if (!cjkFontFile)
    {
        debugLog("Failed to reopen CJK font file");
    }
    else
    {
        debugLog("CJK font file reopened");
    }
}

static bool readBitmapBytes(uint32_t offset, uint8_t *dst, uint16_t len)
{
    if (!cjkFontFile)
    {
        cnFontReopen();
        if (!cjkFontFile) return false;
    }
    if (offset > cjkFont.bitmapSize) return false;
    cjkFontFile.seek(4 + offset);
    size_t bytesRead = cjkFontFile.read(dst, len);
    if (bytesRead != len)
    {
        Serial.printf("CJK bitmap read fail: offset=%u len=%u read=%u\n", offset, len, bytesRead);
        cnFontReopen();
        cjkFontFile.seek(4 + offset);
        bytesRead = cjkFontFile.read(dst, len);
        if (bytesRead != len)
        {
            Serial.printf("CJK bitmap read fail after reopen\n");
            return false;
        }
    }
    return true;
}

void initCnFont()
{
    if (fsSetup() == false)
       {
        debugLog("Failed to setup fs for CJK font");
        return;
    }
    debugLog("CJK: Free heap before load: " + String(ESP.getFreeHeap()));

    cjkFontFile = LittleFS.open("/font/NotoSansSC16");
    if (!cjkFontFile)
    {
        debugLog("Failed to open CJK font file");
        return;
    }

    uint8_t sizeBuf[4];
    if (cjkFontFile.read(sizeBuf, 4) != 4)
    {
        debugLog("Failed to read CJK font header");
        cjkFontFile.close();
        return;
    }

    cjkFont.bitmapSize = sizeBuf[0] | (sizeBuf[1] << 8) | (sizeBuf[2] << 16) | (sizeBuf[3] << 24);

    cjkFontFile.seek(4 + cjkFont.bitmapSize);
    uint8_t countBuf[2];
    if (cjkFontFile.read(countBuf, 2) != 2)
    {
        debugLog("Failed to read CJK glyph count");
        cjkFontFile.close();
        return;
    }
    cjkFont.glyphCount = countBuf[0] | (countBuf[1] << 8);
    debugLog("CJK: bitmapSize=" + String(cjkFont.bitmapSize) + " glyphCount=" + String(cjkFont.glyphCount));

    size_t glyphDataSize = (size_t)cjkFont.glyphCount * GLYPH_SIZE;
    size_t cpDataSize = (size_t)cjkFont.glyphCount * 2;
    debugLog("CJK: glyphData=" + String(glyphDataSize) + " cpData=" + String(cpDataSize) + " total=" + String(glyphDataSize + cpDataSize));

    cjkGlyphBuf = (uint8_t *)malloc(glyphDataSize);
    cjkCpBuf = (uint8_t *)malloc(cpDataSize);
    if (!cjkGlyphBuf || !cjkCpBuf)
    {
        debugLog("CJK: Failed to alloc index, heap=" + String(ESP.getFreeHeap()));
        if (cjkGlyphBuf) { free(cjkGlyphBuf); cjkGlyphBuf = nullptr; }
        if (cjkCpBuf) { free(cjkCpBuf); cjkCpBuf = nullptr; }
        cjkFontFile.close();
        return;
    }

    size_t glyphOffset = 4 + cjkFont.bitmapSize + 2;
    cjkFontFile.seek(glyphOffset);
    if (cjkFontFile.read(cjkGlyphBuf, glyphDataSize) != (int)glyphDataSize)
    {
        debugLog("CJK: Failed to read glyph data");
        free(cjkGlyphBuf); cjkGlyphBuf = nullptr;
        free(cjkCpBuf); cjkCpBuf = nullptr;
        cjkFontFile.close();
        return;
    }

    size_t cpOffset = glyphOffset + glyphDataSize;
    cjkFontFile.seek(cpOffset);
    if (cjkFontFile.read(cjkCpBuf, cpDataSize) != (int)cpDataSize)
    {
        debugLog("CJK: Failed to read codepoint data");
        free(cjkGlyphBuf); cjkGlyphBuf = nullptr;
        free(cjkCpBuf); cjkCpBuf = nullptr;
        cjkFontFile.close();
        return;
    }

    size_t yaOffset = cpOffset + cpDataSize;
    cjkFontFile.seek(yaOffset);
    uint8_t ya;
    cjkFontFile.read(&ya, 1);
    cjkFont.yAdvance = ya;

    cjkFont.glyphData = cjkGlyphBuf;
    cjkFont.codepoints = cjkCpBuf;
    cjkFont.rawBuf = nullptr;
    cjkFont.bitmap = nullptr;

    debugLog("CJK font loaded, glyphs: " + String(cjkFont.glyphCount) + ", bitmap: " + String(cjkFont.bitmapSize) + ", heap: " + String(ESP.getFreeHeap()));
    debugLog("CJK first cp: " + String(readCodepoint(0)) + ", yAdv: " + String(cjkFont.yAdvance));
    debugLog("CJK glyph0: w=" + String(glyphWidth(0)) + " h=" + String(glyphHeight(0)) + " adv=" + String(glyphXAdvance(0)));
    debugLog("CJK glyph0: bo=" + String(glyphBitmapOffset(0)) + " xo=" + String(glyphXOffset(0)) + " yo=" + String(glyphYOffset(0)));
}

static int cjkDrawCount = 0;
static void drawCjkGlyph(Adafruit_GFX *g, int16_t idx, int16_t x, int16_t y, uint16_t color)
{
    uint32_t bo = glyphBitmapOffset(idx);
    uint8_t w = glyphWidth(idx);
    uint8_t h = glyphHeight(idx);
    int8_t xo = glyphXOffset(idx);
    int8_t yo = glyphYOffset(idx);
    if (w == 0 || h == 0) return;
    if (cjkDrawCount < 3) {
        Serial.printf("CJK draw[%d]: idx=%d w=%d h=%d bo=%u\n", cjkDrawCount, idx, w, h, bo);
        cjkDrawCount++;
    }
    uint16_t bitmapBytes = ((uint16_t)w * h + 7) / 8;
    uint8_t *glyphBitmap = (uint8_t *)malloc(bitmapBytes);
    if (!glyphBitmap) return;
    if (!readBitmapBytes(bo, glyphBitmap, bitmapBytes))
    {
        free(glyphBitmap);
        return;
    }
    g->startWrite();
    for (uint8_t yy = 0; yy < h; yy++)
    {
        for (uint8_t xx = 0; xx < w; xx++)
        {
            uint16_t bitIdx = (uint16_t)yy * w + xx;
            uint8_t bitPos = 7 - (bitIdx % 8);
            if (glyphBitmap[bitIdx / 8] & (1 << bitPos))
                g->writePixel(x + xo + xx, y + yo + yy, color);
        }
    }
    g->endWrite();
    free(glyphBitmap);
}

static uint8_t cjkGlyphAdvance(int16_t idx)
{
    if (idx < 0 || idx >= cjkFont.glyphCount) return 0;
    return glyphXAdvance(idx);
}

static uint16_t decodeUtf8(const String &str, int i, uint8_t &bytes)
{
    uint8_t c = str[i];
    uint16_t codepoint = 0;
    if ((c & 0xE0) == 0xC0) { codepoint = c & 0x1F; bytes = 2; }
    else if ((c & 0xF0) == 0xE0) { codepoint = c & 0x0F; bytes = 3; }
    else if ((c & 0xF8) == 0xF0) { codepoint = c & 0x07; bytes = 4; }
    else { bytes = 1; return 0; }
    for (uint8_t j = 1; j < bytes && i + j < (int)str.length(); j++)
    {
        if ((str[i + j] & 0xC0) == 0x80)
            codepoint = (codepoint << 6) | (str[i + j] & 0x3F);
        else
            { bytes = j + 1; return 0; }
    }
    return codepoint;
}

void cnPrint(Adafruit_GFX *g, String str, uint16_t color)
{
    if (cjkFont.glyphCount == 0) {
        g->print(str);
        return;
    }
    int i = 0;
    bool hasCjk = false;
    while (i < (int)str.length())
    {
        uint8_t c = str[i];
        if (c < 0x80)
        {
            g->print((char)c);
            i++;
        }
        else
        {
            hasCjk = true;
            uint8_t bytes;
            uint16_t cp = decodeUtf8(str, i, bytes);
            if (cp > 0)
            {
                int16_t idx = findCjkGlyph(cp);
                if (idx >= 0)
                {
                    int16_t cx = g->getCursorX();
                    int16_t cy = g->getCursorY();
                    drawCjkGlyph(g, idx, cx, cy, color);
                    g->setCursor(cx + cjkGlyphAdvance(idx), cy);
                }
                else
                {
                    Serial.printf("CJK NOT FOUND: U+%04X\n", cp);
                    int16_t cx = g->getCursorX();
                    int16_t cy = g->getCursorY();
                    uint8_t adv = cjkFont.yAdvance > 0 ? cjkFont.yAdvance : 16;
                    g->setCursor(cx + adv, cy);
                }
            }
            else
            {
                Serial.printf("CJK decode failed at i=%d c=0x%02X\n", i, c);
            }
            i += bytes;
        }
    }
    if (hasCjk) {
        Serial.printf("cnPrint done: '%s' cursor=%d,%d color=%d\n", str.c_str(), g->getCursorX(), g->getCursorY(), color);
    }
}

uint16_t cnGetTextWidth(String str)
{
    uint16_t totalW = 0;
    int i = 0;
    while (i < (int)str.length())
    {
        uint8_t c = str[i];
        if (c < 0x80)
        {
            if (font)
            {
                uint8_t first = pgm_read_byte(&font->first);
                uint8_t last = pgm_read_byte(&font->last);
                if (c >= first && c <= last)
                {
                    GFXglyph *glyph = &font->glyph[c - first];
                    totalW += pgm_read_byte(&glyph->xAdvance);
                }
                else
                {
                    totalW += 6;
                }
            }
            else
            {
                totalW += 6;
            }
            i++;
        }
        else
        {
            uint8_t bytes;
            uint16_t cp = decodeUtf8(str, i, bytes);
            if (cp > 0)
            {
                int16_t idx = findCjkGlyph(cp);
                if (idx >= 0) totalW += cjkGlyphAdvance(idx);
                else totalW += (cjkFont.yAdvance > 0 ? cjkFont.yAdvance : 16);
            }
            i += bytes;
        }
    }
    return totalW;
}

uint16_t cnGetTextHeight(String str)
{
    bool hasCjk = false;
    int i = 0;
    while (i < (int)str.length())
    {
        if ((uint8_t)str[i] >= 0x80)
        {
            hasCjk = true;
            break;
        }
        i++;
    }
    if (!hasCjk || cjkFont.glyphCount == 0)
        return 0;
    int16_t minY = 0;
    int16_t maxY = 0;
    i = 0;
    while (i < (int)str.length())
    {
        uint8_t c = str[i];
        if (c < 0x80)
        {
            i++;
            continue;
        }
        uint8_t bytes;
        uint16_t cp = decodeUtf8(str, i, bytes);
        if (cp > 0)
        {
            int16_t idx = findCjkGlyph(cp);
            if (idx >= 0)
            {
                int8_t yo = glyphYOffset(idx);
                uint8_t h = glyphHeight(idx);
                if (yo < minY) minY = yo;
                if (yo + (int8_t)h > maxY) maxY = yo + (int8_t)h;
            }
        }
        i += bytes;
    }
    return (uint16_t)(maxY - minY);
}