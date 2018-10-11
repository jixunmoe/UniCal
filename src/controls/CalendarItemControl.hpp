#pragma once

#include "./IBaseControl.hpp"
#include "./LabelControl.hpp"
#include "../ics/types.hpp"
#include "../safe_function.h"

#include <SDL2/SDL.h>
#include <vector>

class CalendarItemControl : public IBaseControl {
private:
  LabelControl* m_location;
  LabelControl* m_subject;
  LabelControl* m_lecturer;
  LabelControl* m_time;

  CalendarEvent m_event{};
  bool m_hide;

public:
  CalendarItemControl(SDL_Rect* pos = nullptr): IBaseControl(pos) {
    m_location = new LabelControl();
    m_location->set_text("-");
    m_subject = new LabelControl();
    m_subject->set_text("-");
    m_lecturer = new LabelControl();
    m_lecturer->set_text("-");

    const auto font_12 = FontManager::get_instance()->get_font("default", 12);
    m_location->set_font(font_12);
    m_subject->set_font(font_12);
    m_lecturer->set_font(font_12);
    
    const auto font_16 = FontManager::get_instance()->get_font("default", 16);
    m_time = new LabelControl();
    m_time->set_text("00:00");
    m_time->set_font(font_16);

    resize();
  }

  void update()
  {
    char temp_buffer[1024];

    snprintf(temp_buffer, sizeof(temp_buffer), "%02d:%02d", m_event.start.hour, m_event.start.min);
    m_time->set_text(temp_buffer);

    m_lecturer->set_text(m_event.staff);

#if CALENDAR_UNIVERSITY_YORK_EXTEND_INFO
    m_location->set_text(m_event.room);
    
    snprintf(temp_buffer, sizeof(temp_buffer), "%s (%s)", m_event.module, m_event.type);
    m_subject->set_text(temp_buffer);
#else
    m_location->set_text(m_event.location);
    m_subject->set_text(m_event.summary);
#endif
  }

  void set_calender(CalendarEvent& calender)
  {
    memcpy(&m_event, &calender, sizeof(CalendarEvent));
    update();
  }

  void set_location(const std::string& text)
  {
    Safe::strcpy(m_event.location, sizeof(m_event.location), text.c_str());
    update();
  }

  void set_summary(const std::string& text)
  {
    Safe::strcpy(m_event.summary, sizeof(m_event.summary), text.c_str());
    update();
  }

  void set_lecturer(std::string text)
  {
    Safe::strcpy(m_event.staff, sizeof(m_event.staff), text.c_str());
    update();
  }

  void internal_set_pos(SDL_Rect& pos) override
  {
    IBaseControl::internal_set_pos(pos);
    resize();
  }

  void hide()
  {
    m_hide = true;
  }

  void show()
  {
    m_hide = false;
  }

  void resize()
  {
    printf("CalItem.size = {%d, %d, %d, %d}\n", m_pos.x, m_pos.y, m_pos.w, m_pos.h);

    const int timeWidth = 3 * 14;
    const int fieldWidth = m_pos.w - timeWidth;
    const int fieldHeight = (m_pos.h - 4) / 3;

    m_time->set_pos(SDL_Rect{
      m_pos.x, m_pos.y,
      timeWidth, m_pos.h
    });

    m_subject->set_pos(SDL_Rect{
      m_pos.x + timeWidth, m_pos.y,
      fieldWidth, fieldHeight
    });

    m_lecturer->set_pos(SDL_Rect{
      m_pos.x + timeWidth, m_pos.y + fieldHeight,
      fieldWidth, fieldHeight
    });

    m_location->set_pos(SDL_Rect{
      m_pos.x + timeWidth, m_pos.y + fieldHeight * 2,
      fieldWidth, fieldHeight
    });
    
    m_time->debug("Cal.Time");
    m_subject->debug("Cal.Subj");
    m_lecturer->debug("Cal.Lect");
    m_location->debug("Cal.Loca");
  }

  void render() {
    if (m_hide) return;

    SDL_SetRenderDrawColor(get_renderer(), 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawRect(get_renderer(), m_time->get_pos_ptr());
    SDL_RenderDrawRect(get_renderer(), &m_pos);

    m_time->render();
    m_subject->render();
    m_lecturer->render();
    m_location->render();
  }
};
