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

#ifndef PRIVACYINTERNAL_H
#define PRIVACYINTERNAL_H

#include <e32base.h>
#include <lbs/epos_privacy.h>

//CONSTANTS
const TUint KPosPrivacyServerCancelReasonArg = 0;
const TUint KPosPrivacyServerRequestIdArg    = 0;
const TUint KPosPrivacyServerDataPtrArg      = 1;
const TUint KPosPrivacyServerRequestDataArg  = 2;

// DATA TYPES
struct TPosRequestData
    {
    TPosRequestSource   iRequestSource;
    TPosRequestDecision iTimeoutStrategy;
    TPosRequestDecision iRequestDecision;
    TPosNotificationReason iNotificationReason;
    TPosVerifyCancelReason iCancelReason;
    };

enum TPosPrivacyClientServer
    {
    EPosPrivacyVerifyService = 0,
    EPosPrivacyCancel,
    EPosPrivacyNotifyNonVerified,
    EPosPrivacyGetRequestId,
    EPosPrivacyServerMaxEnumValue,
    EPosPrivacyServerLast
    };

const TInt KPosBufFlatExpandSize = 100;

template <class T>
class CleanupResetAndDestroy
    {
public:
    inline static void PushL(T& aRef);
private:
    static void ResetAndDestroy(TAny *aPtr);
    };
template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef);

#include "EPos_PrivacyInternal.inl"

#endif      // PRIVACYINTERNAL_H

// End of File
