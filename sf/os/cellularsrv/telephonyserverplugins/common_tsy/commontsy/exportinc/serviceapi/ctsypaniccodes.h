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
//



/**
 @file
 @internalComponent
 @released
*/


#ifndef CTSYPANICCODES_H
#define CTSYPANICCODES_H

#ifdef USING_CTSY_DISPATCHER

#include <e32base.h>

_LIT( KCtsyDispatcherPanicCategory, "CtsyDispatcher" );
_LIT( KCtsyPanicCategory, "Ctsy" );

/** Ctsy panic values*/
enum TCtsyPanicCodes
    {
    EUnhandledCtsyIpc,
    EInvalidNullPtr,
    ERequestNotFound,
	EInvalidParameter,
    ESlotIndexesDontMatch,
    EBadLength,
    ECorruptTlvData,
    ELeaveTrapped
    };

#endif //USING_CTSY_DISPATCHER

#endif //CTSYPANICCODES_H
