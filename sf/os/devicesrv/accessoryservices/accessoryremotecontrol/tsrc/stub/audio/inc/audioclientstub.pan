/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains panic codes.
*
*/


#ifndef __AUDIOCLIENTSTUB_PAN__
#define __AUDIOCLIENTSTUB_PAN__

/** audioclientstub application panic codes */
enum TaudioclientstubPanics
    {
    EaudioclientstubUi = 1
    // add further panics here
    };

inline void Panic(TaudioclientstubPanics aReason)
    {
    _LIT(applicationName, "audioclientstub");
    User::Panic(applicationName, aReason);
    }

#endif // __AUDIOCLIENTSTUB_PAN__
