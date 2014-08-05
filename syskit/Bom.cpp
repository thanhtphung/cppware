/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#include "syskit-pch.h"
#include "syskit/Bom.hpp"
#include "syskit/macros.h"

const unsigned char BOM[] =
{
    //None
    0xefU, 0xbbU, 0xbfU,        //Utf8
    0xffU, 0xfeU,               //Utf16
    0xfeU, 0xffU,               //Utf61
    0xffU, 0xfeU, 0x00U, 0x00U, //Utf32
    0x00U, 0x00U, 0xfeU, 0xffU  //Utf23
};

BEGIN_NAMESPACE1(syskit)

const Bom::bom_t Bom::bom_[] =
{
    {&BOM[0], 0}, //None
    {&BOM[0], 3}, //Utf8
    {&BOM[3], 2}, //Utf16
    {&BOM[5], 2}, //Utf61
    {&BOM[7], 4}, //Utf32
    {&BOM[11], 4} //Utf23
};


//!
//! Return the leading byte-order-mark in given string.
//!
Bom Bom::decode(const void* p, size_t byteSize)
{
    const unsigned char* s = static_cast<const unsigned char*>(p);
    unsigned int /*type_e*/ type = None;
    if (byteSize >= 2)
    {
        switch (s[0])
        {
        case 0x00U:
            if ((byteSize >= 4) && (s[1] == 0x00U) && (s[2] == 0xfeU) && (s[3] == 0xffU))
            {
                type = Utf23;
            }
            break;
        case 0xefU:
            if ((byteSize >= 3) && (s[1] == 0xbbU) && (s[2] == 0xbfU))
            {
                type = Utf8;
            }
            break;
        case 0xfeU:
            if (s[1] == 0xffU)
            {
                type = Utf61;
            }
            break;
        case 0xffU:
            if (s[1] == 0xfeU)
            {
                type = ((byteSize >= 4) && (s[2] == 0x00U) && (s[3] == 0x00U))? Utf32: Utf16;
            }
            break;
        default:
            break;
        }
    }

    return Bom(type);
}

END_NAMESPACE1
