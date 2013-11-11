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
* Name        : CSatNotifyOpenChannel.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Specific notifications class
* Version     : 1.0
*
*/



#ifndef CSATNOTIFYOPENCHANNEL_H
#define CSATNOTIFYOPENCHANNEL_H


//  INCLUDES
#include <etelsat.h>
#include "MSatNotificationsBase.h"

// FORWARD DECLARATIONS
class CSatDataPackage;
class CBerTlv;

/**
* DESCRIPTION
* SAT Open channel notification class.
* Created when client requests to be notified when Open Channel
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
NONSHARABLE_CLASS ( CSatNotifyOpenChannel ) : public CBase,
                              public MSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * return CSatNotifyOpenChannel*: created object 
        */
        static CSatNotifyOpenChannel* NewL( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatNotifyOpenChannel();

	private:
	
        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * @return None
        */
        CSatNotifyOpenChannel( CSatNotificationsTsy* aNotificationsTsy );

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
        * @param aDataPackage: Packed return data
        * @param aErrorCode: Possible error code
        * @return KErrNone
        */
        TInt CompleteNotifyL( CSatDataPackage* aDataPackage, TInt aErrorCode );

        /**
        * Handles the Display text terminal response
        * @param aRsp: Response structure
        * @return Success/ Failure value
        */
        TInt TerminalResponseL( TDes8* aRsp );

	private: // New functions

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
    	
		RSat::TOpenChannelBaseV2Pckg* iOpenBaseChannelV2Pckg;
        // Open Cs Channel package store. Package allocated
        // in this class and received from ETel
        RSat::TOpenCsChannelV2Pckg* iOpenCsChannelV2Pckg;
        // Open Gprs Channel package store. Package allocated
        // in this class and received from ETel
        RSat::TOpenGprsChannelV4Pckg* iOpenGprsChannelV4Pckg;
        // Open Local Link Channel package store. Package allocated
        // in this class and received from ETel
        RSat::TOpenLocalLinksChannelV2Pckg* iOpenLocalLinksChannelV2Pckg;
        // Pointer to the notifications tsy class
        CSatNotificationsTsy* iNotificationsTsy;
		// Bearer type
		TUint8 iBearerType;
		// Bearer
		TBuf8<RSat::KBearerParamsMaxSize> iBearer;
		// Buffer size
        TUint16 iBufferSize;
    };

#endif // CSATNOTIFYOPENCHANNEL_H