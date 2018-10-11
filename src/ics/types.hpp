#pragma once

#define CALENDAR_DESC_FIELD (1)
#define CALENDAR_UNIVERSITY_YORK_EXTEND_INFO (1)

#define SIZE_TEXT (1024)
#define SIZE_SMALLTEXT (255)
#define SIZE_TINYTEXT (40)
#define SIZE_MODULE_CODE (4)

#if !defined(CALENDAR_DESC_FIELD)
#define CALENDAR_DESC_FIELD (0)
#endif

struct CalendarTime
{
  uint16_t year;
  uint8_t month;
  uint8_t day;

  uint8_t hour;
  uint8_t min;

  char timezone[SIZE_TINYTEXT];
};

inline uint32_t getCalendarStamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min)
{
  // 9999: 12 bit
  // 12: 4 bit
  // 31: 5 bit
  // 24: 5 bit
  // 60: 6 bit
  // ---- 12 + 4 + 5 + 5 + 6 = 32 bit
  // ----- year << 20
  // -------- month << 16
  // ----------- day << 11
  // -------------- hour << 6
  // ----------------- min << 0

  return
    static_cast<uint32_t>(year << 20) |
    static_cast<uint32_t>(month << 16) |
    static_cast<uint32_t>(day << 11) |
    static_cast<uint32_t>(hour << 6) |
    static_cast<uint32_t>(min << 0);
}

inline long long int getCalendarStamp(CalendarTime& time)
{
  return getCalendarStamp(time.year, time.month, time.day, time.hour, time.min);
}

struct CalendarEvent
{
  char location[SIZE_TEXT];
  char summary[SIZE_TEXT];
  
#if CALENDAR_DESC_FIELD
  char description[SIZE_TEXT];
#endif

#if CALENDAR_UNIVERSITY_YORK_EXTEND_INFO
  char room[SIZE_TINYTEXT];
  char module[SIZE_MODULE_CODE + 1];
  char staff[SIZE_SMALLTEXT];
  char type[SIZE_TINYTEXT];
#endif

  CalendarTime start;
  CalendarTime end;
};
