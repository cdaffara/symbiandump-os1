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
// Test the behaviour of graphics_openvgheaders in version 1.1
// The purpose of this source code is to thrown an error during compilation
// if the VG/openvg.h header does not re-direct to the VG/1.1/openvg.h
// This code must be compiled with __OPENVGHEADERS_USE_VG_1_1 to ensure that version 1.1 will be chosen
// 
//

/**
 @file
 @internalTechnology
 @released
*/
#include <e32base.h>

/*
 * Include the VG/openvg.h which will be re-directed to OPENVG 1.1 headers
 */
#include <VG/openvg.h>

#ifndef OPENVG_VERSION_1_1
#error "openglesheaders testcase failure: VG/openvg.h expects to include VG/1.1/openvg.h"
#endif

GLDEF_C TInt E32Main()
    {
    return 0;
    }
	
// Getting this far without a compilation error indicates success.
