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
* Name        : CSatNotifyMoSmControlRequest.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Class for mobile oriented short message control
* Version     : 1.0
*
*/



#ifndef CSATNOTIFYMOSMCONTROLREQUEST_H
#define CSATNOTIFYMOSMCONTROLREQUEST_H


//  INCLUDES
#include "MSatNotificationsBase.h"

// FORWARD DECLARATIONS
class CSatDataPackage;
class CSatNotificationsTsy;

/**
* DESCRIPTION
* SAT MO-SMS Control notification class.
* Used when client requests to be notified when MO SM control request
* command arrives from SIM.
*
* This is used especially for returning AlphaId and
* ControlResult to SatServer.
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatNotifyMoSmControlRequest ) :  public CBase,
							public MSatNotificationsBase
    {           
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * return CSatNotifyMoSmControlRequest*: created object 
        */
        static CSatNotifyMoSmControlRequest* NewL( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatNotifyMoSmControlRequest();

	private:
        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * @return None
        */
        CSatNotifyMoSmControlRequest( CSatNotificationsTsy* aNotificationsTsy );

        /**
        * Class attributes are created in ConstructL. 
        * @param None
        * @return None
        */
        void ConstructL();

    public: // Functions from base classes
         
        
        /**
        * GetInput Notification request received from client
        * @param aTsyReqHandle request handle from ETel
        * @param aPackage: Packed data
        * @return KErrNone
        */
        TInt Notify( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

		/**
        * Cancels notification request for GetInput.
        * @param aReqHandle notify request handle
        * @return KErrNone
        */
        TInt CancelNotification( const TTsyReqHandle aReqHandle );
        
		/**
        * Completes GetInput message to client
        * @param aDataPackage: Packaged return data
        * @param aErrorCode: Possible error code
        * @return KErrNone/ KErrCorrupt
        */
        TInt CompleteNotifyL( CSatDataPackage* aDataPackage, TInt aErrorCode );
			
	public: // New methods
			
		/*
        * Create an MO-SMS envelope to in order to check wheter the SMS
        * can be sent or not. The response will be used to send forward
        * to the server needing the information
        * @param aDataPackage   contains aAddressData1 RP address and
        *                       aAddressData2 TP address
        * @param aResult KErrNone
        */
        void CreateMoSmsCtrlEnvelopeL( CSatDataPackage* aDataPackage, 
        	TInt aResult ); 
            
        /*
        * Set iIsMoSmsCtrlActivated to the given value
        * @param aDataPackage   contains aAddressData1 RP address and
        *                       aAddressData2 TP address
        * @return none
        */
        void SetActivationStatus( CSatDataPackage* aDataPackage );
        	
        /*
        * Returns MO-SMS Control activation status
        * If ETrue: Allows this object to process incoming Mo-Sm requests
        * If EFalse: Prevents this object to process incoming Mo-Sm requests
        * @return activation status
        */
        TBool ActivationStatus();    
        
        /*
        * This method cleans the address data, so that all the byte found
        * after a 0xXF or a 0xFX are ignored and removed.
        * @param  aAddr Address data to be cleaned
        * @return None
        */
        void CleanAddressData( TDes8& aAddr );
        
        /*
        * Extracts data from AtkSwDataNtf message data part
        * @param aAtkData, TLV data from NAA
        * @param aAddr1, contains the RP address provided by the SIM
        * @param aAddr2, contains the TP address provided by the SIM
        * @param aAlphaId, contains the alpha id provided by the SIM
        * @return none
        */
        void ParseNotification( const TDesC8& aAtkData, TDes8& aAddr1, 
            TDes8& aAddr2, RSat::TAlphaId& aAlphaId );
        	
       	/*
        * This method checks the consistency of the data sent by the NAA to 
        * the ME, when the call is allowed and modified.
        * @param aRPAddr SMSC RP address
        * @param aTPAddr  TP Destinatiion address
        * @return ETtrue if data is consistent, else EFalse
        */
        TBool VerifyNotificationData( TDes8& aRPAddr, TDes8& aTPAddr );
        
    private: // Data
        
        // Pointer to the notifications tsy class
        CSatNotificationsTsy* iNotificationsTsy;
        
        // Notify Call Control struct. Package allocated
        // in this class and received from ETel
        RSat::TMoSmControlV1Pckg* iMoSmControlV1Pckg;
        
        // Boolean to check whether MO-SMS Control is activated. This is used 
        // by CTsySatMessaging::MoSmsControlReceived, in order to know if this
        // object is in use or not
        TBool iIsMoSmsCtrlActivated;
       
    };

#endif // CSATNOTIFYMOSMCONTROLREQUEST_H
