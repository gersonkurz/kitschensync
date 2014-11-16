// kitschensync.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "directory_description.h"
#include "directory_comparer.h"
#include "list_directory.h"
#include "directory_mismatch.h"
#include "utilities.h"
#include "ini.h"
#include "file_system.h"
#include "console_app.h"

std::string read_line()
{
    char input[200];
    if (fgets(input, sizeof(input), stdin))
    {
        // strip newline
        for (int i = 0; i < sizeof(input); ++i)
        {
            if (input[i] == '\r' || input[i] == '\n')
            {
                input[i] = 0;
            }
        }
        return input;
    }
    return "";
}

// todo: make interface for this
bool affirmative_input(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    std::string formatted(string::vformat(fmt, args));
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
            name_b.c_str(),
            name_a.c_str()))
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

    DWORD t0 = GetTickCount();
    directory_description* a(lister.read(name_a.c_str()));
    printf("%s read after %d ms.\r\n", name_a.c_str(), GetTickCount() - t0);
    t0 = GetTickCount();
    directory_description* b(lister.read(name_b.c_str()));
    printf("%s read after %d ms.\r\n", name_b.c_str(), GetTickCount() - t0);

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
            if(ro == relationship_order::undefined)
            {
                printf("*** WARNING: was unable to determine relationship between these two directories...\r\n\r\n");
                diffs->dump();

                printf("1: copy %s => %s\r\n", name_a.c_str(), name_b.c_str());
                printf("2: copy %s => %s\r\n", name_b.c_str(), name_a.c_str());
                printf("anything else: abort >>");
                std::string input(read_line());
                if (string::equals(input, "1"))
                {
                    ro = relationship_order::a_newer_than_b;
                    previously_known = true;
                }
                else if (string::equals(input, "2"))
                { 
                    ro = relationship_order::b_newer_than_a;
                    previously_known = true;
                }
            }
        }
        if (ro == relationship_order::a_newer_than_b)
        {
            if (previously_known || affirmative_input("%s is newer than %s. Are you sure to proceed? [Y/n]",
                name_a.c_str(),
                name_b.c_str()))
            {
                DWORD t0 = GetTickCount();
                diffs->apply_changes(ro, directory_sync_mode::copy_missing_objects_and_delete_obsolete_ones);
                printf("Time to apply changes: %d ms\r\n", GetTickCount() - t0);
            }
        }
        else if (ro == relationship_order::b_newer_than_a)
        {
            if (previously_known || affirmative_input("%s is newer than %s. Are you sure to proceed? [Y/n]",
                name_b.c_str(),
                name_a.c_str()))
            {
                DWORD t0 = GetTickCount();
                diffs->apply_changes(ro, directory_sync_mode::copy_missing_objects_and_delete_obsolete_ones);
                printf("Time to apply changes: %d ms\r\n", GetTickCount() - t0);
            }
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

console_app::console_app()
    :
    m_show_help(false)
{
    std::string components[] = {
        file_system::get_current_directory(),
        ".kitschensync"
    };
    m_config_filename = file_system::path::join(components);
}

console_app::~console_app()
{
}

bool console_app::handle_input_args(int argc, char* argv[])
{
    enum class mode {
        expect_argument,
        expect_name_a,
        expect_name_b,
    };

    mode current_mode = mode::expect_argument;

    for (int i = 1; i < argc; ++i)
    {
        const char* arg = argv[i];
        assert(arg != nullptr);
        switch (current_mode)
        {
        case mode::expect_argument:
            if ((*arg == '-') || (*arg == '/'))
            {
                ++arg;

                if (string::equalsI(arg, "a"))
                {
                    current_mode = mode::expect_name_a;
                }
                else if (string::equalsI(arg, "b"))
                {
                    current_mode = mode::expect_name_b;
                }
                else if (string::equals(arg, "?"))
                {
                    m_show_help = true;
                }
                else
                {
                    printf("ERROR, unknown parameter '%s'\r\n", argv[i]);
                    return false;
                }
            }
            else
            {
                printf("ERROR, expected argument, got '%s' instead\r\n", arg);
                return false;
            }
            break;

        case mode::expect_name_a:
            m_a = arg;
            current_mode = mode::expect_argument;
            break;

        case mode::expect_name_b:
            m_b = arg;
            current_mode = mode::expect_argument;
            break;
        default:
            assert(false);
        }
    }
    if (current_mode != mode::expect_argument)
    {
        printf("ERROR, invalid arguments\r\n");
        return false;
    }
    return true;
}

int console_app::run()
{
    if (m_show_help)
        return show_help();

    read_configuration_file();

    if (string::is_empty(m_a) || string::is_empty(m_b))
    {
        printf("ERROR, must specify two directories to sync\r\n");
        return false;
    }
    synchronize_directories(m_a, m_b);
    return 0;
}

void console_app::read_configuration_file()
{
    ini::file_reader reader;
    ini::section* section = reader.read(m_config_filename);
    if (section)
    {
        section->lookup("a", m_a);
        section->lookup("b", m_b);
        delete section;
    }
    else
    {
        printf("WARNING: unable to read configuration file '%s'\r\n", m_config_filename.c_str());
    }
}

int console_app::show_help()
{
    printf("USAGE: kitchensync {OPTIONS}\r\n");
    printf("OPTIONS:\r\n");
    printf("/a ...... first directory\r\n");
    printf("/b ...... second directory\r\n");
    printf("/cfg .... configuration filename (defaults to %s)", m_config_filename.c_str());
    printf("\r\n");
    return CONSOLE_RC_COMPLETED_WITHOUT_RUNNING;
}