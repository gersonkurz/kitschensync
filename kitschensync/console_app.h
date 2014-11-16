#ifndef kitschensync_console_app_h
#define kitschensync_console_app_h

const int CONSOLE_RC_SUCCESS = 0;
const int CONSOLE_RC_COMPLETED_WITHOUT_RUNNING = 5;
const int CONSOLE_RC_INPUT_ARGS_FAILED = 10;

class console_app
{
public:
    console_app();
    virtual ~console_app();

private:
    console_app(const console_app& objectSrc);
    console_app& operator=(const console_app& objectSrc);

public:
    bool handle_input_args(int argc, char* argv[]);
    int run();

private:
    int show_help();
    void read_configuration_file();

private:
    std::string m_config_filename;
    std::string m_a;
    std::string m_b;
    bool m_show_help;

};
#endif // kitschensync_console_app_h
