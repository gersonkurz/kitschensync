// kitschensync.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "directory_description.h"


directory_description::~directory_description()
{
}

directory_description* directory_description::push(const char* path, WIN32_FIND_DATA& wfd)
{
#ifndef FILE_SYSTEM_IS_CASE_SENSITIVE
    // need lowercase filename 
    CharLowerBuff(
        wfd.cFileName,
        lstrlen(wfd.cFileName));
#endif
    const char* name = strrchr(path, '\\') + 1;
    assert(name != nullptr);
    
    if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        directory_description* dl = new directory_description(name, this);

        // the key is lowercase, the actual entry is not
        m_subdirectories.insert({ wfd.cFileName, dl });
        return dl;
    }
    else
    {
        LARGE_INTEGER li;
        li.HighPart = wfd.nFileSizeHigh;
        li.LowPart = wfd.nFileSizeLow;

        file_description* fd = new file_description(name, li.QuadPart, this);

        m_files.insert({ name, fd });
        return nullptr;
    }
}


// created the first time it is used, afterwards reused
const char* directory_description::path() const
{
    std::deque<std::string> sections;

    const directory_description* dl = this;
    while (dl)
    {
        sections.push_front(dl->m_name);
        dl = dl->m_parent;
    }

    std::vector<char> buffer;
    bool first = true;
    for (auto var : sections)
    {
        if (first)
            first = false;
        else
            buffer.push_back('\\');

        for (const char* c = var.c_str(); *c; ++c)
        {
            buffer.push_back(*c);
        }
    }
    buffer.push_back(0);

    m_path = &buffer[0];
    return m_path.c_str();

}
