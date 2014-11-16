// kitschensync.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "directory_comparer.h"
#include "directory_description.h"

file_mismatch_reason directory_comparer::compare_files(const file_description* a, const file_description* b)
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




directory_mismatch* directory_comparer::compare_directories(const directory_description* a, const directory_description* b)
{
    DWORD t0 = GetTickCount();
    directory_mismatch* result = compare_directories_recursive(a, b);

    printf("Comparison after %d ms:\r\n\r\n", GetTickCount() - t0);
    printf("%10s | %10s | %20s | %s\r\n",
        "Dirs", "Files", "Bytes", "");
    printf("---------------------------------------------------------------------------------------------------------------\r\n");
    printf("%10I64d | %10I64d | %20I64d | %s\r\n",
        m_stats_a.m_total_directories,
        m_stats_a.m_total_files,
        m_stats_a.m_total_bytes,
        a->get_path());

    printf("%10I64d | %10I64d | %20I64d | %s\r\n",
        m_stats_b.m_total_directories,
        m_stats_b.m_total_files,
        m_stats_b.m_total_bytes,
        b->get_path());

    printf("%10I64d | %10I64d | %20I64d | mismatch\r\n\r\n",
        m_stats_mismatch.m_total_directories,
        m_stats_mismatch.m_total_files,
        m_stats_mismatch.m_total_bytes,
        b->get_path());
    
    return result;
}

directory_mismatch* directory_comparer::compare_directories_recursive(const directory_description* a, const directory_description* b)
{
    directory_mismatch* result = nullptr; // (a, b);

    m_stats_a.m_total_files += a->m_files.size();
    m_stats_b.m_total_files += b->m_files.size();
    ++m_stats_a.m_total_directories;
    ++m_stats_b.m_total_directories;

    for (auto var_a : a->m_files)
    {
        m_stats_a.m_total_bytes += var_a.second->get_file_size();

        auto var_b = b->m_files.find(var_a.first);
        if (var_b == b->m_files.end())
        {
            ++m_stats_mismatch.m_total_files;
            m_stats_mismatch.m_total_bytes += var_a.second->get_file_size();

            if (result == nullptr)
                result = new directory_mismatch(a, b);
            result->m_files_missing[1].push_back(var_a.second);
        }
        else
        {
            m_stats_b.m_total_bytes += var_b->second->get_file_size();

            file_mismatch_reason reason = compare_files(var_a.second, var_b->second);
            if (reason != file_mismatch_reason::none)
            {
                ++m_stats_mismatch.m_total_files;
                m_stats_mismatch.m_total_bytes += llabs(
                    var_a.second->get_file_size() -
                    var_b->second->get_file_size());

                if (result == nullptr)
                    result = new directory_mismatch(a, b);
                result->m_file_mismatches.push_back({ reason, var_a.second, var_b->second });
            }
        }
    }
    for (auto var_b : b->m_files)
    {
        if (a->m_files.find(var_b.first) == a->m_files.end())
        {
            m_stats_b.m_total_bytes += var_b.second->get_file_size();
            ++m_stats_mismatch.m_total_files;
            m_stats_mismatch.m_total_bytes += var_b.second->get_file_size();
            if (result == nullptr)
                result = new directory_mismatch(a, b);
            result->m_files_missing[0].push_back(var_b.second);
        }
    }

    for (auto var_a : a->m_subdirectories)
    {
        auto var_b = b->m_subdirectories.find(var_a.first);
        if (var_b == b->m_subdirectories.end())
        {
            directory_stats stats(var_a.second->get_stats());
            m_stats_a += stats;
            m_stats_mismatch += stats;

            if (result == nullptr)
                result = new directory_mismatch(a, b);
            result->m_directories_missing[1].push_back(var_a.second);
        }
        else
        {
            directory_mismatch* reason = compare_directories_recursive(var_a.second, var_b->second);
            if (reason)
            {
                // ok, there is a mismatch here; report it
                if (result == nullptr)
                    result = new directory_mismatch(a, b);
                result->m_directory_mismatches.push_back(reason);
            }
        }
    }
    for (auto var_b : b->m_subdirectories)
    {
        if (a->m_subdirectories.find(var_b.first) == a->m_subdirectories.end())
        {
            directory_stats stats(var_b.second->get_stats());
            m_stats_b += stats;
            m_stats_mismatch += stats;

            ++m_stats_b.m_total_directories;
            if (result == nullptr)
                result = new directory_mismatch(a, b);
            result->m_directories_missing[0].push_back(var_b.second);
        }
    }

    return result;
}
