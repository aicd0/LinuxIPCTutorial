#pragma once

enum class MessageType : long {
    Header = 1,
    Content = 2,
};

struct msg_header_t
{
    const MessageType msgtype = MessageType::Header;
    size_t size;

    inline static size_t msg_size() { return sizeof(size); }
};

struct msg_content_t
{
    const MessageType msgtype = MessageType::Content;
};
