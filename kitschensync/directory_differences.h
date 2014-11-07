#ifndef kitschensync_directory_differences_h
#define kitschensync_directory_differences_h

class directory_description;
class file_description;

enum class file_mismatch_reason
{
    none = 0,
    file_size,
    a_is_older_than_b,
    a_is_newer_than_b,
    file_data
};

typedef struct
{
    file_mismatch_reason reason;
    const file_description* a;
    const file_description* b;
} file_mismatch;

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
        m_file_mismatches(moveSrc.m_file_mismatches),
        m_directories_missing_in_a(moveSrc.m_directories_missing_in_a),
        m_directories_missing_in_b(moveSrc.m_directories_missing_in_b)
    {
    }

    const directory_description& m_directory_a;
    const directory_description& m_directory_b;

    std::vector<const file_description*> m_files_missing_in_a;
    std::vector<const file_description*> m_files_missing_in_b;
    std::vector<file_mismatch> m_file_mismatches;
    std::vector<const directory_description*> m_directories_missing_in_a;
    std::vector<const directory_description*> m_directories_missing_in_b;

    // subdirectories that exist in both a and b, but that are - somewhere - different
    std::vector<directory_differences*> m_directory_differences;
};

#endif // kitschensync_directory_differences_h
