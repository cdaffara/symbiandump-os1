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
 * @file rtsrc/ASN1Context.h 
 * Common C++ type and class definitions.
 */

/**
@file
@internalTechnology
*/

#ifndef _ASN1CONTEXT_H_
#define _ASN1CONTEXT_H_

#include "rtxsrc/rtxDiag.h"
#include "rtxsrc/rtxError.h"
#include "rtxsrc/OSRTContext.h"

/**
 * @defgroup asn1context Context Management Classes 
 * This group of classes manages an OSCTXT structure. This is the C structure
 * use to keep track of all of the working variables required to encode or
 * decode an ASN.1 message. @{
 */
/** 
 * Reference counted ASN.1 context class. This keeps track of all encode/decode
 * function variables between function invocations. It is reference counted to
 * allow a message buffer and type class to share access to it.
 */
class EXTRTCLASS ASN1Context : public OSRTContext {
 public:
   /**
    * The default constructor initializes the mCtxt member variable for 
    * ASN.1 encoding/decoding.
    */
   EXTRTMETHOD ASN1Context ();

   /**
    * This method sets run-time key to the context. This method does nothing
    * for unlimited redistribution libraries.
    *
    * @param key - array of octets with the key
    * @param keylen - number of octets in key array.
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    */
   virtual EXTRTMETHOD int setRunTimeKey (const OSOCTET* key, size_t keylen);

   // deprecated methods: all have been changed to begin with lowercase 
   // character (AB, 7/14/04)..
   inline OSCTXT* GetPtr () { return &mCtxt; }
   inline void PrintErrorInfo () { printErrorInfo (); }
};

/** @} */

#endif /* ASN1CONTEXT */
