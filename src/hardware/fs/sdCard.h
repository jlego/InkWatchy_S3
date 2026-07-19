#pragma once

#include "defines.h"

#if BOOK_SD_CARD

#include <SD_MMC.h>

bool sdCardSetup(bool oneBitMode = true);
void sdCardEnd();
bool sdCardAvailable();
File sdCardOpen(String path);
bool sdCardExists(String path);
void sdCardMigrateBooks();

#endif