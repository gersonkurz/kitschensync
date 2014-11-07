#include "stdafx.h"
#include "directory_description.h"
#include "directory_mismatch.h"
#include "file_description.h"

bool directory_mismatch::determine_relationship(const directory_description** pa, const directory_description** pb) const
{
    switch (determine_relationship_order())
    {
    case relationship_order::a_newer_than_b:
        *pa = m_a;
        *pb = m_b;
        return true;

    case relationship_order::b_newer_than_a:
        *pa = m_b;
        *pb = m_a;
        return true;

    default:
        return false;
    }
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