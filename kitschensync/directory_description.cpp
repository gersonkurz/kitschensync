// kitschensync.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "directory_description.h"
#include "file_system.h"

directory_description::~directory_description()
{
}

void directory_description::copy_recursive(const std::string& target_path) const
{
    std::vector<std::string> components = { target_path, m_name.c_str() };
    std::string new_directory_name(file_system::path::join(components));

    file_system::create_directory(new_directory_name);
    components[0] = new_directory_name;
    for (auto var : m_files)
    {
        components[1] = var.second->get_name();
        file_system::copy_file(
            var.second->get_path(),
            file_system::path::join(components));;
    }
    for (auto var : m_subdirectories)
    {
        var.second->copy_recursive(new_directory_name);
    }
}

void directory_description::delete_recursive() const
{
    for (auto var : m_subdirectories)
    {
        var.second->delete_recursive();
    }
    for (auto var : m_files)
    {
        file_system::delete_file(var.second->get_path());
    }
    file_system::delete_empty_directory(get_path());
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

        LARGE_INTEGER wt;
        wt.HighPart = wfd.ftLastWriteTime.dwHighDateTime;
        wt.LowPart = wfd.ftLastWriteTime.dwLowDateTime;

        file_description* fd = new file_description(name, li.QuadPart, wt.QuadPart, this);

        m_files.insert({ name, fd });
        return nullptr;
    }
}

std::string directory_description::get_in_path(const char* part) const
{
    std::deque<std::string> components;

    if (part)
        components.push_front(part);

    const directory_description* dl = this;
    while (dl)
    {
        components.push_front(dl->m_name);
        dl = dl->m_parent;
    }
    return file_system::path::join(components);
}

// created the first time it is used, afterwards reused
const char* directory_description::get_path() const
{
    if (m_path.size() == 0)
    {
        m_path = get_in_path(nullptr);
    }
    return m_path.c_str();
}
