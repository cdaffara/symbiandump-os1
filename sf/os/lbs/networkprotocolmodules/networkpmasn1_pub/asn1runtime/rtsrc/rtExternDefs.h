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
@file
@internalTechnology
*/

#ifndef _RTEXTERNDEFS_H_
#define _RTEXTERNDEFS_H_

/** 
 * @file rtExternDefs.h 
 * Common definitions of external function modifiers used to define the 
 * scope of functions used in DLL's (Windows only).
 */

#if defined(__SYMBIAN32__)   /* OS: Symbian */
#define EXTRTCLASS

#ifndef EXTERN
#ifdef BUILDASN1RTDLL
#define EXTERN EXPORT_C
#elif defined (USEASN1RTDLL)
#define EXTERN IMPORT_C
#else
#define EXTERN
#endif /* BUILDASN1RTDLL */
#endif /* EXTERN */

#ifndef EXTERNRT
#ifdef BUILDASN1RTDLL
#define EXTERNRT     EXPORT_C
#define EXTRTMETHOD EXPORT_C
#elif defined (USEASN1RTDLL)
#define EXTERNRT     IMPORT_C
#define EXTRTMETHOD IMPORT_C
#else
#define EXTERNRT
#define EXTRTMETHOD
#endif /* BUILDASN1RTDLL */
#endif /* EXTERNRT */

#else                         /* OS: Win, Unix, Linux, ... */
#define EXTRTMETHOD

#ifndef EXTERNRT
#if defined(BUILDASN1RTDLL) || defined(BUILDRTXDLL)
#define EXTERNRT     __declspec(dllexport)
#define EXTRTCLASS __declspec(dllexport)
#elif defined (USEASN1RTDLL)
#define EXTERNRT     __declspec(dllimport)
#define EXTRTCLASS __declspec(dllimport)
#else
#define EXTERNRT
#define EXTRTCLASS
#endif /* BUILDRTDLL */
#endif

#endif

#endif
