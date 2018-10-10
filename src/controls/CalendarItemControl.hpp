#pragma once

#include "./IBaseControl.hpp"
#include "./LabelControl.hpp"

#include <SDL2/SDL.h>
#include <vector>

class CalendarItemControl : public IBaseControl {
private:
  LabelControl* m_location;
  LabelControl* m_subject;
  LabelControl* m_lecturer;
  LabelControl* m_time;

public:
  CalendarItemControl(SDL_Rect* pos = nullptr): IBaseControl(pos) {
    m_location = new LabelControl();
    m_location->set_text("RCH/070");
    m_subject = new LabelControl();
    m_subject->set_text("TPOP");
    m_lecturer = new LabelControl();
    m_lecturer->set_text("-");

    m_time = new LabelControl();
    m_time->set_text("00:00");

    resize();
  }

  void set_location(std::string text) const
  {
    m_location->set_text(text);
  }

  void set_title(std::string text) const
  {
    m_subject->set_text(text);
  }

  void set_lecturer(std::string text) const
  {
    m_lecturer->set_text(text);
  }

  void set_time(std::string text) const
  {
    m_time->set_text(text);
  }
  
  void internal_set_pos(SDL_Rect& pos) override
  {
    // 5 CHAR | REST OF CONTENT

    //   MM   | SUBJECT
    //   ..   | LECTURER
    //   SS   | LOCATION
    // ----------------- <- Progress Bar (HEIGHT = 4px)
    IBaseControl::internal_set_pos(pos);
    resize();
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
    m_time->render();
    m_subject->render();
    m_lecturer->render();
    m_location->render();
  }
};
