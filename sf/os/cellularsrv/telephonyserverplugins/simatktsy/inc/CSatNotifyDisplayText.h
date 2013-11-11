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
* Name        : CSatNotifyDisplayText.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Specific notifications class
* Version     : 1.0
*
*/



#ifndef CSATNOTIFYDISPLAYTEXT_H
#define CSATNOTIFYDISPLAYTEXT_H


//  INCLUDES
#include <etelsat.h>
#include "MSatNotificationsBase.h"

// FORWARD DECLARATIONS
class CSatDataPackage;
class CSatNotificationsTsy;

/**
* DESCRIPTION
* SAT Display Text notification class.
* Created when client requests to be notified when Display Text
* command arrives from SIM.
*
* This command instructs the SAT to display a text message.
* It allows SIM to define the priority of that message, 
* and the text string format.
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatNotifyDisplayText ) : public CBase,
                              public MSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * return CSatNotifyDisplayText*: created object 
        */
        static CSatNotifyDisplayText* NewL( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatNotifyDisplayText();

	private:
        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * @return None
        */
        CSatNotifyDisplayText( CSatNotificationsTsy* aNotificationsTsy );

        /**
        * Class attributes are created in ConstructL. 
        * @param None
        * @return None
        */
        void ConstructL();

    public: // Functions from base classes
                
        /**
        * Notification request received from client
        * @param aTsyReqHandle: request handle from ETel
        * @param aPackage: Packed data
        * @return KErrNone
        */
        TInt Notify( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

        /**
        * Cancels notification request about display text change.
        * @param aReqHandle notify request handle
        * @return KErrNone
        */
        TInt CancelNotification( const TTsyReqHandle aReqHandle );
        
        /**
        * Completes the notification request received from client
        * @param aDataPackage: Packaged return data
        * @param aErrorCode: Possible error code
        * @return KErrNone/ KErrCorrupt
        */
        TInt CompleteNotifyL( CSatDataPackage* aDataPackage, TInt aErrorCode );

        /**
        * Handles the Display text terminal response
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
                                 const TDesC16& aAdditionalInfo );
        

    private:    // Data

        // Package received from ETel, used by CSatNotifyDisplayText
        RSat::TDisplayTextV2Pckg* iDisplayTextV2Pckg;
        // Pointer to the notifications tsy class
        CSatNotificationsTsy* iNotificationsTsy;

    };

#endif // CSATNOTIFYDISPLAYTEXT_H