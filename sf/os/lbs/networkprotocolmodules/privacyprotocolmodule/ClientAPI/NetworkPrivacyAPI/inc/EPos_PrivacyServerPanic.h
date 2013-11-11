// EPos_PrivacyServerPanic.h

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



#ifndef EPOS_PRIVACYSERVERPANIC_H
#define EPOS_PRIVACYSERVERPANIC_H

#include <e32def.h>

/**
Panic category for panics from the privacy server.
@publishedPartner
@released
*/
_LIT(KPosPrivacyPanicCategory, "EPos privacy");

/**
Panic codes.
@publishedPartner
@released
*/
enum TPosPrivSrvPanic
    {
    EPosPrivSrvPanicRequestOverflow = 0,
    EPosPrivSrvPanicInvalidArguments,
    EPosPrivSrvPanicUnknownServerMessage
    };

#endif      // EPOS_PRIVACYSERVERPANIC_H

// End of File