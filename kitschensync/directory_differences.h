#ifndef kitschensync_directory_differences_h
#define kitschensync_directory_differences_h

class directory_listing;

struct file_reference
{
    directory_listing* dl;
    int index;
};

class directory_differences // recursive 
{
public:

    directory_listing* m_directory_a;
    directory_listing* m_directory_b;

    std::vector<int> m_files_missing_in_a;
    std::vector<int> m_files_missing_in_b;
    std::vector<int> m_directories_missing_in_a;    // the 'int' is an index into m_directory_b->m_subdirectories
    std::vector<int> m_directories_missing_in_b;    // the 'int' is an index into m_directory_b->m_subdirectories

    // subdirectories that exist in both a and b, but that are - somewhere - different
    std::vector<directory_differences*> m_directory_differences;

    // 
    // std::vector<file_differences*> m_file_differences;
};

#endif // kitschensync_directory_differences_h
