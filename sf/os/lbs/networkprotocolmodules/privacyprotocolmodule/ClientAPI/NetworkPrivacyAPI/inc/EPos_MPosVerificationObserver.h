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



#ifndef MPOSVERIFICATIONOBSERVER_H
#define MPOSVERIFICATIONOBSERVER_H

#include <e32base.h>


/**
Interface to observe completion of CPosNetworkPrivacy::VerifyLocationRequestL()

@see CPosNetworkPrivacy::VerifyLocationRequestL()
@publishedPartner
@released
*/
class MPosVerificationObserver
    {
    public:
        /**
        Called when a call to CPosNetworkPrivacy::VerifyLocationRequestL
        completes, in which this observer object was passed as a parameter.

        @param aRequestId The request ID which was returned by
                          CPosNetworkPrivacy::VerifyLocationRequestL.
        @param aCompletionCode KErrNone if the location request is accepted, 
                               KErrAccessDenied if the location request is rejected,
                               including if the request is rejected because the maximum
                               number of simultaneous requests is exceeded (as defined by KLbsSettingMaximumExternalLocateRequests),
                               or any of the system wide error codes.
        @see KLbsSettingMaximumExternalLocateRequests
        */
        virtual void HandleVerifyComplete(TInt aRequestId, TInt aCompletionCode) = 0;
    };

#endif      // MPOSVERIFICATIONOBSERVER_H
