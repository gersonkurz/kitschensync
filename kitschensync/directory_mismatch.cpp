#include "stdafx.h"
#include "directory_description.h"
#include "directory_mismatch.h"
#include "file_description.h"

relationship_order directory_mismatch::determine_relationship(const directory_description** pa, const directory_description** pb) const
{
    relationship_order result = determine_relationship_order();
    switch (result)
    {
    case relationship_order::a_newer_than_b:
        *pa = m_a;
        *pb = m_b;
        break;

    case relationship_order::b_newer_than_a:
        *pa = m_b;
        *pb = m_a;
        break;
    }
    return result;
}
void directory_mismatch::dump() const
{
    if (m_files_missing_in_a.size())
    {
        printf("%d files are missing in '%s' (A):\r\n", m_files_missing_in_a.size(), m_a->get_path());
        for (auto var : m_files_missing_in_a)
        {
            printf("- '%s'\r\n", var->get_path());
        }
        printf("\r\n");
    }

    if (m_files_missing_in_b.size())
    {
        printf("%d files are missing in '%s' (B):\r\n", m_files_missing_in_b.size(), m_b->get_path());
        for (auto var : m_files_missing_in_b)
        {
            printf("- '%s'\r\n", var->get_path());
        }
        printf("\r\n");
    }

    if (m_directories_missing_in_a.size())
    {
        printf("%d directories are missing in '%s' (A):\r\n", m_directories_missing_in_a.size(), m_a->get_path());
        for (auto var : m_directories_missing_in_a)
        {
            printf("- '%s'\r\n", var->get_path());
        }
        printf("\r\n");
    }
    
    if (m_directories_missing_in_b.size())
    {
        printf("%d directories are missing in '%s' (B):\r\n", m_directories_missing_in_b.size(), m_b->get_path());
        for (auto var : m_directories_missing_in_b)
        {
            printf("- '%s'\r\n", var->get_path());
        }
        printf("\r\n");
    }

    if (m_file_mismatches.size())
    {
        printf("%d files have a mismatch:\r\n", m_file_mismatches.size());

        for (auto& var : m_file_mismatches)
        {
            switch (var.reason)
            { 
            case file_mismatch_reason::file_size:
                printf("size of %s (%lld) <> size of %s (%lld)\r\n",
                    var.a->get_path(),
                    var.a->get_file_size(),
                    var.b->get_path(),
                    var.b->get_file_size() );
                break;
            case file_mismatch_reason::a_is_newer_than_b:
                printf("%s is newer than %s\r\n",
                    var.a->get_path(),
                    var.b->get_path());
                break;
            case file_mismatch_reason::a_is_older_than_b:
                printf("%s is older than %s\r\n",
                    var.a->get_path(),
                    var.b->get_path());
                break;
            default:
                assert(false);
            }
        }

        for (auto var : m_directory_mismatches)
        {
            var->dump();
        }
    }
}

void directory_mismatch::copy_file(const char* source, const char* target) const
{
    printf("copy file %s => %s\r\n", source, target);
    //::CopyFile(source, target, false);
}

void directory_mismatch::copy_missing_files(
    const directory_description* source, 
    const directory_description* target, 
    const std::vector<const file_description*>& files_missing) const
{
    for (auto var : files_missing)
    {
        copy_file(var->get_path(), target->get_in_path(var->get_name()).c_str());
    }
}

void directory_mismatch::copy_missing_directories(
    const directory_description* source, 
    const directory_description* target, 
    const std::vector<const directory_description*>& directories_missing) const
{
    for (auto var : directories_missing)
    {
        var->copy_recursive(target->get_path());
    }
}

void directory_mismatch::apply_changes(relationship_order ro) const
{
    switch (ro)
    {
    case relationship_order::b_newer_than_a:
        copy_missing_files(m_b, m_a, m_files_missing_in_a);
        copy_missing_directories(m_b, m_a, m_directories_missing_in_a);
        break;

    case relationship_order::a_newer_than_b:
        copy_missing_files(m_a, m_b, m_files_missing_in_b);
        copy_missing_directories(m_a, m_b, m_directories_missing_in_b);
        break;
    }
    /*

    if (m_file_mismatches.size())
    {
        for (auto& var : m_file_mismatches)
        {
            switch (var.reason)
            {
            case file_mismatch_reason::a_is_newer_than_b:
                b_is_newer_than_a = false;
                break;
            case file_mismatch_reason::a_is_older_than_b:
                a_is_newer_than_b = false;
                break;
            }
        }

        for (auto var : m_directory_mismatches)
        {
            switch (var->determine_relationship_order())
            {
            case relationship_order::a_newer_than_b:
                b_is_newer_than_a = false;
                break;

            case relationship_order::b_newer_than_a:
                a_is_newer_than_b = false;
                break;
            }
        }
    }
    if (a_is_newer_than_b && !b_is_newer_than_a)
        return relationship_order::a_newer_than_b;

    if (b_is_newer_than_a && !a_is_newer_than_b)
        return relationship_order::b_newer_than_a;
        */

}


relationship_order directory_mismatch::determine_relationship_order() const
{
    bool a_is_newer_than_b = true;
    bool b_is_newer_than_a = true;

    if (m_files_missing_in_a.size())
    {
        a_is_newer_than_b = false;
    }

    if (m_files_missing_in_b.size())
    {
        b_is_newer_than_a = false;
    }

    if (m_directories_missing_in_a.size())
    {
        a_is_newer_than_b = false;
    }

    if (m_directories_missing_in_b.size())
    {
        b_is_newer_than_a = false;
    }

    if (m_file_mismatches.size())
    {
        for (auto& var : m_file_mismatches)
        {
            switch (var.reason)
            {
            case file_mismatch_reason::a_is_newer_than_b:
                b_is_newer_than_a = false;
                break;
            case file_mismatch_reason::a_is_older_than_b:
                a_is_newer_than_b = false;
                break;
            }
        }

        for (auto var : m_directory_mismatches)
        {
            switch (var->determine_relationship_order())
            {
            case relationship_order::a_newer_than_b:
                b_is_newer_than_a = false;
                break;

            case relationship_order::b_newer_than_a:
                a_is_newer_than_b = false;
                break;
            }
        }
    }
    if (a_is_newer_than_b && !b_is_newer_than_a)
        return relationship_order::a_newer_than_b;

    if (b_is_newer_than_a && !a_is_newer_than_b)
        return relationship_order::b_newer_than_a;

    return relationship_order::undefined;
}