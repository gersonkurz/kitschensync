#ifndef kitschensync_file_system_h
#define kitschensync_file_system_h

namespace file_system
{
    namespace path
    {
        const char separator = '\\';

        template <typename T> std::string join(const T& components)
        {
            std::vector<char> buffer;
            bool first = true;
            for (auto var : components)
            {
                if (first)
                    first = false;
                else
                    buffer.push_back(separator);

                for (const char* c = var.c_str(); *c; ++c)
                {
                    buffer.push_back(*c);
                }
            }
            buffer.push_back(0);
            return &buffer[0];
        }
    };
    
    std::string get_current_directory();

    bool copy_file(const std::string& source, const std::string& target);
    bool delete_file(const std::string& source);
    bool delete_empty_directory(const std::string& source);
    bool create_directory(const std::string& path);
    bool does_directory_exist(const std::string& path);
    bool does_file_exist(const std::string& path);
    

}

#endif // kitschensync_file_system_h
