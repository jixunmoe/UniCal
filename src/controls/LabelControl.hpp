#pragma once

#include "../types.h"
#include "./IBaseControl.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include "../FontManager.hpp"

class LabelControl : public IBaseControl
{
private:
  std::string m_text;
  SDL_Color m_colour;
  TTF_Font* m_font;

  bool m_invalidate_texture;
  bool m_invalidate_position;

  SDL_Surface* m_surface = nullptr;
  SDL_Texture* m_texture = nullptr;

  SDL_Rect m_src_rect;
  SDL_Rect m_dst_rect;
  
  bool m_vertical_center;
  bool m_horizontal_center;

public:
  LabelControl(SDL_Rect* pos = nullptr): IBaseControl(pos)
  {
    m_font = FontManager::get_instance()->get_font("default", 14);
    m_text = "";
    m_colour = SDL_Color{255, 255, 255, 255};

    m_src_rect = SDL_Rect{0, 0, 0, 0};
    m_dst_rect = SDL_Rect{0, 0, 0, 0};

    m_vertical_center = true;
    m_horizontal_center = false;

    m_invalidate_texture = true;
    m_invalidate_position = true;
  }

  ~LabelControl()
  {
    invalidate_texture();
  }

  void set_text(std::string text)
  {
    if (m_text != text)
    {
      m_text = text;
      invalidate_texture();
    }
  }

  void set_colour(SDL_Color* colour)
  {
    m_colour = *colour;
    invalidate_texture();
  }

  void set_font(TTF_Font* font)
  {
    m_font = font;
    invalidate_texture();
  }

  void debug(const std::string& name = "Control") const
  {
    printf("%s.size = {%d, %d, %d, %d}\n", name.c_str(), m_pos.x, m_pos.y, m_pos.w, m_pos.h);
  }

  void render() override
  {
    if (m_text[0] == '\x00') return;

    // Create Texture
    if (m_invalidate_texture) if (!create_texture()) return;
    if (m_invalidate_position) calculate_position();

    SDL_RenderCopy(get_renderer(), m_texture, &m_src_rect, &m_dst_rect);
  }

protected:
  void internal_set_pos(SDL_Rect& pos) override
  {
    IBaseControl::internal_set_pos(pos);
    m_invalidate_position = true;
  }

private:
  void invalidate_texture()
  {
    FREE_SDL(SDL_DestroyTexture, m_texture);
    FREE_SDL(SDL_FreeSurface, m_surface);
    m_invalidate_texture = true;
  }

  bool create_texture()
  {
    m_surface = TTF_RenderText_Blended(m_font, m_text.c_str(), m_colour);

    if (!m_surface)
    {
      printf("surface is null (text=%s)!\n", m_text.c_str());
      return false;
    }

    m_texture = SDL_CreateTextureFromSurface(get_renderer(), m_surface);
    if (!m_texture)
    {
      SDL_FreeSurface(m_surface);

      printf("texture is null!\n");
      return false;
    }
    
    m_invalidate_texture = false;
    m_invalidate_position = true;
    return true;
  }

  void calculate_position()
  {
    // Calculate width/height
    const int w = MIN(m_surface->w, m_pos.w);
    const int h = MIN(m_surface->h, m_pos.h);

    m_src_rect = SDL_Rect{
      0, 0,
      w, h
    };

    // Center the label
    const int xOffset = MAX((m_pos.w - m_surface->w) / 2, 0);
    const int yOffset = MAX((m_pos.h - m_surface->h) / 2, 0);

    m_dst_rect = SDL_Rect{
      m_pos.x + xOffset, m_pos.y + yOffset,
      w, h,
    };

    m_invalidate_position = false;
  }
};
