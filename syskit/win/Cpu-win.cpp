/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "syskit-pch.h"
#include "syskit/Cpu.hpp"
#include "syskit/macros.h"

const unsigned int HERTZ_PER_MEGAHERTZ = 1000000U;
const wchar_t CPU_KEY[] = L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";
const wchar_t MEGAHERTZ_VALUE[] = L"~Mhz";

BEGIN_NAMESPACE1(syskit)


//
// Return my processor's frequency in hertz. This method assumes the
// frequency is about the same for all CPUs in the same system. The
// value returned is the frequency of the first CPU entry the method
// sees. Return zero if the information is unavailable (most likely
// due to a coding error).
//
unsigned int Cpu::getHertz()
{

    // Return zero if the information is unavailable.
    unsigned int hertz = 0U;

    // Assume frequency is about the same for all CPUs in the same system.
    // Look in the registry for the frequency of CPU zero.
    HKEY k;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, CPU_KEY, 0, KEY_READ, &k) == ERROR_SUCCESS)
    {
        unsigned char value[sizeof(DWORD)];
        DWORD valueSize = sizeof(value);
        DWORD valueType;
        if ((RegQueryValueExW(k, MEGAHERTZ_VALUE, 0, &valueType, value, &valueSize) == ERROR_SUCCESS) &&
            (valueType == REG_DWORD) &&
            (valueSize == sizeof(value)))
        {
            hertz = *(DWORD*)(value)* HERTZ_PER_MEGAHERTZ;
        }
        RegCloseKey(k);
    }

    // Return my processor's frequency in hertz.
    return hertz;
}

END_NAMESPACE1
