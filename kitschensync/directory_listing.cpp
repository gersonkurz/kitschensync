// kitschensync.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "directory_listing.h"


directory_listing::~directory_listing()
{
}

directory_listing* directory_listing::push(const char* path, const WIN32_FIND_DATA& wfd)
{
    size_t n = m_entries.size();
    m_entries.push_back(wfd);
    assert(m_entries.size() == n + 1);
    assert(lstrcmp(m_entries[n].cFileName, wfd.cFileName) == 0);

    WIN32_FIND_DATA* pwfd = &(m_entries[n]);

#ifndef FILE_SYSTEM_IS_CASE_SENSITIVE
    // need lowercase filename 
    CharLowerBuff(
        pwfd->cFileName,
        lstrlen(wfd.cFileName));
#endif
    std::string name(pwfd->cFileName);

    if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        m_subdirectories.insert({ name, n });
        directory_listing* dl = new directory_listing(path, this);
        m_subdirectory_listings.push_back(dl);
        return dl;
    }
    else
    {
        m_files.insert({ name, n });
        return nullptr;
    }
}
