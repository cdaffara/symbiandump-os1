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
* Name        : CSatNotifySendSm.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Specific notifications class
* Version     : 1.0
*
*/



#ifndef CSATNOTIFYSENDSM_H
#define CSATNOTIFYSENDSM_H


//  INCLUDES
#include <etelsat.h>
#include "MSatNotificationsBase.h"

// FORWARD DECLARATIONS
class CSatDataPackage;
class CSatNotificationsTsy;
class CBerTlv;
/**
* DESCRIPTION
* SAT Send SM notification class.
* Created when client requests to be notified when Send SM
* command arrives from NAA.
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatNotifySendSm ) : public CBase,
                           public MSatNotificationsBase
    {
    public:   // Constructors and destructor
       
        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * return created object 
        */
        static CSatNotifySendSm* NewL( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatNotifySendSm();

	private:
	
        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * @return None
        */
        CSatNotifySendSm( CSatNotificationsTsy* aNotificationsTsy );

        /**
        * Class attributes are created in ConstructL. 
        * @param None
        * @return None
        */
        void ConstructL();

    public: // Functions from base classes

        /**
        * Send Sm Notification request received from client
        * @param aTsyReqHandle: Request handle from ETel
        * @param aPackage: Packed data
        * @return KErrNone
        */
        TInt Notify( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

        /**
        * Cancels notification request for Send Sm.
        * @param aReqHandle: Notify request handle
        * @return KErrNone
        */
        TInt CancelNotification( const TTsyReqHandle aReqHandle );
                 
        /**
        * Completes Send Sm message to client
        * @param aDataPackage: Packaged return data
        * @param aErrorCode: Possible error code
        * @return KErrNone/ KErrCorrupt
        */
        TInt CompleteNotifyL( CSatDataPackage* aDataPackage, TInt aErrorCode );

        /**
        * From CSatNotificationsBase handles Send Sm terminal response 
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

	private: // New functions

		/**
        * Set Alpha id and Address into send sm structure
        * @param aBerTlv: bertlv data
        * @param aSendSmV1: Etel sm structure
        * @return None
        */
		void SetAlphaIdAndAddressData( CBerTlv* aBerTlv, 
			RSat::TSendSmV1& aSendSmV1 );
			
		/**
        * Check TPDU validity
        * @param aTpdu: TPDU to be checked
        * @return KErrNone or KErrCorrupt
        */
        TInt CheckTpdu( TPtrC8 aTpdu );

		/**
        * Get TP-UDL(User Data Length) field from Tpdu
        * @param aTpdu: Tpdu where to read TP-UDL or TP-CDL
        * @return TP-UDL
        */
        TUint8 GetTpUdl( TPtrC8 aTpdu );

		/**
        * Get TP-DCS field from Tpdu
        * @param aTpdu: TPDU to be analyzed
        * @return TP-DCS
        */
        TUint8 GetTpDcs( TPtrC8 aTpdu );

		/**
        * Pack SMS if it is required by the sim
        * @param aTpdu: Received Tpdu
        * @param aSendSm: Package where to store packed sms
        * @return KErrNone/KErrCorrupt
        */
        TInt PackSms( TPtrC8 aTpdu, TTpdu& aSendSm );

    private: // Data

        // Send Sm package store. Package received from ETel
        RSat::TSendSmV1Pckg* iSendSmV1Pckg;
        // Requested response format
        TUint8 iRequestedRspFormat;
        // Pointer to the notifications Tsy class
        CSatNotificationsTsy* iNotificationsTsy;
    };

#endif // CSATNOTIFYSENDSM_H