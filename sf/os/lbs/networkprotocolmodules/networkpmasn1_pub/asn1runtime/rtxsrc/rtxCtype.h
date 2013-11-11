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
 * @file rtxCtype.h 
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXCTYPE_H_
#define _RTXCTYPE_H_

/* Ctype substitution macros */

#define OS_ISASCII(c) ((unsigned)(c) < 0x80)

#define OS_ISUPPER(c) (c >= 'A' && c <= 'Z')
#define OS_ISLOWER(c) (c >= 'a' && c <= 'z')
#define OS_ISDIGIT(c) (c >= '0' && c <= '9')
#define OS_ISALPHA(c) (OS_ISUPPER(c) || OS_ISLOWER(c))
#define OS_ISSPACE(c) ((c >= 0x09 && c <= 0x0d) || (c == ' '))
#define OS_ISPUNCT(c) (c >= 0 && c <= 0x20)
#define OS_ISALNUM(c) (OS_ISALPHA(c) || OS_ISDIGIT(c))
#define OS_ISPRINT(c) (c >= ' ' && c <= '~')
#define OS_ISGRAPH(c) (c >= '!' && c <= '~')
#define OS_ISCNTRL(c) ((c >= 0 && c <= 0x1F) || c == 0x7F)
#define OS_ISXDIGIT(c) \
(OS_ISDIGIT(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))

#define OS_TOLOWER(c) (OS_ISUPPER(c) ? (c) - 'A' + 'a' : (c))
#define OS_TOUPPER(c) (OS_ISLOWER(c) ? (c) - 'a' + 'A' : (c))

#endif /* _RTXCTYPE_H_ */

