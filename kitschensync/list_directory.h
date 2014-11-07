#ifndef kitschensync_list_directory_h
#define kitschensync_list_directory_h

#include "directory_description.h"

class list_directory
{
public:
    directory_description read(LPCTSTR directory);

private:
    void ensure_path_buffer_size_is_at_least(size_t size_in_bytes)
    {
        const size_t existing_size = m_path_buffer.size();
        size_t new_size = existing_size;
        if (new_size == 0)
            new_size = 1024;
        while (new_size < size_in_bytes)
        {
            new_size *= 2;
        }
        if (new_size != existing_size)
        {
            m_path_buffer.resize(new_size);
        }
    }

private:
    std::vector<TCHAR> m_path_buffer;
};

#endif // kitschensync_list_directory_h
