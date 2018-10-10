#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <map>

class FontManager
{
private:
  struct FontData
  {
    SDL_RWops* font_file;
    std::map<int, std::map<int, TTF_Font*>> font_index_size;
  };
  std::map<std::string, FontData> m_fonts;

public:
  static FontManager* get_instance()
  {
    static FontManager* instance = nullptr;
    if (instance == nullptr)
    {
      instance = new FontManager();
    }
    return instance;
  }

  TTF_Font* get_font(const std::string& name, int size, int index = 0)
  {
    // Font does not exist or not registered
    const auto& font_iter = m_fonts.find(name);
    if (font_iter == m_fonts.end())
    {
      printf("font %s has not beed registered.\n", name.c_str());
      return nullptr;
    }

    FontData& font = font_iter->second;
    auto& index_map = font.font_index_size;
    auto index_iter = index_map.find(index);
    if (index_iter == index_map.end())
    {
      std::map<int, TTF_Font*> local_size;
      index_map.insert(std::pair<int, std::map<int, TTF_Font*>>(index, local_size));
      index_iter = index_map.find(index);
    }

    std::map<int, TTF_Font*>& size_map = index_iter->second;

    const auto& size_iter = size_map.find(size);
    if (size_iter != size_map.end()) return size_iter->second;

    TTF_Font* font_ptr = TTF_OpenFontIndexRW(font.font_file, 1, size, index);
    size_map.insert(std::pair<int, TTF_Font*>(size, font_ptr));
    return font_ptr;
  }

  void register_font(const std::string& name, const std::string& path)
  {
    m_fonts.insert(std::pair<std::string, FontData>(name, FontData{
      SDL_RWFromFile(path.c_str(), "rb"),
      std::map<int, std::map<int, TTF_Font*>>{}
    }));
  }
  
  static SDL_Point measure_font(const std::string& text, TTF_Font* font)
  {
    SDL_Point result;
    TTF_SizeUTF8(font, text.c_str(), &result.x, &result.y);
    return result;
  }

  SDL_Point measure_text(const std::string& text, const std::string& name, int size, int index = 0)
  {
    return measure_font(text, get_font(name, size, index));
  }
};
