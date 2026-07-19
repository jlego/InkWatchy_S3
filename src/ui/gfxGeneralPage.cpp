#include "gfxGeneralPage.h"
#include "ui/cnFont.h"
#include "ui/functionsUi.h"

struct GfxLine
{
    uint16_t id;
    String text;
    String value;
};

static GfxLine lines[GFX_GENPAGE_MAX_LINES];
static int lineCount = 0;
static uint16_t nextId = 0;
static String titleText = "";
static bool titleEnabled = true;
static bool centerAlign = false;
static bool centerVertical = false;
static bool gfxShowMenu = false;
static int currentItem = 0;
static GeneralPageButton buttons[GFX_GENPAGE_MAX_BUTTONS];
static int buttonCount = 0;
static int scrollOffset = 0;
static bool needsRedraw = true;
static int totalMenuItems = 0;

void gfxGeneralPageInit(int lineCapacity)
{
    lineCount = 0;
    nextId = 0;
    titleText = "";
    titleEnabled = true;
    centerAlign = false;
    centerVertical = false;
    gfxShowMenu = false;
    currentItem = 0;
    buttonCount = 0;
    scrollOffset = 0;
    needsRedraw = true;
    totalMenuItems = 0;
}

void gfxGeneralPageSetTitle(const char *title)
{
    titleText = String(title);
    needsRedraw = true;
}

void gfxGeneralPageDisableTitle()
{
    titleEnabled = false;
    needsRedraw = true;
}

uint16_t gfxGeneralPageAdd(const char *str)
{
    if (lineCount >= GFX_GENPAGE_MAX_LINES)
        return 0xFFFF;
    lines[lineCount].id = nextId;
    lines[lineCount].text = String(str);
    lines[lineCount].value = "";
    lineCount++;
    nextId++;
    needsRedraw = true;
    return nextId - 1;
}

uint16_t gfxGeneralPageAddKeyValue(const char *label, const char *val)
{
    if (lineCount >= GFX_GENPAGE_MAX_LINES)
        return 0xFFFF;
    lines[lineCount].id = nextId;
    lines[lineCount].text = String(label);
    lines[lineCount].value = String(val);
    lineCount++;
    nextId++;
    needsRedraw = true;
    return nextId - 1;
}

void gfxGeneralPageChange(const char *str, uint16_t lineId)
{
    for (int i = 0; i < lineCount; i++)
    {
        if (lines[i].id == lineId)
        {
            if (lines[i].text != String(str))
            {
                lines[i].text = String(str);
                needsRedraw = true;
            }
            return;
        }
    }
}

void gfxGeneralPageSetButtons(GeneralPageButton *btns, int count)
{
    buttonCount = 0;
    for (int i = 0; i < count && buttonCount < GFX_GENPAGE_MAX_BUTTONS; i++)
    {
        buttons[buttonCount++] = btns[i];
    }
    totalMenuItems = buttonCount + 1;
    needsRedraw = true;
}

static uint16_t calcLineHeight(const String &text, const String &val = "")
{
    uint16_t h = cnGetTextHeight(text);
    if (val.length() > 0)
    {
        uint16_t vh = cnGetTextHeight(val);
        if (vh > h)
            h = vh;
    }
    if (font != nullptr)
    {
        uint16_t fy = pgm_read_byte(&font->yAdvance);
        if (fy > h)
            h = fy;
    }
    if (cjkFont.glyphCount > 0 && cjkFont.yAdvance > h)
        h = cjkFont.yAdvance;
    if (h == 0)
        h = 14;
    return h;
}

static void renderPage()
{
    dis->fillScreen(SCWhite);
    setTextSize(1);
    setFont(&FreeSansBold9pt7b);

    int16_t y = 2;
    int16_t screenW = dis->width();
    int16_t screenH = dis->height();

    if (titleEnabled && titleText.length() > 0)
    {
        uint16_t tw = cnGetTextWidth(titleText);
        uint16_t th = calcLineHeight(titleText);
        int16_t tx = ((int16_t)screenW - (int16_t)tw) / 2;
        if (tx < 0)
            tx = 0;
        dis->setCursor(tx, y + th);
        cnPrint(dis, titleText);
        y += th + 3;
        dis->drawFastHLine(0, y, screenW, SCBlack);
        y += 3;
    }

    int16_t startY = y;
    int16_t availH = screenH - startY;

    if (centerVertical && lineCount > 0)
    {
        int totalH = 0;
        for (int i = 0; i < lineCount; i++)
        {
            uint16_t th = calcLineHeight(lines[i].text);
            totalH += th + 3;
        }
        if (totalH < availH)
        {
            startY += (availH - totalH) / 2;
        }
    }

    y = startY;
    for (int i = 0; i < lineCount; i++)
    {
        uint16_t th = calcLineHeight(lines[i].text, lines[i].value);

        if (y + th > screenH)
            break;

        if (lines[i].value.length() > 0)
        {
            dis->setCursor(2, y + th);
            cnPrint(dis, lines[i].text);
            uint16_t vw = cnGetTextWidth(lines[i].value);
            int16_t vx = (int16_t)screenW - (int16_t)vw - 2;
            if (vx < 2)
                vx = 2;
            dis->setCursor(vx, y + th);
            cnPrint(dis, lines[i].value);
        }
        else if (centerAlign)
        {
            uint16_t tw = cnGetTextWidth(lines[i].text);
            int16_t tx = ((int16_t)screenW - (int16_t)tw) / 2;
            if (tx < 0)
                tx = 0;
            dis->setCursor(tx, y + th);
            cnPrint(dis, lines[i].text);
        }
        else
        {
            dis->setCursor(2, y + th);
            cnPrint(dis, lines[i].text);
        }
        y += th + 3;
    }

    if (gfxShowMenu && buttonCount > 0)
    {
        int16_t menuW = 160;
        int16_t menuH = 12 + totalMenuItems * 20;
        int16_t menuX = (screenW - menuW) / 2;
        int16_t menuY = (screenH - menuH) / 2;

        dis->fillRect(menuX, menuY, menuW, menuH, SCWhite);
        dis->drawRect(menuX, menuY, menuW, menuH, SCBlack);

        int16_t by = menuY + 4;
        for (int i = 0; i < totalMenuItems; i++)
        {
            const char *label;
            if (i == 0)
                label = "exit";
            else
                label = buttons[i - 1].string;

            if (i == currentItem)
            {
                dis->fillRect(menuX + 2, by, menuW - 4, 18, SCBlack);
                dis->setTextColor(SCWhite);
            }
            else
            {
                dis->setTextColor(SCBlack);
            }

            uint16_t tw = cnGetTextWidth(String(label));
            int16_t tx = menuX + (menuW - tw) / 2;
            dis->setCursor(tx, by + 14);
            cnPrint(dis, String(label));
            dis->setTextColor(SCBlack);
            by += 20;
        }
    }

    disUp(true, false, true);
}

void gfxGeneralPageSetMain()
{
    if (needsRedraw)
    {
        needsRedraw = false;
        renderPage();
    }
}

bool gfxGeneralPageIsMenu()
{
    return gfxShowMenu;
}

void gfxGeneralPageSetCenter()
{
    centerAlign = true;
    needsRedraw = true;
}

void gfxGeneralPageSetCenterVertical()
{
    centerVertical = true;
    needsRedraw = true;
}

void gfxGeneralPageForceUpdate()
{
    needsRedraw = true;
}

void gfxGeneralPageLoop()
{
    resetSleepDelay();

    if (gfxShowMenu)
    {
        buttonState btn = useButton();
        switch (btn)
        {
        case Up:
        case LongUp:
            currentItem = (currentItem - 1 + totalMenuItems) % totalMenuItems;
            needsRedraw = true;
            break;
        case Down:
        case LongDown:
            currentItem = (currentItem + 1) % totalMenuItems;
            needsRedraw = true;
            break;
        case Menu:
        case LongMenu:
            if (currentItem == 0)
            {
                gfxShowMenu = false;
            }
            else if (currentItem - 1 < buttonCount)
            {
                buttons[currentItem - 1].function();
                gfxShowMenu = false;
            }
            needsRedraw = true;
            break;
        default:
            break;
        }
    }
    else
    {
        buttonState btn = useButton();
        if (btn == Menu || btn == LongMenu)
        {
            if (buttonCount > 0)
            {
                gfxShowMenu = true;
                currentItem = 0;
                needsRedraw = true;
            }
        }
        else if (btn == Up || btn == LongUp)
        {
            scrollOffset -= 20;
            if (scrollOffset < 0)
                scrollOffset = 0;
            needsRedraw = true;
        }
        else if (btn == Down || btn == LongDown)
        {
            scrollOffset += 20;
            needsRedraw = true;
        }
    }
    gfxGeneralPageSetMain();
}