// kitschensync.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "directory_description.h"
#include "file_system.h"
#include "utilities.h"

directory_description::~directory_description()
{
    utilities::clean(m_subdirectories);
    utilities::clean(m_files);

#ifdef _DEBUG
    m_parent = nullptr;
#endif
}

void directory_description::copy_recursive(const std::string& target_path) const
{
    typedef std::pair<std::string, const directory_description*> lookup;

    std::deque<lookup> pairs;
    pairs.push_back({ target_path, this });
    
    while (!pairs.empty())
    {
        lookup that(*pairs.begin());
        pairs.pop_front();
        assert(that.second != nullptr);

        std::vector<std::string> components = { that.first, that.second->m_name.c_str() };
        std::string new_directory_name(file_system::path::join(components));

        file_system::create_directory(new_directory_name);
        components[0] = new_directory_name;
        for (auto var : that.second->m_files)
        {
            components[1] = var.second->get_name();
            file_system::copy_file(
                var.second->get_path(),
                file_system::path::join(components));;
        }
        for (auto var : that.second->m_subdirectories)
        {
            pairs.push_back({ new_directory_name, var.second });
        }
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
