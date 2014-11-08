#ifndef kitschensync_utilities_h
#define kitschensync_utilities_h

namespace utilities
{
    template <typename KEY, typename VALUE> void clean_map(std::unordered_map<KEY, VALUE>& items)
    {
        for (auto var : items)
        {
            delete var.second;
        }
        items.clear();
    }
}

#endif // kitschensync_utilities_h
