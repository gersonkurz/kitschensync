#ifndef kitschensync_directory_comparer_h
#define kitschensync_directory_comparer_h

class directory_description;

#include "directory_differences.h"



class directory_comparer
{
public:
    directory_differences compare(const directory_description& a, const directory_description& b) const;


private:
    file_mismatch_reason compare_files(const file_description* a, const file_description* b) const;
};

#endif // kitschensync_directory_comparer_h
