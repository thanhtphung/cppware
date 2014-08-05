/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_MACROS_H
#define SYSKIT_MACROS_H

#ifndef _WINNT_
typedef void* HANDLE;
#endif

#ifdef BEGIN_NAMESPACE
#undef BEGIN_NAMESPACE
#endif
#define BEGIN_NAMESPACE \
namespace\
{

#ifdef BEGIN_NAMESPACE1
#undef BEGIN_NAMESPACE1
#endif
#define BEGIN_NAMESPACE1(namespace0)\
namespace namespace0\
{

#ifdef BEGIN_NAMESPACE2
#undef BEGIN_NAMESPACE2
#endif
#define BEGIN_NAMESPACE2(namespace0,namespace1)\
namespace namespace0\
{\
namespace namespace1\
{

// forward declare a namespaced class
// DECLARE_CLASS1(n,c) --> namespace n {class c;}
#ifdef DECLARE_CLASS1
#undef DECLARE_CLASS1
#endif
#define DECLARE_CLASS1(namespace0,className)\
namespace namespace0 { class className; }

// forward declare a namespaced class
// DECLARE_CLASS2(n0,n1,c) --> namespace n0 {namespace n1 {class c;}}
#ifdef DECLARE_CLASS2
#undef DECLARE_CLASS2
#endif
#define DECLARE_CLASS2(namespace0,namespace1,className)\
namespace namespace0 { namespace namespace1 { class className; } }

// forward declare a namespaced struct
// DECLARE_STRUCT1(n,s) --> namespace n {struct s;}
#ifdef DECLARE_STRUCT1
#undef DECLARE_STRUCT1
#endif
#define DECLARE_STRUCT1(namespace0,structName)\
namespace namespace0 { struct structName; }

// forward declare a namespaced struct
// DECLARE_STRUCT2(n0,n1,s) --> namespace n0 {namespace n1 {struct s;}}
#ifdef DECLARE_STRUCT2
#undef DECLARE_STRUCT2
#endif
#define DECLARE_STRUCT2(namespace0,namespace1,structName)\
namespace namespace0 { namespace namespace1 { struct structName; } }

#ifdef END_NAMESPACE
#undef END_NAMESPACE
#endif
#define END_NAMESPACE };

#ifdef END_NAMESPACE1
#undef END_NAMESPACE1
#endif
#define END_NAMESPACE1 };

#ifdef END_NAMESPACE2
#undef END_NAMESPACE2
#endif
#define END_NAMESPACE2 } }

// stringify given argument
// STRINGIFY(abc) --> "abc"
#ifdef STRINGIFY
#undef STRINGIFY
#endif
#define STRINGIFY(x) #x

#endif
