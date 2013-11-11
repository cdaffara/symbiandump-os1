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
* Name        : CSatNotifyRefresh.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Specific notifications class
* Version     : 1.0
*
*/



#ifndef CSATNOTIFYREFRESH_H
#define CSATNOTIFYREFRESH_H


//  INCLUDES
#include <etelsat.h>
#include "MSatNotificationsBase.h"


// FORWARD DECLARATIONS
class CSatDataPackage;
class CSatNotificationsTsy;

/**
* DESCRIPTION
* SAT Refresh notification class.
* Created when client requests to be notified when Refresh
* command arrives from SIM.
*
* The purpose of this command is to enable the SAT
* (which alerts other applications in the phone) to be notified
* of the changes to the SIM configuration that have occurred as
* the result of a SIM application activity. It is up to SAT to
* ensure this is done correctly.
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatNotifyRefresh ) : public CBase,
                              public MSatNotificationsBase
    {
    private:    // Enumerations
    
        // Enumaration for refresh (SimInit) + change of EfImsi    
        enum TImsiState
            {
            ENotSet,
            ERefreshSimInitReceived,
            EImsiNotEqual
            };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * return CSatNotifyRefresh*: created object 
        */
        static CSatNotifyRefresh* NewL( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatNotifyRefresh();

	private:
	
        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * @return None
        */
        CSatNotifyRefresh( CSatNotificationsTsy* aNotificationsTsy );

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
        * Cancels notification of RefreshRequiredParams request.
        * @param aReqHandle notify request handle
        * @return KErrNone
        */
        TInt CancelNotification( const TTsyReqHandle aReqHandle );
        
        /**
        * Completes the notification request received from client
        * @param aDataPackage: Packed return data
        * @param aErrorCode: Possible error code
        * @return KErrNone/ KErrCorrupt
        */
        TInt CompleteNotifyL( CSatDataPackage* aDataPackage, TInt aErrorCode );

        /**
        * Handles the refresh terminal response
        * @param aRsp: Response structure
        * @return Success/ Failure indication
        */
        TInt TerminalResponseL( TDes8* aRsp );
        
        /**
        * Completes the indication notifying the change of IMSI
        * @return none
        */
        void CompleteImsiChange();

    public: // New methods
        
        /**
        * Create notification specific terminal response data
        * @param aPCmdNumber: Proactive command number
        * @param aGeneralResult: Result of the proactive command
        * @param aAdditionalInfo: Additional info for terminal response
        * @return Success/Failure value
        */
        TInt CreateTerminalRespL( TUint8 aPCmdNumber, TUint8 aGeneralResult,
                                  TDesC16& aAdditionalInfo );
                                          
        /**
        * Notification request for RefreshRequired received from client
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return KErrNone
        */
        TInt NotifyRefreshRequired( const TTsyReqHandle aRegHandle, 
                                    const TDataPackage& aPackage );
		
		/**
        * Cancels notification of RefreshRequiredParams request.
        * @param aReqHandle notify request handle
        * @return KErrNone
        */                            
        TInt CancelRefreshRequiredNotification( 
        		const TTsyReqHandle aReqHandle );
        
        /**
        * Response for RefreshRequired notification
        * @param aPackage response structure
        * @return Success/ Failure indication
        */
        TInt RefreshAllowedL( const TDataPackage& aPackage );
                
        /**
        * Forwards terminal response data to CreateTerminalRespL - method
        * @param aDataPackage packed data
        * @param aErrorCode	error code
        * @return Success/ Failure indication
        */
        TInt CompleteTerminalRespDataL( CSatDataPackage* aDataPackage,   
                                  	TInt aErrorCode );
                                  
        /**
        * Completes refresh request
        * @param aDataPackage packed data
        * @param aErrorCode
        * @return None
        */        
        void CompleteRequest( CSatDataPackage* aDataPackage,
        					TInt aErrorCode );                                  

    private: // Data
		
		// Refresh package store. Package received from ETel
        RSat::TRefreshV2Pckg* iRefreshV2Pckg;
        // Notify refresh Required package store
        RSat::TRefreshV2Pckg* iRefreshRequiredV2Pckg;
        // File list with full path
        TBuf8<RSat::KRefreshedFileListMaxSize> iFileList;
        // To store Aplication id
        TBuf8<RSat::KAidMaxSize> iAid;
		// List of files that are cached in SIM ATK TSY
        TUint16 iInternalCache;
        // Pointer to the notifications tsy class
        CSatNotificationsTsy* iNotificationsTsy;
        // Current state of EfImsi and refresh (SimInit)
        TImsiState iImsiState;

    };

#endif // CSATNOTIFYREFRESH_H