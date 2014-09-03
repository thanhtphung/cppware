#include "appkit/DelimitedTxt.hpp"
#include "appkit/StringDic.hpp"
#include "appkit/StringVec.hpp"

#include "appkit-ut-pch.h"
#include "StringDicSuite.hpp"

using namespace appkit;

const char COLON = ':';
const char COMMA = ',';
const char EQUALS_SIGN = '=';
const char ITEM[] = "aRandomStringUsedForStringDicPopulation!!!";
const char NEW_LINE = '\n';
const char NEW_LINE_STRING[] = "\n";
const char SAMPLE0[] = "a=1" "\n" "bb=22" "\n" "ccc=333" "\n" "dddd=4444" "\n" "eeeee=55555" "\n";

BEGIN_NAMESPACE


class Sample0: public StringDic
{
public:
    Sample0();
    ~Sample0();
private:
    Sample0(const Sample0&); //prohibit usage
    const Sample0& operator =(const Sample0&); //prohibit usage
};

Sample0::Sample0():
StringDic(false /*ignoreCase*/)
{
    String k;
    String v;
    for (const char* p = ITEM; *p; ++p)
    {
        size_t count = (*p % 7) + 1;
        k.reset(count, *p);
        count = *p;
        v.reset(count, *p);
        add(k, v);
    }
}

Sample0::~Sample0()
{
}

END_NAMESPACE


StringDicSuite::StringDicSuite()
{
}


StringDicSuite::~StringDicSuite()
{
}


//
// Check if value can be updated.
// Callback is expected to be invoked for each key-value pair.
//
bool StringDicSuite::cb0a(void* arg, const String& k, String& v)
{
    v = k;
    const StringDic* dic = static_cast<const StringDic*>(arg);
    bool keepGoing = ((*dic)[k] == k);
    return keepGoing;
}


//
// Check if all key-value pairs are visited.
// Callback is expected to be invoked for each key-value pair.
//
bool StringDicSuite::cb1a(void* arg, const String& k, const String& v)
{
    StringDic* dic = static_cast<StringDic*>(arg);
    bool keepGoing = dic->add(k, v);
    return keepGoing;
}


//
// Add random key-value pairs.
//
void StringDicSuite::testAdd00()
{
    Sample0 dic0;

    bool ignoreCase = false;
    StringDic dic1(ignoreCase);
    StringDic dic2(ignoreCase);

    String k;
    String v;
    String oldV;
    bool ok = true;
    for (const char* p = ITEM; *p; ++p)
    {
        size_t count = (*p % 7) + 1;
        k.reset(count, *p);
        if (!dic1.contains(k))
        {
            count = *p;
            v.reset(count, *p);
            if ((!dic1.add(k, v)) || (dic1[k] != v) || (!dic2.associate(k, v, oldV)) || (dic2.getValue(k) != v) || (!oldV.empty()))
            {
                ok = false;
                break;
            }
        }
    }
    CPPUNIT_ASSERT(ok);

    ok = (dic1 == dic2) && dic1.contains(dic2);
    CPPUNIT_ASSERT(ok);
    ok = (dic1 == dic0) && dic1.contains(dic0);
    CPPUNIT_ASSERT(ok);

    StringDic added;
    StringDic updated;
    StringDic removed;
    dic1.compare(dic2, &added, &updated, 0 /*removed*/);
    ok = (added.numKvPairs() == 0) && (updated.numKvPairs() == 0);
    CPPUNIT_ASSERT(ok);
    dic1.compare(dic2, &added, &updated, &removed);
    ok = (added.numKvPairs() == 0) && (updated.numKvPairs() == 0) && (removed.numKvPairs() == 0);
    CPPUNIT_ASSERT(ok);

    ok = dic1.rm(dic0) && (dic1.numKvPairs() == 0);
    CPPUNIT_ASSERT(ok);
    ok = (!dic1.rm(dic0));
    CPPUNIT_ASSERT(ok);

    ok = dic2.rm(dic2) && (dic2.numKvPairs() == 0);
    CPPUNIT_ASSERT(ok);
}


void StringDicSuite::testAdd01()
{
    Sample0 dic0;

    // Adding unique keys.
    StringDic dic1;
    bool ok = dic1.add(dic0) && (dic1 == dic0) && dic1.contains(dic0);
    CPPUNIT_ASSERT(ok);

    // No change if adding duplicate keys.
    ok = (!dic1.add(dic0));
    CPPUNIT_ASSERT(ok);
    ok = (!dic1.add(dic1));
    CPPUNIT_ASSERT(ok);

    String k;
    String v;
    dic1.any(k, v);
    ok = dic1.contains(k, v);
    CPPUNIT_ASSERT(ok);
    dic1.rm(k);
    ok = (!dic1.contains(k, v));
    CPPUNIT_ASSERT(ok);

    StringDic added;
    StringDic updated;
    StringDic removed;
    dic1.compare(dic0, &added, 0 /*updated*/, 0 /*removed*/);
    dic1.compare(dic0, 0 /*added*/, &updated, 0 /*removed*/);
    dic1.compare(dic0, 0 /*added*/, 0 /*updated*/, &removed);
    ok = (added.numKvPairs() == 0) && (updated.numKvPairs() == 0) && (removed.numKvPairs() == 1) && removed.contains(k, v);
    CPPUNIT_ASSERT(ok);

    ok = dic1.add(dic0) && (dic1 == dic0) && dic1.contains(dic0) && dic1.contains(dic1);
    CPPUNIT_ASSERT(ok);

    ok = dic1.rm(dic0) && (dic1.numKvPairs() == 0);
    CPPUNIT_ASSERT(ok);
}


void StringDicSuite::testApply00()
{
    Sample0 dic0;
    StringDic dic1(dic0);
    void* arg = &dic1;
    bool ok = dic1.apply(cb0a, arg) && (dic1 != dic0) && (!dic1.contains(dic0));
    CPPUNIT_ASSERT(ok);

    StringDic dic2(dic0);
    StringDic dic3;
    arg = &dic3;
    ok = dic2.apply(cb1a, arg) && (dic3 == dic2) && dic3.contains(dic2);
    CPPUNIT_ASSERT(ok);
}


void StringDicSuite::testAssociate00()
{
    bool ignoreCase = true;
    StringDic dic(ignoreCase);

    String k("k0");
    String v("v0a");
    char delim = COMMA;
    bool kvAdded = dic.associate(k, v, delim);
    bool ok = kvAdded && (dic[k] == v);
    CPPUNIT_ASSERT(ok);

    k = "K0";
    v = "v0b";
    StringDic b4(dic);
    kvAdded = dic.associate(k, v, delim);
    ok = (!kvAdded) && (dic[k] == "v0a,v0b");
    CPPUNIT_ASSERT(ok);
    StringDic updated;
    dic.compare(b4, 0 /*added*/, &updated, 0 /*removed*/);
    ok = (updated.numKvPairs() == 1) && (updated["k0"] == "v0a,v0b");
    CPPUNIT_ASSERT(ok);

    k = "k1";
    v = "v1";
    ok = dic.associate(k, v) && (!dic.associate(k, v)) && (dic[k] == v);
    CPPUNIT_ASSERT(ok);

    k = "k2";
    v = "v2";
    String oldV("xxx");
    ok = dic.associate(k, v, oldV) && (oldV == "xxx") && (dic[k] == v);
    CPPUNIT_ASSERT(ok);
    ok = (!dic.associate(k, v, oldV)) && (oldV == "v2");
    CPPUNIT_ASSERT(ok);
}


void StringDicSuite::testAssociate01()
{
    Sample0 dic0;

    // Associating unique keys.
    StringDic dic1;
    bool ok = dic1.associate(dic0) && (dic1 == dic0) && dic1.contains(dic0);
    CPPUNIT_ASSERT(ok);

    // No change if associating duplicate keys.
    ok = (!dic1.associate(dic0));
    CPPUNIT_ASSERT(ok);

    String k;
    String v;
    dic1.any(k, v);
    dic1.rm(k);
    ok = dic1.associate(dic0) && (dic1 == dic0) && dic1.contains(dic0);
    CPPUNIT_ASSERT(ok);
}


void StringDicSuite::testCtor00()
{
    Sample0 dic0;

    StringDic dic1;
    bool ok = (dic1.numKvPairs() == 0) && dic1.stringify().empty() && (dic1 != dic0) && dic0.contains(dic1);
    CPPUNIT_ASSERT(ok);
    dic1 = dic0;
    ok = (dic1 == dic0) && dic1.contains(dic0);
    CPPUNIT_ASSERT(ok);

    StringDic dic2(dic0);
    ok = (dic2 == dic0) && dic2.contains(dic0);
    CPPUNIT_ASSERT(ok);
}


void StringDicSuite::testCtor01()
{
    DelimitedTxt txt0(SAMPLE0, sizeof(SAMPLE0) - 1, false /*makeCopy*/, NEW_LINE);
    char kvDelim = EQUALS_SIGN;
    bool trimLines = true;
    bool ignoreCase = false;
    StringDic dic0(txt0, kvDelim, trimLines, ignoreCase);
    bool ok = (dic0.numKvPairs() == 5);
    CPPUNIT_ASSERT(ok);

    String s(dic0.stringify(kvDelim, NEW_LINE_STRING));
    ok = (s == SAMPLE0);
    CPPUNIT_ASSERT(ok);

    StringDic dic1;
    ok = dic1.reset(txt0, kvDelim, trimLines) && (dic1 == dic0) && dic1.contains(dic0);
    CPPUNIT_ASSERT(ok);
    dic1.reset();
    kvDelim = COLON;
    ok = (!dic1.reset(txt0, kvDelim, trimLines)) && (dic1.numKvPairs() == 0);
    CPPUNIT_ASSERT(ok);

    String k("a");
    String v("value-to-be-overwritten");
    dic1.add(k, v);
    dic1 += dic0;
    ok = (dic1 == dic0) && dic1.contains(dic0);
    CPPUNIT_ASSERT(ok);

    // Merge key-value pairs into a non-empty dictionary.
    dic1.reset();
    k = "ccc";
    v = "value-to-be-overwritten";
    dic1.associate(k, v);
    dic0.mergeInto(dic1);
    ok = (dic0.numKvPairs() == 0) && (dic1.stringify() == SAMPLE0);
    CPPUNIT_ASSERT(ok);

    // Merge key-value pairs into an empty compatible/incompatible dictionary.
    dic1.mergeInto(dic0);
    ok = (dic1.numKvPairs() == 0) && (dic0.stringify() == SAMPLE0);
    CPPUNIT_ASSERT(ok);
    ignoreCase = true;
    StringDic dic2(ignoreCase);
    dic0.mergeInto(dic2);
    ok = (dic0.numKvPairs() == 0) && (dic2.stringify() == SAMPLE0);
    CPPUNIT_ASSERT(ok);
}


void StringDicSuite::testCtor02()
{
    Sample0 dic0;

    StringDic dic1(dic0);
    StringDic dic2(&dic1);
    bool ok = (dic2 == dic0) && (dic1.numKvPairs() == 0);
    CPPUNIT_ASSERT(ok);

    dic2 = &dic2; //no-op
    ok = (dic2 == dic0);
    CPPUNIT_ASSERT(ok);

    dic1 = &dic2;
    ok = (dic1 == dic0) && (dic2.numKvPairs() == 0);
    CPPUNIT_ASSERT(ok);
}


void StringDicSuite::testOp00()
{
    Sample0 dic0;

    StringDic dic1(dic0);
    String k("not-a-key");
    String v("not-a-value");
    dic1.any(k, v);
    bool ok = (dic1[k] == v) && dic1.rm(k) && (dic1 != dic0) && (dic0 != dic1) && dic0.contains(dic1) && (!dic1.contains(dic0));
    CPPUNIT_ASSERT(ok);
    dic1["new-key"] = "new-value";
    ok = (dic1["new-key"] == "new-value") && (dic1 != dic0) && (dic0 != dic1) && (!dic0.contains(dic1)) && (!dic1.contains(dic0));
    CPPUNIT_ASSERT(ok);

    StringDic added;
    StringDic updated;
    StringDic removed;
    dic1.compare(dic0, &added, &updated, &removed);
    ok = (added.numKvPairs() == 1) && added.contains(String("new-key")) &&
        (updated.numKvPairs() == 0) &&
        (removed.numKvPairs() == 1) && removed.contains(k, v);
    CPPUNIT_ASSERT(ok);

    dic1.reset();
    dic1.any(k, v);
    ok = k.empty() && v.empty();
    CPPUNIT_ASSERT(ok);
}


void StringDicSuite::testOp01()
{
    bool ignoreCase = false;
    StringDic dic0(ignoreCase);
    dic0["new-key"] = "new-value";
    dic0["NEW-KEY"] = "new-value";

    ignoreCase = true;
    StringDic dic1(ignoreCase);
    dic1 = dic0;
    bool ok = (dic1.numKvPairs() < dic0.numKvPairs());
    CPPUNIT_ASSERT(ok);
}


void StringDicSuite::testRm00()
{
    Sample0 dic0;
    StringVec keys;
    StringVec values(1U /*capacity*/, -1 /*growBy*/);
    dic0.vectorize(keys, values);

    StringDic dic1(dic0);

    // First key-value pair.
    String k(keys[0]);
    bool ok = dic1.rm(k) && (!dic1.find(k));
    CPPUNIT_ASSERT(ok);

    // Last key-value pair.
    k = keys[keys.numItems() - 1];
    String removedV;
    ok = dic1.rm(k, removedV) && (!dic1.find(k)) && (removedV == values[values.numItems() - 1]);
    CPPUNIT_ASSERT(ok);

    // Non-existent key-value pair.
    k = "non-existent";
    ok = (!dic1.rm(k));
    CPPUNIT_ASSERT(ok);

    dic1 = dic0;
    ok = dic1.rm(keys) && (dic1.numKvPairs() == 0);
    CPPUNIT_ASSERT(ok);
    ok = (!dic1.rm(keys));
    CPPUNIT_ASSERT(ok);
}


void StringDicSuite::testRm01()
{
    Sample0 dic0;
    StringVec keys;
    StringVec values(1U /*capacity*/, -1 /*growBy*/);
    dic0.vectorize(keys, values);

    StringDic dic1(dic0);

    // First key-value pair.
    String k(keys[0]);
    bool ok = dic1.rm(k.ascii()) && (!dic1.find(k));
    CPPUNIT_ASSERT(ok);

    // Last key-value pair.
    k = keys[keys.numItems() - 1];
    String removedV;
    ok = dic1.rm(k.ascii(), removedV) && (!dic1.find(k)) && (removedV == values[values.numItems() - 1]);
    CPPUNIT_ASSERT(ok);

    // Non-existent key-value pair.
    k = "non-existent";
    ok = (!dic1.rm(k.ascii()));
    CPPUNIT_ASSERT(ok);
    removedV.reset();
    ok = (!dic1.rm(k.ascii(), removedV)) && removedV.empty();
    CPPUNIT_ASSERT(ok);
}
