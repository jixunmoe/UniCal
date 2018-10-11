#include "controls/FrameControl.hpp"
#include "controls/CalendarListControl.hpp"
#include "types.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <vector>
#include "FontManager.hpp"

#define WND_WIDTH  (320)
#define WND_HEIGHT (240)

// Manage error messages
void check_error_sdl(bool check, const char* message);
void check_error_sdl_img(bool check, const char* message);

// Load an image from "fname" and return an SDL_Texture with the content of the image
SDL_Texture* load_texture(const char* fname, SDL_Renderer* renderer);

std::vector<IBaseControl*> controls;

#if defined(__WIN32__)
#undef main
#endif

int main(int argc, char** argv)
{
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    printf("failed to initialise video: %s\n", SDL_GetError());
    SDL_Quit();
  }

  SDL_ShowCursor(false);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  // Create and initialize a 800x600 window
  SDL_Window* window = SDL_CreateWindow("Test SDL 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                        WND_WIDTH, WND_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
  check_error_sdl(window == nullptr, "Unable to create window");

  // Create and initialize a hardware accelerated renderer that will be refreshed in sync with your monitor (at approx. 60 Hz)
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  IBaseControl::setup(renderer);
  check_error_sdl(renderer == nullptr, "Unable to create a renderer");

  // Initialize SDL_img
  {
    const int flags = IMG_INIT_PNG;
    const int initted = IMG_Init(IMG_INIT_PNG);
    check_error_sdl_img((initted & flags) != flags, "Unable to initialize SDL_image");
  }

  // Initialise SDL_ttf
  check_error_sdl(TTF_Init() == -1, "ttf init failed");
  FontManager::get_instance()->register_font("default", "SourceSansPro-Regular.ttf");
  FontManager::get_instance()->register_font("mono", "04B_08__.ttf");

  {
    SDL_Rect position = {
      4, 4,
      WND_WIDTH - 8, WND_HEIGHT - 8
    };
    FrameControl* app_frame = new FrameControl(&position);
    controls.push_back(app_frame);
  }
  {
    SDL_Rect position = {
      4, 4,
      10, 10
    };
    FrameControl* app_frame = new FrameControl(&position);
    controls.push_back(app_frame);
  }
  {
    SDL_Rect position = {
      4, 4,
      WND_WIDTH - 8, WND_HEIGHT - 8
    };
    CalendarListControl* cal_list = new CalendarListControl(&position);
    controls.push_back(cal_list);
  }

  bool quit = false;
  SDL_Event event;

  SDL_ShowCursor(SDL_DISABLE);
  while (!quit)
  {
    // Clear the window content (using the default renderer color)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (auto& control : controls)
    {
      control->render();
    }

    // Update the window surface (show the renderer)
    SDL_RenderPresent(renderer);

    if (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        quit = true;
      }
      else if (event.type == SDL_KEYDOWN)
      {
        // TODO: Do something?
      }
    }
  }

  // Wait for 10 seconds
  // SDL_Delay(10000);

  // Clear the allocated resources
  TTF_Quit();
  IMG_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

// In case of error, print the error code and close the application
void check_error_sdl(bool check, const char* message)
{
  if (check)
  {
    std::cout << message << " " << SDL_GetError() << std::endl;
    SDL_Quit();
    std::exit(-1);
  }
}

// In case of error, print the error code and close the application
void check_error_sdl_img(bool check, const char* message)
{
  if (check)
  {
    std::cout << message << " " << IMG_GetError() << std::endl;
    IMG_Quit();
    SDL_Quit();
    std::exit(-1);
  }
}

// Load an image from "fname" and return an SDL_Texture with the content of the image
SDL_Texture* load_texture(const char* fname, SDL_Renderer* renderer)
{
  SDL_Surface* image = IMG_Load(fname);

  // SDL_RWops *rwop = SDL_RWFromFile(fname, "rb");
  // SDL_Surface *image = IMG_LoadPNG_RW(rwop);

  check_error_sdl_img(image == nullptr, "Unable to load image");

  SDL_Texture* img_texture = SDL_CreateTextureFromSurface(renderer, image);
  check_error_sdl_img(img_texture == nullptr, "Unable to create a texture from the image");
  SDL_FreeSurface(image);
  return img_texture;
}
