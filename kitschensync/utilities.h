#ifndef kitschensync_utilities_h
#define kitschensync_utilities_h

namespace utilities
{
    template <typename KEY, typename VALUE> void clean(std::unordered_map<KEY, VALUE*>& items)
    {
        for (auto var : items)
        {
            delete var.second;
        }
        items.clear();
    }

    template <typename ITEM> void clean(std::vector<ITEM*>& items)
    {
        for (auto var : items)
        {
            delete var;
        }
        items.clear();
    }
}


namespace string
{
    std::string vformat(const char* fmt, va_list args);
    inline std::string format(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        return vformat(fmt, args);
    }
    std::string lowercase(const std::string& input);
    std::string UPPERCASE(const std::string& input);

    inline bool equals(const char* a, const char* b)
    {
        return strcmp(a, b) == 0;
    }

    inline bool equals(const std::string& a, const char* b)
    {
        return strcmp(a.c_str(), b) == 0;
    }

    inline bool equals(const char* a, const std::string& b)
    {
        return strcmp(a, b.c_str()) == 0;
    }

    inline bool equals(const std::string& a, const std::string& b)
    {
        return strcmp(a.c_str(), b.c_str()) == 0;
    }


    inline bool equalsI(const char* a, const char* b)
    {
        return _strcmpi(a, b) == 0;
    }

    inline bool equalsI(const std::string& a, const char* b)
    {
        return _strcmpi(a.c_str(), b) == 0;
    }

    inline bool equalsI(const char* a, const std::string& b)
    {
        return _strcmpi(a, b.c_str()) == 0;
    }

    inline bool equalsI(const std::string& a, const std::string& b)
    {
        return _strcmpi(a.c_str(), b.c_str()) == 0;
    }

    inline bool is_empty(const char* name)
    {
        return !name || !*name;
    }

    inline bool is_empty(const std::string& name)
    {
        return name.empty();
    }

    inline bool endswith(const char* a, const char* b)
    {
        size_t n_a = strlen(a);
        size_t n_b = strlen(b);
        if (n_a < n_b)
            return false;

        a += n_a - n_b;
        return equals(a, b);
    }
}

#endif // kitschensync_utilities_h
