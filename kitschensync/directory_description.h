#ifndef kitschensync_directory_listing_h
#define kitschensync_directory_listing_h

#include "file_description.h"

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

    // created the first time it is used, afterwards reused
    const char* path() const;

    long long total_files() const
    {
        long long result = m_files.size();
        
        for (auto var : m_subdirectories)
        {
            result += var.second->total_files();
        }
        return result;
    }

    long long total_bytes() const
    {
        long long result = 0;

        for (auto var : m_files)
        {
            result += var.second->get_file_size();
        }
        for (auto var : m_subdirectories)
        {
            result += var.second->total_bytes();
        }
        return result;
    }

    long long total_subdirectories() const
    {
        long long result = m_subdirectories.size();
        for (auto var : m_subdirectories)
        {
            result += var.second->total_subdirectories();
        }
        return result;
    }

    bool does_file_exist(const std::string& name) const
    {
        return m_files.find(name) != m_files.end();
    }

    bool is_root() const
    {
        return m_parent != nullptr;
    }

private:
    std::unordered_map<std::string, directory_description*> m_subdirectories;
    std::unordered_map<std::string, file_description*> m_files;
    std::string m_name;
    mutable std::string m_path;
    directory_description* m_parent;
};

#endif // kitschensync_directory_listing_h
