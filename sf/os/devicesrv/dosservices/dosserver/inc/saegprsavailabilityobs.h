/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef SAEGPRSAVAILABILITYOBS_H
#define SAEGPRSAVAILABILITYOBS_H

//  INCLUDES
#include <etelpckt.h>


// FORWARD DECLARATIONS
class CSaeEtelObserver;

// CLASS DECLARATION

/**
*  This class observs the GPRS availability.
*/
NONSHARABLE_CLASS( CSaeGprsAvailabilityObs ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CSaeGprsAvailabilityObs( CSaeEtelObserver* aEtelObserver, RPacketService* aService );

        /**
        * Destructor.
        */
        virtual ~CSaeGprsAvailabilityObs();

    private:

        // By default, prohibit copy constructor
        CSaeGprsAvailabilityObs( const CSaeGprsAvailabilityObs& );
        // Prohibit assigment operator
        CSaeGprsAvailabilityObs& operator= ( const CSaeGprsAvailabilityObs& );

		/**
        * Issues the request.
        */
        void IssueRequest();

		/**
        * Initialize GPRS registration status.
        */
		void InitializeRegStatus();

		/**
        * Cancels the request.
        */
		void DoCancel();

        /**
        * Handles the event.
        */
		void RunL();

		/**
		* Checks the current network registration status.
		*/
		void CheckRegistrationStatusAndInformPubSub();

    private:    // Data
        
		// A pointer to the ETel observer. Not owned.
		CSaeEtelObserver* iEtelObserver;
		// A pointer to Etel GPRS session. Not owned.
		RPacketService* iService;
		RPacketService::TRegistrationStatus iRegistrationStatus;
    };

#endif      // SAEGPRSSTATUSOBS   
            
// End of File
