#include "syskit/Trie.hpp"

#include "syskit-ut-pch.h"
#include "TrieSuite.hpp"

using namespace syskit;


TrieSuite::TrieSuite()
{
}


TrieSuite::~TrieSuite()
{
}


bool TrieSuite::validateOrder(void* arg, const unsigned char* k, void* /*v*/)
{
    unsigned int* k0 = (unsigned int*)(arg);
    unsigned int k1 = Trie::U32Key::decode(k);
    bool ok = (*k0 < k1);
    *k0 = k1;
    return ok;
}


void TrieSuite::deleteV(void* /*arg*/, const unsigned char* /*k*/, void* v)
{
    delete (unsigned int*)(v);
}


void TrieSuite::rmKv(void* arg, const unsigned char* k, void* v)
{
    Trie& trie = *static_cast<Trie*>(arg);
    unsigned int numKvPairs = trie.numKvPairs() - 1;
    void* removedV = 0;
    bool ok = trie.rm(k, removedV) && (v == removedV) && (trie.numKvPairs() == numKvPairs) && (!trie.find(k));
    CPPUNIT_ASSERT(ok);
}


//
// Add random keys. Also verify the copy constructor and
// the comparison operators.
//
void TrieSuite::testAdd00()
{
    unsigned char maxDigit = 0x0fU;
    Trie trie(maxDigit);
    const char* s = "aRandomStringUsedForTriePopulation!!!~@#$%^&*()_+=-`{}|][:;'<>?/.,";
    unsigned int numKvPairs = 0;
    bool ok = true;
    for (const char* p = s; *p; ++p)
    {
        for (unsigned int u32 = *p; u32 != 0; u32 <<= 1)
        {
            void* foundV = 0;
            Trie::U32Key k(u32);
            unsigned int* v = new unsigned int(u32);
            ++numKvPairs;
            if (trie.find(k, foundV))
            {
                delete v;
                --numKvPairs;
            }
            else if ((!trie.add(k.asBytes(), v)) ||
                (trie.numKvPairs() != numKvPairs) ||
                (!trie.find(k, foundV)) ||
                (v != foundV))
            {
                ok = false;
                for (; *(p + 1) != 0; ++p); //terminate outer loop
                break;
            }
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (trie.numKvPairs() == numKvPairs) && (trie.numNodes() > numKvPairs);
    CPPUNIT_ASSERT(ok);

    unsigned int arg = 0;
    ok = trie.applyChildFirst(validateOrder, &arg);
    CPPUNIT_ASSERT(ok);
    arg = 0;
    ok = trie.applyParentFirst(validateOrder, &arg);
    CPPUNIT_ASSERT(ok);

    Trie clone(trie);
    ok = (clone == trie);
    CPPUNIT_ASSERT(ok);

    Trie::U32Key k('!');
    void* oldV0 = 0;
    ok = clone.associate(k, &k, oldV0);
    CPPUNIT_ASSERT(ok);
    void* oldV1 = 0;
    ok = clone.associate(k, &clone, oldV1) && (oldV1 == &k);
    CPPUNIT_ASSERT(ok);
    ok = (clone != trie);
    CPPUNIT_ASSERT(ok);
    ok = clone.associate(k, oldV0);
    ok = (clone == trie);
    CPPUNIT_ASSERT(ok);

    clone.reset();
    ok = (clone != trie);
    CPPUNIT_ASSERT(ok);

    trie.applyChildFirst(deleteV, 0);
}


//
// Add keys to a single-branch trie.
// Also verify the assignment and comparison operators and the isValid() method.
//
void TrieSuite::testAdd01()
{
    unsigned char maxDigit = 127U;
    Trie trie(maxDigit);
    char s[] = "aRandomStringUsedForTriePopulation!!!";
    unsigned char n[] = {1, 2, 3, 5, 10, 9, 7, 17, 18, 16};
    bool ok = true;
    for (unsigned int i = 0; i < 10; ++i)
    {
        Trie::StrKey key(Trie::StrKey::Ascii, s, n[i]);
        char* value = s + n[i];
        void* foundV = 0;
        if ((!trie.add(key.asBytes(), value)) ||
            (!trie.find(key)) ||
            (!trie.find(key, foundV)) ||
            (foundV != value))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
    ok = (trie.numKvPairs() == 10) && (trie.numNodes() == 19);
    CPPUNIT_ASSERT(ok);

    Trie::StrKey k(Trie::StrKey::Ascii, s, 1);
    ok = (trie.countKvPairs(k.asBytes()) == 10);
    CPPUNIT_ASSERT(ok);
    k.reset(Trie::StrKey::Ascii, s, 17);
    ok = (trie.countKvPairs(k.asBytes()) == 2);
    CPPUNIT_ASSERT(ok);
    k.reset(Trie::StrKey::Ascii, s, 18);
    ok = (trie.countKvPairs(k.asBytes()) == 1);
    CPPUNIT_ASSERT(ok);

    Trie clone0(127U /*maxDigit*/);
    clone0 = trie;
    ok = (clone0 == trie);
    CPPUNIT_ASSERT(ok);

    Trie::StrKey key(Trie::StrKey::Ascii, s, 5);
    ok = clone0.associate(key, &key);
    CPPUNIT_ASSERT(ok);
    void* removedV = 0;
    ok = clone0.associate(key, &clone0, removedV) && (removedV == &key);
    CPPUNIT_ASSERT(ok);
    ok = (clone0 != trie);
    CPPUNIT_ASSERT(ok);

    clone0.reset();
    ok = (clone0 != trie);
    CPPUNIT_ASSERT(ok);

    Trie clone1('a' /*maxDigit*/);
    clone1 = trie;
    ok = (clone1 != trie) && (clone1.numKvPairs() == 3) && (clone1.numNodes() == 4);
    CPPUNIT_ASSERT(ok);

    for (unsigned int i = 0; i < 3; ++i)
    {
        Trie::StrKey key(Trie::StrKey::Ascii, s, n[i]);
        if (!clone1.find(key))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);
}


//
// Try erroneous adds. Also verify the isValid() method.
//
void TrieSuite::testAdd02()
{
    Trie trie;

    // Adding a zero key or an empty key should fail.
    unsigned char emptyKey[1] = {0};
    void* value = emptyKey;
    bool ok = (!trie.add(0, value)) &&
        (!trie.isValid(0)) &&
        (!trie.add(emptyKey, value)) &&
        (!trie.isValid(emptyKey));
    CPPUNIT_ASSERT(ok);

    // Adding an invalid key should fail.
    for (unsigned int i = 1; i <= 3; ++i)
    {
        unsigned char badKey[1 + 3] = {3, 0, 0, 0};
        badKey[i] = trie.maxDigit() + 1;
        if (trie.add(badKey, value) || trie.isValid(badKey))
        {
            ok = false;
            break;
        }
    }
    CPPUNIT_ASSERT(ok);

    // Adding a zero value should fail.
    Trie::U32Key key(123);
    ok = (!trie.add(key, 0));
    CPPUNIT_ASSERT(ok);
}


void TrieSuite::testCtor00()
{
    Trie trie;
    bool ok = (trie.maxDigit() == Trie::DefaultMaxDigit) && (trie.numKvPairs() == 0) && (trie.numNodes() == 1);
    CPPUNIT_ASSERT(ok);

    Trie::StrKey k;
    ok = (trie.countKvPairs(k) == 0);
    CPPUNIT_ASSERT(ok);

    const unsigned char* p = 0;
    ok = (trie.countKvPairs(p) == 0);
    CPPUNIT_ASSERT(ok);
}


void TrieSuite::testFind00()
{
    unsigned char maxDigit = 0xffU;
    Trie trie(maxDigit);

    Trie::StrKey k(Trie::StrKey::Ascii, "abc");
    bool ok = trie.add(k, this);
    CPPUNIT_ASSERT(ok);

    const Trie::Node* found = trie.root();
    found = found->child('a');
    found = found->child('b');
    found = found->child('c');
    ok = (found->v() == this) && (found->numChildren() == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Try U16Key.
//
void TrieSuite::testKey00()
{
    const unsigned char rawKey0[1 + 4] = {4U, 0U, 0U, 0U, 0U};
    Trie::U16Key key;
    bool ok = (memcmp(rawKey0, key.asBytes(), sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);

    const unsigned char rawKey1[1 + 4] = {4U, 1U, 2U, 3U, 4U};
    key = 0x1234U;
    ok = (memcmp(rawKey1, key.asBytes(), sizeof(rawKey1)) == 0);
    CPPUNIT_ASSERT(ok);

    unsigned char rawKey2[1 + 4];
    Trie::U16Key::encode(rawKey2, 0x1234U);
    ok = (Trie::U16Key::decode(rawKey2) == 0x1234U);
    CPPUNIT_ASSERT(ok);
    ok = (Trie::U16Key::decode(key) == 0x1234U);
    CPPUNIT_ASSERT(ok);

    key = 0;
    key.reset(0x1234U);
    ok = (memcmp(rawKey1, key.asBytes(), sizeof(rawKey1)) == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Try U28Key.
//
void TrieSuite::testKey01()
{
    const unsigned char rawKey0[1 + 7] = {7U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
    Trie::U28Key key;
    bool ok = (memcmp(rawKey0, key.asBytes(), sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);

    const unsigned char rawKey1[1 + 7] = {7U, 1U, 2U, 3U, 4U, 5U, 6U, 7U};
    key = 0x01234567U;
    ok = (memcmp(rawKey1, key.asBytes(), sizeof(rawKey1)) == 0);
    CPPUNIT_ASSERT(ok);

    unsigned char rawKey2[1 + 7];
    Trie::U28Key::encode(rawKey2, 0x01234567U);
    ok = (Trie::U28Key::decode(rawKey2) == 0x01234567U);
    CPPUNIT_ASSERT(ok);
    ok = (Trie::U28Key::decode(key) == 0x01234567U);
    CPPUNIT_ASSERT(ok);

    key = 0;
    key.reset(0x01234567U);
    ok = (memcmp(rawKey1, key.asBytes(), sizeof(rawKey1)) == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Try U32Key.
//
void TrieSuite::testKey02()
{
    const unsigned char rawKey0[1 + 8] = {8U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
    Trie::U32Key key;
    bool ok = (memcmp(rawKey0, key.asBytes(), sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);

    const unsigned char rawKey1[1 + 8] = {8U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U};
    key = 0x12345678U;
    ok = (memcmp(rawKey1, key.asBytes(), sizeof(rawKey1)) == 0);
    CPPUNIT_ASSERT(ok);

    unsigned char rawKey2[1 + 8];
    Trie::U32Key::encode(rawKey2, 0x12345678U);
    ok = (Trie::U32Key::decode(rawKey2) == 0x12345678U);
    CPPUNIT_ASSERT(ok);
    ok = (Trie::U32Key::decode(key) == 0x12345678U);
    CPPUNIT_ASSERT(ok);

    key = 0;
    key.reset(0x12345678U);
    ok = (memcmp(rawKey1, key.asBytes(), sizeof(rawKey1)) == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Try U56Key.
//
void TrieSuite::testKey03()
{
    const unsigned char rawKey0[1 + 7] = {7U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U};
    Trie::U56Key key;
    bool ok = (memcmp(rawKey0, key.asBytes(), sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);

    const unsigned char rawKey1[1 + 7] = {7U, 0x12U, 0x34U, 0x56U, 0x78U, 0x9aU, 0xbcU, 0xdeU};
    key = 0x00123456789abcdeULL;
    ok = (memcmp(rawKey1, key.asBytes(), sizeof(rawKey1)) == 0);
    CPPUNIT_ASSERT(ok);

    unsigned char rawKey2[1 + 7];
    Trie::U56Key::encode(rawKey2, 0x00123456789abcdeULL);
    ok = (Trie::U56Key::decode(rawKey2) == 0x00123456789abcdeULL);
    CPPUNIT_ASSERT(ok);
    ok = (Trie::U56Key::decode(key) == 0x00123456789abcdeULL);
    CPPUNIT_ASSERT(ok);

    key = 0;
    key.reset(0x00123456789abcdeULL);
    ok = (memcmp(rawKey1, key.asBytes(), sizeof(rawKey1)) == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Try U64Key.
//
void TrieSuite::testKey04()
{
    const unsigned char rawKey0[1 + 8] = {8U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U};
    Trie::U64Key key;
    bool ok = (memcmp(rawKey0, key.asBytes(), sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);

    const unsigned char rawKey1[1 + 8] = {8U, 0x12U, 0x34U, 0x56U, 0x78U, 0x9aU, 0xbcU, 0xdeU, 0xf0U};
    key = 0x123456789abcdef0ULL;
    ok = (memcmp(rawKey1, key.asBytes(), sizeof(rawKey1)) == 0);
    CPPUNIT_ASSERT(ok);

    unsigned char rawKey2[1 + 8];
    Trie::U64Key::encode(rawKey2, 0x123456789abcdef0ULL);
    ok = (Trie::U64Key::decode(rawKey2) == 0x123456789abcdef0ULL);
    CPPUNIT_ASSERT(ok);
    ok = (Trie::U64Key::decode(key) == 0x123456789abcdef0ULL);
    CPPUNIT_ASSERT(ok);

    key = 0;
    key.reset(0x123456789abcdef0ULL);
    ok = (memcmp(rawKey1, key.asBytes(), sizeof(rawKey1)) == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Try StrKey -- ASCII form.
//
void TrieSuite::testKey05()
{
    Trie::StrKey key0;
    bool ok = (*key0.asBytes() == 0);
    CPPUNIT_ASSERT(ok);

    const unsigned char rawKey0[1 + 3] = {3U, 'a', 'b', 'c'};
    key0.reset(Trie::StrKey::Ascii, "abc");
    ok = (memcmp(rawKey0, key0.asBytes(), sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);

    char k0[3 + 1] = "abc";
    unsigned char* rawKey1 = Trie::StrKey::encode(Trie::StrKey::Ascii, k0);
    ok = (reinterpret_cast<char*>(rawKey1) == k0);
    CPPUNIT_ASSERT(ok);
    ok = (memcmp(rawKey1, rawKey0, sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);

    unsigned char rawKey2[1 + 3];
    ok = (Trie::StrKey::encode(rawKey2, Trie::StrKey::Ascii, "abc") == rawKey2);
    CPPUNIT_ASSERT(ok);
    ok = (memcmp(rawKey2, rawKey0, sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);
    char* k1 = Trie::StrKey::decode(Trie::StrKey::Ascii, rawKey2);
    ok = (reinterpret_cast<unsigned char*>(k1) == rawKey2);
    CPPUNIT_ASSERT(ok);
    ok = (strcmp(k1, "abc") == 0);
    CPPUNIT_ASSERT(ok);
    ok = (Trie::StrKey::encode(rawKey2, Trie::StrKey::Ascii, "abc123", 3) == rawKey2);
    CPPUNIT_ASSERT(ok);
    ok = (memcmp(rawKey2, rawKey0, sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);
    char k2[3 + 1];
    ok = (Trie::StrKey::decode(k2, Trie::StrKey::Ascii, rawKey2) == k2);
    CPPUNIT_ASSERT(ok);
    ok = (strcmp(k2, "abc") == 0);
    CPPUNIT_ASSERT(ok);

    Trie::StrKey key1(key0);
    ok = (key1 == key0);
    CPPUNIT_ASSERT(ok);

    Trie::StrKey key2;
    ok = (key2 != key1);
    CPPUNIT_ASSERT(ok);

    key2 = key0;
    ok = (key2 == key0);
    CPPUNIT_ASSERT(ok);
}


//
// Try StrKey -- decimal form.
//
void TrieSuite::testKey06()
{
    const unsigned char rawKey0[1 + 10] = {10U, 0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U};
    Trie::StrKey key0(Trie::StrKey::Dec, "0123456789");
    bool ok = (memcmp(rawKey0, key0.asBytes(), sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);

    key0.reset(Trie::StrKey::Dec, "123456");
    const unsigned char* key = key0.asBytes();
    ok = (key[0] == 6) && (memcmp(rawKey0 + 2, key + 1, 6) == 0);
    CPPUNIT_ASSERT(ok);

    key0.reset(Trie::StrKey::Dec, "789012", 3);
    key = key0.asBytes();
    ok = (key[0] == 3) && (memcmp(rawKey0 + 8, key + 1, 3) == 0);
    CPPUNIT_ASSERT(ok);

    Trie::StrKey key1(Trie::StrKey::Dec, "0123456789", 10);
    ok = (memcmp(rawKey0, key1.asBytes(), sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);

    char k0[10 + 1] = "0123456789";
    unsigned char* rawKey1 = Trie::StrKey::encode(Trie::StrKey::Dec, k0);
    ok = (reinterpret_cast<char*>(rawKey1) == k0);
    CPPUNIT_ASSERT(ok);
    ok = (memcmp(rawKey1, rawKey0, sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);

    unsigned char rawKey2[1 + 10];
    ok = (Trie::StrKey::encode(rawKey2, Trie::StrKey::Dec, "0123456789") == rawKey2);
    CPPUNIT_ASSERT(ok);
    ok = (memcmp(rawKey2, rawKey0, sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);
    char* k1 = Trie::StrKey::decode(Trie::StrKey::Dec, rawKey2);
    ok = (reinterpret_cast<unsigned char*>(k1) == rawKey2);
    CPPUNIT_ASSERT(ok);
    ok = (strcmp(k1, "0123456789") == 0);
    CPPUNIT_ASSERT(ok);
    ok = (Trie::StrKey::encode(rawKey2, Trie::StrKey::Dec, "0123456789abc", 10) == rawKey2);
    CPPUNIT_ASSERT(ok);
    ok = (memcmp(rawKey2, rawKey0, sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);
    char k2[10 + 1];
    ok = (Trie::StrKey::decode(k2, Trie::StrKey::Dec, rawKey2) == k2);
    CPPUNIT_ASSERT(ok);
    ok = (strcmp(k2, "0123456789") == 0);
    CPPUNIT_ASSERT(ok);
}


//
// Try StrKey -- hexadecimal form.
//
void TrieSuite::testKey07()
{
    const unsigned char rawKey0[1 + 22] =
    {
        22U,
        0x00U, 0x01U, 0x02U, 0x03U, 0x04U, 0x05U, 0x06U, 0x07U, 0x08U, 0x09U, 0x0aU,
        0x0aU, 0x0bU, 0x0bU, 0x0cU, 0x0cU, 0x0dU, 0x0dU, 0x0eU, 0x0eU, 0x0fU, 0x0fU
    };
    Trie::StrKey key0(Trie::StrKey::Hex, "0123456789aAbBcCdDeEfF");
    bool ok = (memcmp(rawKey0, key0.asBytes(), sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);

    key0.reset(Trie::StrKey::HEX, "789aAB");
    const unsigned char* key = key0.asBytes();
    ok = (key[0] == 6) && (memcmp(rawKey0 + 8, key + 1, 6) == 0);
    CPPUNIT_ASSERT(ok);

    key0.reset(Trie::StrKey::Hex, "aAbBcCdDeEfF", 5);
    key = key0.asBytes();
    ok = (key[0] == 5) && (memcmp(rawKey0 + 11, key + 1, 5) == 0);
    CPPUNIT_ASSERT(ok);

    Trie::StrKey key1(Trie::StrKey::HEX, "0123456789aAbBcCdDeEfF", 22);
    ok = (memcmp(rawKey0, key1.asBytes(), sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);

    char k0[22 + 1] = "0123456789aAbBcCdDeEfF";
    unsigned char* rawKey1 = Trie::StrKey::encode(Trie::StrKey::Hex, k0);
    ok = (reinterpret_cast<char*>(rawKey1) == k0);
    CPPUNIT_ASSERT(ok);
    ok = (memcmp(rawKey1, rawKey0, sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);

    unsigned char rawKey2[1 + 22];
    ok = (Trie::StrKey::encode(rawKey2, Trie::StrKey::HEX, "0123456789aAbBcCdDeEfF") == rawKey2);
    CPPUNIT_ASSERT(ok);
    ok = (memcmp(rawKey2, rawKey0, sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);
    char* k1 = Trie::StrKey::decode(Trie::StrKey::Hex, rawKey2);
    ok = (reinterpret_cast<unsigned char*>(k1) == rawKey2);
    CPPUNIT_ASSERT(ok);
    ok = (strcmp(k1, "0123456789aabbccddeeff") == 0);
    CPPUNIT_ASSERT(ok);
    ok = (Trie::StrKey::encode(rawKey2, Trie::StrKey::Hex, "0123456789aAbBcCdDeEfFxyz", 22) == rawKey2);
    CPPUNIT_ASSERT(ok);
    ok = (memcmp(rawKey2, rawKey0, sizeof(rawKey0)) == 0);
    CPPUNIT_ASSERT(ok);
    char k2[22 + 1];
    ok = (Trie::StrKey::decode(k2, Trie::StrKey::HEX, rawKey2) == k2);
    CPPUNIT_ASSERT(ok);
    ok = (strcmp(k2, "0123456789AABBCCDDEEFF") == 0);
    CPPUNIT_ASSERT(ok);
}


void TrieSuite::testRm00()
{
    Trie trie;

    void* value = this;
    Trie::StrKey key(Trie::StrKey::Dec, "123");
    trie.add(key, value);
    void* oldValue = 0;
    bool ok = (!trie.add(key, value, oldValue)) && (oldValue == this);
    CPPUNIT_ASSERT(ok);

    // Empty key.
    Trie::StrKey badKey(Trie::StrKey::Dec, "");
    ok = (!trie.rm(0));
    CPPUNIT_ASSERT(ok);
    ok = (!trie.rm(badKey));
    CPPUNIT_ASSERT(ok);

    // Non-existent key. Non-existent node.
    badKey.reset(Trie::StrKey::Dec, "124");
    ok = (!trie.rm(badKey));
    CPPUNIT_ASSERT(ok);

    // Non-existent key. Existent node.
    badKey.reset(Trie::StrKey::Dec, "12");
    ok = (!trie.rm(badKey));
    CPPUNIT_ASSERT(ok);

    // Only child.
    void* removedV;
    ok = (trie.rm(key, removedV) && (removedV == value));
    CPPUNIT_ASSERT(ok);
    ok = (trie.numKvPairs() == 0) && (trie.numNodes() == 1);
    CPPUNIT_ASSERT(ok);
}


void TrieSuite::testRm01()
{
    Trie trie;

    Trie::StrKey key0(Trie::StrKey::Dec, "123");
    trie.add(key0, &key0);
    Trie::StrKey key1(Trie::StrKey::Dec, "123456789");
    trie.add(key1, &key1);
    Trie::StrKey key2(Trie::StrKey::Dec, "12345");
    trie.add(key2, &key2);
    Trie::StrKey key3(Trie::StrKey::Dec, "210");
    trie.add(key3, &key3);

    // Leaf node. W/ morphing.
    void* removedV;
    bool ok = (trie.rm(key1, removedV) && (removedV == &key1));
    CPPUNIT_ASSERT(ok);
    ok = (trie.numKvPairs() == 3) && (trie.numNodes() == 9);
    CPPUNIT_ASSERT(ok);

    // Non-leaf node.
    ok = (trie.rm(key0, removedV) && (removedV == &key0));
    CPPUNIT_ASSERT(ok);
    ok = (trie.numKvPairs() == 2) && (trie.numNodes() == 9);
    CPPUNIT_ASSERT(ok);

    // Leaf node. No morphing.
    ok = (trie.rm(key3, removedV) && (removedV == &key3));
    CPPUNIT_ASSERT(ok);
    ok = (trie.numKvPairs() == 1) && (trie.numNodes() == 6);
    CPPUNIT_ASSERT(ok);
}


void TrieSuite::testRm02()
{
    Trie trie;

    Trie::StrKey key0;
    for (char c = '0'; c <= '9'; ++c)
    {
        char k[3 + 1] = {'1', '2', c, 0};
        key0.reset(Trie::StrKey::Dec, k);
        trie.add(key0, &key0);
    }
    Trie::StrKey key1(Trie::StrKey::Dec, "123456789");
    trie.add(key1, &key1);
    Trie::StrKey key2(Trie::StrKey::Dec, "12345");
    trie.add(key2, &key2);
    Trie::StrKey key3(Trie::StrKey::Dec, "210");
    trie.add(key3, &key3);
    Trie::StrKey key4(Trie::StrKey::Dec, "3");
    trie.add(key4, &key4);

    // Leaf node. W/ morphing.
    bool ok = (trie.rmAll(key1) == 1);
    CPPUNIT_ASSERT(ok);
    ok = (trie.numKvPairs() == 13) && (trie.numNodes() == 19);
    CPPUNIT_ASSERT(ok);

    // Non-leaf node. Full key.
    key0.reset(Trie::StrKey::Dec, "123");
    ok = (trie.rmAll(key0) == 2);
    CPPUNIT_ASSERT(ok);
    ok = (trie.numKvPairs() == 11) && (trie.numNodes() == 16);
    CPPUNIT_ASSERT(ok);

    // Leaf node. No morphing.
    ok = (trie.rmAll(key3) == 1);
    CPPUNIT_ASSERT(ok);
    ok = (trie.numKvPairs() == 10) && (trie.numNodes() == 13);
    CPPUNIT_ASSERT(ok);

    // Non-leaf node. Partial key.
    key0.reset(Trie::StrKey::Dec, "12");
    ok = (trie.rmAll(key0) == 9);
    CPPUNIT_ASSERT(ok);
    ok = (trie.numKvPairs() == 1) && (trie.numNodes() == 2);
    CPPUNIT_ASSERT(ok);

    // No-op. Empty key.
    key0.reset();
    ok = (trie.rmAll(key0) == 0);
    CPPUNIT_ASSERT(ok);
}


void TrieSuite::testRm03()
{
    Trie trie0;

    // Add random items.
    const char* s = "aRandomStringUsedForTriePopulation!!!~@#$%^&*()_+=-`{}|][:;'<>?/.,";
    for (const char* p = s; *p; ++p)
    {
        for (unsigned int u32 = *p; u32 != 0; u32 <<= 1)
        {
            Trie::U32Key key(u32);
            unsigned int* value = new unsigned int(u32);
            if (!trie0.add(key.asBytes(), value))
            {
                delete value;
            }
        }
    }

    bool ok = (trie0.numKvPairs() == 1116);
    CPPUNIT_ASSERT(ok);
    ok = (trie0.countKvPairs(0) == trie0.numKvPairs());
    CPPUNIT_ASSERT(ok);

    // Remove all items. Children first.
    Trie trie(trie0);
    trie0.applyChildFirst(rmKv, &trie);
    ok = (trie.numKvPairs() == 0);
    CPPUNIT_ASSERT(ok);

    // Remove all items. Parent first.
    trie = trie0;
    trie0.applyParentFirst(rmKv, &trie);
    ok = (trie.numKvPairs() == 0);
    CPPUNIT_ASSERT(ok);

    // Remove and delete all items in a slightly random order.
    // All these removal patterns should cover all node configurations.
    const char* p0 = s + 23;
    for (const char* p = p0;;)
    {
        unsigned int u32a = *p << 5;
        for (unsigned int u32 = u32a;;)
        {
            void *removedV;
            Trie::U32Key key(u32);
            if (trie0.rm(key.asBytes(), removedV))
            {
                delete static_cast<unsigned int*>(removedV);
            }
            if ((u32 <<= 1) == 0) { u32 = *p; continue; }
            if (u32 == u32a) break;
        }
        if (*++p == 0) { p = s; continue; }
        if (p == p0) break;
    }
    ok = (trie0.numKvPairs() == 0);
    CPPUNIT_ASSERT(ok);
}


void TrieSuite::testSize00()
{
    bool ok = (sizeof(Trie) == ((sizeof(void*) == 8)? 24: 16)) &&
        (sizeof(Trie::Node0) == sizeof(void*) * 2) &&
        (sizeof(Trie::Node1) == sizeof(void*) * 4) &&
        (sizeof(Trie::Node4) == sizeof(void*) * 7) &&
        (sizeof(Trie::NodeN) == sizeof(void*) * 4);
    CPPUNIT_ASSERT(ok);
}
