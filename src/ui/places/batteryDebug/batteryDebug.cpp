#include "batteryDebug.h"
#include "rtcMem.h"
#include "ui/gfxGeneralPage.h"

#if DEBUG || DEBUG_MENUS

batteryInfo bDdata;

uint16_t lineCurrentVoltage;
uint16_t linePercentage;
uint16_t lineCharging;
uint16_t lineFullyCharged;

void initBatteryDebugDisplay()
{
    bDdata = rM.bat;

    gfxGeneralPageInit(7);
    gfxGeneralPageSetTitle(DEBUG_MENU_BATTERY);

    lineCurrentVoltage = gfxGeneralPageAddKeyValue(DEBUG_BATTERY_CURRENT_V, String(bDdata.curV).c_str());

    gfxGeneralPageAddKeyValue(DEBUG_BATTERY_MINIMUM_V, String(BATTERY_MIN_VOLTAGE).c_str());
    gfxGeneralPageAddKeyValue(DEBUG_BATTERY_MAXIMUM_V, String(BATTERY_MAX_VOLTAGE).c_str());
    gfxGeneralPageAddKeyValue(DEBUG_BATTERY_CRITICAL_V, String(BATTERY_CRIT_VOLTAGE).c_str());

    linePercentage = gfxGeneralPageAddKeyValue(DEBUG_BATTERY_LEVEL, String(bDdata.percentage).c_str());
    lineCharging = gfxGeneralPageAddKeyValue(DEBUG_BATTERY_CHARGING, BOOL_STR(bDdata.isCharging).c_str());

#if ATCHY_VER == YATCHY
    lineFullyCharged = gfxGeneralPageAddKeyValue(DEBUG_BATTERY_FULLY, BOOL_STR(bDdata.isFullyCharged).c_str());
#endif

    resetSleepDelay(SLEEP_EVERY_MS);
    gfxGeneralPageSetMain();
}

void loopBatteryDebugDisplay()
{
    loopBattery();
    isChargingCheck();

    if (bDdata.curV > rM.bat.curV + 0.01 || bDdata.curV < rM.bat.curV - 0.01)
    {
        bDdata.curV = rM.bat.curV;
        gfxGeneralPageChange(String(rM.bat.curV).c_str(), lineCurrentVoltage);
    }

    if (bDdata.isCharging != rM.bat.isCharging)
    {
        bDdata.isCharging = rM.bat.isCharging;
        gfxGeneralPageChange(BOOL_STR(rM.bat.isCharging).c_str(), lineCharging);
    }

#if ATCHY_VER == YATCHY
    if (bDdata.isFullyCharged != rM.bat.isFullyCharged)
    {
        bDdata.isFullyCharged = rM.bat.isFullyCharged;
        gfxGeneralPageChange(BOOL_STR(rM.bat.isFullyCharged).c_str(), lineFullyCharged);
    }
#endif

    if (bDdata.percentage != rM.bat.percentage)
    {
        bDdata.percentage = rM.bat.percentage;
        gfxGeneralPageChange(String(rM.bat.percentage).c_str(), linePercentage);
    }

    gfxGeneralPageSetMain();
    gfxGeneralPageLoop();
}

#endif