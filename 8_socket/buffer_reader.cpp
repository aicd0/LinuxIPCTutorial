#include "buffer_reader.h"
#include <cstring>

void BufferReader::read(void* buf, ssize_t size) {
  int buf_offset = 0;

  while (buf_offset < size) {
    char* current_buf = reinterpret_cast<char*>(buf) + buf_offset;
    unsigned long current_size = static_cast<unsigned long>(size) - buf_offset;

    if (m_offset < sizeof(m_header)) {
      // Header section.
      int cpy_size = std::min(current_size, sizeof(m_header) - m_offset);
      memcpy(reinterpret_cast<char*>(&m_header) + m_offset, current_buf, cpy_size);

      buf_offset += cpy_size;
      m_offset += cpy_size;
    }
    else {
      // Data section.
      unsigned long data_offset = m_offset - sizeof(m_header);
      int cpy_size = std::min(current_size, m_header.size - data_offset);
      m_buf.resize(m_buf.size() + cpy_size);
      memcpy(m_buf.data() + data_offset, current_buf, cpy_size);

      data_offset += cpy_size;
      buf_offset += cpy_size;
      m_offset += cpy_size;

      if (data_offset >= m_header.size) {
        // Emplace data.
        m_queue.emplace(move(m_buf));
        m_offset = 0;
      }
    }
  }
}
