#pragma once

#include <cstdint>
#include <string>

class StreamBackend
{
public:
	StreamBackend();
	~StreamBackend();

  virtual size_t get_size() = 0;
  virtual void read(void* buffer, size_t size) = 0;
  virtual void seek(size_t position) = 0;
  virtual bool is_eof() = 0;
  virtual uint8_t peek() = 0;
  virtual void get_line(std::string& line_read) = 0;
};

