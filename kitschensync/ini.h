#ifndef kitschensync_ini_h
#define kitschensync_ini_h

namespace ini
{
    class section
    {
    public:
        section(const std::string& name)
            : m_name(name)
        {
        }

        bool lookup(const std::string& name, std::string& value) const
        {
            auto result = m_values.find(string::lowercase(name));
            if (result == m_values.end())
                return false;

            value = result->second;
            return true;
        }

        void define(const std::string& name, const std::string& value)
        {
            m_values[string::lowercase(name)] = value;
        }

        void dump() const
        {
            for (auto var : m_values)
            {
                printf("'%s': '%s'\r\n",
                    var.first.c_str(),
                    var.second.c_str());
            }
        }

    private:
        std::unordered_map<std::string, std::string> m_values;
        std::unordered_map<std::string, section*> m_sections;
        std::string m_name;
    };

    class file_reader
    {
    public:
        file_reader()
        {
        }
        virtual ~file_reader();

    private:
        file_reader(const file_reader &objectSrc);
        file_reader& operator=(const file_reader& objectSrc);

    public:
        section* read(const std::string& filename);

    private:
        bool expect_start_of_name(char c);
        bool expect_end_of_name(char c);
        bool expect_assignment_operator(char c);
        bool expect_start_of_value(char c);
        bool expect_end_of_quoted_value(char c);
        bool expect_end_of_value(char c);
        bool decode_line(const char* line);
        bool parser_failure(const std::string& msg) const;

    private:
        typedef bool (file_reader::*parser_state)(char c);

    private:
        int m_line;
        int m_column;
        int m_start_of_recording;
        section* m_current_section;
        std::string m_filename;
        std::string m_name;
        std::string m_value;
        std::string m_current_line;
        std::vector<char> m_format_buffer; // used for escaped lines
        parser_state m_current_parser_state;
        bool m_done;
        bool m_escape;
        char m_quote_char;

    };
} // namespace ini

#endif // kitschensync_ini_h
