#include "wifiDebug.h"

int statusLine;
int ipLine;
int ssidLine;
int signalStrengthLine;
int taskStatusLine;
int macAddressLine;

String previousMac;
String wifiStatusLastStr;
String taskStatusLastStr;
int wifiSignalLast;

#define EMPTY_MAC "00:00:00:00:00:00"
#define EMPTY_IP "0.0.0.0"

void wifiOnBtn()
{
    turnOnWifiRegular();
    vibrateMotor(VIBRATION_ACTION_TIME);
}

void wifiOffBtn()
{
    turnOffWifi();
    vibrateMotor(VIBRATION_ACTION_TIME);
}

void wifiNtpBtn()
{
    turnOnWifiNtpOnly();
    vibrateMotor(VIBRATION_ACTION_TIME);
}

void initWifiDebugDisplay()
{
    gfxGeneralPageInit(6);
    gfxGeneralPageSetTitle(DEBUG_MENU_WIFI);

    GeneralPageButton button[] = {GeneralPageButton{DEBUG_WIFI_ON, wifiOnBtn}, GeneralPageButton{DEBUG_WIFI_OFF, wifiOffBtn}, GeneralPageButton("NTP", wifiNtpBtn)};
    gfxGeneralPageSetButtons(button, 3);

    if (wifiStatusWrap() != wifiStatusSimple::WifiOff)
    {
        previousMac = WiFi.macAddress();
    }
    else
    {
        previousMac = EMPTY_MAC;
    }
    macAddressLine = gfxGeneralPageAddKeyValue(DEBUG_WIFI_MAC_ADDRESS, previousMac.c_str());

    wifiStatusLastStr = wifiStatus();
    statusLine = gfxGeneralPageAddKeyValue(DEBUG_WIFI_STATUS, wifiStatusLastStr.c_str());

    if (wifiStatusWrap() != wifiStatusSimple::WifiOff)
    {
        ipLine = gfxGeneralPageAddKeyValue(DEBUG_WIFI_IP, WiFi.localIP().toString().c_str());
        ssidLine = gfxGeneralPageAddKeyValue(DEBUG_WIFI_SSID, WiFi.SSID().c_str());
        signalStrengthLine = gfxGeneralPageAddKeyValue(DEBUG_WIFI_SIGNAL, (String(getSignalStrength()) + DEBUG_COMMON_PERCENT).c_str());
    }
    else
    {
        ipLine = gfxGeneralPageAddKeyValue(DEBUG_WIFI_IP, EMPTY_IP);
        ssidLine = gfxGeneralPageAddKeyValue(DEBUG_WIFI_SSID, "");
        signalStrengthLine = gfxGeneralPageAddKeyValue(DEBUG_WIFI_SIGNAL, (String("0") + DEBUG_COMMON_PERCENT).c_str());
    }

    taskStatusLastStr = BOOL_STR(isWifiTaskCheck());
    taskStatusLine = gfxGeneralPageAddKeyValue(DEBUG_WIFI_CONNECTING, taskStatusLastStr.c_str());

    gfxGeneralPageSetMain();
}

void loopWifiDebugDisplay()
{
    if (gfxGeneralPageIsMenu() == false)
    {
        String macNow = EMPTY_MAC;

        if (wifiStatusWrap() != wifiStatusSimple::WifiOff)
        {
            macNow = WiFi.macAddress();
        }
        if (previousMac != macNow)
        {
            previousMac = macNow;
            gfxGeneralPageChange(macNow.c_str(), macAddressLine);
        }

        String wifiStatusStr = wifiStatus();
        if (wifiStatusStr != wifiStatusLastStr)
        {
            wifiStatusLastStr = wifiStatusStr;
            gfxGeneralPageChange(wifiStatusLastStr.c_str(), statusLine);
            if (wifiStatusWrap() != wifiStatusSimple::WifiOff)
            {

                gfxGeneralPageChange(WiFi.localIP().toString().c_str(), ipLine);
                gfxGeneralPageChange(WiFi.SSID().c_str(), ssidLine);
            }
            else
            {
                gfxGeneralPageChange(EMPTY_IP, ipLine);
                gfxGeneralPageChange("", ssidLine);
            }
        }

        int wifiSignal = 0;
        if (wifiStatusWrap() != wifiStatusSimple::WifiOff)
        {
            wifiSignal = getSignalStrength();
        }
        if (wifiSignal != wifiSignalLast)
        {
            wifiSignalLast = wifiSignal;
            gfxGeneralPageChange((String(wifiSignal) + DEBUG_COMMON_PERCENT).c_str(), signalStrengthLine);
        }

        String wifiTaskStatus = BOOL_STR(isWifiTaskCheck());
        if (wifiTaskStatus != taskStatusLastStr)
        {
            taskStatusLastStr = wifiTaskStatus;
            gfxGeneralPageChange(wifiTaskStatus.c_str(), taskStatusLine);
        }
    }

    // Never exit this wifi is on or connected
    gfxGeneralPageSetMain();
    gfxGeneralPageLoop();
}