#pragma once

#include <queue>
#include <vector>
#include "shared.h"

class BufferReader {
    std::queue<std::vector<char>> m_queue;
    unsigned long m_offset = 0;
    MessageHeader m_header;
    std::vector<char> m_buf;

public:
    void read(void* buf, ssize_t size);

    inline bool empty() const {
        return m_queue.empty();
    }

    inline const std::vector<char>& front() const {
        return m_queue.front();
    }

    inline void pop() {
        m_queue.pop();
    }
};
