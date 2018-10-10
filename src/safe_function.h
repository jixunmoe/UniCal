#pragma once
#include <cstring>
#include <cassert>
#include <ctime>

namespace Safe
{
  inline void strncpy(char* dest, size_t size, const char* src, size_t n_copy)
  {
    if (n_copy < size)
    {
      memcpy(dest, src, n_copy);

      // set null terminator
      dest[n_copy] = 0;
    } else
    {
      assert("strlen is larger than size for dest");
    }
  }

  inline void strcpy(char* dest, size_t size, const char* src)
  {
    strncpy(dest, size, src, strlen(src));
  }


  inline void localtime(std::tm* result, const std::time_t* time)
  {
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    localtime_s(result, time);
#else
    localtime_r(time, result); // POSIX
#endif
  }
}
