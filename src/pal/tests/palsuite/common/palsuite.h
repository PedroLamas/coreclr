//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information. 
//

/*============================================================================
**
** Source:  palsuite.h
**
** Purpose: Define constants and implement functions that are useful to
**          multiple function categories. If common functions are useful
**          only amongst the test cases for a particular function, a separate
**          header file is placed in the root of those test cases.
**
**
**==========================================================================*/

#ifndef __PALSUITE_H__
#define __PALSUITE_H__

#include <pal_assert.h>
#include <pal.h>

enum
{
    PASS = 0,
    FAIL = 1
};


void Trace(const char *format, ...)
{
    va_list arglist;
	
    va_start(arglist, format);

    vprintf(format, arglist);

    va_end(arglist);
}

void Fail(const char *format, ...)
{
    va_list arglist;
	
    va_start(arglist, format);

    vprintf(format, arglist);    

    va_end(arglist);
    printf("\n");

    // This will exit the test process
    PAL_TerminateEx(FAIL);
}

#ifdef PAL_PERF

int __cdecl Test_Main(int argc, char **argv);
int PAL_InitializeResult = 0;
static const char PALTEST_LOOP_ENV[]="PALTEST_LOOP_COUNT";

int __cdecl main(int argc, char **argv)
{
  int lastMainResult=0;

  int loopCount=1; // default: run the test's main once
  int loopIndex=0;
  char *szPerfLoopEnv = NULL;

  // Run PAL_Initialize once, save off the result. Any failures here
  // will be detected later by calls to PAL_Initialize in the test's main.
  PAL_InitializeResult = PAL_Initialize(argc, argv);

  // Check the environment to see if we need to run the test's main
  // multiple times. Ideally, we want to do this before PAL_Initialize so
  // that the overhead of checking the environment is not included in the
  // time between PAL_Initialize and PAL_Terminate. However, getenv in PAL
  // can be run only after PAL_Initialize.
  szPerfLoopEnv = getenv(PALTEST_LOOP_ENV);
  if (szPerfLoopEnv != NULL) 
  {
     loopCount = atoi(szPerfLoopEnv);
     if (loopCount <= 0) loopCount = 1;
  }

  // call the test's actual main in a loop
  for(loopIndex=0; loopIndex<loopCount; loopIndex++) {
      lastMainResult = Test_Main(argc, argv);
  }

  // call PAL_Terminate for real
  PAL_TerminateEx(lastMainResult);

  return lastMainResult;
}

// Test's calls to PAL_Initialize and PAL_Terminate are redirected
// to these bogus functions. These rely on PAL_Initialize and PAL_Terminate
// being called by the 'main' above.
#define PAL_Initialize(a, b) Bogus_PAL_Initialize(a, b)
#define PAL_Terminate()  Bogus_PAL_Terminate()
int Bogus_PAL_Initialize(int argc, char* argv[])
{
  // PAL_Initialize has already been called by the real main.
  // Just return the result.
  return PAL_InitializeResult;
}

void Bogus_PAL_Terminate()
{
  // Don't call PAL_Terminate. It will be called later by the
  // real main.
  return;
}

// Rename the main provided by the test case
#define main Test_Main

#endif // PAL_PERF

#ifdef BIGENDIAN
inline ULONG   VAL32(ULONG x)
{
    return( ((x & 0xFF000000L) >> 24) |               
            ((x & 0x00FF0000L) >>  8) |              
            ((x & 0x0000FF00L) <<  8) |              
            ((x & 0x000000FFL) << 24) );
}
#define th_htons(w)  (w)
#else   // BIGENDIAN
#define VAL32(x)    (x)
#define th_htons(w)  (((w) >> 8) | ((w) << 8))
#endif  // BIGENDIAN



WCHAR* convert(char * aString) 
{
    int size;
    WCHAR* wideBuffer;

    size = MultiByteToWideChar(CP_ACP,0,aString,-1,NULL,0);
    wideBuffer = (WCHAR*) malloc(size*sizeof(WCHAR));
    if (wideBuffer == NULL)
    {
        Fail("ERROR: Unable to allocate memory!\n");
    }
    MultiByteToWideChar(CP_ACP,0,aString,-1,wideBuffer,size);
    return wideBuffer;
}

char* convertC(WCHAR * wString) 
{
    int size;
    char * MultiBuffer = NULL;

    size = WideCharToMultiByte(CP_ACP,0,wString,-1,MultiBuffer,0,NULL,NULL);
    MultiBuffer = (char*) malloc(size);
    if (MultiBuffer == NULL)
    {
        Fail("ERROR: Unable to allocate memory!\n");
    }
    WideCharToMultiByte(CP_ACP,0,wString,-1,MultiBuffer,size,NULL,NULL);
    return MultiBuffer;
}

UINT64 GetHighPrecisionTimeStamp(LARGE_INTEGER performanceFrequency)
{
    LARGE_INTEGER ts;
    if (!QueryPerformanceCounter(&ts))
    {
        Fail("ERROR: Unable to query performance counter!\n");      
    }
    
    return ts.QuadPart / (performanceFrequency.QuadPart / 1000);    
}

#endif



