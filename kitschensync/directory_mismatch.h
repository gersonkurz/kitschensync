#ifndef kitschensync_directory_differences_h
#define kitschensync_directory_differences_h

#include "directory_description.h"
#include "file_system.h"

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

enum class directory_sync_mode
{
    only_copy_missing_objects,
    copy_missing_objects_and_delete_obsolete_ones
};

std::string as_string(relationship_order ro);


class directory_mismatch // recursive 
{
public:
    directory_mismatch(const directory_description* a, const directory_description* b)
    {
        m_d[0] = a;
        m_d[1] = b;
    }
    virtual ~directory_mismatch();

    void dump() const;

    relationship_order determine_relationship(const directory_description** pa, const directory_description** pb) const;
    relationship_order determine_relationship_order() const;

    void apply_changes(relationship_order ro, directory_sync_mode dsm) const;

private:
    directory_mismatch(const directory_mismatch& objectSrc);
    directory_mismatch& operator=(const directory_mismatch& objectSrc);

private:

    static void copy_missing_objects(
        const directory_description* target,
        const std::vector<const file_description*>& files_missing,
        const std::vector<const directory_description*>& directories_missing) 
    {
        for (auto var : files_missing)
        {
            file_system::copy_file(
                var->get_path(),
                target->get_in_path(var->get_name()).c_str());
        }

        for (auto var : directories_missing)
        {
            var->copy_recursive(target->get_path());
        }
    }

    static void delete_obsolete_objects(
        const std::vector<const file_description*>& obsolete_files,
        const std::vector<const directory_description*>& obsolete_directories) 
    {
        for (auto var : obsolete_files)
        {
            file_system::delete_file(var->get_path());
        }

        for (auto var : obsolete_directories)
        {
            var->delete_recursive();
        }
    }

private:
    friend class directory_comparer;

    const directory_description* m_d[2];
    std::vector<const file_description*> m_files_missing[2];
    std::vector<const directory_description*> m_directories_missing[2];

    std::vector<file_mismatch> m_file_mismatches;
    std::vector<const directory_mismatch*> m_directory_mismatches;
};

#endif // kitschensync_directory_differences_h
