#ifndef kitschensync_file_description_h
#define kitschensync_file_description_h

class directory_description;

class file_description
{
public:
    file_description(const char* name, long long size, long long timestamp, directory_description* parent)
        : 
        m_parent(parent),
        m_size(size),
        m_timestamp(timestamp),
        m_name(name)
    {
    }

    ~file_description()
    {
    }

private:
    file_description(const file_description& objectSrc);
    file_description& operator=(const file_description& objectSrc);

public:
    const char* get_path() const;

    const char* get_name() const
    {
        return m_name.c_str();
    }

    long long get_file_size() const
    {
        return m_size;
    }

    long long get_timestamp() const
    {
        return m_timestamp;
    }

private:
    directory_description* m_parent;
    long long m_size;
    long long m_timestamp;
    std::string m_name;
    mutable std::string m_path;
};

#endif // kitschensync_file_description_h
