// kitschensync.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "file_description.h"
#include "directory_description.h"

const char* file_description::get_path() const
{
    if (m_path.size() == 0)
    {
        m_path = m_parent->get_in_path(m_name.c_str());
    }
    return m_path.c_str();
}