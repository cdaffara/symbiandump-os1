// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// VG/OPENVG.H
// 
//

#ifndef __VG_OPENVG_H_
#define __VG_OPENVG_H_


/**
@file
@publishedAll
@released

WARNING: File for internal and partner use ONLY.  Compatibility is not guaranteed in future releases.

__OPENVGHEADERS_USE_VG_1_1
    
The purpose of this define is to allow a staged migration from
OPENVG 1.0.1 to OPENVG 1.1.

If __OPENVGHEADERS_USE_VG_1_1 is defined, then VG/openvg.h will redirect to 
the OPENVG 1.1 API.
If __OPENVGHEADERS_USE_VG_1_1 is not defined, then VG/openvg.h 
will redirect to the Symbian default version, currently OPENVG 1.0.1. 

SDK creators should place this #define in their OEM-specific system-wide
.hrh file in order to make client programs use OpenVG 1.1 headers.  
Symbian product configurations never set this behaviour.
*/
#if defined(__OPENVGHEADERS_USE_VG_1_1) && !defined(OPENVG_RESTRICTED_TO_1_0)
#include <VG/1.1/openvg.h>
#else
#include <VG/1.0/openvg.h>
#endif

#endif /* __VG_OPENVG_H_ */

