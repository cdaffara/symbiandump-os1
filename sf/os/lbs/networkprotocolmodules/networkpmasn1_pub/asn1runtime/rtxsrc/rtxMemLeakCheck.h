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

/** 
 * @file rtxMemLeakCheck.h 
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXMEMLEAKCHECK_H_
#define _RTXMEMLEAKCHECK_H_

/* VC++ and _DEBUG/_TRACE only */
#if defined (_MSC_VER) && defined (_DEBUG) && defined (_TRACE)
#include <crtdbg.h>
#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"

#define DECLARE_MEMLEAK_DETECTOR0 { \
int LeakTmpFlag; \
_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE); \
_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT); \
_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE); \
_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT); \
_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE); \
_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT); \
LeakTmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); \
LeakTmpFlag |= _CRTDBG_CHECK_ALWAYS_DF; \
LeakTmpFlag |= _CRTDBG_LEAK_CHECK_DF;   \
LeakTmpFlag |= _CRTDBG_DELAY_FREE_MEM_DF; \
_CrtSetDbgFlag(LeakTmpFlag); \
}

#ifdef __cplusplus
#define DECLARE_MEMLEAK_DETECTOR static class ___MemLeakDetector___ { \
public: \
___MemLeakDetector___ () DECLARE_MEMLEAK_DETECTOR0 \
} ___memleakdetector__
#else
#define DECLARE_MEMLEAK_DETECTOR DECLARE_MEMLEAK_DETECTOR0
#endif /* __cplusplus */

#else /* NOT VC++ OR NOT _DEBUG OR NOT _TRACE */
#define DECLARE_MEMLEAK_DETECTOR 
#endif /* _MSC_VER */

#endif /* _RTXMEMLEAKCHECK_H_ */
