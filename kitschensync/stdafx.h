// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#include <stdio.h>
#include <tchar.h>

#include <Windows.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <assert.h>
#include <deque>
#include <fstream>

#define TARGET_IS_WINDOWS
#undef FILE_SYSTEM_IS_CASE_SENSITIVE 
#ifdef _M_AMD64
#define TARGET_IS_64BIT
#else
#undef TARGET_IS_64BIT
#endif

