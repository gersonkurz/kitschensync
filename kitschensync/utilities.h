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
}

#endif // kitschensync_utilities_h
