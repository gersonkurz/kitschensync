#ifndef kitschensync_directory_comparer_h
#define kitschensync_directory_comparer_h

class directory_description;

#include "directory_mismatch.h"

class directory_comparer
{
public:
    directory_mismatch* compare_directories(const directory_description* a, const directory_description* b);

private:
    directory_mismatch* compare_directories_recursive(const directory_description* a, const directory_description* b);
    file_mismatch_reason compare_files(const file_description* a, const file_description* b);

private:
    directory_stats m_stats_a;
    directory_stats m_stats_b;
    directory_stats m_stats_mismatch;
};

#endif // kitschensync_directory_comparer_h
