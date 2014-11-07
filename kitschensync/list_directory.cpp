// kitschensync.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "list_directory.h"

directory_description list_directory::read(LPCTSTR directory)
{

    DWORD t0 = GetTickCount();
    long nItems = 0;
    const char* original_name = directory;

    

    directory_description _result(directory);

    std::deque<directory_description*> directories;
    directories.push_back(&_result);

    while (!directories.empty())
    {
        directory_description* current(*directories.begin());
        directories.pop_front();
        
        directory = current->path();

        assert(directory != nullptr);
        assert(*directory);

        size_t path_length = lstrlen(directory);
        assert(path_length > 0);

        ensure_path_buffer_size_is_at_least(path_length + MAX_PATH + 2); // backslash + nullbyte
        lstrcpy(&m_path_buffer[0], directory);

        // input should have a trailing backslash
        if (m_path_buffer[path_length - 1] != '\\')
        {
            m_path_buffer[path_length++] = '\\';
            m_path_buffer[path_length] = 0;
        }

        WIN32_FIND_DATA wfd;
        ZeroMemory(&wfd, sizeof(wfd));

        // preliminary, will be overwritten immediately:
        lstrcpy(&m_path_buffer[path_length], "*");

        HANDLE hFF = FindFirstFile(&m_path_buffer[0], &wfd);

        if (hFF != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (wfd.cFileName[0] == '.')
                {
                    if (wfd.cFileName[1] == '.')
                    {
                        if (wfd.cFileName[2] == 0)
                        {
                            continue;
                        }
                    }
                    else if (wfd.cFileName[1] == 0)
                    {
                        continue;
                    }
                }

                ++nItems;
                lstrcpy(&m_path_buffer[path_length], wfd.cFileName);
                directory_description* dl = current->push(&m_path_buffer[0], wfd);
                if (dl)
                {
                    directories.push_back(dl);
                }
            } 
            while (FindNextFile(hFF, &wfd));

            FindClose(hFF);
        }
    }

    printf("Time to read %ld items in '%s': %ld ms.\r\n", nItems, original_name, GetTickCount() - t0);
    return _result;
}
