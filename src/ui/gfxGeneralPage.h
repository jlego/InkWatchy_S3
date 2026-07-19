#pragma once

#include "defines.h"

#define GFX_GENPAGE_MAX_LINES 30
#define GFX_GENPAGE_MAX_BUTTONS 6

void gfxGeneralPageInit(int lineCapacity);
void gfxGeneralPageSetTitle(const char *title);
void gfxGeneralPageDisableTitle();
uint16_t gfxGeneralPageAdd(const char *str);
uint16_t gfxGeneralPageAddKeyValue(const char *label, const char *val);
void gfxGeneralPageChange(const char *str, uint16_t lineId);
void gfxGeneralPageSetButtons(GeneralPageButton *buttons, int count);
void gfxGeneralPageSetMain();
bool gfxGeneralPageIsMenu();
void gfxGeneralPageSetCenter();
void gfxGeneralPageSetCenterVertical();
void gfxGeneralPageForceUpdate();
void gfxGeneralPageLoop();