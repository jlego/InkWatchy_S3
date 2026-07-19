#include "hardwareDebug.h"
#include "rtcMem.h"

#if DEBUG || DEBUG_MENUS

uint32_t usedHeapLast;
uint8_t lineHeap;
uint8_t lineTemp;

#if TEMP_CHECKS_ENABLED
uint8_t lineInitTemp;
float previousTempUi;
#endif

String getRtcType()
{
    int rtcType = rM.SRTC.getType();
    switch (rtcType)
    {
    case 0:
        return DEBUG_RTC_UNKNOWN;
    case 1:
        return DEBUG_RTC_DS3231;
    case 2:
        return DEBUG_RTC_PCF8563;
    case 3:
        return DEBUG_RTC_INTERNAL;
    default:
        return DEBUG_RTC_INVALID;
    }
}

String getHeapStr()
{
    String heapStr = String(DEBUG_HW_USED_HEAP) +
                     String((ESP.getHeapSize() - ESP.getFreeHeap()) / 1024) +
                     "/" +
                     String(ESP.getHeapSize() / 1024);
    return heapStr;
}

void initGeneralDebugDisplay()
{
    gfxGeneralPageInit(5);
    gfxGeneralPageSetTitle(DEBUG_MENU_HARDWARE);

    gfxGeneralPageAdd(DEBUG_HW_CHIP_MODEL);
    gfxGeneralPageAdd(String(ESP.getChipModel()).c_str());

    gfxGeneralPageAdd(String(DEBUG_HW_RTC_TYPE + getRtcType()).c_str());

    String heapStr = getHeapStr();
    usedHeapLast = ESP.getFreeHeap();
    lineHeap = gfxGeneralPageAdd(heapStr.c_str());

    gfxGeneralPageAdd(getLittleFsSizeString().c_str());

#if TEMP_CHECKS_ENABLED
    previousTempUi = getTemp();
    debugLog("Previous temp ui is: " + String(previousTempUi));
    lineTemp = gfxGeneralPageAdd(String(DEBUG_HW_CPU_TEMP + String(previousTempUi)).c_str());
    lineInitTemp = gfxGeneralPageAdd(String(DEBUG_HW_INIT_TEMP + String(rM.initialTemp)).c_str());
#endif

    resetSleepDelay(SLEEP_EVERY_MS);
    gfxGeneralPageSetMain();
}

void loopGeneralDebugDisplay()
{
    uint32_t newHeap = ESP.getFreeHeap();
    if (newHeap != usedHeapLast)
    {
        usedHeapLast = newHeap;
        String newHeapStr = getHeapStr();
        gfxGeneralPageChange(newHeapStr.c_str(), lineHeap);
    }
#if TEMP_CHECKS_ENABLED
    float tempUi = getTemp();
    if(previousTempUi != tempUi) {
        previousTempUi = tempUi;
        gfxGeneralPageChange(String(DEBUG_HW_CPU_TEMP + String(tempUi)).c_str(), lineTemp);
    }
#endif

    gfxGeneralPageSetMain();
    gfxGeneralPageLoop();
}

#endif

#if DEBUG

void initGeneralDebug()
{
    debugLog("Chip Model: " + String(ESP.getChipModel()));
    debugLog("RTC type: " + String(getRtcType()));
    debugLog("Used Heap KB: " + String((ESP.getHeapSize() - ESP.getFreeHeap()) / 1024) + "/" + String(ESP.getHeapSize() / 1024));
}

void loopGeneralDebug()
{
    debugLog(getHeapStr());
    // This gives the same result
    // size_t totalHeap = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    // size_t freeHeap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    // debugLog("Free Heap: " + String(freeHeap / 1024) + "/" + String(totalHeap / 1024) + " kB");
    debugLog("Current CPU frequency: " + String(getCpuFrequencyMhz()) + "Mhz");

#if false
    size_t freeSize = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t largestFreeBlock = heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT);
    size_t minimumFreeSize = heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT);
    multi_heap_info_t heapInfo;
    heap_caps_get_info(&heapInfo, MALLOC_CAP_DEFAULT);

    debugLog("Free Memory: " + String(freeSize) + " bytes");
    debugLog("Largest Free Block: " + String(largestFreeBlock) + " bytes");
    debugLog("Minimum Free Size Since Boot: " + String(minimumFreeSize) + " bytes");
    debugLog("Total Allocated Bytes: " + String(heapInfo.total_allocated_bytes) + " bytes");
    debugLog("Total Free Bytes: " + String(heapInfo.total_free_bytes) + " bytes");
    debugLog("Largest Free Block in Heap Info: " + String(heapInfo.largest_free_block) + " bytes");
    debugLog("Allocated Blocks: " + String(heapInfo.allocated_blocks));
    debugLog("Free Blocks: " + String(heapInfo.free_blocks));
    debugLog("Total Blocks: " + String(heapInfo.total_blocks));
#endif
}

#endif