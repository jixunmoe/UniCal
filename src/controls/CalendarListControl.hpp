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

#define NUMBER_OF_EVENTS_PER_PAGE (7)
#define CALENDAR_ITEM_HEIGHT (14*3 + 4)
#define CALENDAR_DATE_HEIGHT (15)

class CalendarListControl : public IBaseControl {
private:
  std::mutex m_update_mutex;
  unsigned int m_label_render_size{0};
  std::vector<CalendarItemControl*> m_items;
  IcsParse* ics_parser = nullptr;
  std::future<void> m_update_task;

  std::vector<LabelControl*> m_date_labels;
  std::map<int, CalendarTime> m_dates;

public:
  void update_calendar()
  {
    std::lock_guard<std::mutex> update_mutex_guard(m_update_mutex);
    printf("update_calendar...\n");
    m_dates.clear();

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
    int day = local.tm_mday;
    
    CalendarEvent event{};
    ics_parser->restart();

    for(unsigned int i = 0; i < NUMBER_OF_EVENTS_PER_PAGE; i++)
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
        && e.start.day >= local.tm_mday
        && e.start.hour >= local.tm_hour;
      });

      printf("found %d event; title = %s\n", found, event.summary);

      if (day != event.start.day)
      {
        day = event.start.day;
        m_dates.insert(std::pair<int, CalendarTime>(p.y, event.start));
        p.y = p.y + CALENDAR_DATE_HEIGHT - 1;
      }

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

    unsigned int date_label_index = 0;
    char buffer[1024];
    p.x += 8;
    p.h = CALENDAR_DATE_HEIGHT;
    
    for (auto && date : m_dates)
    {
      if (date_label_index >= m_date_labels.size())
      {
        auto label = new LabelControl(&p);
        label->set_font(FontManager::get_instance()->get_font("default", 12));
        label->set_align(LabelControl::LabelAlign::LEFT);
        m_date_labels.push_back(label);
      }
      p.y = date.first;
      snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", date.second.day, date.second.month, date.second.year);
      printf("date.first = %d; text=%s\n", date.first, buffer);
      m_date_labels.at(date_label_index)->set_pos(p);
      m_date_labels.at(date_label_index)->set_text(buffer);

      date_label_index++;
    }

    m_label_render_size = date_label_index;

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
    std::lock_guard<std::mutex> update_mutex_guard(m_update_mutex);
    for (auto && item : m_items)
    {
      item->render();
    }
    
    SDL_SetRenderDrawColor(get_renderer(), 0xFF, 0xFF, 0xFF, 0xFF);
    for(unsigned int i = 0; i < m_label_render_size; i++)
    {
      SDL_Rect pos = m_date_labels.at(i)->get_pos();
      pos.x = m_pos.x;
      pos.w = m_pos.w;
      SDL_RenderDrawRect(get_renderer(), &pos);
      m_date_labels.at(i)->render();
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
