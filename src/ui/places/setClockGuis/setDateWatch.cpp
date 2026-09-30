#include "setDateWatch.h"

#if SET_CLOCK_GUI

#define DATE_FIELD_YEAR 0
#define DATE_FIELD_MONTH 1
#define DATE_FIELD_DAY 2

static int editYear;
static int editMonth;
static int editDay;
static int selectedField;

static int daysInMonth(int year, int month)
{
    if (month == 2) {
        if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) return 29;
        return 28;
    } else if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
    return 31;
}

static void clampDay()
{
    int maxDay = daysInMonth(editYear, editMonth);
    if (editDay > maxDay) editDay = maxDay;
    if (editDay < 1) editDay = 1;
}

static void drawDateEditor()
{
    dis->setFont(NULL);
    dis->fillScreen(SCWhite);
    dis->setTextColor(SCBlack);

    dis->setTextSize(2);
    dis->setCursor(10, 30);
    dis->print("Set Date");

    dis->setTextSize(3);
    dis->setTextColor(SCBlack, SCWhite);

    dis->setCursor(10, 95);

    if (selectedField == DATE_FIELD_YEAR) dis->setTextColor(SCWhite, SCBlack);
    dis->print(String(editYear));
    if (selectedField == DATE_FIELD_YEAR) dis->setTextColor(SCBlack, SCWhite);

    dis->print("/");

    if (selectedField == DATE_FIELD_MONTH) dis->setTextColor(SCWhite, SCBlack);
    if (editMonth < 10) dis->print("0");
    dis->print(String(editMonth));
    if (selectedField == DATE_FIELD_MONTH) dis->setTextColor(SCBlack, SCWhite);

    dis->print("/");

    if (selectedField == DATE_FIELD_DAY) dis->setTextColor(SCWhite, SCBlack);
    if (editDay < 10) dis->print("0");
    dis->print(String(editDay));
    if (selectedField == DATE_FIELD_DAY) dis->setTextColor(SCBlack, SCWhite);

    dis->setFont(NULL);
    dis->setTextSize(1);
    dis->setCursor(5, 185);
    dis->print("UP/DN: adj  MENU: sel  BK: save");

    disUp(true, true, true);
}

void initSetDateWatch()
{
    readRTC();
    editYear = timeRTCLocal.Year + 1970;
    editMonth = timeRTCLocal.Month + 1;
    editDay = timeRTCLocal.Day;
    selectedField = DATE_FIELD_YEAR;
    drawDateEditor();
}

void loopSetDateWatch()
{
    buttonState btn = useButton();
    bool redraw = false;

    switch (btn) {
    case Up:
        if (selectedField == DATE_FIELD_YEAR) {
            editYear++;
            if (editYear > 2100) editYear = 1970;
        } else if (selectedField == DATE_FIELD_MONTH) {
            editMonth++;
            if (editMonth > 12) { editMonth = 1; editYear++; }
        } else {
            editDay++;
            if (editDay > daysInMonth(editYear, editMonth)) { editDay = 1; editMonth++; if (editMonth > 12) { editMonth = 1; editYear++; } }
        }
        redraw = true;
        break;
    case Down:
        if (selectedField == DATE_FIELD_YEAR) {
            editYear--;
            if (editYear < 1970) editYear = 2100;
        } else if (selectedField == DATE_FIELD_MONTH) {
            editMonth--;
            if (editMonth < 1) { editMonth = 12; editYear--; }
        } else {
            editDay--;
            if (editDay < 1) { editMonth--; if (editMonth < 1) { editMonth = 12; editYear--; } editDay = daysInMonth(editYear, editMonth); }
        }
        redraw = true;
        break;
    case Menu:
        selectedField = (selectedField + 1) % 3;
        redraw = true;
        break;
    default:
        break;
    }

    if (redraw) {
        clampDay();
        drawDateEditor();
    }
}

void exitSetDateWatch()
{
    clampDay();
    readRTC();
    int dayDiff = int(timeRTCLocal.Day) - int(timeRTCUTC0.Day);
    int yearDiff = int(timeRTCLocal.Year) - int(timeRTCUTC0.Year);
    timeRTCUTC0.Day = editDay + dayDiff;
    timeRTCUTC0.Month = (editMonth - 1);
    timeRTCUTC0.Year = (editYear - 1970) + yearDiff;
    saveRTC(timeRTCUTC0);
    readRTC();
}

#endif