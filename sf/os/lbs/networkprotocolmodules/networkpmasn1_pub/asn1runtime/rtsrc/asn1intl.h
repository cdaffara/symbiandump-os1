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
 * asn1intl.h - ASN.1 runtime internal definitions 
 */
/**
 * @file asn1intl.h
 */

/**
@file
@internalTechnology
*/

#ifndef _ASN1INTL_H_
#define _ASN1INTL_H_

/* ASN1C run-time version number: this should match the ASN1C compiler  */
/* version in no-dot format (ex. 5.70 would be 570)..                   */

#define ASN1RT_VERSION  600

#ifndef _COMPACT

/* For ARM compilers. */
#ifndef __MODULE__
#define LOG_ASN1ERR(ctxt,stat) \
      rtErrSetData(&ctxt->errInfo,stat,__FILE__,__LINE__)
#else
#define LOG_ASN1ERR(ctxt,stat) \
      rtErrSetData(&ctxt->errInfo,stat,__MODULE__,__LINE__)
#endif /* ifndef __MODULE__ */

#define ASN1_PRINT_OCTSTR(os) \
printf ("%-*.*s", os.numocts, os.numocts, os.data)

#else /* if defined _COMPACT */
#undef _TRACE
#define ASN1_PRINT_OCTSTR(os)
#endif

#define LOG_RTERR_AND_FREE(ctxt_p,stat,lctxt) \
rtFreeContext ((lctxt)), LOG_RTERR(ctxt_p, stat)

#define LOG_RTERR_AND_FREE_MEM(ctxt_p,stat,mem_p) \
rtxMemFreePtr ((ctxt_p),(mem_p)), LOG_RTERR(ctxt_p, stat)
/* 
 * Memory allocations macros.  These are for use in generated ASN1C code only.
 */
#define ALLOC_ASN1ELEMDNODE_MEM(pctxt,type) \
(type*) (((char*)rtxMemHeapAlloc (&(pctxt)->pMemHeap, sizeof(type) + \
sizeof(OSRTDListNode))) + sizeof(OSRTDListNode))

#define ALLOC_ASN1ELEMDNODE(pctxt,type) \
(type*) (((char*)rtxMemHeapAllocZ (&(pctxt)->pMemHeap, sizeof(type) + \
sizeof(OSRTDListNode))) + sizeof(OSRTDListNode))

#define ALLOC_ASN1ELEMSNODE_MEM(pctxt,type) \
(type*) (((char*)rtxMemHeapAlloc (&(pctxt)->pMemHeap, sizeof(type) + \
sizeof(OSRTSListNode))) + sizeof(OSRTSListNode))

#define ALLOC_ASN1ELEMSNODE(pctxt,type) \
(type*) (((char*)rtxMemHeapAllocZ (&(pctxt)->pMemHeap, sizeof(type) + \
sizeof(OSRTSListNode))) + sizeof(OSRTSListNode))

#endif /* _ASN1INTL_H_ */
