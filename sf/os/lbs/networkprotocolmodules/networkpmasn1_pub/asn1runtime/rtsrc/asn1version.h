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

#ifndef __ASN1VERSION_H
#define __ASN1VERSION_H

#define OS_ASN1RT_VERSION 611
#define OS_ASN1RT_VERSION_STR "6.11"


#define ACVERSION OS_ASN1RT_VERSION
#define OS_ASN1RT_MAJOR_VERSION (OS_ASN1RT_VERSION/100)
#define OS_ASN1RT_MINOR_VERSION (OS_ASN1RT_VERSION%100/10)
#define OS_ASN1RT_PATCH_VERSION (OS_ASN1RT_VERSION%10)

#define OS_ASN1RT_MAJOR_VERSION_STR (OS_ASN1RT_VERSION_STR[0])
#define OS_ASN1RT_MINOR_VERSION_STR (OS_ASN1RT_VERSION_STR[2])
#define OS_ASN1RT_PATCH_VERSION_STR (OS_ASN1RT_VERSION_STR[3])

#endif /* __ASN1VERSION_H */
