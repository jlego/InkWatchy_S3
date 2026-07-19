#include "clockDebug.h"

#if DEBUG_MENUS
#include "rtcMem.h"

uint16_t timeClockLine;

String zB(String str)
{
    while (str.length() < 2)
    {
        str = "0" + str;
    }
    return str;
}

String getClockPrecise()
{
    return zB(String(timeRTCLocal.Hour)) + ":" + zB(String(timeRTCLocal.Minute)) + ":" + zB(String(timeRTCLocal.Second));
}

String timeSince(int64_t unixTime)
{
    int64_t currentTime = getUnixTime(timeRTCUTC0);
    int64_t elapsedTime = currentTime - unixTime;

    int64_t days = elapsedTime / (24 * 3600);
    elapsedTime %= (24 * 3600);
    int64_t hours = elapsedTime / 3600;
    elapsedTime %= 3600;
    int64_t minutes = elapsedTime / 60;
    int64_t seconds = elapsedTime % 60;

    String output = String(days) + "d " + String(hours) + "h " + String(minutes) + "m " + String(seconds) + "s";
    return output;
}

void cleanClockFiles()
{
    fsRemoveFile("/conf/" + String(CONF_UNIX_LAST_SYNC));
    fsRemoveFile("/conf/" + String(CONF_UNIX_PREVIOUS_SYNC));
    fsRemoveFile("/conf/" + String(CONF_UNIX_LAST_CHARGE));
    initClockDebug();
}

#if TIME_DRIFT_CORRECTION
void resetDrift()
{
    fsRemoveFile("/conf/" + String(CONF_DRIFT));
    fsRemoveFile("/conf/" + String(CONF_DRIFT_FAST));
    rM.SRTC.setDrift(0, 0);
    rM.driftStartUnix = 0;
    rM.driftDone = false;
    initClockDebug();
}
#endif

void initClockDebug()
{
    gfxGeneralPageInit(7);
    gfxGeneralPageSetTitle(DEBUG_MENU_CLOCK);

#if TIME_DRIFT_CORRECTION
    GeneralPageButton buttons[2] = {GeneralPageButton{DEBUG_CLOCK_REMOVE_FILES, cleanClockFiles}, GeneralPageButton{"Undo drift fix", resetDrift}};
    gfxGeneralPageSetButtons(buttons, 2);
#else
    GeneralPageButton buttons[1] = {GeneralPageButton{DEBUG_CLOCK_REMOVE_FILES, cleanClockFiles}};
    gfxGeneralPageSetButtons(buttons, 1);
#endif

    readRTC();
    timeClockLine = gfxGeneralPageAdd(getClockPrecise().c_str());

    gfxGeneralPageAddKeyValue("Timezone:", rM.posixTimeZone);

    if (strlen(TIMEZONE_OLSON) != 0)
    {
        gfxGeneralPageAddKeyValue("Olson:", TIMEZONE_OLSON);
    }

    {
        String lastSync = fsGetString(CONF_UNIX_LAST_SYNC, "");
        debugLog("Last sync time: " + lastSync);
        if (lastSync != "")
        {
            lastSync = timeSince(lastSync.toInt());
        }
        else
        {
            lastSync = DEBUG_CLOCK_NOT_AVAILABLE;
        }
        gfxGeneralPageAddKeyValue(DEBUG_CLOCK_LAST_SYNC, lastSync.c_str());
    }
    {
        String lastSync = fsGetString(CONF_UNIX_PREVIOUS_SYNC, "");
        debugLog("Previous sync time: " + lastSync);
        if (lastSync != "")
        {
            lastSync = timeSince(lastSync.toInt());
        }
        else
        {
            lastSync = DEBUG_CLOCK_NOT_AVAILABLE;
        }
        gfxGeneralPageAddKeyValue(DEBUG_CLOCK_PREVIOUS_SYNC, lastSync.c_str());
    }
    {
        String lastSync = fsGetString(CONF_UNIX_LAST_CHARGE, "");
        debugLog("Last charge time: " + lastSync);
        if (lastSync != "")
        {
            lastSync = timeSince(lastSync.toInt());
        }
        else
        {
            lastSync = DEBUG_CLOCK_NOT_AVAILABLE;
        }
        gfxGeneralPageAddKeyValue(DEBUG_CLOCK_LAST_CHARGE, lastSync.c_str());
    }
#if TIME_DRIFT_CORRECTION
    {
        gfxGeneralPageAddKeyValue(DEBUG_CLOCK_DRIFT_SYNCS, fsGetString(CONF_SECONDS_DRIFT, DEBUG_CLOCK_NOT_AVAILABLE).c_str());
        gfxGeneralPageAddKeyValue(DEBUG_CLOCK_DRIFT_VALUES, ("D:" + fsGetString(CONF_DRIFT, "?") + " F:" + fsGetString(CONF_DRIFT_FAST, "?")).c_str());
    }
#endif
    gfxGeneralPageSetMain();
}

uint8_t savedSeconds = 0;
void loopClockDebug()
{
    if (gfxGeneralPageIsMenu() == false)
    {
        readRTC();
        if (savedSeconds != timeRTCLocal.Second)
        {
            savedSeconds = timeRTCLocal.Second;
            // debugLog("timeRTCLocal.Second: " + String(timeRTCLocal.Second));
            gfxGeneralPageChange(getClockPrecise().c_str(), timeClockLine);
        }
    }

    gfxGeneralPageSetMain();
    gfxGeneralPageLoop();
}

#endif