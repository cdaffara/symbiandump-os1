 // Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __HELLOWORLDBASIC_PAN__
#define __HELLOWORLDBASIC_PAN__


#include <e32std.h>


/** HelloWorldBasic application panic codes */
enum THelloWorldBasicPanics
    {
    EHelloWorldBasicUi = 1
    // add further panics here
    };

enum TFileListPanics 
    {
    EFileListIvalidIndex = 1
    };

inline void Panic(TFileListPanics aReason)
{
    _LIT(applicationName,"FileList");
    User::Panic(applicationName, aReason);
}

inline void Panic(THelloWorldBasicPanics aReason)
    {
    _LIT(applicationName,"HelloWorldBasic");
    User::Panic(applicationName, aReason);
    }

#endif // __HELLOWORLDBASIC_PAN__
