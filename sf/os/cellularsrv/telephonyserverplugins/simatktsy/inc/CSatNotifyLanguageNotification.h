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
* Name        : CSatNotifyLanguageNotification.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Specific notifications class
* Version     : 1.0
*
*/



#ifndef CSATNOTIFYLANGUAGENOTIFICATION_H
#define CSATNOTIFYLANGUAGENOTIFICATION_H


//  INCLUDES
#include "MSatNotificationsBase.h"

// FORWARD DECLARATIONS
class CSatDataPackage;
class CSatNotificationsTsy;

/**
* DESCRIPTION
* SAT  Language Notification information notification class
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatNotifyLanguageNotification ) : public CBase,
                           public MSatNotificationsBase
    {
    public:   // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * return CSatNotifyLanguageNotification*: created object 
        */
        static CSatNotifyLanguageNotification* NewL( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatNotifyLanguageNotification();

	private:
        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * @return None
        */
        CSatNotifyLanguageNotification( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * Class attributes are created in ConstructL. 
        * @param None
        * @return None
        */
        void ConstructL();

    public: // Functions from base classes

        /**
        * Language Notification request received from client
        * @param aTsyReqHandle: request handle from ETel
        * @param aPackage: Packed data
        * @return KErrNone
        */
        TInt Notify( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

        /**
        * Cancels notification request for LanguageNotification.
        * @param aReqHandle notify request handle
        * @return KErrNone
        */
        TInt CancelNotification( const TTsyReqHandle aReqHandle );
                 
        /**
        * Completes LanguageNotification message to client
        * @param aDataPackage: Packaged return data
        * @param aErrorCode: Possible error code
        * @return KErrNone/ KErrCorrupt
        */
        TInt CompleteNotifyL( CSatDataPackage* aDataPackage, TInt aErrorCode );

        /**
        * From CSatNotificationsBase handles Language notification terminal response 
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
        TInt CreateTerminalRespL( TUint8 aPCmdNumber, TUint8 aGeneralResult,
            					 TDesC16& aAdditionalInfo );  

    private: // Data

        // Pointer to the notifications tsy class
        CSatNotificationsTsy* iNotificationsTsy;
		// Package received from ETel 
        RSat::TLanguageNotificationV2Pckg* iLanguageNotificationV2Pckg;
    };

#endif // CSATNOTIFYLANGUAGENOTIFICATION_H