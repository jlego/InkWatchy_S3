#include "sdCard.h"

#if BOOK_SD_CARD

static bool sdMounted = false;

bool sdCardSetup(bool oneBitMode)
{
    if (sdMounted) return true;
    debugLog("Trying to mount SD card, mode: " + String(oneBitMode ? "1-bit" : "4-bit"));
#ifdef SD_EN
    pinMode(SD_EN, OUTPUT);
    digitalWrite(SD_EN, LOW);
    debugLog("SD_EN powered on (active low), waiting 300ms");
    delay(300);
#endif
    debugLog("SD pins: CLK=" + String(SD_CLK) + " CMD=" + String(SD_CMD) + " DATA0=" + String(SD_DATA0));
    SD_MMC.setPins(SD_CLK, SD_CMD, SD_DATA0);
    for (int attempt = 0; attempt < 3; attempt++)
    {
        debugLog("SD mount attempt " + String(attempt + 1));
        if (SD_MMC.begin("/sdcard", oneBitMode))
        {
            unsigned long totalMB = (unsigned long)(SD_MMC.cardSize() / (1024 * 1024));
            debugLog("SD card mounted, size: " + String(totalMB) + "MB");
            sdMounted = true;
            return true;
        }
        debugLog("SD mount attempt " + String(attempt + 1) + " failed");
        delay(500);
    }
    debugLog("All SD mount attempts failed");
#ifdef SD_EN
    digitalWrite(SD_EN, HIGH);
    debugLog("SD_EN powered off (active low)");
#endif
    sdMounted = false;
    return false;
}

void sdCardEnd()
{
    if (sdMounted)
    {
        SD_MMC.end();
        sdMounted = false;
#ifdef SD_EN
        digitalWrite(SD_EN, HIGH);
        debugLog("SD_EN powered off (active low)");
#endif
        debugLog("SD card unmounted");
    }
}

bool sdCardAvailable()
{
    return sdMounted;
}

File sdCardOpen(String path)
{
    if (!sdMounted) return File();
    File f = SD_MMC.open(path);
    if (!f)
    {
        debugLog("Failed to open SD file: " + path);
    }
    return f;
}

bool sdCardExists(String path)
{
    if (!sdMounted) return false;
    return SD_MMC.exists(path);
}

void sdCardMigrateBooks()
{
    if (!sdMounted) return;
    if (!SD_MMC.exists("/book"))
    {
        SD_MMC.mkdir("/book");
    }
    File srcRoot = LittleFS.open("/book/");
    if (!srcRoot || !srcRoot.isDirectory())
    {
        debugLog("No /book/ dir in LittleFS to migrate");
        return;
    }
    bool migrated = false;
    File file = srcRoot.openNextFile();
    while (file)
    {
        if (!file.isDirectory())
        {
            String name = String(file.name());
            int lastSlash = name.lastIndexOf('/');
            if (lastSlash >= 0) name = name.substring(lastSlash + 1);
            String dstPath = "/book/" + name;
            if (!SD_MMC.exists(dstPath))
            {
                debugLog("Migrating book: " + name);
                File dst = SD_MMC.open(dstPath, FILE_WRITE);
                if (dst)
                {
                    uint8_t buf[512];
                    while (file.available())
                    {
                        int len = file.read(buf, sizeof(buf));
                        dst.write(buf, len);
                    }
                    dst.close();
                    debugLog("Migrated: " + name);
                    migrated = true;
                }
                else
                {
                    debugLog("Failed to create SD file: " + dstPath);
                }
            }
            else
            {
                debugLog("Book already on SD: " + name);
                migrated = true;
            }
        }
        file = srcRoot.openNextFile();
    }
    srcRoot.close();
    if (migrated)
    {
        debugLog("Removing migrated books from LittleFS");
        File delRoot = LittleFS.open("/book/");
        if (delRoot && delRoot.isDirectory())
        {
            File delFile = delRoot.openNextFile();
            while (delFile)
            {
                if (!delFile.isDirectory())
                {
                    String delPath = String(delFile.path());
                    delFile.close();
                    LittleFS.remove(delPath);
                    debugLog("Deleted from LittleFS: " + delPath);
                }
                else
                {
                    delFile.close();
                }
                delFile = delRoot.openNextFile();
            }
            delRoot.close();
        }
    }
    debugLog("Book migration done");
}

#endif