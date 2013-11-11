/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Header GDR.H
*
*/


#ifndef __GDR_H__
#define __GDR_H__

/**
@file
@publishedAll
*/

/** WARNING: Typedefs for internal use ONLY.  Compatibility is not guaranteed in future releases. */
typedef char chardim;				// used for character dimensions, can be increased to short int (2 bytes)
/** WARNING: Typedefs for internal use ONLY.  Compatibility is not guaranteed in future releases. */
typedef unsigned char uint8;		// used for values which are written as unsigned short int
/** WARNING: Typedefs for internal use ONLY.  Compatibility is not guaranteed in future releases. */
typedef unsigned short int uint16;	// used for values which are written as unsigned short int
/** WARNING: Typedefs for internal use ONLY.  Compatibility is not guaranteed in future releases. */
typedef signed long int32;			// used for file signature
/** WARNING: Typedefs for internal use ONLY.  Compatibility is not guaranteed in future releases. */
typedef unsigned long uint32;
/** WARNING: Typedefs for internal use ONLY.  Compatibility is not guaranteed in future releases. */
typedef signed int uid;
/** WARNING: Typedefs for internal use ONLY.  Compatibility is not guaranteed in future releases. */
typedef uint8 boolean;

/** WARNING: Constants for internal use ONLY.  Compatibility is not guaranteed in future releases. */
const boolean efalse = 0;
/** WARNING: Constants for internal use ONLY.  Compatibility is not guaranteed in future releases. */
const boolean etrue = 1;

/** WARNING: Constants for internal use ONLY.  Compatibility is not guaranteed in future releases. */
const int MaxLineLen = 256;	// maximum line length
/** WARNING: Constants for internal use ONLY.  Compatibility is not guaranteed in future releases. */
const int KUndefinedInteger=0x7fff;

/** WARNING: For internal use ONLY.  Compatibility is not guaranteed in future releases. */
#if defined(__VC32__)
#define IMPORT_C __declspec(dllexport)
#define EXPORT_C __declspec(dllexport)
#else
#define IMPORT_C
#define EXPORT_C
#endif

#endif
