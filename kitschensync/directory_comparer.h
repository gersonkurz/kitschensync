#ifndef kitschensync_directory_comparer_h
#define kitschensync_directory_comparer_h

class directory_description;

#include "directory_differences.h"

class directory_comparer
{
public:
    directory_differences compare(const directory_description& a, const directory_description& b) const;

private:
    void find_missing_files(const directory_description& a, const directory_description& b, std::vector<const WIN32_FIND_DATA*>& missing) const;
};

#endif // kitschensync_directory_comparer_h
