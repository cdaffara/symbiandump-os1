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
* Name        : CSatNotifyLaunchBrowser.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Specific notifications class
* Version     : 1.0
*
*/



#ifndef CSATNOTIFYLAUNCHBROWSER_H
#define CSATNOTIFYLAUNCHBROWSER_H


//  INCLUDES
#include <etelsat.h>
#include "MSatNotificationsBase.h"
#include "CSatNotificationsTsy.h"

// FORWARD DECLARATIONS
class CSatDataPackage;
class CBerTlv;

/**
* DESCRIPTION
* SAT Notify launch browser notification class.
* Created when client requests to be notified if a request (of any other 
* existing client) has been modified.
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatNotifyLaunchBrowser ) : public CBase,
                           public MSatNotificationsBase
    {  
    public:   // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * return CSatNotifyLaunchBrowser*: created object 
        */
        static CSatNotifyLaunchBrowser* NewL( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatNotifyLaunchBrowser();

	private:
        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * @return None
        */
        CSatNotifyLaunchBrowser( CSatNotificationsTsy* aNotificationsTsy );

        /**
        * Class attributes are created in ConstructL. 
        * @param None
        * @return None
        */
        void ConstructL();

    public: // Functions from base classes

        /**
        * Launch Browser Notification request received from client
        * @param aTsyReqHandle: request handle from ETel
        * @param aPackage: Packed data
        * @return KErrNone
        */
        TInt Notify( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

        /**
        * Cancels notification request for Launch Browser.
        * @param aReqHandle notify request handle
        * @return KErrNone
        */
        TInt CancelNotification( const TTsyReqHandle aReqHandle );
                 
        /**
        * Completes Launch Browser message to client
        * @param aDataPackage: Packaged return data
        * @param aErrorCode: Possible error code
        * @return KErrNone/ KErrCorrupt
        */
        TInt CompleteNotifyL( CSatDataPackage* aDataPackage, TInt aErrorCode );

        /**
        * From CSatNotificationsBase handles Launch Browser terminal response 
        * coming from client.
        * @param aRsp: Response structure
        * @return KErrNone/ KErrCorrupt
        */
        TInt TerminalResponseL( TDes8* aRsp );
        
        /**
        * Stores Ss status information.
        * @param aDataPackage: contains status information
        * @return none
        */    					 
        void SetSsStatus( CSatDataPackage* aDataPackage );

	private: // New functions
	
	    /**
        * Help method for CompleteNotify, parsing Browse ID data
        * @param aPCmdNumber: Proactive Command number
        * @param aBerTlv: Source data
        * @param aLaunchBrowserV2: Target data
        * @return Success/Failure value
        */
	    TInt ParseBrowserIdL( TUint8 aPCmdNumber, CBerTlv& aBerTlv, 
	        RSat::TLaunchBrowserV2& aLaunchBrowserV2 );
	
	    /**
        * Help method for CompleteNotify, parsing URL data
        * @param aPCmdNumber: Proactive Command number
        * @param aBerTlv: Source data
        * @param aLaunchBrowserV2: Target data
        * @return Success/Failure value
        */
	    TInt ParseUrlL( TUint8 aPCmdNumber, CBerTlv& aBerTlv, 
	        RSat::TLaunchBrowserV2& aLaunchBrowserV2 );
	
	    /**
        * Help method for CompleteNotify, parsing Provisioning file list data
        * @param aPCmdNumber: Proactive Command number
        * @param aBerTlv: Source data
        * @param aLaunchBrowserV2: Target data
        * @return Success/Failure value
        */
	    TInt ParseProvisioningFileListL( TUint8 aPCmdNumber, CBerTlv& aBerTlv, 
	        RSat::TLaunchBrowserV2& aLaunchBrowserV2 );
	        
	    /**
        * Help method for CompleteNotify, parsing Bearer List data
        * @param aPCmdNumber: Proactive Command number
        * @param aBerTlv: Source data
        * @param aLaunchBrowserV2: Target data
        * @return Success/Failure value
        */
	    TInt ParseBearerListL( TUint8 aPCmdNumber, CBerTlv& aBerTlv, 
	        RSat::TLaunchBrowserV2& aLaunchBrowserV2 );
	        
        /**
        * Create notification specific terminal response data
        * @param aPCmdNumber: Proactive command number
        * @param aGeneralResult: Result of the proactive command
        * @param aAdditionalInfo: Additional info for terminal response
        * @return Success/Failure value
        */
        TInt CreateTerminalRespL( TUint8 aPCmdNumber, TUint8 aGeneralResult,
            TUint8 aAdditionalInfo );  

    private: // Data

        // Notify Launch Browser struct
        RSat::TLaunchBrowserV2Pckg* iLaunchBrowserV2Pckg;
        // Pointer to the notifications tsy class
        CSatNotificationsTsy* iNotificationsTsy;
        
        // Ss Status
        CSatNotificationsTsy::TSsStatus iSsStatus;
    };

#endif // CSATNOTIFYGETINKEY_H
