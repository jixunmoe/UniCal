#pragma once

#include "types.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <functional>

#define ICS_STR_EQUAL_TO(a, b) (strncmp((a), (b), sizeof(b)) == 0)
// typedef bool (*ics_event_filter)(CalendarEvent& event);
typedef std::function<bool(CalendarEvent& event)> ics_event_filter;


class IcsParse
{
public:
  IcsParse(const std::string& path)
  {
    file.open(path, std::ifstream::in | std::ifstream::binary);

    char buffer[20];
    size_t size = sizeof(buffer);

    while(!file.eof())
    {
      if (!readline(buffer, size))
      {
        break;
      }
      if (strncmp(buffer, "BEGIN:VCALENDAR", size) == 0)
      {
        break;
      }
    }
  }
  ~IcsParse()
  {
    if (file.is_open())
    {
      file.close();
    }
  }

  static bool event_filter_all(CalendarEvent& event)
  {
    return true;
  }

  bool next_event(CalendarEvent& event, ics_event_filter filter = event_filter_all)
  {
    size_t str_size;

    bool vevent_begin = false;
    
    while(!file.eof())
    {
      readline(event_buffer, SIZE_TEXT, &str_size);

      if (vevent_begin)
      {
        if (ICS_STR_EQUAL_TO(event_buffer, "END:VEVENT"))
        {
          if (filter(event))
          {
            break;
          }
        }
      } else
      {
        if (ICS_STR_EQUAL_TO(event_buffer, "BEGIN:VEVENT"))
        {
          vevent_begin = true;
          continue;
        }
      }

      const auto split = strchr(event_buffer, ':');
      if (split == nullptr)
      {
        continue;
      }

      const int length = split - event_buffer;
      *split = '\x00';
      char* value = split + 1;

      
      char* name_params = strchr(event_buffer, ';');
      if (name_params)
      {
        *name_params = '\x00';
        name_params += 1;
      }

      // DOSA Drop-in Session (optional) - COM00023H-A (Practical)
      // FUML Problem class - Group 1 - COM00028H-A (Seminar)
      // [Initials] [optional Name] - ModuleID (Type)

      // Description:
      // Staff member(s): Kaul, Chaitanya, Manandhar, Suresh (Dr)
      // ^ Every second comma is for new line.
      if (ICS_STR_EQUAL_TO(event_buffer, "SUMMARY"))
      {
        strcpy_s(event.summary, sizeof(event.summary), value);
#if CALENDAR_UNIVERSITY_YORK_EXTEND_INFO
        strncpy_s(event.module, sizeof(event.module), event.summary, 4);
        
        {
          // Populate event type (lecture/practial)
          char* beginType = strrchr(event.summary, '(');
          if (beginType != nullptr)
          {
            beginType += 1;
            char* endType = strchr(beginType, ')');
            strncpy_s(event.type, sizeof(event.type), beginType, endType - beginType);
          }
        }
#endif
      } else if (ICS_STR_EQUAL_TO(event_buffer, "LOCATION"))
      {
        strcpy_s(event.location, sizeof(event.location), value);
#if CALENDAR_UNIVERSITY_YORK_EXTEND_INFO
        {
          char* comma = strchr(value, ',');
          if (comma != nullptr) *comma = '\x00';
        }

        strcpy_s(event.room, sizeof(event.room), value);
#endif
      } else if (ICS_STR_EQUAL_TO(event_buffer, "DTSTART"))
      {
        parse_time(event.start, value, name_params);
      } else if (ICS_STR_EQUAL_TO(event_buffer, "DTEND"))
      {
        parse_time(event.end, value, name_params);
      }
#if CALENDAR_DESC_FIELD
      else if (ICS_STR_EQUAL_TO(event_buffer, "DESCRIPTION"))
      {
        strcpy_s(event.description, sizeof(event.description), value);
#if CALENDAR_UNIVERSITY_YORK_EXTEND_INFO
        extract_desc_text(event.description, "Staff member(s): ", event_buffer, sizeof(event_buffer));
        if (event_buffer[0] != 0)
        {
          strcpy_s(event.staff, sizeof(event.staff), event_buffer);
        }
#endif
      }
#endif
    }

    return false;
  }

private:
  std::ifstream file;
  char event_buffer[SIZE_TEXT];

  static void extract_desc_text(char* desc, const char* keyword, char* buffer, size_t size)
  {
    if (size == 0) return;

    const int keyword_len = strlen(keyword);

    char* line = desc;

    while(line)
    {
      if (strncmp(line, keyword, keyword_len) == 0)
      {
        char* value_begin = line + keyword_len;
        char* value_end = strchr(value_begin, '\n');
        if (value_end == nullptr)
        {
          // end of text
          strcpy_s(buffer, size, value_begin);
        } else
        {
          strncpy_s(buffer, size, value_begin, value_end - value_begin);
        }
        return;
      }

      line = strchr(line, '\n');
      if (line == nullptr)
      {
        break;
      }
      line++;
    }

    buffer[0] = 0;
  }

  static unsigned int read_dec(const char* str, int size)
  {
    unsigned int result = 0;
    for(int i = 0; i < size; i++)
    {
      result = result * 10 + (str[i] - '0');
    }

    printf("read %s(size=%d)=%d\n", str, size, result);

    return result;
  }

  static void parse_time(CalendarTime& time, char* time_str, char* params)
  {
    // DTSTART;TZID=Europe/London:20180812T234500

    char* timezone = params == nullptr ? nullptr : strstr(params, "TZID=");

    if (timezone == nullptr)
    {
      strcpy_s(time.timezone, sizeof(time.timezone), "UTC");
    } else
    {
      strcpy_s(time.timezone, sizeof(time.timezone), timezone + 5);
    }
    
    time.year = read_dec(time_str, 4);
    time.month = read_dec(time_str + 4, 2);
    time.day = read_dec(time_str + 4 + 2, 2);
    time.hour = read_dec(time_str + 4 + 2 + 2 + 1, 2);
    time.min = read_dec(time_str + 4 + 2 + 2 + 1 + 2, 2);
  }

  std::string readline()
  {
    if (file.eof()) return "";

    std::string line_read;
    std::getline(file, line_read);

    std::stringstream builder(line_read);

    while(!file.eof())
    {
      if (file.peek() == ' ')
      {
        // skip the space
        file.seekg(1, std::ios_base::cur);

        std::getline(file, line_read);

        // Connect the line
        builder << line_read;
      } else
      {
        break;
      }
    }
    
    return builder.str();
  }

  bool readline(char* buffer, size_t size, size_t* str_size = nullptr)
  {
    if (file.eof()) return false;

    bool escape_next = false;
    char next_byte;
    unsigned int pos = 0;

    while(true)
    {
      file.read(&next_byte, 1);

      if (escape_next)
      {
        switch (next_byte)
        {
        case 'n':
          next_byte = '\n';
          break;

        case 't':
          next_byte = '\t';
          break;

        default:
          // do nothing.
          break;
        }
        escape_next = false;
      } else if (next_byte == '\\')
      {
        escape_next = true;
        continue;
      } else if (next_byte == '\r')
      {
        continue;
      } else if (next_byte == '\n')
      {
        if (file.peek() == ' ')
        {
          file.seekg(1, std::ios_base::cur);
          continue;
        }

        break;
      }

      if (pos < size)
      {
        buffer[pos] = next_byte;
        pos++;
      }
    }

    if (pos < size)
    {
      buffer[pos] = '\x00';
    }
    if (str_size != nullptr)
    {
      *str_size = pos;
    }
    return true;
  }
};
