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
#endif
    printf("Press return to close this program.");
    getchar();
    return rc;
}

