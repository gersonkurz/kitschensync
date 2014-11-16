#include "stdafx.h"
#include "console_app.h"


int _tmain(int argc, _TCHAR* argv[])
{
    int rc = CONSOLE_RC_INPUT_ARGS_FAILED;
    {
        console_app app;
        if (app.handle_input_args(argc, argv))
        {
            rc = app.run();
        }
    }
#ifdef _DEBUG
    _CrtDumpMemoryLeaks();
    printf("press ANY key to close this app()");
    getchar();
#endif
    return rc;
}

