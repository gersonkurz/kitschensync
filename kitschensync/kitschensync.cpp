#include "stdafx.h"
#include "directory_description.h"
#include "directory_comparer.h"
#include "list_directory.h"
#include "directory_mismatch.h"

void test()
{
    list_directory lister;
    directory_description* a(lister.read("C:\\Projects\\kitschensync - Kopie"));
    directory_description* b(lister.read("T:\\kitschensync"));



    directory_comparer dc;
    directory_mismatch* diffs = dc.compare_directories(a, b);
    if (diffs)
    {
        const directory_description* newer;
        const directory_description* older;

        diffs->dump();

        relationship_order ro = diffs->determine_relationship(&newer, &older);
        if (ro != relationship_order::undefined)
        {
            printf("--------------------------------------------\r\n");
            printf("%s is newer than %s\r\n",
                newer->get_path(),
                older->get_path());
            printf("--------------------------------------------\r\n");
            DWORD t0 = GetTickCount();
            diffs->apply_changes(ro, directory_sync_mode::copy_missing_objects_and_delete_obsolete_ones);
            printf("Time to sync directories: %ld ms.\r\n", GetTickCount() - t0);
        }
        else
        {
            printf("*** WARNING: was unable to determine relationship between these two directories...\r\n");
            diffs->dump();
        }
        delete diffs;
        diffs = nullptr;
    }
    else
    {
        printf("Great: directories are identical!\r\n");
    }
    delete a;
    delete b;
}

int _tmain(int, _TCHAR* [])
{
    test();
#ifdef _DEBUG
    _CrtDumpMemoryLeaks();
#endif
    getchar();
    return 0;
}

