#include "stdafx.h"
#include "directory_description.h"
#include "directory_mismatch.h"
#include "file_description.h"
#include "file_system.h"
#include "utilities.h"

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

directory_mismatch::~directory_mismatch()
{
    utilities::clean(m_directory_mismatches);
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


void directory_mismatch::apply_changes(relationship_order ro, directory_sync_mode dsm) const
{
    assert((relationship_order::b_newer_than_a == ro) || 
           (relationship_order::a_newer_than_b == ro));

    assert((directory_sync_mode::only_copy_missing_objects == dsm) ||
           (directory_sync_mode::copy_missing_objects_and_delete_obsolete_ones == dsm));

    std::deque<const directory_mismatch*> items;
    items.push_back(this);
    while (!items.empty())
    {
        const directory_mismatch* that(*items.begin());
        items.pop_front();
        assert(that != nullptr);

        if (relationship_order::b_newer_than_a == ro)
        {
            copy_missing_objects(that->m_a, that->m_files_missing_in_a, that->m_directories_missing_in_a);
            if (directory_sync_mode::copy_missing_objects_and_delete_obsolete_ones == dsm)
            {
                delete_obsolete_objects(that->m_files_missing_in_b, that->m_directories_missing_in_b);
            }
        }
        else
        {
            copy_missing_objects(that->m_b, that->m_files_missing_in_b, that->m_directories_missing_in_b);
            if (directory_sync_mode::copy_missing_objects_and_delete_obsolete_ones == dsm)
            {
                delete_obsolete_objects(that->m_files_missing_in_a, that->m_directories_missing_in_a);
            }
        }

        for (auto var : that->m_file_mismatches)
        {
            if (relationship_order::a_newer_than_b == ro)
            {
                file_system::copy_file(
                    var.a->get_path(),
                    var.b->get_path());
            }
            else
            {
                file_system::copy_file(
                    var.b->get_path(),
                    var.a->get_path());
            }
        }

        for (auto var : that->m_directory_mismatches)
        {
            items.push_back(var);
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


    relationship_order result = relationship_order::undefined;
    if (a_is_newer_than_b && !b_is_newer_than_a)
        result = relationship_order::a_newer_than_b;

    else if (b_is_newer_than_a && !a_is_newer_than_b)
        result = relationship_order::b_newer_than_a;

    return result;
}

std::string as_string(relationship_order ro)
{
    switch (ro)
    {
    case relationship_order::undefined:
        return "undefined";
    case relationship_order::a_newer_than_b:
        return "a_newer_than_b";
    case relationship_order::b_newer_than_a:
        return "b_newer_than_a";
    default:
        assert(false);
        return "invalid";
    }
}