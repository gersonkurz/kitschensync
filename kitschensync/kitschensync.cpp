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
        printf("OK too: found mismatches!\r\n");
        diffs->dump();
        delete diffs;
    }
    else
    {
        printf("Great: directories are identical!\r\n");
    }
   
    getchar();
    return 0;
}

