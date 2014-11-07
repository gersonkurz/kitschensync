// kitschensync.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "directory_comparer.h"
#include "directory_description.h"

file_mismatch_reason directory_comparer::compare_files(const file_description* a, const file_description* b) const
{
    if (a->get_timestamp() < b->get_timestamp())
    {
        return file_mismatch_reason::a_is_older_than_b;
    }
    if (a->get_timestamp() > b->get_timestamp())
    {
        return file_mismatch_reason::a_is_newer_than_b;
    }
    if (a->get_file_size() != b->get_file_size())
    {
        return file_mismatch_reason::file_size;
    }
    return file_mismatch_reason::none;
}

directory_differences* directory_comparer::compare_directories(const directory_description* a, const directory_description* b) const
{
    directory_differences* result = nullptr; // (a, b);

    for (auto var_a : a->m_files)
    {
        auto var_b = b->m_files.find(var_a.first);
        if (var_b == b->m_files.end())
        {
            if (result == nullptr)
                result = new directory_differences(a, b);
            result->m_files_missing_in_b.push_back(var_a.second);
        }
        else
        {
            file_mismatch_reason reason = compare_files(var_a.second, var_b->second);
            if (reason != file_mismatch_reason::none)
            {
                if (result == nullptr)
                    result = new directory_differences(a, b);
                result->m_file_mismatches.push_back({ reason, var_a.second, var_b->second });
            }
        }
    }
    for (auto var_b : b->m_files)
    {
        if (a->m_files.find(var_b.first) == a->m_files.end())
        {
            if (result == nullptr)
                result = new directory_differences(a, b);
            result->m_files_missing_in_a.push_back(var_b.second);
        }
    }

    for (auto var_a : a->m_subdirectories)
    {
        auto var_b = b->m_subdirectories.find(var_a.first);
        if (var_b == b->m_subdirectories.end())
        {
            if (result == nullptr)
                result = new directory_differences(a, b);
            result->m_directories_missing_in_b.push_back(var_a.second);
        }
        else
        {
            directory_differences* reason = compare_directories(var_a.second, var_b->second);
            if (reason)
            {
                // ok, there is a mismatch here; report it
                if (result == nullptr)
                    result = new directory_differences(a, b);
                result->m_directory_mismatches.push_back(reason);
            }
        }
    }
    for (auto var_b : b->m_subdirectories)
    {
        if (a->m_subdirectories.find(var_b.first) == a->m_subdirectories.end())
        {
            if (result == nullptr)
                result = new directory_differences(a, b);
            result->m_directories_missing_in_a.push_back(var_b.second);
        }
    }

    return result;
}
