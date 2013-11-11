/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




/**
 @file
 @internalComponent
 @released
*/

#ifndef EPOS_PRIVACYSERVERDEBUGPANIC_H
#define EPOS_PRIVACYSERVERDEBUGPANIC_H

#include <e32base.h>

_LIT(KPosPrivSrvDebugPanicCategory, "EPos Priv debug");

enum TPosPrivSrvDebugPanic
    {
    EPosPrivSrvPanicUnknownActivity = 0
    };

// FUNCTION PROTOTYPES
GLDEF_C void DebugPanic(TPosPrivSrvDebugPanic aPanic);

#endif      // EPOS_PRIVACYSERVERDEBUGPANIC_H

// End of File
