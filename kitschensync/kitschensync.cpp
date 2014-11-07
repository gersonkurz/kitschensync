#include "stdafx.h"
#include "directory_description.h"
#include "directory_comparer.h"
#include "list_directory.h"
#include "directory_differences.h"

int _tmain(int, _TCHAR* [])
{
    list_directory lister;
    directory_description* a(lister.read("C:\\Projects\\kitschensync"));
    directory_description* b(lister.read("T:\\kitschensync"));
    
    directory_comparer dc;
    directory_differences* diffs = dc.compare_directories(a, b);
    if (diffs)
    {
        const directory_description* newer;
        const directory_description* older;

        if (diffs->determine_relationship(&newer, &older))
        {
            printf("%s is newer than %s\r\n",
                newer->get_path(),
                older->get_path());
        }
        else
        {
            printf("*** WARNING: was unable to determine relationship between these two directories...\r\n");
            diffs->dump();
        }
        delete diffs;
    }
    else
    {
        printf("Great: directories are identical!\r\n");
    }
   
    getchar();
    return 0;
}

