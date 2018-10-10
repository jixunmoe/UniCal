#pragma once

#include "./CalendarItemControl.hpp"
#include "./IBaseControl.hpp"

#include <SDL2/SDL.h>
#include <vector>
#include "../ics/ics_parse.hpp"
#include <chrono>

#define CALENDAR_ITEM_HEIGHT (14*3 + 4)

class CalendarListControl : public IBaseControl {
private:
  std::vector<CalendarItemControl*> m_items;
  IcsParse* ics_parser = nullptr;

public:
  CalendarListControl(SDL_Rect* pos = nullptr): IBaseControl(pos) {
    char temp_buffer[1024];
    printf("before load ical parser...\n");

    ics_parser = new IcsParse("./ical.ics");

    time_t tt;
    time(&tt);
    // ReSharper disable once CppDeprecatedEntity
    tm local;
    localtime_s(&local, &tt);

    int year = 1900 + local.tm_year;
    int month = local.tm_mon + 1;
    
    CalendarEvent event;
    ics_parser->next_event(event, [&local, &year, &month](CalendarEvent& e)->bool
    {
      return 
        e.start.year >= year
      && e.start.month >= month
      && e.start.day >= local.tm_mday;
    });

    printf("ok, first event loaded.");

    SDL_Rect* p1 = new SDL_Rect{
      m_pos.x, m_pos.y,
      m_pos.w, CALENDAR_ITEM_HEIGHT
    };
    auto test_cal1 = new CalendarItemControl(p1);



    snprintf(temp_buffer, sizeof(temp_buffer), "%s (%s)", event.module, event.type);
    test_cal1->set_title(temp_buffer);
    
    snprintf(temp_buffer, sizeof(temp_buffer), "%02d:%02d", event.start.hour, event.start.min);
    test_cal1->set_time(temp_buffer);

    test_cal1->set_location(event.room);
    test_cal1->set_lecturer(event.staff);
    printf("event: name=%s, location=%s\n", event.summary, event.location);

    SDL_Rect* p2 = new SDL_Rect{
      m_pos.x, m_pos.y + CALENDAR_ITEM_HEIGHT + 2,
      m_pos.w, CALENDAR_ITEM_HEIGHT
    };
    auto test_cal2 = new CalendarItemControl(p2);
    ics_parser->next_event(event, [&local, &year, &month](CalendarEvent& e)->bool
    {
      return 
        e.start.year >= year
      && e.start.month >= month
      && e.start.day >= local.tm_mday;
    });
    
    snprintf(temp_buffer, sizeof(temp_buffer), "%s (%s)", event.module, event.type);
    test_cal2->set_title(temp_buffer);

    snprintf(temp_buffer, sizeof(temp_buffer), "%02d:%02d", event.start.hour, event.start.min);
    test_cal2->set_time(temp_buffer);

    test_cal2->set_location(event.room);
    test_cal2->set_lecturer(event.staff);

    printf("ok, second event loaded.");

    m_items.push_back(test_cal1);
    m_items.push_back(test_cal2);
  }

  ~CalendarListControl()
  {
    free_ics_parser();
  }

  void render() override
  {
    for (auto && item : m_items)
    {
      item->render();
    }
  }

private:
  void free_ics_parser()
  {
    if (ics_parser)
    {
      free(ics_parser);
      ics_parser = nullptr;
    }
  }
};
