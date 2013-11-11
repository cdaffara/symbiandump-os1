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
 * @file rtxStreamBuffered.h 
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXSTREAMBUFFERED_H_
#define _RTXSTREAMBUFFERED_H_

#include "rtxsrc/rtxStream.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OSRTSTRMCM_RESTORE_UNDERLAYING_AFTER_RESET 0x0001

EXTERNRT int rtxStreamBufferedCreate (OSCTXT* pctxt, OSUINT32 mode);

EXTERNRT int rtxStreamBufferedRelease (OSCTXT* pctxt);

EXTERNRT int rtxStreamBufferedSetPreReadBuf 
   (OSCTXT* pctxt, const OSOCTET* pdata, size_t datalen);

EXTERNRT int rtxStreamBufferedPrependReadBuf 
   (OSCTXT* pctxt, const OSOCTET* pdata, size_t datalen);

#ifdef __cplusplus
}
#endif

#endif /* _RTXSTREAMBUFFERED_H_ */

