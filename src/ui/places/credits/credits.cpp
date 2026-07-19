#include "credits.h"

#if CREDITS

#define CREDITS_INKWATCHY_QR "InkWatchy QR"
#define CREDITS_YATCHY_QR "Yatchy QR"
#define CREDITS_ICON "Logo"

bool isQrOn = false;
int qrToShow = 0;

#define NO_PAGE -1
#define ICON_PAGE 0
#define QR_INKWATCHY_PAGE 1
#define QR_YATCHY_PAGE 2

void turnInkWatchyQr()
{
    isQrOn = true;
    qrToShow = QR_INKWATCHY_PAGE;
}

void turnYatchyQr()
{
    isQrOn = true;
    qrToShow = QR_YATCHY_PAGE;
}

void turnIcon()
{
    isQrOn = true;
    qrToShow = ICON_PAGE;
}

void initCredits()
{
    isQrOn = false;

    gfxGeneralPageInit(10);
    gfxGeneralPageSetTitle(MENU_CREDITS);

    gfxGeneralPageAdd("Created by:");
    gfxGeneralPageAdd("Szybet");
    gfxGeneralPageAdd("github.com/Szybet");
    gfxGeneralPageAdd("Helped:");
    gfxGeneralPageAdd(get_credits_nicks());

    GeneralPageButton buttons[] = {
        GeneralPageButton{CREDITS_INKWATCHY_QR, turnInkWatchyQr},
        GeneralPageButton{CREDITS_YATCHY_QR, turnYatchyQr},
        GeneralPageButton{CREDITS_ICON, turnIcon}};
    gfxGeneralPageSetButtons(buttons, 3);

    gfxGeneralPageSetMain();
}

void loopCredits()
{
    if (isQrOn == false)
    {
        gfxGeneralPageLoop();
    }
    else
    {
        if (useButton() != None)
        {
            isQrOn = false;
            gfxGeneralPageForceUpdate();
            return;
        }
        if (qrToShow != NO_PAGE)
        {
            dis->fillScreen(SCWhite);
        }
        if (qrToShow == QR_INKWATCHY_PAGE)
        {
            writeImageN(10, 10, getImg("credits/inkwatchy_qr"));
        }
        else if (qrToShow == QR_YATCHY_PAGE)
        {
            writeImageN(10, 10, getImg("credits/yatchy_qr"));
        }
        else if (qrToShow == ICON_PAGE)
        {
            writeImageN(0, 0, getImg("credits/inkwatchy_icon"));
        }
        if (qrToShow != NO_PAGE)
        {
            qrToShow = NO_PAGE;
            dUChange = true;
        }
        disUp();
    }
    resetSleepDelay();
}

#endif