#pragma once

#include "./IBaseControl.hpp"
#include <SDL2/SDL.h>

class FrameControl : public IBaseControl {
public:
  FrameControl(SDL_Rect* pos = nullptr): IBaseControl(pos) {
    // do nothing
  }

  void render() {
    SDL_SetRenderDrawColor(get_renderer(), 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawRect(get_renderer(), &m_pos);
  }
};
