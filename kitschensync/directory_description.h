#ifndef kitschensync_directory_listing_h
#define kitschensync_directory_listing_h

#include "file_description.h"

struct directory_stats
{
    long long m_total_files;
    long long m_total_bytes;
    long long m_total_directories;

    directory_stats()
        :
        m_total_files(0),
        m_total_bytes(0),
        m_total_directories(0)
    {
    }

    void operator +=(const directory_stats& input)
    {
        m_total_bytes += input.m_total_bytes;
        m_total_files += input.m_total_files;
        m_total_directories += input.m_total_directories;
    }
};

class directory_description
{
public:
    directory_description(const char* name, directory_description* parent = nullptr)
        :
        m_name(name),
        m_parent(parent)
    {

    }
    virtual ~directory_description();

private:
    directory_description(const directory_description& objectSrc);
    directory_description& operator=(const directory_description& objectSrc);

public:
    directory_description(directory_description&& source) 
        : 
            m_subdirectories(source.m_subdirectories),
            m_files(source.m_files),
            m_name(source.m_name),
            m_parent(source.m_parent)
    {
    }

    directory_description* push(const char* path, WIN32_FIND_DATA& wfd);

    void copy_recursive(const std::string& target_path) const;
    void delete_recursive() const;

    const char* get_path() const;

    std::string get_in_path(const char* part) const;

    directory_stats get_stats() const
    {
        directory_stats result;
        for (auto var : m_files)
        {
            result.m_total_bytes += var.second->get_file_size();
        }
        result.m_total_files = m_files.size();
        result.m_total_directories += m_subdirectories.size();

        for (auto var : m_subdirectories)
        {
            result += var.second->get_stats();
        }
        return result;
    }

    bool does_file_exist(const std::string& name) const
    {
        return m_files.find(name) != m_files.end();
    }

    bool does_subdirectory_exist(const std::string& name) const
    {
        return m_subdirectories.find(name) != m_subdirectories.end();
    }

    bool is_root() const
    {
        return m_parent != nullptr;
    }

private:
    friend class directory_comparer;

    std::unordered_map<std::string, directory_description*> m_subdirectories;
    std::unordered_map<std::string, file_description*> m_files;
    std::string m_name;
    mutable std::string m_path;
    directory_description* m_parent;
};

#endif // kitschensync_directory_listing_h
