// kitschensync.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "utilities.h"

namespace string
{
    std::string lowercase(const std::string& input)
    {
        std::vector<char> buffer;
        buffer.resize(input.length() + 1);
        LPCSTR text = input.c_str();
        int pos = 0;
        do
        {
            buffer[pos] = (char) tolower(text[pos]);
        } while (buffer[pos++]);
        return &buffer[0];
    }

    std::string UPPERCASE(const std::string& input)
    {
        std::vector<char> buffer;
        buffer.resize(input.length() + 1);
        LPCSTR text = input.c_str();
        int pos = 0;
        do
        {
            buffer[pos] = (char) toupper(text[pos]);
        } while (buffer[pos++]);
        return &buffer[0];
    }


    std::string vformat(const char* fmt, va_list args)
    {
        // Allocate a buffer on the stack that's big enough for us almost
        // all the time.  Be prepared to allocate dynamically if it doesn't fit.
        size_t size = 1024;
        std::vector<char> dynamicbuf;
        dynamicbuf.resize(size);
        char *buf = &dynamicbuf[0];

        for (;;)
        {
            // Try to vsnprintf into our buffer.
            int needed = vsprintf_s(buf, size, fmt, args);

            // NB. C99 (which modern Linux and OS X follow) says vsnprintf
            // failure returns the length it would have needed.  But older
            // glibc and current Windows return -1 for failure, i.e., not
            // telling us how much was needed.

            if (needed <= (int)size && needed >= 0)
            {
                // It fit fine so we're done.
                return std::string(buf, (size_t)needed);
            }

            // vsnprintf reported that it wanted to write more characters
            // than we allotted.  So try again using a dynamic buffer.  This
            // doesn't happen very often if we chose our initial size well.
            size = (needed > 0) ? (needed + 1) : (size * 2);
            dynamicbuf.resize(size);
            buf = &dynamicbuf[0];
        }
    }
}
    
