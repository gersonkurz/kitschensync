// kitschensync.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "file_system.h"

namespace file_system
{
    bool copy_file(const std::string& source, const std::string& target)
    {
        if (!::CopyFile(source.c_str(), target.c_str(), false))
        {
            printf("ERROR: unable to copy %s => %s\r\n",
                source.c_str(),
                target.c_str());
            return false;
        }
        return true;
    }

    bool create_directory(const std::string& path)
    {
        // todo: make a better - i.e. more sensible - security attribute
        if (!::CreateDirectory(path.c_str(), nullptr))
        {
            printf("ERROR, unable to create %s\r\n",
                path.c_str());
            return false;
        }
        return true;
    }

    bool delete_file(const std::string& path)
    {
        if (!::DeleteFile(path.c_str()))
        {
            printf("ERROR, unable to delete file %s\r\n",
                path.c_str());
            return false;
        }
        return true;
    }

    bool delete_empty_directory(const std::string& path)
    {
        if (!::RemoveDirectory(path.c_str()))
        {
            printf("ERROR, unable to delete directory %s\r\n",
                path.c_str());
            return false;
        }
        return true;
    }
}
