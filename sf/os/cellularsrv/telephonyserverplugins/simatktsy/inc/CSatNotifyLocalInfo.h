/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : CSatNotifyLocalInfo.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Specific notifications class
* Version     : 1.0
*
*/



#ifndef CSATNOTIFYLOCALINFO_H
#define CSATNOTIFYLOCALINFO_H


//  INCLUDES
#include "MSatNotificationsBase.h"  // The base class header
#include "TSatUtility.h"            // For operator code length
#include "CSatNotificationsTsy.h"   // For Local Information

// FORWARD DECLARATIONS
class CSatDataPackage;
class TTlv;


/**
* DESCRIPTION
* SAT Notify local information notification class.
* This command is handled completely in the TSY and does not require any 
* interaction with the Symbian OS side.
* @lib Commonsimatktsy
* @since 3.1 
*/
NONSHARABLE_CLASS ( CSatNotifyLocalInfo ) : public CBase,
                              public MSatNotificationsBase
    {	        
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * @return created object 
        */
        static CSatNotifyLocalInfo* NewL( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatNotifyLocalInfo();

	private:
        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        */
        CSatNotifyLocalInfo( CSatNotificationsTsy* aNotificationsTsy );

        /**
        * Class attributes are created in ConstructL. 
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
        * Cancels notification request about Local info.
        * @param aReqHandle: Notify request handle
        * @return KErrNone
        */
        TInt CancelNotification( const TTsyReqHandle aReqHandle );
        
        /**
        * Completes the notification request received from client
        * @param aDataPackage: Packaged return data
        * @param aErrorCode: Possible error code
        * @return KErrNone/KErrCorrupt
        */
        TInt CompleteNotifyL( CSatDataPackage* aDataPackage, TInt aErrorCode );

        /**
        * Handles the Local info terminal response
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
        * @return Success/Failure value
        */
        TInt CreateTerminalRespL( TUint8 aPCmdNumber, TUint8 aGeneralResult,
                                 TDesC16& aAdditionalInfo );    
        
        /**
        * Unpacks local info response data, calls CreateTerminalRespL
        * @param aDataPackage: terminal response data
        * @param aErrorCode: Possible error code    
        * @return KErrNone
        */
        TInt CompleteTerminalRespDataL( CSatDataPackage* aDataPackage, 
                               		TInt aErrorCode );
        /**
        * Update local info data
        * @param aDataPackage: Packed location area code, cell id and operator
        * code
        * @param aErrorCode: Possible error code        
        */                               
		void CompleteUpdateLocalInfo( CSatDataPackage* aDataPackage, 
                               		TInt aErrorCode );
        /**
        * Update NMR and BCCHChannel list
        * @param aDataPackage: Packed NMR and BCCH channel list
        * @param aErrorCode: Possible error code        
        */                                                                                           
		void CompleteUpdateNetInfo( CSatDataPackage* aDataPackage, 
                       				TInt aErrorCode );
        /**
        * Update Timing advance and ME status
        * @param aDataPackage: Packed timing advance and ME status
        * @param aErrorCode: Possible error code        
        */                        		
		void CompleteUpdateTimingAdvAndMEStatus( CSatDataPackage* aDataPackage, 
                       				TInt aErrorCode );
        /**
        * Update Current access technology
        * @param aDataPackage: Packed access technology
        * @param aErrorCode: Possible error code        
        */                        		
        void CompleteUpdateAccTech( CSatDataPackage* aDataPackage, 
                       				TInt aErrorCode );
        /**
        * Update IMEI
        * @param aDataPackage: Packed IMEI
        * @param aErrorCode: Possible error code        
        */                        		
        void CompleteUpdateIMEI( CSatDataPackage* aDataPackage, 
                       				TInt aErrorCode );
        /**
        * Update Timezone
        * @param aDataPackage: Packed timezone
        * @param aErrorCode: Possible error code        
        */                         		  
        void CompleteUpdateTimeZone( CSatDataPackage* aDataPackage,
                       				TInt aErrorCode );
                       				
        /**
        * Returns the current local info data
        * @return Location info: Location area code, Operator code and Cell ID
        */
        const CSatNotificationsTsy::TLocalInformation& LocalInformation();
        
    private:

        /**
        * Set Date, Time and Timezone into local info structure
        */     
    	void SetDateTimeTimeZone();  

        /**
        * Sets NMR and BCCHChannelList to tlv data
        * @param aTlvSpecificData: tlv data structure   
        */
		void SetInformationNmr( TTlv* aTlvSpecificData );
		    	                 		                       	
    private:    // Data
	
		// Flag
        TBool iLocalInfoIsOngoing;
        // Timing advance
        TUint8 iTimingAdvance;
        // Time zone            
        TUint8 iTimeZone;
        // Imei code
        TBuf8<16> iIMEI;
        // Location info data 
        CSatNotificationsTsy::TLocalInformation iLocalInformation;
        // Network measurement result
        TBuf8<16> iNMR;
        // For storing BCCH channel list 
        TBuf<32> iBCCHChannelList;
        // Variable for storing ME Status
        TUint8 iMEStatus;
        // Variable for storing Current Access Technology
        TUint8 iCurrentAccTech;
        // Notify Local Info struct. Package allocated
        // in this class and received from ETel
        RSat::TLocalInfoV3Pckg* iLocalInfoV3Pckg; 
        // Response package
        RSat::TLocalInfoRspV3 iLocalInfoRspV3;
        // Pointer to the notifications Tsy class
        CSatNotificationsTsy* iNotificationsTsy;

    };

#endif // CSATNOTIFYLOCALINFO_H
