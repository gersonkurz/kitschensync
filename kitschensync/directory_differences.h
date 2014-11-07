#ifndef kitschensync_directory_differences_h
#define kitschensync_directory_differences_h

class directory_description;

struct file_reference
{
    directory_description* dl;
    int index;
};

class directory_differences // recursive 
{
public:
    directory_differences(const directory_description& a, const directory_description& b)
        : 
        m_directory_a(a),
        m_directory_b(b)
    {

    }

private:
    directory_differences(const directory_differences& objectSrc);
    directory_differences& operator=(const directory_differences& objectSrc);

public:
    directory_differences(directory_differences&& moveSrc)
        :
        m_directory_a(moveSrc.m_directory_a),
        m_directory_b(moveSrc.m_directory_b),
        m_files_missing_in_a(moveSrc.m_files_missing_in_a),
        m_files_missing_in_b(moveSrc.m_files_missing_in_b),
        m_directories_missing_in_a(moveSrc.m_directories_missing_in_a),
        m_directories_missing_in_b(moveSrc.m_directories_missing_in_b)
    {
    }


    const directory_description& m_directory_a;
    const directory_description& m_directory_b;

    // TODO: introduce a class file_listing that is a wrapper around WIN32_FIND_DATA
    std::vector<const WIN32_FIND_DATA*> m_files_missing_in_a;
    std::vector<const WIN32_FIND_DATA*> m_files_missing_in_b;

    std::vector<const directory_description*> m_directories_missing_in_a;
    std::vector<const directory_description*> m_directories_missing_in_b;

    // subdirectories that exist in both a and b, but that are - somewhere - different
    std::vector<directory_differences*> m_directory_differences;

    // 
    // std::vector<file_differences*> m_file_differences;
};

#endif // kitschensync_directory_differences_h
