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



#ifndef EPOSPRIVACY_H
#define EPOSPRIVACY_H

#include <e32def.h>

/** @file
 @publishedPartner
 @deprecated
 */

/**
Specifies the source of the location request.
@see CPosPrivacyNotifier::RequestSource()
*/
enum TPosRequestSource
    {
    /** Request source is not specified. */
    EPosRequestSourceNotAvailable = 0x0000,
    /** Request is from the cellular network. */
    EPosRequestSourceNetwork     = 0x0001
    };

/**
Specifies whether the request was accepted or rejected.
@see CPosPrivacyNotifier::QueryTimeoutStrategy()
*/
enum TPosRequestDecision
    {
    /** The location request decision is not given. */
    EPosDecisionNotAvailable = 0,
    /** A location request was rejected. */
    EPosDecisionRejected,
    /** A location request was accepted. */
    EPosDecisionAccepted
    };

/**
Specifies the reason for notification.
@see CPosPrivacyNotifier::NotificationReason()
*/
enum TPosNotificationReason
    {
    /** The notification reason is not available. */
    EPosNotificationReasonNotAvailable = 0,
    /** A location request was accepted/rejected by the request
        source. */
    EPosDecisionByRequestSource,
    /** The verification activity timed out. */
    EPosVerificationTimeout
    };

/**
Specifies the reason why a verification request was cancelled.
@see CPosPrivacyNotifier::CancelReason()
*/
enum TPosVerifyCancelReason
    {
    /** No cancel reason available. */
    EPosCancelReasonNotAvailable = 0,
    /** Dialog cancelled because of timeout. */
    EPosCancelReasonTimeout
    };

#endif      // EPOSPRIVACY_H

// End of File
