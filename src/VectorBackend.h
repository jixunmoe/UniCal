#pragma once

#include "StreamBackend.h"
#include <vector>

class VectorBackend :
	public StreamBackend
{
public:
	VectorBackend();
	~VectorBackend();

  size_t get_size();
  void read(void* buffer, size_t size);
  void seek(size_t position);
  bool is_eof();
  uint8_t peek();
  void get_line(std::string& line_read);

  void assign(std::vector<uint8_t>& data);

private:
  size_t pos;
  std::vector<uint8_t> data;
};

