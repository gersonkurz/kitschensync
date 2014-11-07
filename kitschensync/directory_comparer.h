#ifndef kitschensync_directory_comparer_h
#define kitschensync_directory_comparer_h

class directory_listing;

#include "directory_differences.h"

class directory_comparer
{
public:
    directory_differences compare(const directory_listing& a, const directory_listing& b) const;

private:
    void find_missing_files(const directory_listing& a, const directory_listing& b, std::vector<const WIN32_FIND_DATA*>& missing) const;
};

#endif // kitschensync_directory_comparer_h
