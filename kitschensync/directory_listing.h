#ifndef kitschensync_directory_listing_h
#define kitschensync_directory_listing_h

class directory_listing
{
public:
    directory_listing(const char* path, directory_listing* parent = nullptr)
        :
        m_path(path),
        m_parent(parent)
    {

    }
    virtual ~directory_listing();

private:
    directory_listing(const directory_listing& objectSrc);
    directory_listing& operator=(const directory_listing& objectSrc);

public:
    directory_listing(directory_listing&& source) 
        : 
            m_subdirectories(source.m_subdirectories),
            m_files(source.m_files),
            m_entries(source.m_entries), 
            m_subdirectory_listings(source.m_subdirectory_listings),
            m_path(source.m_path),
            m_parent(source.m_parent)
    {
    }

    directory_listing* push(const char* path, const WIN32_FIND_DATA& wfd);

    const char* path() const
    {
        return m_path.c_str();
    } 

    size_t total_files() const
    {
        size_t result = m_files.size();
        
        for (auto var : m_subdirectory_listings)
        {
            result += var->total_files();
        }
        return result;
    }

    size_t total_bytes() const
    {
        size_t result = 0;

        for (auto var : m_files)
        {
            LARGE_INTEGER li;
            const WIN32_FIND_DATA* wfd = &(m_entries[var.second]);
            li.HighPart = wfd->nFileSizeHigh;
            li.LowPart = wfd->nFileSizeLow;

            result += li.QuadPart;
        }
        for (auto var : m_subdirectory_listings)
        {
            result += var->total_bytes();
        }
        return result;
    }

    size_t total_subdirectories() const
    {
        size_t result = m_subdirectories.size();
        for (auto var : m_subdirectory_listings)
        {
            result += var->total_subdirectories();
        }
        return result;
    }

private:
    // subdirectories (not sorted)
    std::unordered_map<std::string, size_t> m_subdirectories;
    std::unordered_map<std::string, size_t> m_files;
    std::vector<WIN32_FIND_DATA> m_entries;
    std::vector<directory_listing*> m_subdirectory_listings;
    std::string m_path;
    directory_listing* m_parent;
};

#endif // kitschensync_directory_listing_h
