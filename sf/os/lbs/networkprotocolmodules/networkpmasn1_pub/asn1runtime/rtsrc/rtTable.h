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
 * @file rtxsrc/rtxTable.h
 */


/**
@file
@internalTechnology
*/

#ifndef _RTTABLE_H_
#define _RTTABLE_H_

#include "rtsrc/asn1type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************/
/*                                                            */
/*  Common RunTime Library Functions		              */
/*                                                            */
/**************************************************************/

EXTERNRT int rtCmpTCINT8 (OSINT8* value, OSINT8* compValue);
EXTERNRT int rtCmpTCUINT8 (OSUINT8* value, OSUINT8* compValue);
#define rtCmpTCCHAR rtCmpTCINT8
#define rtCmpTCOCTET rtCmpTCUINT8
EXTERNRT int rtCmpTCSINT (OSINT16* value, OSINT16* compValue);
EXTERNRT int rtCmpTCUSINT (OSUINT16* value, OSUINT16* compValue);
EXTERNRT int rtCmpTCINT (OSINT32* value, OSINT32* compValue);
EXTERNRT int rtCmpTCUINT (OSUINT32* value, OSUINT32* compValue);
EXTERNRT int rtCmpTCINT64 (OSINT64* value, OSINT64* compValue);
EXTERNRT int rtCmpTCUINT64 (OSUINT64* value, OSUINT64* compValue);
EXTERNRT int rtCmpTCOID (ASN1OBJID* pOID, ASN1OBJID* pcompOID);
EXTERNRT int rtCmpTCReal (OSREAL* value, OSREAL* compValue);
EXTERNRT int rtCmpTCBoolean (OSBOOL* key, OSBOOL* pCompValue);
EXTERNRT int rtCmpTCBitStr (OSUINT32 numbits, const OSOCTET* data, 
   OSUINT32 cmpnumbits, const OSOCTET* cmpData);
EXTERNRT int rtCmpTCOctet (const OSOCTET* pValue, const OSOCTET* pCompValue);
EXTERNRT int rtCmpTCOctStr (OSUINT32 numocts, const OSOCTET* data,
   OSUINT32 cmpnumocts, const OSOCTET* cmpdata);
EXTERNRT int rtCmpTCCharStr (const char* cstring, const char* compCstring);
EXTERNRT int rtCmpTC16BitCharStr 
   (Asn116BitCharString* key, Asn116BitCharString* pCompValue);
EXTERNRT int rtCmpTC32BitCharStr 
   (Asn132BitCharString* key, Asn132BitCharString* pCompValue);

EXTERNRT long rtBSearch (void *key, void *base, 
                          size_t nmemb, size_t size,  
                          int (*compar) (void *, void *));

EXTERNRT int rtLSearch (void *key, void *base, 
                         int nmemb, size_t size,  
                         int (*compar) (void *, void *));

#define RTARRAYTOLIST(arraysize, array, list) \
{\
   static OSRTDListNode ListNodes[arraysize]; \
   OSUINT32 i; \
   for (i = 0; i < arraysize; i ++) { \
      ListNodes[i].data = (void*)&array[i]; \
      ListNodes[i].next = (OSRTDListNode*) 0; \
      if (0 != list.tail) { \
         list.tail->next = &ListNodes[i]; \
         ListNodes[i].prev = list.tail; \
      } \
      if (0 == list.head) { \
         list.head = &ListNodes[i]; \
         ListNodes[i].prev = (OSRTDListNode*) 0; \
      } \
      list.tail = &ListNodes[i]; \
      list.count++; \
   } \
}

EXTERNRT void rtMemFreeNullPtr (OSCTXT* pctxt, void* pobject);

#ifdef __cplusplus
}
#endif
#endif 
