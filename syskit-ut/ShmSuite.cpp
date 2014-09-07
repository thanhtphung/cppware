#include "syskit/Process.hpp"
#include "syskit/Shm.hpp"

#include "syskit-ut-pch.h"
#include "ShmSuite.hpp"

using namespace syskit;


ShmSuite::ShmSuite()
{
}


ShmSuite::~ShmSuite()
{
}


//
// Zero key.
//
void ShmSuite::testCtor00()
{
    Shm m(4092U /*size*/);
    bool ok = (m.addr() != 0);
    CPPUNIT_ASSERT(ok);
    size_t size;
    ok = (m.addr(size) != 0) && (size == 4092U);
    CPPUNIT_ASSERT(ok);
    ok = m.isCreator() && m.isOk() && (m.key() == 0) && (m.size() == 4092U);
    CPPUNIT_ASSERT(ok);

    unsigned char* p = m.addr();
    const unsigned char* pEnd = p + m.size();
    memset(p, 0xc3U, m.size());
    for (; p < pEnd; ++p)
    {
        if (*p != 0xc3U)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Non-zero key.
//
void ShmSuite::testCtor01()
{

    // Create.
    unsigned int key = 0x01020304U + static_cast<unsigned int>(Process::myId());
    Shm* m0 = new Shm(key, 4091U /*size*/);
    bool ok = (m0->addr() != 0);
    CPPUNIT_ASSERT(ok);
    size_t size;
    ok = (m0->addr(size) != 0) && (size == 4091U);
    CPPUNIT_ASSERT(ok);
    ok = m0->isCreator() && m0->isOk() && (m0->key() == key) && (m0->size() == 4091U);
    CPPUNIT_ASSERT(ok);

    // Attach.
    Shm m1(key, 4091U /*size*/);
    ok = (m1.addr() != 0);
    CPPUNIT_ASSERT(ok);
    size = 0;
    ok = (m1.addr(size) != 0) && (size == 4091U);
    CPPUNIT_ASSERT(ok);
    ok = (!m1.isCreator()) && m1.isOk() && (m1.key() == key) && (m1.size() == 4091U);
    CPPUNIT_ASSERT(ok);

    memset(m0->addr(), 0x3cU, m0->size());
    delete m0;
    const unsigned char* p = m1.addr();
    for (const unsigned char* pEnd = p + m1.size(); p < pEnd; ++p)
    {
        if (*p != 0x3cU)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void ShmSuite::testCtor02()
{

    // Bad size.
    Shm m0(0U /*size*/);
    bool ok = (!m0.isOk());
    CPPUNIT_ASSERT(ok);
    Shm m1(0U - 4 /*size*/);
    ok = (!m1.isOk());
    CPPUNIT_ASSERT(ok);

    // Bad size.
    unsigned int key = 0x0a0b0c0dU + static_cast<unsigned int>(Process::myId());
    Shm m2(key, 0U /*size*/);
    ok = (!m2.isOk());
    CPPUNIT_ASSERT(ok);
    Shm m3(key, 0U - 4 /*size*/);
    ok = (!m3.isOk());
    CPPUNIT_ASSERT(ok);

    // Copy from bad instance.
    Shm m4(m3);
    ok = (!m4.isOk());
    CPPUNIT_ASSERT(ok);
}


void ShmSuite::testCtor03()
{
    unsigned int key = 0x11223344U + static_cast<unsigned int>(Process::myId());
    {
        Shm m(key, 2U /*size*/);
        memset(m.addr(), 0x99U, m.size());
        Shm(key, 2U).detach();
        m.detach();
        bool ok = (!m.isOk());
        CPPUNIT_ASSERT(ok);
    }

    // Good key. Mismatched size.
  {
      Shm m(key, 3U /*size*/);
      bool ok = (!m.isOk());
      CPPUNIT_ASSERT(ok);
  }

    // Good key. Mismatched size.
  {
      Shm m(key, 1U /*size*/);
      bool ok = (!m.isOk());
      CPPUNIT_ASSERT(ok);
  }

    // Good key. Any size.
  {
      Shm m(key, 0U /*size*/);
      bool ok = (m.isOk() && (!m.isCreator()) && (m.size() == 2U));
      CPPUNIT_ASSERT(ok);
      const unsigned char* p = m.addr();
      ok = (p[0] == 0x99U) && (p[1] == 0x99U);
      CPPUNIT_ASSERT(ok);
      m.detach();
  }

    // Good key. Good size.
  {
      Shm m(key, 2U);
      bool ok = (m.isOk() && (!m.isCreator()) && (m.size() == 2U));
      CPPUNIT_ASSERT(ok);
      const unsigned char* p = m.addr();
      ok = (p[0] == 0x99U) && (p[1] == 0x99U);
      CPPUNIT_ASSERT(ok);
  }
}


//
// Copy constructor.
// Zero key.
//
void ShmSuite::testCtor04()
{
    Shm* m0 = new Shm(123U /*size*/);
    Shm m1(*m0);
    bool ok = (m1.addr() != 0);
    CPPUNIT_ASSERT(ok);
    size_t size;
    ok = (m1.addr(size) != 0) && (size == 123U);
    CPPUNIT_ASSERT(ok);
    ok = (!m1.isCreator()) && m1.isOk() && (m1.key() == 0) && (m1.size() == 123U);
    CPPUNIT_ASSERT(ok);

    memset(m0->addr(), 0xc3U, m0->size());
    delete m0;
    const unsigned char* p = m1.addr();
    for (const unsigned char* pEnd = p + m1.size(); p < pEnd; ++p)
    {
        if (*p != 0xc3U)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Copy constructor.
// Non-zero key.
//
void ShmSuite::testCtor05()
{
    unsigned int key = 0x44332211U + static_cast<unsigned int>(Process::myId());
    Shm m0(key, 45U /*size*/);
    Shm m1(m0);
    bool ok = (m1.addr() != 0);
    CPPUNIT_ASSERT(ok);
    size_t size;
    ok = (m1.addr(size) != 0) && (size == 45U);
    CPPUNIT_ASSERT(ok);
    ok = (!m1.isCreator()) && m1.isOk() && (m1.key() == key) && (m1.size() == 45U);
    CPPUNIT_ASSERT(ok);

    memset(m0.addr(), 0x99U, m0.size());
    const unsigned char* p = m1.addr();
    const unsigned char* pEnd = p + m1.size();
    for (; p < pEnd; ++p)
    {
        if (*p != 0x99U)
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


void ShmSuite::testRm00()
{
    unsigned int key = 0xaabbccddU + static_cast<unsigned int>(Process::myId());
    Shm m0(key, 124U /*size*/);
    bool ok = m0.rm();
    CPPUNIT_ASSERT(ok);
    ok = (!m0.rm());
    CPPUNIT_ASSERT(ok);

    Shm m1(key, 124U /*size*/);
    ok = (m1.isOk() && m1.isCreator() && (m1.size() == 124U));
    CPPUNIT_ASSERT(ok);
}
