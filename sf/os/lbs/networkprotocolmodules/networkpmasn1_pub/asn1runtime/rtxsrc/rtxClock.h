// Copyright (c) 2001-2009 Objective Systems, Inc. (http://www.obj-sys.com) 
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available 
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Objective Systems, Inc - initial contribution.
//
// Contributors:
//
// Description:
// 

/* Defines a means of performing timing tests.  The macros may be used as 
 * follows:
 *
 * DECLARE_BENCHMARK
 *
 * ...
 *
 * BEGIN_LOOP
 *    // code to be timed goes here
 * END_LOOP
 *
 * PRINT_RESULTS_MS
 *
 * Convenience macros to start and stop the performance clock are also 
 * provided in case a loop is not necessary.  In this case, the ITERCNT
 * value should be redefined to the number of cases being tested. 
 *
 * These timers should be reasonable accurate within microsecond resolution,
 * but results will vary from platform to platform.  On GNU/Linux systems,
 * it is necessary to link against librt.a. 
 *
 * If a program is terminated unexpectedly, results may fail to print since
 * the output buffer may not be flushed.  If exception handling is used,
 * make sure to call fflush(stdout) or the appropriate call for your 
 * platform. */


/**
@file
@internalTechnology
*/

#ifndef __RTXCLOCK_H__
#define __RTXCLOCK_H__

#include <cstdio>

#define ITERCNT 100000L

#ifndef MSC_VER      /* not MSVC++; does this work for Borland? */

#include <time.h>

#define DECLARE_BENCHMARK \
   timespec tp0, tp1; \
   long __clk__ = 0;

#define CLOCK_START \
   clock_gettime(CLOCK_REALTIME, &tp0);

#define CLOCK_STOP \
   clock_gettime(CLOCK_REALTIME, &tp1);

#define BEGIN_LOOP \
   CLOCK_START \
   for (__clk__ = 0; __clk__ < ITERCNT; __clk__ ++) {

#define END_LOOP \
   } \
   CLOCK_STOP

#define PRINT_RESULTS_MS \
   long _ds = tp1.tv_sec - tp0.tv_sec, \
        _dn = tp1.tv_nsec - tp0.tv_nsec; \
   double _dms = (_ds * 1000.f) + (_dn / 1e6); \
   printf ("\t%.6f\t%.6f\n", _dms, _dms/(float)ITERCNT); 

#else                /* MSVC++ */

#include <windows.h>

#define DECLARE_BENCHMARK \
   LARGE_INTEGER _start, _stop, _freq; \
   long __clk__ = 0; \
   QueryPerformanceFrequency(&_freq);

#define CLOCK_START \
   QueryPerformanceCounter(&_start);

#define CLOCK_STOP \
   QueryPerformanceCounter(&_stop);

#define BEGIN_LOOP \
   CLOCK_START \
   for (__clk__ = 0; __clk__ < ITERCNT; __clk__ ++) {

#define END_LOOP \
   } \
   CLOCK_END

#define PRINT_RESULTS_MS \
   double _delta = (_stop.QuadPart - _start.QuadPart), \
          _total = _delta / (double)_freq.QuadPart * 1000; \
   printf("\t%.6f\t%.6f\n", _total, _total/(float)ITERCNT);

#endif

#endif /* DEFINE __RTXCLOCK_H__ */

