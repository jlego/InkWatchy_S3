#include "manager.h"
#include "rtcMem.h"

void generalSwitch(UiPlace place)
{
    currentPlaceIndex += 1;
    rM.placeTree[currentPlaceIndex] = place;
    // debugLog("Current place: " + String(rM.placeTree[currentPlaceIndex]) + " and index: " + String(currentPlaceIndex));
}

void overwriteSwitch(UiPlace place)
{
    rM.placeTree[currentPlaceIndex] = place;
}

void switchDebugMenu()
{
    generalSwitch(debugMenu);
}

void switchSettingsMenu()
{
    generalSwitch(settingsMenu);
}

void switchGamesMenu()
{
    generalSwitch(gamesMenu);
}

void switchWatchfaceSelectorMenu()
{
    generalSwitch(watchfaceSelector);
}

#if SET_CLOCK_GUI
void switchSetTimeWatch()
{
    generalSwitch(setTimeWatchPlace);
}

void switchSetTimezoneUtcWatch() {
    generalSwitch(setTimezoneUtcWatchPlace);
}

void switchSetTimezoneContinentWatchPlace() {
    generalSwitch(setTimezoneContinentWatchPlace);
}

void switchSetTimezoneCityWatchPlace() {
    generalSwitch(setTimezoneCityWatchPlace);
}

void switchSetDateWatch() {
    generalSwitch(setDateWatchPlace);
}
#endif

void switchGeneralDebug()
{
    generalSwitch(generalDebug);
}

void switchClockDebug()
{
    generalSwitch(clockDebug);
}

void switchBatteryDebug()
{
    generalSwitch(batteryDebug);
}

void switchWifiDebug()
{
    generalSwitch(wifiDebug);
}

void switchGitDebug()
{
    generalSwitch(gitDebug);
}

void switchAccDebug()
{
    generalSwitch(accDebug);
}

void switchMotorDebug()
{
    generalSwitch(motorDebug);
}

void switchHealthMenu()
{
    generalSwitch(healthMenu);
}

#if PRECISE_STEP_COUNTING
void switchDailyStepsChartMenu()
{
    generalSwitch(dailyStepsChartMenu);
}
#endif

void switchWeatherMenu()
{
    generalSwitch(weatherMenu);
}

void switchWeatherDateMenu()
{
    generalSwitch(weatherDateMenu);
}

void switchWeatherConditionMenu()
{
    generalSwitch(weatherConditionMenu);
}

void switchAirQualityDateMenu()
{
    generalSwitch(airQualityDateMenu);
}

void switchAirQualityConditionMenu()
{
    generalSwitch(airQualityConditionMenu);
}

#if INK_ALARMS
void switchAlarmSelectorMenu()
{
    generalSwitch(alarmSelectorMenu);
}

void switchAlarmEditMenu()
{
    generalSwitch(alarmEditMenu);
}

void switchAlarmEditDays()
{
    generalSwitch(alarmEditDays);
}

void switchAlarmRing()
{
    generalSwitch(alarmRing);
}

void switchAlarmSetChooser()
{
    generalSwitch(alarmSetChooser);
}

void switchAlarmQuick()
{
    generalSwitch(alarmQuick);
}

void switchPomodoroMenu()
{
    generalSwitch(pomodoroMenu);
}
#endif

#if SET_TIME_GUI
void switchSetTime()
{
    generalSwitch(setTimePlace);
}
#endif

#if SET_DATE_GUI
void switchSetDate()
{
    generalSwitch(setDatePlace);
}
#endif

void switchPowerMenu()
{
    generalSwitch(powerMenu);
}

void switchVault()
{
    generalSwitch(vault);
}

void switchBack()
{
    setButton(Back);
}

#if VIDEO_PLAYER
void switchVideoPlayer()
{
    generalSwitch(videoPlayer);
}

void switchVideoMenu()
{
    generalSwitch(videoMenu);
}
#endif

void switchApple()
{
    generalSwitch(apple);
}

void switchApple2()
{
    generalSwitch(apple2);
}

void switchPong()
{
    generalSwitch(pong);
}

#if PAINT
void switchPaint()
{
    generalSwitch(paint);
}
#endif

#if MAZE
void switchMaze()
{
    generalSwitch(maze);
}
#endif

#if TETRIS
void switchTetris()
{
    generalSwitch(tetris);
}
#endif

#if JUMPER
void switchJumper()
{
    generalSwitch(jumperGame);
}
#endif

#if SNAKE
void switchSnake()
{
    generalSwitch(snake);
}
#endif

#if DICE
void switchDiceMenu()
{
    generalSwitch(diceMenu);
}

void switchDiceApp()
{
    generalSwitch(diceApp);
}
#endif

#if CREDITS
void switchCredits()
{
    generalSwitch(credits);
}
#endif

#if CONWAY
void switchConway()
{
    generalSwitch(conwayApp);
}
#endif

#if RGB_DIODE
void switchParty()
{
    generalSwitch(partyApp);
}
#endif

#if HEART_MONITOR
void switchHeartMonitor()
{
    generalSwitch(heartMonitor);
}
#endif

#if BAIKY
void switchBaiky()
{
    generalSwitch(baikyApp);
}
#endif

#if NOTES_APP
void switchNotes()
{
    generalSwitch(notesApp);
}

void switchNotesManager() 
{
    generalSwitch(notesManager);
}
#endif

#if BLE_HOST_ENABLED
void switchBluetoothHostScanner()
{
    generalSwitch(bluetoothHostScanner);
}
#endif

#if BLE_PERIPHERAL
void switchBlePeripheral()
{
    generalSwitch(blePeripheral);
}

void switchBlePeripheralMenu()
{
    generalSwitch(blePeripheralMenu);
}
#endif

#if FONT_MENU_ENABLED
void switchFontsPreview()
{
    generalSwitch(fontPreview);
}

void switchFontsPreviewMenu()
{
    generalSwitch(fontPreviewMenu);
}
#endif

void switchBook()
{
    generalSwitch(book);
}

void switchBookSelector()
{
    generalSwitch(bookSelector);
}

#if FILE_UPLOAD
void switchFileUpload()
{
    generalSwitch(fileUpload);
}
#endif

void showTextDialog(String str, bool center, String title)
{
    dis->fillScreen(SCWhite);
    setFont(&FreeSansBold9pt7b);
    setTextSize(1);

    int16_t yCursor = 5;
    if (title != "")
    {
        uint16_t tw = cnGetTextWidth(title);
        uint16_t th = cnGetTextHeight(title);
        if (th == 0) { uint16_t tth; getTextBounds(title, NULL, NULL, NULL, &tth); th = tth; }
        int16_t tx = ((int16_t)dis->width() - (int16_t)tw) / 2;
        if (tx < 0) tx = 0;
        dis->setCursor(tx, yCursor + th);
        cnPrint(dis, title);
        yCursor += th + 5;
        dis->drawFastHLine(0, yCursor, dis->width(), SCBlack);
        yCursor += 5;
    }

    if (center)
    {
        uint16_t tw = cnGetTextWidth(str);
        uint16_t th = cnGetTextHeight(str);
        if (th == 0) { uint16_t tth; getTextBounds(str, NULL, NULL, NULL, &tth); th = tth; }
        int remaining = dis->height() - yCursor;
        int16_t ty = yCursor + (remaining - (int16_t)th) / 2;
        int16_t tx = ((int16_t)dis->width() - (int16_t)tw) / 2;
        if (tx < 0) tx = 0;
        dis->setCursor(tx, ty + th);
        cnPrint(dis, str);
    }
    else
    {
        int lineStart = 0;
        while (lineStart < (int)str.length())
        {
            int nlPos = str.indexOf('\n', lineStart);
            String line;
            if (nlPos == -1)
            {
                line = str.substring(lineStart);
                lineStart = str.length();
            }
            else
            {
                line = str.substring(lineStart, nlPos);
                lineStart = nlPos + 1;
            }
            uint16_t th = cnGetTextHeight(line);
            if (th == 0) { uint16_t tth; getTextBounds(line, NULL, NULL, NULL, &tth); th = tth; }
            dis->setCursor(5, yCursor + th);
            cnPrint(dis, line);
            yCursor += th + 3;
        }
    }

    disUp(true);
}