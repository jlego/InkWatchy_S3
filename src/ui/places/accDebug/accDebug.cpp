#include "accDebug.h"

#if DEBUG_MENUS
#include "rtcMem.h"

bool workingAcc = false;

bool runAccelStatus = false;
uint8_t lineRunAccelStatus = 0;

int16_t accX = 0;
uint8_t lineAccX;
int16_t accY = 0;
uint8_t lineAccY;
int16_t accZ = 0;
uint8_t lineAccZ;

#if BMA_VERSION == 530 || BMA_VERSION == 456
int16_t accXPure = 0;
uint8_t lineAccXPure;
int16_t accYPure = 0;
uint8_t lineAccYPure;
int16_t accZPure = 0;
uint8_t lineAccZPure;
#endif

uint16_t stepsAccDebug = 0;
uint8_t lineAccSteps;

int16_t errorCode = 0;
uint8_t lineErrorCode;
int16_t statusCode = 0;
uint8_t lineStatusCode;

uint8_t lineAccXDeg;
uint8_t lineAccYDeg;
uint8_t lineAccZDeg;

bool is3DOn = false;

bool showPureAcc = false;
void togglePureValues()
{
  showPureAcc = !showPureAcc;
  // To clean
  if (showPureAcc == false)
  {
    genpage_change(String("").c_str(), lineAccX);
    genpage_change(String("").c_str(), lineAccY);
    genpage_change(String("").c_str(), lineAccZ);
#if BMA_VERSION == 530 || BMA_VERSION == 456
    genpage_change(String("").c_str(), lineAccXPure);
    genpage_change(String("").c_str(), lineAccYPure);
    genpage_change(String("").c_str(), lineAccZPure);
#endif
  }
  else
  {
    accX = 0;
    accY = 0;
    accZ = 0;
  }
}

void turn3D()
{
#if SCREEN_ROTATION != 0
  if (is3DOn == true)
  {
    dis->setRotation(SCREEN_ROTATION);
  }
  else
  {
    dis->setRotation(0);
  }
#endif
  is3DOn = !is3DOn;
}

void initAccDebug()
{
#if ACC_ENABLED
  initAcc();
#endif

  is3DOn = false;

  gfxGeneralPageInit(5);
  gfxGeneralPageSetTitle(DEBUG_MENU_ACC);

  String accDevice = DEBUG_ACC_IC;
#if !ACC_ENABLED
  accDevice += DEBUG_ACC_DISABLED;
#else
  accDevice += "BMA" + String(BMA_VERSION);
#endif

  gfxGeneralPageAdd(accDevice.c_str());

#if ACC_ENABLED
  initAcc();
  if (rM.initedAcc == false)
  {
    workingAcc = false;
    gfxGeneralPageAdd(DEBUG_ACC_FAILED_INIT);
  }

  if (rM.initedAcc == true)
  {
    // To cause an error if something goes wrong (selftest doesn't work now, this does)
    rM.SBMA.enableAccel();
    Accel acc;
    rM.SBMA.getAccel(&acc);
    rM.SBMA.disableAccel();

    if (rM.SBMA.damagedAcc == true)
    {
      workingAcc = false;
      lineRunAccelStatus = gfxGeneralPageAdd(DEBUG_ACC_DAMAGED);
    }
    else
    {
      workingAcc = true;
      lineRunAccelStatus = gfxGeneralPageAdd(DEBUG_ACC_WORKING);
    }
  }

  if (workingAcc == true)
  {
    delayTask(50);
    rM.SBMA.enableAccel();
    Accel acc;
    runAccelStatus = rM.SBMA.getAccel(&acc);

    GeneralPageButton buttons[2] = {
        {DEBUG_ACC_CLICK_3D, turn3D},
        {"Toggle pure values", togglePureValues}};
    gfxGeneralPageSetButtons(buttons, 2);

    stepsAccDebug = getSteps();
    lineAccSteps = gfxGeneralPageAdd(String(DEBUG_ACC_STEPS + String(stepsAccDebug)).c_str());

    errorCode = rM.SBMA.getErrorCode();
    lineErrorCode = gfxGeneralPageAdd(String(DEBUG_ACC_ERROR_CODE + String(errorCode)).c_str());
    statusCode = rM.SBMA.getStatus();
    lineStatusCode = gfxGeneralPageAdd(String(DEBUG_ACC_STATUS_CODE + String(statusCode)).c_str());

    lineAccXDeg = gfxGeneralPageAdd("X Deg: 0.0");
    lineAccYDeg = gfxGeneralPageAdd("Y Deg: 0.0");
    lineAccZDeg = gfxGeneralPageAdd("Z Deg: 0.0");

    lineAccX = gfxGeneralPageAdd(String(DEBUG_ACC_X + String(acc.x)).c_str());
    accX = acc.x;
    lineAccY = gfxGeneralPageAdd(String(DEBUG_ACC_Y + String(acc.y)).c_str());
    accY = acc.y;
    lineAccZ = gfxGeneralPageAdd(String(DEBUG_ACC_Z + String(acc.z)).c_str());
    accZ = acc.z;

#if BMA_VERSION == 530 || BMA_VERSION == 456
    Accel accPure;
    rM.SBMA.getAccelPure(&accPure);
    lineAccXPure = gfxGeneralPageAdd(String(DEBUG_ACC_PURE_X + String(accPure.x)).c_str());
    accXPure = accPure.x;
    lineAccYPure = gfxGeneralPageAdd(String(DEBUG_ACC_PURE_Y + String(accPure.y)).c_str());
    accYPure = accPure.y;
    lineAccZPure = gfxGeneralPageAdd(String(DEBUG_ACC_PURE_Z + String(accPure.z)).c_str());
    accZPure = accPure.z;
#endif

    if (showPureAcc == false)
    {
      gfxGeneralPageChange("", lineAccX);
      gfxGeneralPageChange("", lineAccY);
      gfxGeneralPageChange("", lineAccZ);
#if BMA_VERSION == 530 || BMA_VERSION == 456
      gfxGeneralPageChange("", lineAccXPure);
      gfxGeneralPageChange("", lineAccYPure);
      gfxGeneralPageChange("", lineAccZPure);
#endif
    }
  }

#endif

  gfxGeneralPageSetMain();
}

int accDataTime = 0;
void loopAccDebug()
{
#if ACC_ENABLED
  if (is3DOn == false)
  {
    // debugLog("damagedAcc: " + BOOL_STR(rM.SBMA.damagedAcc));
    if (workingAcc == true)
    {
      if (gfxGeneralPageIsMenu() == false)
      {
        uint16_t tmpSteps = stepsAccDebug;
        if (millisBetter() - accDataTime > 500)
        {
          accDataTime = millisBetter();
          // It's moved here to get the status before requesting acceleration
          int16_t tmpStatusCode = rM.SBMA.getStatus();

          Accel acc;
          bool runAccelStatusTmp = rM.SBMA.getAccel(&acc);

          // Weird fallback
          /*
          if (runAccelStatusTmp == true && acc.x == 0 && acc.y == 0 && acc.z == 0)
          {
            runAccelStatusTmp = false;
            debugLog("Weird acc workaround fix");
            rM.SBMA.enableAccel();
            rM.SBMA.enableStepCount();
          }
          */

          if (runAccelStatusTmp != runAccelStatus)
          {
            runAccelStatus = runAccelStatusTmp;
            if (runAccelStatus == true)
            {
              gfxGeneralPageChange(String(DEBUG_ACC_WORKING).c_str(), lineRunAccelStatus);
            }
            else
            {
              gfxGeneralPageChange(String(DEBUG_ACC_DAMAGED).c_str(), lineRunAccelStatus);
            }
          }

          if (acc.x != accX)
          {
            if (showPureAcc == true)
            {
              gfxGeneralPageChange(String(DEBUG_ACC_X + String(acc.x)).c_str(), lineAccX);
            }
#if !(BMA_VERSION == 530 || BMA_VERSION == 456)
            gfxGeneralPageChange((String("X Deg: ") + precisionToString(getAxisDegrees(accX, accY, accZ), 1)).c_str(), lineAccXDeg);
#endif
            accX = acc.x;
            // debugLog("Acc x changed");
          }
          if (acc.y != accY)
          {
            if (showPureAcc == true)
            {
              gfxGeneralPageChange(String(DEBUG_ACC_Y + String(acc.y)).c_str(), lineAccY);
            }
#if !(BMA_VERSION == 530 || BMA_VERSION == 456)
            gfxGeneralPageChange((String("Y Deg: ") + precisionToString(getAxisDegrees(accY, accX, accZ), 1)).c_str(), lineAccYDeg);
#endif
            accY = acc.y;
            // debugLog("Acc y changed");
          }
          if (acc.z != accZ)
          {
            if (showPureAcc == true)
            {
              gfxGeneralPageChange(String(DEBUG_ACC_Z + String(acc.z)).c_str(), lineAccZ);
            }
#if !(BMA_VERSION == 530 || BMA_VERSION == 456)
            gfxGeneralPageChange((String("Z Deg: ") + precisionToString(getAxisDegrees(accZ, accX, accY), 1)).c_str(), lineAccZDeg);
#endif
            accZ = acc.z;
            // debugLog("Acc z changed");
          }

          uint16_t tmpSteps = getSteps();
          if (tmpSteps != stepsAccDebug)
          {
            gfxGeneralPageChange(String(DEBUG_ACC_STEPS + String(tmpSteps)).c_str(), lineAccSteps);
            stepsAccDebug = tmpSteps;
            // debugLog("Steps changed");
          }

          int16_t tmpErrorCode = rM.SBMA.getErrorCode();
          if (tmpErrorCode != errorCode)
          {
            gfxGeneralPageChange(String(DEBUG_ACC_ERROR_CODE + String(tmpErrorCode)).c_str(), lineErrorCode);
            errorCode = tmpErrorCode;
          }

          if (tmpStatusCode != statusCode)
          {
            gfxGeneralPageChange(String(DEBUG_ACC_STATUS_CODE + String(tmpStatusCode)).c_str(), lineStatusCode);
            statusCode = tmpStatusCode;
          }

#if BMA_VERSION == 530 || BMA_VERSION == 456
          Accel accPure;
          rM.SBMA.getAccelPure(&accPure);
          if (accPure.x != accXPure)
          {
            if (showPureAcc == true)
            {
              gfxGeneralPageChange(String(DEBUG_ACC_PURE_X + String(accPure.x)).c_str(), lineAccXPure);
            }
            gfxGeneralPageChange((String("X Deg: ") + precisionToString(getAxisDegrees(accPure.x, accPure.y, accPure.z), 1)).c_str(), lineAccXDeg);
            accXPure = accPure.x;
          }
          if (accPure.y != accYPure)
          {
            if (showPureAcc == true)
            {
              gfxGeneralPageChange(String(DEBUG_ACC_PURE_Y + String(accPure.y)).c_str(), lineAccYPure);
            }
            gfxGeneralPageChange((String("Y Deg: ") + precisionToString(getAxisDegrees(accPure.y, accPure.x, accPure.z), 1)).c_str(), lineAccYDeg);
            accYPure = accPure.y;
          }
          if (accPure.z != accZPure)
          {
            if (showPureAcc == true)
            {
              gfxGeneralPageChange(String(DEBUG_ACC_PURE_Z + String(accPure.z)).c_str(), lineAccZPure);
            }
            gfxGeneralPageChange((String("Z Deg: ") + precisionToString(getAxisDegrees(accPure.z, accPure.x, accPure.y), 1)).c_str(), lineAccZDeg);
            accZPure = accPure.z;
          }
#endif
          gfxGeneralPageSetMain();
        }
      }
    }
  }
  else
  {
    if (useButton() == Menu)
    {
      turn3D();
      return;
    }
    loopAcc3D();
    disUp();
  }

  resetSleepDelay();
#endif
  if (is3DOn == false)
  {
    // Here because to show IC disabled too
    gfxGeneralPageLoop();
  }
}

void exitAccDebug()
{
#if SCREEN_ROTATION != 0
  dis->setRotation(SCREEN_ROTATION);
#endif
}
#endif