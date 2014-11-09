#include "stdafx.h"
#include "directory_description.h"
#include "directory_comparer.h"
#include "list_directory.h"
#include "directory_mismatch.h"
#include "utilities.h"

// todo: make interface for this
bool affirmative_input(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    std::string formatted(utilities::string::vformat(fmt, args));
    puts(formatted.c_str());

    char input[200];
    if (fgets(input, sizeof(input), stdin))
    {
        return (input[0] != 'N') &&
               (input[0] != 'n');
    }
    return false;
}

bool synchronize_directories(const std::string& name_a, const std::string& name_b)
{
    relationship_order ro = relationship_order::undefined;

    if (!file_system::does_directory_exist(name_a))
    {
        if (!file_system::does_directory_exist(name_b))
        {
            printf("ERROR, neither '%s' nor '%s' exist, aborting\r\n",
                name_a.c_str(),
                name_b.c_str());
            return false;
        }

        if (affirmative_input("%s does not exist => do you want to clone %s (Y/n)?",
            name_a.c_str(),
            name_b.c_str()))
        {
            ro = relationship_order::b_newer_than_a;
            if (!file_system::create_directory(name_a.c_str()))
            {
                return false;
            }
        }
        else return false;
    }
    else if (!file_system::does_directory_exist(name_b))
    {
        if (affirmative_input("%s does not exist => do you want to clone %s (Y/n)?",
            name_a.c_str(),
            name_b.c_str()))
        {
            ro = relationship_order::a_newer_than_b;
            if (!file_system::create_directory(name_b.c_str()))
            {
                return false;
            }
        }
        else return false;
    }

    list_directory lister;
    directory_description* a(lister.read(name_a.c_str()));
    directory_description* b(lister.read(name_b.c_str()));

    directory_comparer dc;
    directory_mismatch* diffs = dc.compare_directories(a, b);
    if (diffs)
    {
        const directory_description* newer;
        const directory_description* older;
        bool previously_known = true;

        if (ro == relationship_order::undefined)
        {
            ro = diffs->determine_relationship(&newer, &older);
            previously_known = false;
        }
        if (ro == relationship_order::a_newer_than_b)
        {
            if (previously_known || affirmative_input("%s is newer than %s. Are you sure to proceed? [Y/n]",
                name_a.c_str(),
                name_b.c_str()))
            {
                diffs->apply_changes(ro, directory_sync_mode::copy_missing_objects_and_delete_obsolete_ones);
            }
        }
        else if (ro == relationship_order::b_newer_than_a)
        {
            if (previously_known || affirmative_input("%s is newer than %s. Are you sure to proceed? [Y/n]",
                name_b.c_str(),
                name_a.c_str()))
            {
                diffs->apply_changes(ro, directory_sync_mode::copy_missing_objects_and_delete_obsolete_ones);
            }
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
        printf("OK: both directories are already in sync.\r\n");
    }
    delete a;
    delete b;
    return true;
}

int _tmain(int, _TCHAR* [])
{
    synchronize_directories(
        "K:\\GIT\\PROAKT", 
        "H:\\WNBT\\PROAKTIV");
#ifdef _DEBUG
    _CrtDumpMemoryLeaks();
#endif
    printf("press ANY key to close this app()");
    getchar();
    return 0;
}

