#ifndef kitschensync_file_description_h
#define kitschensync_file_description_h

class directory_description;

class file_description
{
public:
    file_description(const char* name, long long size, directory_description* parent)
        : 
        m_parent(parent),
        m_size(size),
        m_name(name)
    {
    }

    ~file_description()
    {
    }

    long long get_file_size() const
    {
        return m_size;
    }

private:
    directory_description* m_parent;
    long long m_size;
    std::string m_name;
};

#endif // kitschensync_file_description_h
