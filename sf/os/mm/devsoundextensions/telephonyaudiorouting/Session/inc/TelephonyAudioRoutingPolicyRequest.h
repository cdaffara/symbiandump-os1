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
* Description:   Active object for each asynchronous request in RTelephonyAudioRoutingSession.
*				 Notifies MTelephonyAudioRoutingPolicyObserver upon request completion.
*
*/




#ifndef TELEPHONYAUDIOROUTINGPOLICYREQUEST_H
#define TELEPHONYAUDIOROUTINGPOLICYREQUEST_H

#include <e32base.h>
#include "TelephonyAudioRoutingManagerSession.h"
#include "TelephonyAudioRoutingClientServer.h"
#include "TelephonyAudioRoutingManager.h"

// CLASS DECLARATION

/**
*  Implements active object for outstanding asynchronous request.
*
*  @lib TelephonyAudioRoutingSession.lib
*  @since Series 60 3.1
*/
class CTelephonyAudioRoutingPolicyRequest : public CActive
    {
    
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
       
        static CTelephonyAudioRoutingPolicyRequest* NewL( RTelephonyAudioRoutingManagerSession& aSession, MTelephonyAudioRoutingPolicyObserver& aObserver, CTelephonyAudioRoutingManager& aAudioRouting, TTelAudRtngServRqst aRequest );


        /**
        * Destructor.
        */
        virtual ~CTelephonyAudioRoutingPolicyRequest();

    public: // New functions

        /**
        * Completes the outstanding request.
        * @since Series 60 3.1
        * @param aRequest Request being completed
        * @param aError A standard system error code
        * @return void
        */
        void CompleteRequest( TTelAudRtngServRqst aRequest, TInt aError );

    private:  // Functions from base classes

        /**
        * From CActive
        * Cancel outstanding request
        * @param none
        * @return void
        
        */
		void DoCancel(); 

        /**
        * From CActive
		* Implementation of CActive::RunL. Called when server request has completed.
        * @param none
        * @return void
		
		**/
		void RunL();

    private:

        /**
        * C++ default constructor.
        */
        CTelephonyAudioRoutingPolicyRequest( RTelephonyAudioRoutingManagerSession& aSession, MTelephonyAudioRoutingPolicyObserver& aObserver, CTelephonyAudioRoutingManager& aAudioRouting, TTelAudRtngServRqst aRequest );


        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

		
		RTelephonyAudioRoutingManagerSession& iManagerSession;
    	MTelephonyAudioRoutingPolicyObserver& iPolicyObserver;
    	CTelephonyAudioRoutingManager& iAudioRoutingManager;
    	TTelAudRtngServRqst iRequest;

	private:    // Friend classes
        friend class RTelephonyAudioRoutingManagerSession;

    };

#endif      // TELEPHONYAUDIOROUTINGPOLICYREQUEST_H

// End of File
