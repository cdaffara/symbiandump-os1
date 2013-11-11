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


/* Benchmark utility functions */

/**
 * @file rtbench.h
 */

/**
@file
@internalTechnology
*/

#ifndef _BENCH_H_
#define _BENCH_H_

#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32_WCE
#include <time.h>
#ifndef _NUCLEUS
#include <sys/stat.h>
#endif
#else
#include "rtxsrc/wceAddon.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ITERCNT 100000

double rtBenchAverageMS (clock_t start, clock_t finish, double icnt);

#ifdef __cplusplus
}
#endif
#endif
