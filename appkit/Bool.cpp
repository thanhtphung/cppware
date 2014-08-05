/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include <cstdlib>
#include "syskit/macros.h"

#include "appkit-pch.h"
#include "appkit/Bool.hpp"
#include "appkit/Str.hpp"

BEGIN_NAMESPACE1(appkit)


Bool::item_t Bool::toBool(const char* val)
{

    // Consider an empty string to indicate true.
    item_t rc = false;
    if ((*val == 0) || (Str::compareKI(val, "true") == 0) || (Str::compareKI(val, "yes") == 0))
    {
        rc = true;
    }

    else if ((Str::compareKI(val, "false") != 0) && (Str::compareKI(val, "no") != 0))
    {
        char* p;
        int v = std::strtol(val, &p, 0);
        if ((p != val) && (*p == 0))
        {
            rc = (v != 0);
        }
    }

    return rc;
}

END_NAMESPACE1
