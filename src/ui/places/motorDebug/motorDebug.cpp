#include "motorDebug.h"
#include "rtcMem.h"

#if DEBUG || DEBUG_MENUS

uint8_t lineVibrationsDisabled;
bool oldDisableAllVibration = false;

uint8_t lineVibrationPower;
int vibrationPowerToSet = VIBRATION_POWER;
int oldVibrationPower = VIBRATION_POWER;
#define VIBRATION_POWER_STEP 5

uint8_t lineVibrationTime;
int vibrationTimeToSet = 50;
int oldVibratiomTime = 50;
#define VIBRATION_TIME_STEP 10

uint8_t lineBatteryVoltage;
float oldBatteryVoltage;

#define SET_POWER 0
#define SET_TIME 1
uint8_t motorSetChanger = SET_POWER;

String vibrationDisabledStr()
{
    oldDisableAllVibration = rM.disableAllVibration;
    return String(DEBUG_MOTOR_VIB_DIS + String(BOOL_STR(rM.disableAllVibration)));
}

String vibrationPowerStr()
{
    oldVibrationPower = vibrationPowerToSet;
    return String(DEBUG_MOTOR_VIB_POWER + String(vibrationPowerToSet));
}

String vibrationTimeStr()
{
    oldVibratiomTime = vibrationTimeToSet;
    return String(DEBUG_MOTOR_VIB_TIME + String(vibrationTimeToSet));
}

String batteryVoltage()
{
    oldBatteryVoltage = rM.bat.curV;
    return String(DEBUG_MOTOR_BATT_V + String(rM.bat.curV));
}

void setMotorSetPower()
{
    motorSetChanger = SET_POWER;
}

void setMotorSetTime()
{
    motorSetChanger = SET_TIME;
}

void initMotorDebug()
{
    gfxGeneralPageInit(5);
    gfxGeneralPageSetTitle(DEBUG_MENU_MOTOR);
    rustLimitButtons = true;

    GeneralPageButton button[] = {GeneralPageButton{DEBUG_MOTOR_SET_POWER, setMotorSetPower}, GeneralPageButton{DEBUG_MOTOR_SET_TIME, setMotorSetTime}};
    gfxGeneralPageSetButtons(button, 2);

    lineVibrationsDisabled = gfxGeneralPageAdd(vibrationDisabledStr().c_str());

    lineBatteryVoltage = gfxGeneralPageAdd(batteryVoltage().c_str());

    lineVibrationPower = gfxGeneralPageAdd(vibrationPowerStr().c_str());

    lineVibrationTime = gfxGeneralPageAdd(vibrationTimeStr().c_str());

    resetSleepDelay(SLEEP_EVERY_MS);
    gfxGeneralPageSetMain();
}

void loopMotorDebug()
{
    if (gfxGeneralPageIsMenu() == false)
    {

        loopBattery();

        if (rM.disableAllVibration != oldDisableAllVibration)
        {
            gfxGeneralPageChange(vibrationDisabledStr().c_str(), lineVibrationsDisabled);
        }

        if (rM.bat.curV != oldBatteryVoltage)
        {
            gfxGeneralPageChange(batteryVoltage().c_str(), lineBatteryVoltage);
        }

        if (vibrationPowerToSet != oldVibrationPower)
        {
            gfxGeneralPageChange(vibrationPowerStr().c_str(), lineVibrationPower);
        }

        if (vibrationTimeToSet != oldVibratiomTime)
        {
            gfxGeneralPageChange(vibrationTimeStr().c_str(), lineVibrationTime);
        }

        buttonState btn = getButtonsRust();
        switch (btn)
        {
        case Menu:
        {
            vibrateMotor(vibrationTimeToSet, vibrationPowerToSet);
            break;
        }
        case Up:
        {
            if (motorSetChanger == SET_POWER)
            {
                vibrationPowerToSet = vibrationPowerToSet + VIBRATION_POWER_STEP;
                checkMaxMin(&vibrationPowerToSet, 255, 0);
            }
            else if (motorSetChanger == SET_TIME)
            {
                vibrationTimeToSet = vibrationTimeToSet + VIBRATION_TIME_STEP;
                checkMaxMin(&vibrationTimeToSet, 1000, 0);
            }
            break;
        }
        case Down:
        {
            if (motorSetChanger == SET_POWER)
            {
                vibrationPowerToSet = vibrationPowerToSet - VIBRATION_POWER_STEP;
                checkMaxMin(&vibrationPowerToSet, 255, 0);
            }
            else if (motorSetChanger == SET_TIME)
            {
                vibrationTimeToSet = vibrationTimeToSet - VIBRATION_TIME_STEP;
                checkMaxMin(&vibrationTimeToSet, 1000, 0);
            }
            break;
        }
        }
    }

    gfxGeneralPageSetMain();
    gfxGeneralPageLoop();
}

#endif