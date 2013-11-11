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
// The purpose of this program is to validate whether it is possible to
// write a vg1.1 client program which detects the available implementation
// at run time and then falls back onto vg1.0 calls if vg1.1 is not available.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include <e32base.h>
#include <e32debug.h>
#include <string.h>


/*
 * The program is compiled with the appropriate macro.
 */
#include <VG/openvg.h>
#include <VG/vgu.h>

GLDEF_C TInt E32Main()
    {
    RDebug::Printf("testing VG 1.0 function");
    VGPath path0 = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
    RDebug::Printf("finished testing VG 1.0 function");

#ifdef OPENVG_VERSION_1_1
    if(strcmp((const char*)vgGetString(VG_VERSION), "1.1")==0)
    	{
    	RDebug::Printf("testing function introduced in 1.1");
    	VGFont font = vgCreateFont(245);
    	vgDrawGlyph(font, 2, 256, VG_FALSE);
    	RDebug::Printf("finished testing function introduced in 1.1");
    	}
#endif
    
    return 0;
    }
