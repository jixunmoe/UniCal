#include "VectorBackend.h"

#include <algorithm>

VectorBackend::VectorBackend()
{
}


VectorBackend::~VectorBackend()
{
  data.clear();
}

size_t VectorBackend::get_size()
{
  return data.size();
}

void VectorBackend::read(void * buffer, size_t size)
{
  if (data.size() < size)
  {
    return;
  }

  size_t copy_size = std::min(size, data.size() - size);
  if (copy_size <= 0)
  {
    return;
  }

  std::memcpy(buffer, data.data() + pos, copy_size);

  pos += copy_size;
}

void VectorBackend::seek(size_t position)
{
  pos = std::min(position, data.size() - 1);
}

bool VectorBackend::is_eof()
{
  return pos >= data.size();
}

uint8_t VectorBackend::peek()
{
  return data.at(pos);
}

void VectorBackend::get_line(std::string & line_read)
{
  uint8_t* start = data.data() + pos;
  uint8_t* end = &data.back();

  uint8_t* found_at = (uint8_t*)memchr(start, '\n', end - start);
  if (found_at != nullptr)
  {
    line_read.assign(start, found_at);
    // +1 for '\n'
    pos = found_at - data.data() + 1;
  }
}

void VectorBackend::assign(std::vector<uint8_t> &data)
{
  this->data = data;
  seek(0);
}
