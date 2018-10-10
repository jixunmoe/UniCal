#pragma once

#define MIN(A, B) \
  ((A < B) ? (A) : (B))

#define MAX(A, B) \
  ((A > B) ? (A) : (B))

#define FREE_SDL(FN, POINTER)     \
  {                               \
    if (POINTER) { FN(POINTER); } \
  }

typedef SDL_Rect* SDL_Rect_PTR;
