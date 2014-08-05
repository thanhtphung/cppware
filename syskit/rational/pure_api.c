/*
 * Header file of Pure API function declarations.
 *
 * Explicitly no copyright.
 * You may recompile and redistribute these definitions as required.
 *
 * NOTE1: In some situations when compiling with MFC, you should
 *        enable the setting 'Not using precompiled headers' in Visual C++
 *        to avoid a compiler diagnostic.
 *
 * NOTE2: This file works through the use of deep magic.  Calls to functions
 *        in this file are replaced with calls into the OCI runtime system
 *        when an instrumented version of this program is run.
 *
 * NOTE3: The static vars avoidGy_n (where n is a unique number) are used
 *        to prevent optimizing the functions away when compiler option
 *        /Gy is set. This is needed so that the deep magic of NOTE2 works 
 *        properly.
 */
#pragma once
#if _DEBUG && _WIN32
#ifndef _M_X64
typedef unsigned int size_t;
#endif
typedef unsigned short wchar_t;
static int avoidGy_1 = 0;
static int avoidGy_2 = 0;
static int avoidGy_3 = 0;
static int avoidGy_4 = 0;
static int avoidGy_5 = 0;
static int avoidGy_6 = 0;
static int avoidGy_7 = 0;
static int avoidGy_8 = 0;
static int avoidGy_9 = 0;
static int avoidGy_10 = 0;
static int avoidGy_11 = 0;
static int avoidGy_12 = 0;
static int avoidGy_13 = 0;
static int avoidGy_14 = 0;
static int avoidGy_15 = 0;
static int avoidGy_16 = 0;
static int avoidGy_17 = 0;
static int avoidGy_18 = 0;
static int avoidGy_19 = 0;
static int avoidGy_20 = 0;
static int avoidGy_21 = 0;
static int avoidGy_22 = 0;
static int avoidGy_23 = 0;
static int avoidGy_24 = 0;
static int avoidGy_25 = 0;
static int avoidGy_26 = 0;
static int avoidGy_27 = 0;
static int avoidGy_28 = 0;
static int avoidGy_29 = 0;
static int avoidGy_30 = 0;
static int avoidGy_31 = 0;
static int avoidGy_32 = 0;
static int avoidGy_33 = 0;
static int avoidGy_34 = 0;
static int avoidGy_35 = 0;
static int avoidGy_36 = 0;
static int avoidGy_37 = 0;
static int avoidGy_38 = 0;
static int avoidGy_39 = 0;
static int avoidGy_40 = 0;
static int avoidGy_41 = 0;
static int avoidGy_42 = 0;
static int avoidGy_43 = 0;
static int avoidGy_44 = 0;
static int avoidGy_45 = 0;
static int avoidGy_46 = 0;
static int avoidGy_47 = 0;
static int avoidGy_48 = 0;
static int avoidGy_49 = 0;
static int avoidGy_50 = 0;
static int avoidGy_51 = 0;
static int avoidGy_52 = 0;
static int avoidGy_53 = 0;
static int avoidGy_54 = 0;
static int avoidGy_55 = 0;
static int avoidGy_56 = 0;
static int avoidGy_57 = 0;
static int avoidGy_58 = 0;
static int avoidGy_59 = 0;
static int avoidGy_60 = 0;
static int avoidGy_61 = 0;
static int avoidGy_62 = 0;
static int avoidGy_63 = 0;
static int avoidGy_64 = 0;
static int avoidGy_65 = 0;
static int avoidGy_66 = 0;
static int avoidGy_67 = 0;
static int avoidGy_68 = 0;
static int avoidGy_69 = 0;
static int avoidGy_70 = 0;
static int avoidGy_71 = 0;
static int avoidGy_72 = 0;
static int avoidGy_73 = 0;
static int avoidGy_74 = 0;
static int avoidGy_75 = 0;
static int avoidGy_76 = 0;
static int avoidGy_77 = 0;
static int avoidGy_PL_01 = 0;
static int avoidGy_PL_02 = 0;
int __cdecl PurePrintf(const char *fmt, ...) { if(!++avoidGy_1); fmt; return 0; }
int __cdecl PurifyIsRunning(void) { if(!++avoidGy_2); return 0; }
int __cdecl PurifyPrintf(const char *fmt, ...) { if(!++avoidGy_3); fmt; return 0; }
size_t __cdecl PurifyNewInuse(void) { if(!++avoidGy_4); return 0; }
size_t __cdecl PurifyAllInuse(void) { if(!++avoidGy_5); return 0; }
size_t __cdecl PurifyClearInuse(void) { if(!++avoidGy_6); return 0; }
size_t __cdecl PurifyNewLeaks(void) { if(!++avoidGy_7); return 0; }
size_t __cdecl PurifyAllLeaks(void) { if(!++avoidGy_8); return 0; }
size_t __cdecl PurifyClearLeaks(void) { if(!++avoidGy_9); return 0; }
size_t __cdecl PurifyAllHandlesInuse(void) { if(!++avoidGy_10); return 0; }
size_t __cdecl PurifyNewHandlesInuse(void) { if(!++avoidGy_11); return 0; }
size_t __cdecl PurifyDescribe(void *addr) { if(!++avoidGy_12); addr; return 0; }
int __cdecl PurifyWhatColors(void *addr, size_t size) { if(!++avoidGy_13); addr; size; return 0; }
int __cdecl PurifyAssertIsReadable(const void *addr, size_t size) { if(!++avoidGy_14); addr; size; return 1; }
int __cdecl PurifyAssertIsWritable(const void *addr, size_t size) { if(!++avoidGy_15); addr; size; return 1; }
int __cdecl PurifyIsReadable(const void *addr, size_t size) { if(!++avoidGy_16); addr; size; return 1; }
int __cdecl PurifyIsWritable(const void *addr, size_t size) { if(!++avoidGy_17); addr; size; return 1; }
int __cdecl PurifyIsInitialized(const void *addr, size_t size) { if(!++avoidGy_18); addr; size; return 1; }
int __cdecl PurifyRed(void *addr, size_t size) { if(!++avoidGy_19); addr; size; return 0; }
int __cdecl PurifyGreen(void *addr, size_t size) { if(!++avoidGy_20); addr; size; return 0; }
int __cdecl PurifyYellow(void *addr, size_t size) { if(!++avoidGy_21); addr; size; return 0; }
int __cdecl PurifyBlue(void *addr, size_t size) { if(!++avoidGy_22); addr; size; return 0; }
int __cdecl PurifyMarkAsInitialized(void *addr, size_t size) { if(!++avoidGy_23); addr; size; return 0; }
int __cdecl PurifyMarkAsUninitialized(void *addr, size_t size) { if(!++avoidGy_24); addr; size; return 0; }
int __cdecl PurifyMarkForTrap(void *addr, size_t size) { if(!++avoidGy_25); addr; size; return 0; }
int __cdecl PurifyMarkForNoTrap(void *addr, size_t size) { if(!++avoidGy_26); addr; size; return 0; }
int __cdecl PurifyHeapValidate(unsigned int hHeap, unsigned int dwFlags, const void *addr) 
 { if(!++avoidGy_27); hHeap; dwFlags; addr; return 1; }
int __cdecl PurifySetLateDetectScanCounter(int counter) { if(!++avoidGy_28); counter; return 0; };
int __cdecl PurifySetLateDetectScanInterval(int seconds) { if(!++avoidGy_29); seconds; return 0; };
size_t __cdecl PurifyPreAlloc(size_t cbRequest, unsigned int dwFlags) { if(!++avoidGy_61); cbRequest; dwFlags; return cbRequest; };
void * __cdecl PurifyPostAlloc(void *pActual, unsigned int dwFlags) { if(!++avoidGy_62); pActual; dwFlags; return pActual; };
void * __cdecl PurifyPreFree(void *pRequest, unsigned int dwFlags) { if(!++avoidGy_63); pRequest; dwFlags; return pRequest; };
void __cdecl PurifyPostFree(unsigned int dwFlags) { if(!++avoidGy_64); dwFlags; return; };
size_t __cdecl PurifyPreRealloc(void *pRequest, size_t cbRequest, void **ppNewRequest, unsigned int dwFlags) { if(!++avoidGy_65); pRequest; cbRequest; ppNewRequest; dwFlags; return cbRequest; };
void * __cdecl PurifyPostRealloc(void *pActual, unsigned int dwFlags) { if(!++avoidGy_66); pActual; dwFlags; return pActual; };
void * __cdecl PurifyPreGetSize(void *pRequest, unsigned int dwFlags) { if(!++avoidGy_67); pRequest; dwFlags; return pRequest; };
size_t __cdecl PurifyPostGetSize(size_t cbActual, unsigned int dwFlags) { if(!++avoidGy_68); cbActual; dwFlags; return cbActual; };
void * __cdecl PurifyPreDidAlloc(void *pRequest, unsigned int dwFlags) { if(!++avoidGy_69); pRequest; dwFlags; return pRequest; };
int __cdecl PurifyPostDidAlloc(void *pRequest, int fActual, unsigned int dwFlags) { if(!++avoidGy_70); pRequest; fActual; dwFlags; return fActual; };
void __cdecl PurifyPreHeapMinimize() { if(!++avoidGy_71); return; };
void __cdecl PurifyPostHeapMinimize() { if(!++avoidGy_72); return; };
void __cdecl PurifySetPoolId(const void *mem, int id) { if(!++avoidGy_73); mem; id; return; };
int __cdecl PurifyGetPoolId(const void *mem) { if(!++avoidGy_74); mem; return 0; };
void __cdecl PurifySetUserData(const void *mem, void *data) { if(!++avoidGy_75); mem; data; return; };
void * __cdecl PurifyGetUserData(const void *mem) { if(!++avoidGy_76); mem; return 0; };
void __cdecl PurifyMapPool(int id, void(*fn)()) { if(!++avoidGy_77); id; fn; return; };
int __cdecl CoverageIsRunning(void) { if(!++avoidGy_30); return 0; }
int __cdecl CoverageDisableRecordingData(void) { if(!++avoidGy_31); return 0; }
int __cdecl CoverageStartRecordingData(void) { if(!++avoidGy_32); return 0; }
int __cdecl CoverageStopRecordingData(void) { if(!++avoidGy_33); return 0; }
int __cdecl CoverageClearData(void) { if(!++avoidGy_34); return 0; }
int __cdecl CoverageIsRecordingData(void) { if(!++avoidGy_35); return 0; }
int __cdecl CoverageAddAnnotation(char *str) { if(!++avoidGy_36); str; return 0; }
int __cdecl CoverageSaveData(void) { if(!++avoidGy_37); return 0; }
int __cdecl QuantifyIsRunning(void) { if(!++avoidGy_42); return 0; }
int __cdecl QuantifyDisableRecordingData(void) { if(!++avoidGy_43); return 0; }
int __cdecl QuantifyStartRecordingData(void) { if(!++avoidGy_44); return 0; }
int __cdecl QuantifyStopRecordingData(void) { if(!++avoidGy_45); return 0; }
int __cdecl QuantifyClearData(void) { if(!++avoidGy_46); return 0; }
int __cdecl QuantifyIsRecordingData(void) { if(!++avoidGy_47); return 0; }
int __cdecl QuantifyAddAnnotation(char *str) { if(!++avoidGy_48); str; return 0; }
int __cdecl QuantifySaveData(void) { if(!++avoidGy_49); return 0; }
int __cdecl QuantifySetThreadName(const char *szName) { if(!++avoidGy_50) ; szName; return 0; }
void __cdecl StartPureRTDebugLogging(void) { if(!++avoidGy_51) ; return; }
void __cdecl StopPureRTDebugLogging(void) { if(!++avoidGy_52) ; return; }
#endif
