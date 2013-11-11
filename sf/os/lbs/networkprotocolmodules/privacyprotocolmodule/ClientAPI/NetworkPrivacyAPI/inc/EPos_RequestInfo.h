/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef REQUESTINFO_H
#define REQUESTINFO_H

#include <e32def.h>


/**
Network request class types.

@internalComponent
@released
*/
enum _TPosNetworkPrivacyRequestInfoType
    {
    EPosGSMPrivacyRequestInfoUnknownClass = 0x00,
    EPosGSMPrivacyRequestInfo			  = 0x01,
    EPosSUPLPrivacyRequestInfo			  = 0x02
    };

#endif      // REQUESTINFO_H
