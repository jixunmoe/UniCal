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
  unsigned short year;
  unsigned short month;
  unsigned short day;

  unsigned short hour;
  unsigned short min;

  char timezone[SIZE_TINYTEXT];
};

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
