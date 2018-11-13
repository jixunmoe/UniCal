#include "FileBackend.h"

#include <string>

FileBackend::FileBackend(const char* path)
{
  open(path);
}


FileBackend::~FileBackend()
{
  if (reader.is_open())
  {
    reader.close();
  }
}

size_t FileBackend::get_size()
{
  auto pos = reader.tellg();
  reader.seekg(0, std::ifstream::end);
  std::streamoff result = reader.tellg();
  reader.seekg(pos, std::ifstream::beg);
  return static_cast<size_t>(result);
}

void FileBackend::read(void* buffer, size_t size)
{
  reader.read(reinterpret_cast<char*>(buffer), size);
}

void FileBackend::seek(size_t position)
{
  reader.seekg(position);
}

bool FileBackend::is_eof()
{
  return reader.eof();
}

uint8_t FileBackend::peek()
{
  return reader.peek();
}

void FileBackend::get_line(std::string & line_read)
{
  std::getline(reader, line_read);
}

void FileBackend::open(const char * path)
{
  reader.open(path, std::ifstream::in | std::ifstream::binary);
}
