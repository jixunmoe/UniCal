#pragma once

#include "./IBaseControl.hpp"
#include "./CalendarItemControl.hpp"

#include "../ics/ics_parse.hpp"
#include "../safe_function.h"

#include <SDL2/SDL.h>
#include <vector>
#include <chrono>
#include <thread>
#include <future>

#define CALENDAR_ITEM_HEIGHT (14*3 + 4)

class CalendarListControl : public IBaseControl {
private:
  std::vector<CalendarItemControl*> m_items;
  IcsParse* ics_parser = nullptr;
  std::future<void> m_update_task;

public:
  void update_calendar()
  {
    SDL_Rect p = SDL_Rect{
      m_pos.x, m_pos.y,
      m_pos.w, CALENDAR_ITEM_HEIGHT
    };
    
    time_t tt;
    tm local{};

    time(&tt);
    Safe::localtime(&local, &tt);

    int year = 1900 + local.tm_year;
    int month = local.tm_mon + 1;
    
    CalendarEvent event{};
    ics_parser->restart();

    for(unsigned int i = 0; i < 5; i++)
    {
      if (i >= m_items.size())
      {
        m_items.push_back(new CalendarItemControl(&p));
      }
      
      const bool found = ics_parser->next_event(event, [&local, &year, &month](CalendarEvent& e)->bool
      {
        return 
          e.start.year >= year
        && e.start.month >= month
        && e.start.day >= local.tm_mday;
      });

      printf("found %d event; title = %s", found, event.summary);

      if (found)
      {
        m_items.at(i)->show();
        m_items.at(i)->set_calender(event);
        m_items.at(i)->set_pos_ref(p);
      } else
      {
        m_items.at(i)->hide();
      }

      p.y = p.y + CALENDAR_ITEM_HEIGHT - 1;
    }

    // Update every 10 mins
    m_update_task = std::async(std::launch::async, [this](){ 
      std::this_thread::sleep_for(std::chrono::minutes(10));
      this->update_calendar();
    });
  }

  CalendarListControl(SDL_Rect* pos = nullptr): IBaseControl(pos) {
    ics_parser = new IcsParse("./ical.ics");

    update_calendar();
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
