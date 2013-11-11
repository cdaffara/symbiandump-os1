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




#ifndef __HWRMLightTestB_PAN__
#define __HWRMLightTestB_PAN__

/** HWRMLightTestB application panic codes */
enum THWRMLightTestBPanics 
    {
    EHWRMLightTestBUi = 1
    // add further panics here
    };

inline void Panic(THWRMLightTestBPanics aReason)
    {
	_LIT(applicationName,"HWRMLightTestB");
    User::Panic(applicationName, aReason);
    }

#endif // __HWRMLightTestB_PAN__
