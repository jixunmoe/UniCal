#pragma once

#include <SDL2/SDL.h>
#include "../types.h"

class IBaseControl
{
private:
  static SDL_Renderer* s_renderer;

public:
  static void setup(SDL_Renderer* renderer)
  {
    s_renderer = renderer;
  }

  static SDL_Renderer* get_renderer()
  {
    return s_renderer;
  }

  IBaseControl(SDL_Rect* pos = nullptr)
  {
    m_pos = SDL_Rect{0, 0, 0, 0};
    if (pos != nullptr)
    {
      set_pos_ref(*pos);
    }
  }

  virtual ~IBaseControl()
  {
    // do nothing
  }

  virtual void handle()
  {
    // do nothing
  }

  virtual void render()
  {
    // do nothing
  }

  void set_pos(SDL_Rect pos)
  {
    internal_set_pos(pos);
  }

  void set_pos_ref(SDL_Rect& pos)
  {
    internal_set_pos(pos);
  }

  virtual SDL_Rect get_pos()
  {
    return m_pos;
  }

  virtual SDL_Rect* get_pos_ptr()
  {
    return &m_pos;
  }

protected:
  SDL_Rect m_pos;

  virtual void hook_pos_updated()
  {
    // Do nothing
  }

  virtual void internal_set_pos(SDL_Rect& pos)
  {
    if (m_pos.x != pos.x || m_pos.y != pos.y || m_pos.w != pos.w || m_pos.h != m_pos.h)
    {
      m_pos = pos;
      hook_pos_updated();
    }
  }
};

SDL_Renderer* IBaseControl::s_renderer;
