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
* Name        : CSatNotifySetUpCall.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Specific notifications class
* Version     : 1.0
*
*/



#ifndef CSATNOTIFYSETUPCALL_H
#define CSATNOTIFYSETUPCALL_H


//  INCLUDES
#include <etelsat.h>
#include "MSatNotificationsBase.h"

// FORWARD DECLARATIONS
class CSatDataPackage;
class CSatNotificationsTsy;

/**
* DESCRIPTION
* SAT Setup Call notification class.
* Created when client requests to be notified when Setup Call
* command arrives from SIM.
*
* Setup a call. Can be setup in three different ways:
* - Set up a call, only if not currently busy on another call
* - Set up a call, put all other call on hold
* - Set up a call, disconnect other calls
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatNotifySetUpCall ) : public CBase,
                            public MSatNotificationsBase
    {
    public:   // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * return CSatNotifySetUpCall*: created object 
        */
        static CSatNotifySetUpCall* NewL( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatNotifySetUpCall();

	private:
        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * @return None
        */
        CSatNotifySetUpCall( CSatNotificationsTsy* aNotificationsTsy );

        /**
        * Class attributes are created in ConstructL. 
        * @param None
        * @return None
        */
        void ConstructL();
        
    public: // Functions from base classes

        /**
        * Set up call Notification request received from client
        * @param aTsyReqHandle: request handle from ETel
        * @param aPackage: Packed data
        * @return KErrNone
        */
        TInt Notify( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

        /**
        * Cancels notification request for SetUpCall.
        * @param aReqHandle notify request handle
        * @return KErrNone
        */
        TInt CancelNotification( const TTsyReqHandle aReqHandle );
                 
        /**
        * Completes SetUpCall message to client
        * @param aDataPackage: Packaged return data
        * @param aErrorCode: Possible error code
        * @return KErrNone/ KErrCorrupt
        */
        TInt CompleteNotifyL( CSatDataPackage* aDataPackage, TInt aErrorCode );

        /**
        * From CSatNotificationsBase handles Set up call terminal response 
        * coming from client.
        * @param aRsp: Response structure
        * @return KErrNone/ KErrCorrupt
        */
        TInt TerminalResponseL( TDes8* aRsp );

	public: // New functions
	
        /**
        * Create notification specific terminal response data
        * @param aPCmdNumber: Proactive command number
        * @param aGeneralResult: Result of the proactive command
        * @param aAdditionalInfo: Additional info for terminal response
        * @return Success/Failure value
        */
        TInt CreateTerminalRespL( 
            TUint8 aPCmdNumber, TUint8 aGeneralResult, 
            TDesC16& aAdditionalInfo );
        
        /**
        * Cache call connected event
        * @param aEnvelope
        * @return none
        */
        void StoreCallConnectedEvent( const TDesC8& aEnvelope );

    private: // Data

        // Setup Call package store. Package received from ETel
        RSat::TSetUpCallV1Pckg* iSetUpCallV1Pckg;
        // Cache for Call connected event
        TBuf8<12>               iCallConnectedEvent;
        // Pointer to the notifications tsy class
        CSatNotificationsTsy* iNotificationsTsy;
        // CC result
        TUint8 iCCResult;
    };

#endif // CSATNOTIFYSETUPCALL_H