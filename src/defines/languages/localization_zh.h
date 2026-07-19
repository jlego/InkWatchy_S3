#pragma once

// CHINESE SIMPLIFIED TRANSLATIONS

// Slate watchface weather conditions
#define WF_S_WEATHER_CLEAR_SKY "晴"
#define WF_S_WEATHER_MOSTLY_CLEAR "大部晴"
#define WF_S_WEATHER_PARTLY_CLOUDY "多云"
#define WF_S_WEATHER_OVERCAST "阴天"
#define WF_S_WEATHER_FOG "雾"
#define WF_S_WEATHER_HEAVY_FOG "大雾"
#define WF_S_WEATHER_LIGHT_DRIZZLE "小毛毛雨"
#define WF_S_WEATHER_DRIZZLE "毛毛雨"
#define WF_S_WEATHER_HEAVY_DRIZZLE "大毛毛雨"
#define WF_S_WEATHER_LIGHT_FREEZING "微冻"
#define WF_S_WEATHER_HEAVY_FREEZING "重冻"
#define WF_S_WEATHER_LIGHT_RAIN "小雨"
#define WF_S_WEATHER_RAIN "中雨"
#define WF_S_WEATHER_HEAVY_RAIN "大雨"
#define WF_S_WEATHER_FREEZING_RAIN "冻雨"
#define WF_S_WEATHER_FREEZING_HARD "强冻雨"
#define WF_S_WEATHER_LIGHT_SNOW "小雪"
#define WF_S_WEATHER_SNOW "中雪"
#define WF_S_WEATHER_HEAVY_SNOW "大雪"
#define WF_S_WEATHER_SNOW_GRAINS "雪粒"
#define WF_S_WEATHER_LIGHT_SHOWERS "小阵雨"
#define WF_S_WEATHER_SHOWERS "阵雨"
#define WF_S_WEATHER_HEAVY_SHOWERS "大阵雨"
#define WF_S_WEATHER_SNOW_SHOWERS "雨夹雪"
#define WF_S_WEATHER_THUNDERSTORM "雷暴"
#define WF_S_WEATHER_LIGHT_HAIL "小冰雹"
#define WF_S_WEATHER_HEAVY_HAIL "大冰雹"
#define WF_S_WEATHER_UNKNOWN "未知"

// Day names (abbreviated)
#define WF_S_DAY_MON "周一"
#define WF_S_DAY_TUE "周二"
#define WF_S_DAY_WED "周三"
#define WF_S_DAY_THU "周四"
#define WF_S_DAY_FRI "周五"
#define WF_S_DAY_SAT "周六"
#define WF_S_DAY_SUN "周日"

// Error messages
#define WF_T_ERROR "ERR"
#define WF_I_ERROR "ERR"
#define WF_I_ERROR_SHORT "ER"

// Time format
#define WF_TIME_AM "AM"
#define WF_TIME_PM "PM"

// Weather/Air Quality Selection items
#define WEATHER_AQ_MENU_TITLE "数据类型"
#define WEATHER_AQ_MENU_WEATHER "天气"
#define WEATHER_AQ_MENU_AIR_QUALITY "空气质量"
#define WEATHER_AQ_NOT_AVAILABLE "天气/空气质量均不可用"

// Weather submenu items
#define WEATHER_MENU_TEMPERATURE "温度"
#define WEATHER_MENU_PRESSURE "气压"
#define WEATHER_MENU_HUMIDITY "湿度"
#define WEATHER_MENU_CONDITIONS "天气状况"
#define WEATHER_MENU_CLOUDINESS "云量"
#define WEATHER_MENU_WIND_SPEED "风速"
#define WEATHER_MENU_WIND_GUSTS "阵风"
#define WEATHER_MENU_VISIBILITY "能见度"
#define WEATHER_MENU_PRECIPITATION "降水概率%"
#define WEATHER_MENU_UV_INDEX "紫外线指数"
#define WEATHER_MENU_UV_INDEX_CLEAR_SKY "晴空UV"
#define WEATHER_MENU_WET_BULB "湿球温度"
#define WEATHER_MENU_CAPE "CAPE"
#define WEATHER_MENU_DEW_POINT "露点"
#define WEATHER_MENU_PRECIPITATION_AMOUNT "降水量"
#define WEATHER_MENU_SNOW_DEPTH "积雪深度"
#define WEATHER_MENU_LIFTED_INDEX "抬升指数"
#define WEATHER_MENU_CONVECTIVE_INHIBITION "对流抑制"
#define WEATHER_MENU_TERRESTRIAL_RADIATION "地面辐射"
#define WEATHER_MENU_DIFFUSE_RADIATION "散射辐射"
#define WEATHER_MENU_DIRECT_RADIATION "直接辐射"
#define WEATHER_MENU_TOTAL_WATER_VAPOUR "总水汽量"

// Weather menu titles and messages
#define WEATHER_SELECT_DATE "选择日期"
#define WEATHER_STAT_TITLE "天气统计"
#define WEATHER_NOT_AVAILABLE "天气不可用"
#define WEATHER_DATE_WRONG "日期有误?"
#define WEATHER_CORRUPTED "天气数据损坏"

// Chart titles with time notation
#define WEATHER_CHART_TEMP "温度/C"
#define WEATHER_CHART_PRESSURE "气压/hPa"
#define WEATHER_CHART_HUMIDITY "湿度/%"
#define WEATHER_CHART_CLOUDS "云量/%"
#define WEATHER_CHART_WIND_SPEED "风速/km/h"
#define WEATHER_CHART_WIND_GUSTS "阵风/km/h"
#define WEATHER_CHART_VISIBILITY "能见度/m"
#define WEATHER_CHART_PRECIPITATION "降水/%"
#define WEATHER_CONDITIONS_TITLE "天气状况"
#define WEATHER_CHART_UV_INDEX "UV指数"
#define WEATHER_CHART_UV_INDEX_CLEAR_SKY "晴空UV"
#define WEATHER_CHART_WET_BULB "湿球/C"
#define WEATHER_CHART_CAPE "CAPE/J/kg"
#define WEATHER_CHART_DEW_POINT "露点/C"
#define WEATHER_CHART_PRECIPITATION_AMOUNT "总降水"
#define WEATHER_CHART_SNOW_DEPTH "积雪深度"
#define WEATHER_CHART_LIFTED_INDEX "抬升指数"
#define WEATHER_CHART_CONVECTIVE_INHIBITION "对流抑制"
#define WEATHER_CHART_TERRESTRIAL_RADIATION "地面辐射"
#define WEATHER_CHART_DIFFUSE_RADIATION "散射辐射"
#define WEATHER_CHART_DIRECT_RADIATION "直接辐射"
#define WEATHER_CHART_TOTAL_WATER_VAPOUR "总水汽量"

// Air quality submenu items
#define AIR_MENU_EUAQI "欧洲AQI"
#define AIR_MENU_USAQI "美国AQI"
#define AIR_MENU_EU_AQI_PM2_5 "EU AQI PM2.5"
#define AIR_MENU_EU_AQI_PM10 "EU AQI PM10"
#define AIR_MENU_EU_AQI_NO2 "EU AQI NO2"
#define AIR_MENU_EU_AQI_O3 "EU AQI O3"
#define AIR_MENU_EU_AQI_SO2 "EU AQI SO2"
#define AIR_MENU_PM2_5 "PM2.5"
#define AIR_MENU_PM10 "PM10"
#define AIR_MENU_CARBON_MONOXIDE "一氧化碳"
#define AIR_MENU_CARBON_DIOXIDE "二氧化碳"
#define AIR_MENU_NITROGEN_DIOXIDE "二氧化氮"
#define AIR_MENU_SULPHUR_DIOXIDE "二氧化硫"
#define AIR_MENU_OZONE "臭氧"
#define AIR_MENU_AEROSOL_OPTICAL_DEPTH "气溶胶光学厚度"
#define AIR_MENU_DUST "粉尘"
#define AIR_MENU_METHANE "甲烷"
#define AIR_MENU_FORMALDEHYDE "甲醛"
#define AIR_MENU_GLYOXAL "乙二醛"
#define AIR_MENU_SEA_SALT_AEROSOL "海盐气溶胶"
#define AIR_MENU_NITROGEN_MONOXIDE "一氧化氮"
#define AIR_MENU_PEROXYACYL_NITRATES "过氧乙酰硝酸酯"

// Air quality menu titles and messages
#define AIR_QUALITY_SELECT_DATE "选择日期"
#define AIR_QUALITY_STAT_TITLE "空气质量统计"
#define AIR_QUALITY_NOT_AVAILABLE "空气质量不可用"
#define AIR_QUALITY_DATE_WRONG "日期有误?"
#define AIR_QUALITY_CORRUPTED "空气质量数据损坏"

// Air quality chart titles with time notation
#define AIR_CHART_EUAQI "欧洲空气质量指数"
#define AIR_CHART_USAQI "美国空气质量指数"
#define AIR_CHART_EU_AQI_PM2_5 "PM2.5/ug/m3"
#define AIR_CHART_EU_AQI_PM10 "PM10/ug/m3"
#define AIR_CHART_EU_AQI_NO2 "NO2/ug/m3"
#define AIR_CHART_EU_AQI_O3 "O3/ug/m3"
#define AIR_CHART_EU_AQI_SO2 "SO2/ug/m3"
#define AIR_CHART_PM2_5 "PM2.5/ug/m3"
#define AIR_CHART_PM10 "PM10/ug/m3"
#define AIR_CHART_CARBON_MONOXIDE "CO/ug/m3"
#define AIR_CHART_CARBON_DIOXIDE "CO2/ppm"
#define AIR_CHART_NITROGEN_DIOXIDE "NO2/ug/m3"
#define AIR_CHART_SULPHUR_DIOXIDE "SO2/ug/m3"
#define AIR_CHART_OZONE "O3/ug/m3"
#define AIR_CHART_AEROSOL_OPTICAL_DEPTH "AOD/"
#define AIR_CHART_DUST "粉尘/ug/m3"
#define AIR_CHART_METHANE "甲烷/ug/m3"
#define AIR_CHART_FORMALDEHYDE "CH2O/ug/m3"
#define AIR_CHART_GLYOXAL "乙二醛/ug/m3"
#define AIR_CHART_SEA_SALT_AEROSOL "海盐/ug/m3"
#define AIR_CHART_NITROGEN_MONOXIDE "NO/ug/m3"
#define AIR_CHART_PEROXYACYL_NITRATES "PAN/ug/m3"

// Month names
#define LANGUAGE_MONTH_NAMES {"1月", "2月", "3月", "4月", "5月", "6月", "7月", "8月", "9月", "10月", "11月", "12月"}

// Menu translations
#define MENU_MAIN "主菜单"
#define MENU_CALENDAR "日历"
#define MENU_SELECT_BOOK "选择书籍"
#define MENU_ALARMS "闹钟"
#define MENU_WEATHER "天气"
#define MENU_SETTINGS "设置"
#define MENU_HEART_MONITOR "心率监测"
#define MENU_HEALTH "健康"
#define MENU_STEPS_PER_DAY "每日步数"
#define MENU_VAULT "保险箱"
#define MENU_WIFI_TOOL "WiFi工具"
#define MENU_FILE_UPLOAD "文件传输"
#define MENU_EATING_APPLES "吃苹果"
#define MENU_SMASHING_APPLES "砸苹果"
#define MENU_GAMES "游戏"
#define MENU_CREDITS "关于"
#define MENU_WIFI "WiFi"
#define MENU_DEBUG "调试"
#define MENU_POWER_SETTINGS "电源设置"
#define MENU_CHANGE_WATCHFACE "更换表盘"
#define MENU_TETRIS "俄罗斯方块"
#define MENU_PONG "乒乓球"
#define MENU_BLE_PERIPHERAL "蓝牙外设"
#define MENU_NOTES "记事本"
#define MENU_SNAKE "贪吃蛇"
#define MENU_JUMPER "跳跃"
#define MENU_DICE "骰子"
#define MENU_VIDEO_PLAYER "视频播放器"
#define MENU_CONWAY "生命游戏"
#define MENU_PARTY "派对"
#define MENU_VIBRATIONS_DIS "震动已禁用"
#define MENU_WAKE_UP_DIS "唤醒已禁用"
#define MENU_POWER "电源菜单"

// APP AND BOOK TRANSLATIONS

// Book reader messages
#define BOOK_NO_SELECTED "未选择书籍"

// Calendar messages
#define CALENDAR_NO_DATA "无日历数据"

// BLE Scanner
#define BLE_SCANNER_TITLE "BLE扫描器"
#define BLE_SCANNER_SCANNING "扫描中..."
#define BLE_SCANNER_NONE "未发现设备"
#define BLE_SCANNER_CHOOSE "选择设备"
#define BLE_SCANNER_CONNECT "正在连接 "
#define BLE_SCANNER_CONNECTED "已连接 "
#define BLE_SCANNER_FAILED "连接失败 "

// Notes
#define NOTES_TITLE "备忘录"
#define NOTES_NEW "新建笔记"
#define NOTES_DELETE "删除笔记"
#define NOTES_CONNECT_KBD "连接键盘"
#define NOTES_CONNECTED "已连接: "

// HEART MONITOR TRANSLATIONS

#define HEART_MONITOR_TITLE "心率监测"
#define HEART_MONITOR_NOT_MEDICAL "非医疗用途"
#define HEART_MONITOR_PLACE_DEVICE "将设备放在心脏处"
#define HEART_MONITOR_CLICK_MENU "然后按菜单键"
#define HEART_MONITOR_START_3S "3秒后开始"
#define HEART_MONITOR_TAKES_15S "需要15秒"
#define HEART_MONITOR_BREATHE_SLOWLY "请缓慢呼吸"
#define HEART_MONITOR_TIME_LEFT "剩余时间: "
#define HEART_MONITOR_BPM "bpm"

// Steps translations
#define STEPS_NOT_AVAILABLE "步数不可用"
#define STEPS_DATE_WRONG "日期有误?"
#define STEPS_CORRUPTED "步数数据损坏"
#define DAILY_STEPS_TITLE "每日步数"
#define STEPS_CHART_TITLE "步数/(24h)"

// VIDEO PLAYER TRANSLATIONS

#define VIDEO_PLAYER_CHOOSE "选择视频"

// WATCHFACE SELECTION TRANSLATIONS

#define WATCHFACE_SELECT_TITLE "选择表盘"

// TETRIS GAME TRANSLATIONS

#define TETRIS_LINE_CLEARS "消行:"
#define TETRIS_SCORE "分数:"
#define TETRIS_LEVEL "等级:"
#define TETRIS_CONTROLS_UP "上:右键"
#define TETRIS_CONTROLS_DOWN "下:左键"
#define TETRIS_GAME_OVER "游戏结束"

// DEBUG MENU TRANSLATIONS

#define DEBUG_MENU_ACC "调试: 加速度计"
#define DEBUG_MENU_BATTERY "调试: 电池"
#define DEBUG_MENU_CLOCK "调试: 时钟"
#define DEBUG_MENU_GIT "调试: Git"
#define DEBUG_MENU_HARDWARE "调试: 硬件"
#define DEBUG_MENU_WIFI "调试: WiFi"
#define DEBUG_MENU_MOTOR "调试: 马达"

#define DEBUG_ITEM_CLOCK "时钟"
#define DEBUG_ITEM_HARDWARE "硬件"
#define DEBUG_ITEM_BATTERY "电池"
#define DEBUG_ITEM_GIT "Git"
#define DEBUG_ITEM_ACC "加速度"
#define DEBUG_ITEM_MOTOR "马达"
#define DEBUG_ITEM_FONT_PREVIEW "字体预览"

// Accelerometer debug
#define DEBUG_ACC_DISABLED "已禁用"
#define DEBUG_ACC_FAILED_INIT "加速度计初始化失败"
#define DEBUG_ACC_DAMAGED "加速度计损坏"
#define DEBUG_ACC_WORKING "加速度计正常"
#define DEBUG_ACC_CLICK_3D "3D预览"
#define DEBUG_ACC_X "加速度 X: "
#define DEBUG_ACC_Y "加速度 Y: "
#define DEBUG_ACC_Z "加速度 Z: "
#define DEBUG_ACC_PURE_X "原始 X: "
#define DEBUG_ACC_PURE_Y "原始 Y: "
#define DEBUG_ACC_PURE_Z "原始 Z: "
#define DEBUG_ACC_STEPS "步数: "
#define DEBUG_ACC_IC "IC: "
#define DEBUG_ACC_ERROR_CODE "错误码: "
#define DEBUG_ACC_STATUS_CODE "状态码: "

// Battery debug
#define DEBUG_BATTERY_CURRENT_V "当前电压: "
#define DEBUG_BATTERY_MINIMUM_V "最低电压: "
#define DEBUG_BATTERY_MAXIMUM_V "最高电压: "
#define DEBUG_BATTERY_CRITICAL_V "临界电压: "
#define DEBUG_BATTERY_LEVEL "电量 %: "
#define DEBUG_BATTERY_CHARGING "充电中: "
#define DEBUG_BATTERY_FULLY "已充满: "

// Clock debug
#define DEBUG_CLOCK_DRIFT_SYNCS "同步间偏差:"
#define DEBUG_CLOCK_NOT_AVAILABLE "不可用"
#define DEBUG_CLOCK_LAST_SYNC "上次同步:"
#define DEBUG_CLOCK_PREVIOUS_SYNC "前次同步:"
#define DEBUG_CLOCK_LAST_CHARGE "上次充电:"
#define DEBUG_CLOCK_REMOVE_FILES "删除时钟文件"
#define DEBUG_CLOCK_TIMEZONE "时区:"
#define DEBUG_CLOCK_OLSON "Olson时区: "
#define DEBUG_CLOCK_DRIFT_VALUES "偏差值:"

// Git debug
#define DEBUG_GIT_COMMIT_HASH "提交哈希:"
#define DEBUG_GIT_BRANCH "分支:"
#define DEBUG_GIT_BUILD_TIME "构建时间:"

// Hardware debug
#define DEBUG_HW_CHIP_MODEL "芯片型号:"
#define DEBUG_HW_RTC_TYPE "RTC类型: "
#define DEBUG_HW_USED_HEAP "已用堆KB: "
#define DEBUG_HW_CPU_TEMP "CPU温度: "
#define DEBUG_HW_INIT_TEMP "初始温度: "

// Motor debug
#define DEBUG_MOTOR_VIB_DIS "震动已禁用: "
#define DEBUG_MOTOR_VIB_POWER "震动强度: "
#define DEBUG_MOTOR_VIB_TIME "震动时长: "
#define DEBUG_MOTOR_BATT_V "电池电压: "
#define DEBUG_MOTOR_SET_POWER "调整强度"
#define DEBUG_MOTOR_SET_TIME "调整时长"

// RTC Types
#define DEBUG_RTC_UNKNOWN "未知"
#define DEBUG_RTC_DS3231 "DS3231"
#define DEBUG_RTC_PCF8563 "PCF8563"
#define DEBUG_RTC_INTERNAL "内置"
#define DEBUG_RTC_INVALID "无效类型"

// WiFi debug
#define DEBUG_WIFI_MAC_ADDRESS "MAC:"
#define DEBUG_WIFI_STATUS "状态:"
#define DEBUG_WIFI_IP "IP: "
#define DEBUG_WIFI_SSID "SSID: "
#define DEBUG_WIFI_SIGNAL "WiFi信号: "
#define DEBUG_WIFI_CONNECTING "连接中: "
#define DEBUG_WIFI_ON "开"
#define DEBUG_WIFI_OFF "关"

// Common debug terms
#define DEBUG_COMMON_PERCENT "%"
#define DEBUG_COMMON_COLON ": "

// GAME AND APP TRANSLATIONS

#define PONG_YOU_LOST "你输了!"

#define JUMPER_LOST "你输了! 分数: "

#define PARTY_MESSAGE "派对时间"

#define VAULT_TITLE "保险箱"
#define VAULT_KEY_INCORRECT "密码错误"
#define VAULT_EMPTY "保险箱为空?"

// ALARM TRANSLATIONS

#define ALARM_MENU_EDIT_DAYS "编辑日期"
#define ALARM_MENU_EDIT_ALARM "编辑闹钟"
#define ALARM_MENU_ALARMS "闹钟"
#define ALARM_MENU_BROWSE_ALARMS "浏览闹钟"
#define ALARM_MENU_QUICK_ALARMS_SET "快捷闹钟设置"
#define ALARM_MENU_QUICK_ALARM "快捷闹钟"
#define ALARM_MENU_POMODORO "番茄钟"

#define ALARM_STATUS_ENABLED "已启用"
#define ALARM_STATUS_DISABLED "已禁用"
#define ALARM_LABEL_TIME "时间: "
#define ALARM_LABEL_ONE_TIME_ALARM "单次闹钟"
#define ALARM_LABEL_DAYS "日期: "
#define ALARM_LABEL_REQUIRE_WIFI "需要WiFi"

#define ALARM_QUICK_ALERT_TITLE "快捷闹钟提醒"
#define ALARM_QUICK_ALARM_SET_FOR "闹钟设于:"
#define ALARM_QUICK_WILL_RING_AT "响铃时间:"

#define ALARM_DAY_MONDAY "周一"
#define ALARM_DAY_TUESDAY "周二"
#define ALARM_DAY_WEDNESDAY "周三"
#define ALARM_DAY_THURSDAY "周四"
#define ALARM_DAY_FRIDAY "周五"
#define ALARM_DAY_SATURDAY "周六"
#define ALARM_DAY_SUNDAY "周日"

#define ALARM_INFO_ALL_DAYS "每天"
#define ALARM_INFO_NEVER "从不"
#define ALARM_INFO_ONCE ", 单次"
#define ALARM_INFO_WIFI ", WiFi"
#define ALARM_INFO_QUICK ", 快捷"
#define ALARM_INFO_POMODORO ", 番茄钟"

// Pomodoro strings
#define POMODORO_STATUS "状态: "
#define POMODORO_RUNNING "运行中, "
#define POMODORO_WORKING "工作中, "
#define POMODORO_PAUSE "休息, "
#define POMODORO_ITER "轮次: "
#define POMODORO_DISABLED "已禁用"
#define POMODORO_RESET_TURN_OFF "重置并关闭"
#define POMODORO_START "开始"

// Set Clock GUI
#define SETCLOCK_SET_TIME "设置时间"
#define SETCLOCK_SET_DATE "设置日期"
#define SETCLOCK_TIMEZONE_MENU "时区菜单"
#define SETCLOCK_CLOCK_SETTINGS "时钟设置"
#define SETCLOCK_CURRENT_TIMEZONE "当前时区: "
#define SETCLOCK_CLEAR_TIMEZONE "清除时区"
#define SETCLOCK_SET_UTC_TIMEZONE "设置UTC时区"
#define SETCLOCK_SET_LOCATION_TIMEZONE "按位置设置时区"
#define SETCLOCK_TIMEZONE_SETTINGS "时区设置"
#define SETCLOCK_TIMEZONE_WARNING "此菜单忽略夏令时/标准时间，将覆盖当前时区设置。\n按返回键确认"
#define SETCLOCK_WARNING_TITLE "警告"
#define SETCLOCK_SELECT_TIMEZONE "选择时区"

// Time unit translations
#define TIME_UNIT_MINUTE "分钟"
#define TIME_UNIT_MINUTES "分钟"
#define TIME_UNIT_HOUR "小时"
#define TIME_UNIT_HOURS "小时"
#define TIME_UNIT_DAY "天"
#define TIME_UNIT_DAYS "天"
#define TIME_UNIT_AND "和"