// kitschensync.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "directory_comparer.h"
#include "directory_description.h"

file_mismatch_reason directory_comparer::compare_files(const file_description* a, const file_description* b) const
{
    if (a->get_file_size() != b->get_file_size())
    {
        return file_mismatch_reason::file_size;
    }
    if (a->get_timestamp() < b->get_timestamp())
    {
        return file_mismatch_reason::a_is_older_than_b;
    }
    if (a->get_timestamp() > b->get_timestamp())
    {
        return file_mismatch_reason::a_is_newer_than_b;
    }
    return file_mismatch_reason::none;
}

directory_differences directory_comparer::compare(const directory_description& a, const directory_description& b) const
{
    directory_differences result(a, b);

    for (auto var_a : a.m_files)
    {
        auto var_b = b.m_files.find(var_a.first);
        if(var_b == b.m_files.end())
        {
            result.m_files_missing_in_b.push_back(var_a.second);
        }
        else
        {
            file_mismatch_reason reason = compare_files(var_a.second, var_b->second);
            if (reason != file_mismatch_reason::none)
            {
                // ok, there is a mismatch here; report it
                result.m_file_mismatches.push_back({ reason, var_a.second, var_b->second });
            }
        }
    }

    a.find_missing_directories(b, result.m_directories_missing_in_b);
    b.find_missing_directories(a, result.m_directories_missing_in_a);

    return result;
}
