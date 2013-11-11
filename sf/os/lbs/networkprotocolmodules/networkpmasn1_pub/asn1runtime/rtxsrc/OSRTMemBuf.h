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
 * @file OSRTMemBuf.h
 */
/* memory buffer */


/**
@file
@internalTechnology
*/

#ifndef _OSRTMEMBUF_H_
#define _OSRTMEMBUF_H_

/* Disable MS VC++ Level 4 warning about unreferenced inline functions */
#ifdef _MSC_VER
#pragma warning(disable: 4514)
#endif /* _MSC_VER */

#include "rtxsrc/rtxMemBuf.h"

/** 
 * Memory Buffer class. This is the base class for generated C++ data type
 * classes for XSD string types (string, token, NMTOKEN, etc.).
 */
class EXTRTCLASS OSRTMemBuf : public OSRTMEMBUF {
 public:
   EXTRTMETHOD OSRTMemBuf ();
   EXTRTMETHOD ~OSRTMemBuf ();
   inline const OSOCTET* getData() { return buffer + startidx; }
   inline size_t getDataLen() { return usedcnt - startidx; }
} ;

#endif // _OSRTMEMBUF_H_
