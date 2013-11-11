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
*				 Notifies MTelephonyAudioObserver upon request completion.
*
*/




#ifndef TELEPHONYAUDIOROUTINGREQUEST_H
#define TELEPHONYAUDIOROUTINGREQUEST_H

#include <e32base.h>
#include "TelephonyAudioRoutingSession.h"
#include "TelephonyAudioRoutingClientServer.h"
#include "TelephonyAudioRouting.h"


// CLASS DECLARATION

/**
*  Implements active object for outstanding asynchronous request.
*
*  @lib TelephonyAudioRoutingSession.lib
*  @since Series 60 3.1
*/
class CTelephonyAudioRoutingRequest : public CActive
    {
    
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTelephonyAudioRoutingRequest* NewL( RTelephonyAudioRoutingSession& aSession, MTelephonyAudioRoutingObserver& aObserver, CTelephonyAudioRouting& aAudioRouting, TTelAudRtngServRqst aRequest );
        
    
        /**
        * Destructor.
        */
        virtual ~CTelephonyAudioRoutingRequest();

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
        CTelephonyAudioRoutingRequest( RTelephonyAudioRoutingSession& aSession, MTelephonyAudioRoutingObserver& aObserver, CTelephonyAudioRouting& aAudioRouting, TTelAudRtngServRqst aRequest );
        
      
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

		
		RTelephonyAudioRoutingSession& iSession;
    	MTelephonyAudioRoutingObserver& iObserver;
		CTelephonyAudioRouting& iAudioRouting;
    	TTelAudRtngServRqst iRequestOpcode;
    	
	private:    // Friend classes
       friend class RTelephonyAudioRoutingSession;

    };

#endif      // TELEPHONYAUDIOROUTINGREQUEST_H

// End of File
