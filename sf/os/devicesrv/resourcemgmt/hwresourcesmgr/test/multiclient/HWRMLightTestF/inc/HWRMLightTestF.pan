// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//




#ifndef __HWRMLightTestF_PAN__
#define __HWRMLightTestF_PAN__

/** HWRMLightTestF application panic codes */
enum THWRMLightTestFPanics 
    {
    EHWRMLightTestFUi = 1
    // add further panics here
    };

inline void Panic(THWRMLightTestFPanics aReason)
    {
	_LIT(applicationName,"HWRMLightTestF");
    User::Panic(applicationName, aReason);
    }

#endif // __HWRMLightTestF_PAN__
