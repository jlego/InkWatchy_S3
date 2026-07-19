#include "defines.h"
#include "rtcMem.h"

// https://github.com/espressif/arduino-esp32/blob/337058ac94e7e3df11d273a93e88d1ea605e6f5f/cores/esp32/main.cpp#L105
// Why is that? well anyway this task set's it and exits
TaskHandle_t priorityLoopHandle;
void priorityLoopSet(void *parameter)
{
  while (true)
  {
    delayTask(350);
    TaskHandle_t loopTaskHandle = xTaskGetHandle("loopTask");
    if (loopTaskHandle != NULL)
    {
      vTaskPrioritySet(loopTaskHandle, MAIN_LOOP_PRIORITY);
      // debugLog("Set loop task priority, exiting...");
      vTaskDelete(NULL);
    }
  }
}

void setup()
{
#if DEBUG
  initLog();
#endif

  initHardware();

#if FILE_UPLOAD
  if (LittleFS.exists("/upload_to_sd"))
  {
    debugLog("Found upload_to_sd flag, copying files to SD...");
    LittleFS.remove("/upload_to_sd");
    bool cleanAll = LittleFS.exists("/sd_clean_all");
    if (cleanAll) LittleFS.remove("/sd_clean_all");
    if (sdCardSetup())
    {
      if (cleanAll)
      {
        debugLog("Cleaning all books from SD /book/...");
        LittleFS.remove("/book_map");
        File sdBookDir = SD_MMC.open("/book");
        if (sdBookDir && sdBookDir.isDirectory())
        {
          File sf = sdBookDir.openNextFile();
          while (sf)
          {
            if (!sf.isDirectory())
            {
              String sn = String(sf.name());
              debugLog("Removing: " + sn);
              SD_MMC.remove(String("/book/") + sn);
            }
            sf = sdBookDir.openNextFile();
          }
          sdBookDir.close();
        }
      }
      File delList = LittleFS.open("/sd_delete_list", FILE_READ);
      if (delList)
      {
        while (delList.available())
        {
          String fn = delList.readStringUntil('\n');
          fn.trim();
          if (fn.length() > 0)
          {
            String delPath = "/book/" + fn;
            debugLog("Deleting from SD: " + delPath);
            SD_MMC.remove(delPath);
            File mapR = LittleFS.open("/book_map", FILE_READ);
            File mapW = LittleFS.open("/book_map_tmp", FILE_WRITE);
            if (mapR && mapW)
            {
              while (mapR.available())
              {
                String ml = mapR.readStringUntil('\n');
                ml.trim();
                int eq = ml.indexOf('=');
                if (eq > 0 && ml.substring(0, eq) == fn) continue;
                if (ml.length() > 0) mapW.println(ml);
              }
              mapR.close();
              mapW.close();
              LittleFS.remove("/book_map");
              LittleFS.rename("/book_map_tmp", "/book_map");
            }
            else
            {
              if (mapR) mapR.close();
              if (mapW) mapW.close();
            }
          }
        }
        delList.close();
        LittleFS.remove("/sd_delete_list");
      }
      if (!SD_MMC.exists("/book"))
      {
        bool mkOk = SD_MMC.mkdir("/book");
        debugLog("mkdir /book: " + String(mkOk));
      }
      File testF = SD_MMC.open("/book/test.txt", FILE_WRITE);
      if (testF)
      {
        testF.println("test");
        testF.close();
        SD_MMC.remove("/book/test.txt");
        debugLog("SD write test OK");
      }
      else
      {
        debugLog("SD write test FAILED");
      }
      File root = LittleFS.open("/upload_tmp");
      if (root && root.isDirectory())
      {
        int bookNum = 1;
        File sdBookDir = SD_MMC.open("/book");
        if (sdBookDir && sdBookDir.isDirectory())
        {
          File sf = sdBookDir.openNextFile();
          while (sf)
          {
            if (!sf.isDirectory()) bookNum++;
            sf = sdBookDir.openNextFile();
          }
          sdBookDir.close();
        }
        File mapFile = LittleFS.open("/book_map", FILE_APPEND);
        File f = root.openNextFile();
        while (f)
        {
          if (!f.isDirectory())
          {
            String name = String(f.name());
            int ls = name.lastIndexOf('/');
            if (ls >= 0) name = name.substring(ls + 1);
            char sdName[20];
            snprintf(sdName, sizeof(sdName), "book_%03d.txt", bookNum);
            String srcPath = "/upload_tmp/" + name;
            String dstPath = "/book/" + String(sdName);
            debugLog("Copying " + name + " -> " + String(sdName));
            File src = LittleFS.open(srcPath, FILE_READ);
            File dst = SD_MMC.open(dstPath, FILE_WRITE);
            if (src && dst)
            {
              uint8_t buf[512];
              while (size_t n = src.read(buf, sizeof(buf)))
              {
                dst.write(buf, n);
              }
              dst.close();
              src.close();
              LittleFS.remove(srcPath);
              if (mapFile) mapFile.println(String(sdName) + "=" + name);
              debugLog("Copied " + name + " to SD as " + String(sdName));
            }
            else
            {
              debugLog("Failed to copy " + name);
              if (src) src.close();
              if (dst) dst.close();
            }
            bookNum++;
          }
          f = root.openNextFile();
        }
        if (mapFile) mapFile.close();
        root.close();
      }
      sdCardEnd();
    }
    else
    {
      debugLog("SD card not available for copy");
    }
  }
#endif

  // debugLog("Starting millis: " + String(millisBetter()));

#if DEBUG
  showInitLogs();
#endif

  initManager();

  if (isFullMode() == true)
  {
    // I trust myself enough now to not need watchdog task running all the time
    initWatchdogTask();

    turnOnButtons();

    xTaskCreate(
        priorityLoopSet,
        "priorityLoop",
        1000,
        NULL,
        20,
        &priorityLoopHandle);
  }

#if INK_ALARMS
  checkAlarms();
#endif
}

void loop()
{
  if (isFullMode() == true)
  {
#if WATCHDOG_TASK_TIMEOUT
    watchdogPing();
#endif

    manageRTC();
#if INK_ALARMS
    checkAlarms();
#endif
    loopBattery(false);
#if !DEBUG || !NO_SYNC
    regularSync();
#endif
  }
#if TEMP_CHECKS_ENABLED
  tempChecker();
#endif

  loopManager();

#if DEBUG
  endLoopDebug();
#endif

#if !DISABLE_SLEEP || !DEBUG
  manageSleep();
#endif
}