#pragma once

#include "StreamBackend.h"
#include <fstream>

class FileBackend :
	public StreamBackend
{
public:
	FileBackend(const char* path);
	~FileBackend();

  size_t get_size();
  void read(void* buffer, size_t size);
  void seek(size_t position);
  bool is_eof();
  uint8_t peek();
  void get_line(std::string& line_read);

  void open(const char* path);

private:
  std::ifstream reader;
};

