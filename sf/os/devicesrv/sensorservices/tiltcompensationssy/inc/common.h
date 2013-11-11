/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common definitions.
*
*/


#ifndef COMMON_H
#define COMMON_H

#include <e32std.h>

// CONSTANTS

/** Panic codes */
enum TPanicReason
    {
    EInvalidState,
    EAlreadyServingTransaction,
    ENotActive,
    ENullPointer,
    EInvalidTransactionId,
    EInvalidEventId,
    EIncorrectStateCount,
    EAccelerometerChannelNotCreated,
    EMagnetometerChannelNotCreated
    };

#endif // COMMON_H

// End of File
