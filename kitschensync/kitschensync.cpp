#include "stdafx.h"
#include "directory_listing.h"
#include "directory_comparer.h"
#include "list_directory.h"
#include "directory_differences.h"

int _tmain(int, _TCHAR* [])
{
    list_directory lister;
    DWORD T0 = GetTickCount();
    directory_listing a(lister.read("C:\\MinGW_DISABLED"));
    directory_listing b(lister.read("T:\\MinGW_DISABLED"));

    directory_comparer dc;
    directory_differences diffs(dc.compare(a, b));
   
    return 0;
}

