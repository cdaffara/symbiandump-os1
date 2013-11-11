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
* Name        : CSatNotifyGetInkey.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Specific notifications class
* Version     : 1.0
*
*/



#ifndef CSATNOTIFYGETINKEY_H
#define CSATNOTIFYGETINKEY_H


//  INCLUDES
#include <etelsat.h>
#include "MSatNotificationsBase.h"

// FORWARD DECLARATIONS
class CSatDataPackage;
class CSatNotificationsTsy;

/**
* DESCRIPTION
* SAT Get Inkey notification class.
* Created when client requests to be notified when Get Inkey
* command arrives from SIM.
*
* This command instructs the SAT to display text and expect the user
* to enter a single character. Any response entered by the user shall
* be passed transparently by the SAT to the SIM.
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatNotifyGetInkey ) : public CBase,
                           public MSatNotificationsBase
    {
    public:   // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * return CSatNotifyGetInkey*: created object 
        */
        static CSatNotifyGetInkey* NewL( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatNotifyGetInkey();

	private:
	
        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * @return None
        */
        CSatNotifyGetInkey( CSatNotificationsTsy* aNotificationsTsy );

        /**
        * Class attributes are created in ConstructL. 
        * @param None
        * @return None
        */
        void ConstructL();

    public: // Functions from base classes

        /**
        * Get Inkey Notification request received from client
        * @param aTsyReqHandle: request handle from ETel
        * @param aPackage: Packed data
        * @return KErrNone
        */
        TInt Notify( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

        /**
        * Cancels notification request for GetInkey.
        * @param aReqHandle notify request handle
        * @return KErrNone
        */
        TInt CancelNotification( const TTsyReqHandle aReqHandle );
                 
        /**
        * Completes GetInkey message to client
        * @param aDataPackage: Packaged return data
        * @param aErrorCode: Possible error code
        * @return KErrNone/ KErrCorrupt
        */
        TInt CompleteNotifyL( CSatDataPackage* aDataPackage, TInt aErrorCode );

        /**
        * From CSatNotificationsBase handles Get Inkey terminal response 
        * coming from client.
        * @param aRsp: Response structure
        * @return KErrNone/ KErrCorrupt
        */
        TInt TerminalResponseL( TDes8* aRsp );

    public: // New methods 
    
        /**
        * Create notification specific terminal response data
        * @param aPCmdNumber: Proactive command number
        * @param aGeneralResult: Result of the proactive command
        * @param aAdditionalInfo: Additional info for terminal response
        * @param aDcs: Data Coding Scheme
        * @return Success/Failure value
        */
        TInt CreateTerminalRespL( TUint8 aPCmdNumber, TUint8 aGeneralResult,
           						 TDesC16& aAdditionalInfo, TUint8 aDcs = NULL );  

    private: // Data

        // Get inkey package store. Package allocated
        // in this class and received from ETel
        RSat::TGetInkeyV2Pckg* iGetInkeyV2Pckg;
        // Requested response format
        TUint8 iRequestedRspFormat;
        // Pointer to the notifications tsy class
        CSatNotificationsTsy* iNotificationsTsy;
    };


#endif // CSATNOTIFYGETINKEY_H