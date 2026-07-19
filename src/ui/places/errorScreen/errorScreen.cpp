#include "errorScreen.h"
#include "rtcMem.h"

String errorScreenStr = "";

void showErrorOnScreen(String err, bool holdBack)
{
    if (rM.currentPlace != errorDialog &&
        errorScreenStr != err)
    {
        debugLog("Showing ERROR on screen: " + err);
        errorScreenStr = err;

        gfxGeneralPageInit(1);
        gfxGeneralPageSetTitle("Error");
        gfxGeneralPageSetCenter();
        gfxGeneralPageAdd(err.c_str());
        gfxGeneralPageAdd("");
        if (!holdBack)
        {
            gfxGeneralPageAdd("If resolved, click back to exit");
        }
        else
        {
            gfxGeneralPageAdd("If resolved, hold back to exit");
        }
        gfxGeneralPageSetMain();

        generalSwitch(errorDialog);
    }
    else
    {
        debugLog("Error already shown: " + err);
    }
}

IWColors currentErrorColor = IWColors::IwRed;
uint32_t screenErrorMillis = 0;

void loopErrorScreen()
{
    if (millisBetter() - screenErrorMillis > 700)
    {
        screenErrorMillis = millisBetter();
        debugLog("Erroring: " + errorScreenStr);
        vibrateMotor();
        setRgb(currentErrorColor);
        if (currentErrorColor == IWColors::IwRed)
        {
            currentErrorColor = IWColors::IwNone;
        }
        else
        {
            currentErrorColor = IWColors::IwRed;
        }
    }
    resetSleepDelay();
    gfxGeneralPageLoop();
}

void exitErrorScreen()
{
    debugLog("Exiting error screen: " + errorScreenStr);
    errorScreenStr = "";
    setRgb(IWColors::IwNone);
}