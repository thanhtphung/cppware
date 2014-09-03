#include "syskit/Bom.hpp"

#include "syskit-ut-pch.h"
#include "BomSuite.hpp"

using namespace syskit;

typedef struct
{
    unsigned int /*Bom::type_e*/ type;
    size_t byteSize;
    const unsigned char seq[4];
} sample_t;

const sample_t DECODE_SAMPLE[] =
{
    {Bom::None, 0, {0x00U, 0x00U, 0x00U, 0x00U}},
    {Bom::None, 1, {0xefU, 0xbbU, 0xbfU, 0x00U}},
    {Bom::None, 2, {0x00U, 0xfeU, 0x00U, 0x00U}},
    {Bom::None, 2, {0xefU, 0xffU, 0x00U, 0x00U}},
    {Bom::None, 2, {0xfeU, 0xfeU, 0x00U, 0x00U}},
    {Bom::None, 2, {0xffU, 0x00U, 0xfeU, 0xffU}},
    {Bom::None, 3, {0x00U, 0xfeU, 0x00U, 0x00U}},
    {Bom::None, 3, {0xefU, 0xbbU, 0x00U, 0x00U}},
    {Bom::None, 3, {0xffU, 0xffU, 0x00U, 0x00U}},
    {Bom::None, 4, {0x00U, 0x00U, 0xfeU, 0x00U}},
    {Bom::Utf16, 4, {0xffU, 0xfeU, 0xfeU, 0x00U}},
    {Bom::Utf16, 4, {0xffU, 0xfeU, 0x00U, 0x12U}}
};

const sample_t SAMPLE[] =
{
    {Bom::None, 0, {0x00U, 0x00U, 0x00U, 0x00U}},
    {Bom::Utf8, 3, {0xefU, 0xbbU, 0xbfU, 0x00U}},
    {Bom::Utf16, 2, {0xffU, 0xfeU, 0x00U, 0x00U}},
    {Bom::Utf61, 2, {0xfeU, 0xffU, 0x00U, 0x00U}},
    {Bom::Utf32, 4, {0xffU, 0xfeU, 0x00U, 0x00U}},
    {Bom::Utf23, 4, {0x00U, 0x00U, 0xfeU, 0xffU}}
};

const size_t NUM_DECODE_SAMPLES = sizeof(DECODE_SAMPLE) / sizeof(DECODE_SAMPLE[0]);
const size_t NUM_SAMPLES = sizeof(SAMPLE) / sizeof(SAMPLE[0]);


BomSuite::BomSuite()
{
}


BomSuite::~BomSuite()
{
}


void BomSuite::testCtor00()
{
    bool ok = true;
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i)
    {
        const sample_t& r = SAMPLE[i];
        Bom bom0(r.type);
        Bom bom1(bom0);
        unsigned char seq[4] = {0x0aU, 0x0bU, 0x0cU, 0x0dU};
        if ((bom0.type() != r.type) ||
            (bom0.byteSize() != r.byteSize) ||
            (bom0.encode(seq) != r.byteSize) ||
            (memcmp(seq, r.seq, r.byteSize) != 0) ||
            (Bom::decode(seq, r.byteSize) != bom0) ||
            (bom1 != bom0))
        {
            ok = false;
            break;
        }
    }

    CPPUNIT_ASSERT(ok);
}


void BomSuite::testDecode00()
{
    Bom bom(Bom::Utf16);
    bom = Bom::decode(0, 0);
    bool ok = (bom.type() == Bom::None);
    CPPUNIT_ASSERT(ok);

    ok = true;
    for (unsigned int i = 0; i < NUM_DECODE_SAMPLES; ++i)
    {
        const sample_t& r = DECODE_SAMPLE[i];
        bom.reset(r.type);
        if (Bom::decode(r.seq, r.byteSize) != bom)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}
