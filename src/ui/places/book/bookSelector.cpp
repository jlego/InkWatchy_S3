#include "bookSelector.h"

#if BOOK

static String strToLower(const String &s)
{
    String r = s;
    for (unsigned int i = 0; i < r.length(); i++)
        if (r[i] >= 'A' && r[i] <= 'Z') r[i] += 32;
    return r;
}

static String getBookDisplayName(const String &sdName)
{
    String sdLower = strToLower(sdName);
    File mapFile = LittleFS.open("/book_map", FILE_READ);
    if (mapFile)
    {
        while (mapFile.available())
        {
            String line = mapFile.readStringUntil('\n');
            line.trim();
            int eq = line.indexOf('=');
            if (eq > 0)
            {
                String sName = strToLower(line.substring(0, eq));
                String dName = line.substring(eq + 1);
                if (sName == sdLower) { mapFile.close(); return dName; }
            }
        }
        mapFile.close();
    }
    return sdName;
}

static String getSdNameFromDisplay(const String &displayName)
{
    File mapFile = LittleFS.open("/book_map", FILE_READ);
    if (mapFile)
    {
        while (mapFile.available())
        {
            String line = mapFile.readStringUntil('\n');
            line.trim();
            int eq = line.indexOf('=');
            if (eq > 0)
            {
                String sName = line.substring(0, eq);
                String dName = line.substring(eq + 1);
                if (dName == displayName) { mapFile.close(); return sName; }
            }
        }
        mapFile.close();
    }
    return displayName;
}

void initBookSelector()
{
    debugLog("Current book: " + fsGetString(CONF_BOOK_CURRENT_BOOK, "", "/book/conf/"));
    resetSleepDelay(SLEEP_EVERY_MS);
#if BOOK_SD_CARD
    if (sdCardSetup() == false)
    {
        debugLog("SD card not available, trying LittleFS");
    }
    else
    {
        debugLog("SD card available for books");
    }
#endif
    int books = 0;
    {
#if BOOK_SD_CARD
        File root = sdCardAvailable() ? SD_MMC.open("/book/") : LittleFS.open("/book/");
#else
        File root = LittleFS.open("/book/");
#endif
        if (!root)
        {
            debugLog("Failed to open directory book");
            entryMenu emptyButtons[1];
            emptyButtons[0] = {"No books", "", nullptr};
            initMenu(emptyButtons, 1, MENU_SELECT_BOOK, 1);
            return;
        }
        if (root.isDirectory() == false)
        {
            debugLog("Not a directory book?");
            root.close();
            entryMenu emptyButtons[1];
            emptyButtons[0] = {"No books", "", nullptr};
            initMenu(emptyButtons, 1, MENU_SELECT_BOOK, 1);
            return;
        }
        File file = root.openNextFile();
        while (file)
        {
            if (file.isDirectory() == false)
            {
                books = books + 1;
            }
            file = root.openNextFile();
        }
        root.close();
    }
    debugLog("There are so many books: " + String(books));
    if (books == 0)
    {
        entryMenu emptyButtons[1];
        emptyButtons[0] = {"No books", "", nullptr};
        initMenu(emptyButtons, 1, MENU_SELECT_BOOK, 1);
        return;
    }
    entryMenu *buttons = new entryMenu[books];
#if BOOK_SD_CARD
    File root = sdCardAvailable() ? SD_MMC.open("/book/") : LittleFS.open("/book/");
#else
    File root = LittleFS.open("/book/");
#endif
    File file = root.openNextFile();
    uint8_t counter = 0;
    while (file)
    {
        if (file.isDirectory() == false)
        {
            String name = String(file.name());
            int lastSlash = name.lastIndexOf('/');
            if (lastSlash >= 0) name = name.substring(lastSlash + 1);
            String displayName = getBookDisplayName(name);
            buttons[counter] = {displayName, "", selectedBook};
            counter = counter + 1;
        }
        file = root.openNextFile();
    }
    root.close();
    initMenu(buttons, counter, MENU_SELECT_BOOK, 1);
    delete[] buttons;
}

void selectedBook()
{
    String latestBook = lastMenuSelected;
    String sdName = getSdNameFromDisplay(latestBook);
    debugLog("Called selectedBook: display=" + latestBook + " sd=" + sdName);
    fsSetString(CONF_BOOK_CURRENT_BOOK, sdName, "/book/conf/");
    resetBookVars();
    generalSwitch(book);
}
#endif