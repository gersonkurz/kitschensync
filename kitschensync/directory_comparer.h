#ifndef kitschensync_directory_comparer_h
#define kitschensync_directory_comparer_h

class directory_listing;

#include "directory_differences.h"

class directory_comparer
{
public:
    directory_differences compare(const directory_listing& a, const directory_listing& b) const;

};

#endif // kitschensync_directory_comparer_h
