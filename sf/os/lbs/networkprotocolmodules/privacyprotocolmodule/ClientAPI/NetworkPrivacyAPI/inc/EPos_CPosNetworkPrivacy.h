//EPos_CPosNetworkPrivacy.h

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
 @publishedPartner
 @released
*/

#ifndef CPOSNETWORKPRIVACY_H
#define CPOSNETWORKPRIVACY_H

#include <e32base.h>
#include <lbsposition.h>

// FORWARD DECLARATIONS
class CPosNetworkPrivacyRequestInfo;
class MPosVerificationObserver;
class CPosNetworkPrivacyImpl;


/**
Main class for handling network privacy.

This class controls the access to the Privacy Framework. It contains methods
for handling privacy for location requests from the network.

This class also contains functions for configuring the Privacy Framework
connection.

In some functions, the client can acquire a request ID. This ID represents
the location request. In other functions the request ID is required as an
input parameter to specify which location request it is connected to.
*/
class CPosNetworkPrivacy :  public CBase
                                
	{
    public:
        /**
        When canceling a verification request, this enum can be used to
        specify why the verification was cancelled.
        */
        enum TCancelReason
            {
 			/** The cancel reason is not given. */
            ECancelReasonNotAvailable = 0,
 			/** Verification request timed out. */
            ECancelReasonTimeout
            };

        /**
        Whether a location request is accepted or rejected.
        */
        enum TRequestDecision
            {
 			/** Location request decision is not specified. */
            EDecisionNotAvailable = 0,
 			/** Location request is rejected. */
            EDecisionRejected,
 			/** Location request is accepted. */
            EDecisionAccepted
            };

         /**
        This enum specifies whether the requestor is a contact or a service.
        */
        enum TRequestorType
            {
            /** The requestor is a service. */
            ERequestorClientName,
   			/** The requestor is a contact. */
            ERequesterId
            }; 
    
    
    public:
        IMPORT_C static CPosNetworkPrivacy* NewL();
        virtual ~CPosNetworkPrivacy();
    
    public:
        IMPORT_C void SetConnectionTimeout(TTimeIntervalMicroSeconds32 aTimeout);
        
        IMPORT_C TTimeIntervalMicroSeconds32 ConnectionTimeout() const;
        
        IMPORT_C void VerifyLocationRequestL(
        				const CPosNetworkPrivacyRequestInfo& aRequestInfo,
        				TInt& aRequestId,
        				MPosVerificationObserver& aObserver);

        IMPORT_C void VerifyLocationRequestL(
        				const CPosNetworkPrivacyRequestInfo& aRequestInfo,
        				TInt& aRequestId,
        				MPosVerificationObserver& aObserver,
        				TRequestDecision aTimeoutStrategy);

        IMPORT_C void CancelVerifyLocationRequest(TInt aRequestId);

        IMPORT_C void CancelVerifyLocationRequest(
        				TInt aRequestId,
        				TCancelReason aCancelReason);

        IMPORT_C void NotifyLocationRequestL(
        				const CPosNetworkPrivacyRequestInfo& aRequestInfo,
        				TInt& aRequestId);

        IMPORT_C void NotifyVerificationTimeoutL(
        				const CPosNetworkPrivacyRequestInfo& aRequestInfo,
        				TInt aRequestId,
        				TRequestDecision aRequestDecision);

        IMPORT_C void PositionUpdateL( 
        		TInt aRequestId,const TPosition& aPosition);
    
    private:
        CPosNetworkPrivacy();
        void ConstructL();

        // By default, prohibit copy constructor
        CPosNetworkPrivacy(const CPosNetworkPrivacy&);
        // Prohibit assigment operator
        CPosNetworkPrivacy& operator= (const CPosNetworkPrivacy&);
                       
    private:
    CPosNetworkPrivacyImpl* iImpl;
    
    };

#endif      //EPos_CPosNetworkPrivacy.h
