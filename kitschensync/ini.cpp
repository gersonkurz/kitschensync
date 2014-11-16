// kitschensync.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "utilities.h"
#include "ini.h"
#include "file_system.h"

namespace ini
{
#define is_valid_name_element(c) (isalnum(c) || (c== '_'))

    bool file_reader::parser_failure(const std::string& msg) const
    {
        fprintf(stderr, "ERROR while parsing '%s'\r\n", m_filename.c_str());
        fprintf(stderr, "in line %d, column %d: %s\r\n", m_line, m_column + 1, msg.c_str());
        return false;
    }

    bool file_reader::expect_start_of_name(char c)
    {
        // ok for empty line
        if (!c)
        {
            m_done = true;
            return true;
        }

        // skip leading spaces
        if (isspace(c))
        {
            return true;
        }

        // line starts with a comment: done
        if (c == '#')
        {
            m_done = true;
            return true;
        }

        if (is_valid_name_element(c))
        {
            // ok, start of name
            assert(m_start_of_recording == -1);
            m_start_of_recording = m_column;
            m_current_parser_state = &file_reader::expect_end_of_name;
            return true;
        }
        return parser_failure(string::format("Expected start of variable name, got '%c' instead", c));
    }

    bool file_reader::expect_end_of_name(char c)
    {
        if (!c)
            return parser_failure("Unexpected: variable name without value");

        if (isspace(c))
        {
            assert(m_start_of_recording != -1);
            assert(m_name.empty());
            m_name = m_current_line.substr(m_start_of_recording, m_column - m_start_of_recording);
            m_start_of_recording = -1;
            m_current_parser_state = &file_reader::expect_assignment_operator;
            return true;
        }

        if (c == '=')
        {
            assert(m_start_of_recording != -1);
            assert(m_name.empty());
            m_name = m_current_line.substr(m_start_of_recording, m_column - m_start_of_recording);
            m_start_of_recording = -1;
            m_current_parser_state = &file_reader::expect_start_of_value;
            return true;
        }

        if (is_valid_name_element(c))
        {
            return true;
        }
        return parser_failure(string::format("Expected end of name, got '%c' instead", c));
    }

    bool file_reader::expect_assignment_operator(char c)
    {
        if (!c)
            return parser_failure("Unexpected: variable name without value");

        if (c == '=')
        {
            m_current_parser_state = &file_reader::expect_start_of_value;
            return true;
        }
        if (isspace(c))
        {
            return true;
        }
        return parser_failure(string::format("Expected assignment operator, got '%c' instead", c));
    }

    bool file_reader::expect_start_of_value(char c)
    {
        if (!c || (c == '#'))
        {
            // ok: this just means that the assignment is an empty string.
            m_done = true;
            return true;
        }

        if (isspace(c))
        {
            return true;
        }
        if ((c == '"') || (c == '\''))
        {
            assert(m_quote_char == 0);
            m_quote_char = c;
            assert(m_start_of_recording == -1);
            assert(m_value.empty());
            m_format_buffer.clear();
            m_current_parser_state = &file_reader::expect_end_of_quoted_value;
            return true;
        }

        assert(m_start_of_recording == -1);
        assert(m_value.empty());
        m_start_of_recording = m_column;
        m_current_parser_state = &file_reader::expect_end_of_value;
        return true;
    }

    bool file_reader::expect_end_of_value(char c)
    {
        if (!c || (c == '#'))
        {
            // OK, we are at the end of the string. But we don't want any trailing spaces, so let's remove them:
            int n = m_column;
            const char* text = m_current_line.c_str();
            while (n > 0)
            {
                if (isspace(text[n - 1]))
                    --n;
                else break;
            }
            assert(n > 0);
            assert(m_start_of_recording != -1);
            assert(m_value.empty());
            m_value = m_current_line.substr(m_start_of_recording, n - m_start_of_recording);
            m_start_of_recording = -1;
            m_done = true;
            return true;
        }
        return true;
    }

    bool file_reader::expect_end_of_quoted_value(char c)
    {
        if (!c || (c == '#'))
        {
            // NOT OK! we expected a string with quotation marks
            return parser_failure(string::format("Expected end-of-string ('%c') , got '%c' instead", m_quote_char, c));
        }

        if (m_escape)
        {
            m_escape = false;
            m_format_buffer.push_back(c);
        }
        else if (c == '\\')
        {
            m_escape = true;
        }
        else if (c == m_quote_char)
        {
            // ok, THIS is the end of the string
            m_format_buffer.push_back(0);
            m_value = &m_format_buffer[0];
            m_done = true;
        }
        else
        {
            m_format_buffer.push_back(c);
        }
        return true;
    }

    bool file_reader::decode_line(const char* line)
    {
        m_escape = false;
        m_current_parser_state = &file_reader::expect_start_of_name;
        m_name.clear();
        m_value.clear();
        m_start_of_recording = -1;
        m_done = false;
        //printf("LINE %ld: '%s'\r\n", m_line, line);
        for (m_column = 0; !m_done; ++m_column)
        {
            if (!(this->*m_current_parser_state)(line[m_column]))
                return false;
        }
        if (!m_name.empty())
        {
            m_current_section->define(m_name, m_value);
        }
        return true;
    }

    file_reader::~file_reader()
    {
    }

    section* file_reader::read(const std::string& filename)
    {
        if (!file_system::does_file_exist(filename))
            return nullptr;

        auto result = new section("");
        m_current_section = result;
        m_filename = filename;
        std::ifstream infile(filename);
        m_quote_char = 0;
        m_line = 1;
        while (std::getline(infile, m_current_line))
        {
            if (!decode_line(m_current_line.c_str()))
            {
                delete result;
                return nullptr;
            }

            ++m_line;
        }
        return result;
    }
}
