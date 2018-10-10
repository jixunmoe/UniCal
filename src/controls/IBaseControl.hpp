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

protected:
  SDL_Rect m_pos;

  virtual void internal_set_pos(SDL_Rect& pos)
  {
    m_pos = pos;
  }
};

SDL_Renderer* IBaseControl::s_renderer;
