#pragma once

#include <iostream>

#if defined(_MSC_VER) || defined(__BORLANDC__)
    typedef __int64 int64;
#else
    typedef long long int64;
#endif

int64 get_microseconds();
