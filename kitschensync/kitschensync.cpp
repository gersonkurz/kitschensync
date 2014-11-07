#include "stdafx.h"
#include "directory_listing.h"
#include "directory_comparer.h"
#include "list_directory.h"
#include "directory_differences.h"

int _tmain(int, _TCHAR* [])
{
    list_directory lister;
    directory_listing a(lister.read("G:\\FIDUCIA"));
    directory_listing b(lister.read("K:\\FIDUCIA\\fiducia_windows"));

    directory_comparer dc;
    directory_differences diffs(dc.compare(a, b));
   
    return 0;
}

