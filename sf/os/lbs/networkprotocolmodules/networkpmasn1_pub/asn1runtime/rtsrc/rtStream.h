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

/*
 * This file is deprecated and maintained for backward compatibility only.
 * Definitions in rtxsrc/rtxStream.h should be used for any new 
 * development. 
 */

/**
@file
@internalTechnology
*/

#ifndef _RTSTREAM_H_
#define _RTSTREAM_H_

#include "rtxsrc/rtxStreamFile.h"
#include "rtxsrc/rtxStreamMemory.h"
#include "rtxsrc/rtxStreamSocket.h"
#include "rtsrc/rtExternDefs.h"
#include "rtsrc/asn1compat.h"

#ifdef __cplusplus
extern "C" {
#endif

#define rtStreamRelease rtxStreamRelease
#define rtStreamClose rtxStreamClose
#define rtStreamFlush rtxStreamFlush
#define rtStreamInit  rtxStreamInit
#define rtStreamRead  rxtStreamRead
#define rtStreamBlockingRead  rxtStreamBlockingRead
#define rtStreamSkip  rtxStreamSkip
#define rtStreamWrite rxtStreamWrite
#define rtStreamGetIOBytes rtxStreamGetIOBytes
#define rtStreamMark  rtxStreamMark
#define rtStreamReset rtxStreamReset
#define rtStreamMarkSupported rtxStreamMarkSupported
#define rtStreamIsOpened rtxStreamIsOpened
#define rtStreamIsReadable rtxStreamIsReadable
#define rtStreamIsWritable rtxStreamIsWritable

#define rtStreamBufClose rtxStreamClose
#define rtStreamBufFlush rtxStreamFlush
#define rtStreamBufInit  rtxStreamInit
#define rtStreamBufMark  rtxStreamMark
#define rtStreamBufSkip  rtxStreamSkip
#define rtStreamBufReset rtxStreamReset
#define rtStreamBufWrite rtxStreamWrite

/* special case: return code was changed in rtx to return number of 
   bytes read whereas rt version returned 0 if success */
EXTERNRT int rtStreamBufRead (ASN1CTXT* pctxt, ASN1OCTET* pdata, size_t size);

#define rtStreamFileAttach rtxStreamFileAttach
#define rtStreamFileOpen rtxStreamFileOpen
#define rtStreamFileCreateReader rtxStreamFileCreateReader
#define rtStreamFileCreateWriter rtxStreamFileCreateWriter

#define rtStreamSocketAttach rtxStreamSocketAttach
#define rtStreamSocketCreateReader rtxStreamSocketCreateReader
#define rtStreamSocketCreateWriter rtxStreamSocketCreateWriter

#define rtStreamMemoryCreate rtxStreamMemoryCreate
#define rtStreamMemoryAttach rtxStreamMemoryAttach
#define rtStreamMemoryGetBuffer rxtStreamMemoryGetBuffer
#define rtStreamMemoryCreateReader rtxStreamMemoryCreateReader
#define rtStreamMemoryCreateWriter rtxStreamMemoryCreateWriter

#ifdef __cplusplus
}
#endif

#endif /* _RTSTREAM_H_ */

