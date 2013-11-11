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
* Name        : CSatNotifySendSs.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Specific notifications class
* Version     : 1.0
*
*/



#ifndef CSATNOTIFYSENDSS_H
#define CSATNOTIFYSENDSS_H


//  INCLUDES
#include <etelsat.h>
#include "MSatNotificationsBase.h"


// FORWARD DECLARATIONS
class CSatDataPackage;
class CSatNotificationsTsy;

/**
* DESCRIPTION
* SAT Send SS notification class.
* Created when client requests to be notified when Send SS
* command arrives from SIM.
*
* Sends a SS string towards the network. Upon receiving this command,
* the SAT shall decide if it is able to execute the command.
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatNotifySendSs ) : public CBase,
                           public MSatNotificationsBase
    {
    public:   // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * return CSatNotifySendSs*: created object 
        */
        static CSatNotifySendSs* NewL( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatNotifySendSs();

	private:
        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * @return None
        */
        CSatNotifySendSs( CSatNotificationsTsy* aNotificationsTsy );

        /**
        * Class attributes are created in ConstructL. 
        * @param None
        * @return None
        */
        void ConstructL();

    public: // Functions from base classes

        /**
        * Send Ss Notification request received from client
        * @param aTsyReqHandle: request handle from ETel
        * @param aPackage: Packed data
        * @return KErrNone
        */
        TInt Notify( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

        /**
        * Cancels notification request for Send Ss.
        * @param aReqHandle notify request handle
        * @return KErrNone
        */
        TInt CancelNotification( const TTsyReqHandle aReqHandle );
                 
        /**
        * Completes Send Ss message to client
        * @param aDataPackage: Packaged return data
        * @param aErrorCode: Possible error code
        * @return KErrNone/ KErrCorrupt
        */
        TInt CompleteNotifyL( CSatDataPackage* aDataPackage, TInt aErrorCode );

        /**
        * From CSatNotificationsBase handles Send Ss terminal response 
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
        TInt CreateTerminalRespL( TUint8 aPCmdNumber,
			TUint8 aGeneralResult, const TDesC16& aAdditionalInfo );

	public: // New functions

        /**
        * Checks SS string if it is call forwarding and append '+' character.
        * @param aSource Original string is received from SIM
        * @param aSsString SS string
        * @return None
        */
        void CheckCallForwarding( TPtrC8 aSource, 
                                  RSat::TSsString& aSsString );
        /**
        * Checks validity of SS string. If string includes undefined SS 
        * characters or length is zero then return KErrCorrupt
        * @param aSsString Original string is received from SIM
        * @return value: KErrNone or KErrCorrupt
        */
        TInt CheckSsStringValidity( TPtrC8 aSsString );


    private: // Data

        // Send Ss package store. Package received from ETel
        RSat::TSendSsV1Pckg* iSendSsV1Pckg;

        // Requested response format
        TUint8 iRequestedRspFormat;

        // Pointer to the notifications tsy class
        CSatNotificationsTsy* iNotificationsTsy;
    };


#endif // CSATNOTIFYSENDSS_H