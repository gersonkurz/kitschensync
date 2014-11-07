// kitschensync.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "directory_comparer.h"
#include "directory_listing.h"

void directory_comparer::find_missing_files(const directory_listing& a, const directory_listing& b, std::vector<const WIN32_FIND_DATA*>& missing) const
{
    for (auto var : a.get_const_file_indizes())
    {
        if (!b.does_file_exist(var.first))
        {
            missing.push_back(a.get_const_file(var.second));
        }
    }
}

directory_differences directory_comparer::compare(const directory_listing& a, const directory_listing& b) const
{
    directory_differences result(a, b);

    find_missing_files(a, b, result.m_files_missing_in_a);
    find_missing_files(b, a, result.m_files_missing_in_b);

    /*
    std::vector<int> m_directories_missing_in_a;    // the 'int' is an index into m_directory_b->m_subdirectories
    std::vector<int> m_directories_missing_in_b;    // the 'int' is an index into m_directory_b->m_subdirectories

    // subdirectories that exist in both a and b, but that are - somewhere - different
    std::vector<directory_differences*> m_directory_differences;

        // 
        // std::vector<file_differences*> m_file_differen
        */
    return result;
}
