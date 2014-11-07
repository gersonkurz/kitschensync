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

enum class relationship_order
{
    undefined,
    a_newer_than_b,
    b_newer_than_a
};



class directory_mismatch // recursive 
{
public:
    directory_mismatch(const directory_description* a, const directory_description* b)
        : 
        m_a(a),
        m_b(b)
    {
    }

    void dump() const;

    bool determine_relationship(const directory_description** pa, const directory_description** pb) const;
    relationship_order determine_relationship_order() const;

private:
    directory_mismatch(const directory_mismatch& objectSrc);
    directory_mismatch& operator=(const directory_mismatch& objectSrc);

public:
    const directory_description* m_a;
    const directory_description* m_b;

    std::vector<const file_description*> m_files_missing_in_a;
    std::vector<const file_description*> m_files_missing_in_b;
    std::vector<file_mismatch> m_file_mismatches;

    std::vector<const directory_description*> m_directories_missing_in_a;
    std::vector<const directory_description*> m_directories_missing_in_b;

    // subdirectories that exist in both a and b, but that are - somewhere - different
    // now this is where we're going to: differences in directories
    std::vector<const directory_mismatch*> m_directory_mismatches;
};

#endif // kitschensync_directory_differences_h
