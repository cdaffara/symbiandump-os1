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
* Name        : CSatTsy.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Main entry point for Etel requests
* Version     : 1.0    
*
*/



#ifndef CSATTSY_H
#define CSATTSY_H

//  INCLUDES
#include <e32base.h>				// Etel base
#include <etelsat.h>				// Etel sat
#include <ctsy/serviceapi/ctsysatmessagingbase.h>	// SatMessaging Base class 

#include "CSatTimer.h"				// SatTimer class
// CONSTANTS 
// Max address length
const TUint8 KAddrMaxLength = 255;
const TUint8 KCountryCodeLength = 3;

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMmSmsTsy;
class CSatMessageRouterBase;
class CSatCCTsy;
class CSatIconTsy;
class CMmMessageRouterBase;
class CMmMessageManagerBase;
class CMmMessageManager;
class CSatTsyReqHandleStore;
class CSatNotificationsTsy;
class CSatDataDownloadTsy;
class CSatEventDownloadTsy;
class CTsySatMessaging;

class MMessageRouter;
class MCtsySatService;
class MmMessageManagerCallback;


// DESCRIPTION
// SAT TSY class.
//
// This class is a main entry point for all the requests coming from Etel.
// The requests are routed to specific Tsy if such exists, otherwise the 
// request is handled here by calling the corresponding method directly.
// e.g. SMS message sending and menu selection SAT features are handled here.
// The functions that are not supported are completed at arrival.

/*
*  @lib Commonsimatktsy
*  @since 3.1
*/
NONSHARABLE_CLASS ( CSatTsy ) : public CTsySatMessagingBase
    {
    public: // Enumerations

        enum TSatRequestTypes
            {
            ESatReqHandleUnknown, // 0
            ESatNotifyDisplayTextPCmdReqType,
            ESatNotifyProactiveSimSessionEndReqType,
            ESatNotifyGetInkeyPCmdReqType,
			ESatNotifyGetInputPCmdReqType,
			ESatNotifyPlayTonePCmdReqType,
			ESatNotifySetUpMenuPCmdReqType,
			ESatNotifySelectItemPCmdReqType,
			ESatNotifySendSmPCmdReqType,
			ESatNotifySendSsPCmdReqType,
			ESatNotifySetUpEventListPCmdReqType,
            ESatNotifySendDtmfPCmdReqType,
            ESatNotifyPollingOffPCmdReqType,
            ESatNotifyPollIntervalPCmdReqType,
			ESatNotifySendUssdPCmdReqType,
			ESatNotifySetUpIdleModeTextPCmdReqType,
			ESatNotifyLaunchBrowserPCmdReqType,
			ESatNotifyCallControlRequestPCmdReqType,
            ESatNotifySetUpCallPCmdReqType,
            ESatNotifyLanguageNotificationPCmdReqType,
            ESatNotifyLocalInfoPCmdReqType,
            ESatNotifyRefreshPCmdReqType,
            ESatNotifyRefreshRequiredPCmdReqType,
            ESatNotifyOpenChannelPCmdReqType,
            ESatNotifyGetChannelStatusPCmdReqType,
            ESatNotifyMoSmControlRequestPCmdReqType,
            ESatNotifyCloseChannelPCmdReqType,
            ESatNotifySendDataPCmdReqType,
            ESatNotifyReceiveDataPCmdReqType,
            ESatGetIconReqType,
		    ESatGetImageInstanceReqType,
		    ESatGetClutReqType,
			ESatTerminalRspReqType,
			ESatSendMessageNoLoggingReqType,
            // ATTENTION:: Declare constant for those requests that need
            // own request handle record in iSatTsyReqHandleStore above the
            // following!
            ESatMaxNumOfRequests
            };
        
    	/*
        * This structure contains data needed for LocationStatus
        * envelope. (used by EventDownload and MoSmContol)
        */
        struct TLocationStatusEnvelope 
            {
            TUint8 iLocationStatus;  // Location satus
            TBuf8<KCountryCodeLength> iCountryCode;     // Country code
            TUint16 iLocationAreaCode;// Location area code
            TUint16 iCellId;          // Cell id
            };

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aMmPhoneTsy: Pointer to TSY phone object 
        * @param aMmMessageRouter: Pointer to Message router base in Common TSY
        * @param aName: Sat application fixed name
        * @param aTsySatMessaging: SAT messaging pointer
        * @return CSatTsy        
        */
        static CSatTsy* NewL( MMessageRouter& aMmMessageRouter, MCtsySatService& aSatService);
                                  
        /**
        * Destructor.
        */
        ~CSatTsy();
        
    private:

        /**
        * C++ default constructor.
        * @param aMmPhone: Pointer to TSY phone object 
        * @param aName: Sat application fixed name
        * @param aTsySatMessaging: Sat messaging pointer
        */
        CSatTsy( MMessageRouter& aMmMessageRouter, MCtsySatService& SatService);
            
    	/**
        * By default EPOC constructor is private.
        * @param aMmPhoneTsy: Pointer to TSY phone object 
        */
        void ConstructL();
        
    public: // Functions from base classes
        
        /**
        * Command handler for Etel messages.
        * @param aTsyReqHandle: Tsy request handle
        * @param aIpc: command identifier
        * @param aPackage: Data pointer
        * @return status
        */
        TInt ExtFunc( const TTsyReqHandle aTsyReqHandle, 
			const TInt aIpc, const TDataPackage& aPackage );

        /**
        * Saves the request type to request handle table
        * @param aTsyReqHandle: Tsy request handle
        * @param aSatRequestType: Request type to be saved
        */
        void SaveReqHandle( const TTsyReqHandle aTsyReqHandle, 
			const TSatRequestTypes aSatRequestType );
                                                     
        /**
        * ReqMode method for checking what modes this component supports
        * @param aIpc: Command identifier
        * @return reqmode
        */
        CTelObject::TReqMode ReqModeL( const TInt aIPC );
        
        /**
        * Cancels the ongoing service
        * @param aIpc: Command identifier
        * @param aTsyReqHandle: Tsy request handle
        * @return status
        */
        TInt CancelService( const TInt aIpc, 
			const TTsyReqHandle aTsyReqHandle );
			        
        /**
        * Register given command, checks if the command is supported
        * @param aIpc: Command identifier
        * @return status
        */
        TInt RegisterNotification( const TInt aIpc );
        
        /**
        * Deregisters given command, checks if the command is supported
        * @param aIpc: Command identifier
        * @return status
        */
        TInt DeregisterNotification( const TInt aIpc );
        
        /**
        * Returns number of slots to be used for given IPC
        * @param aIpc: Command identifier
        * @return status
        */
        TInt NumberOfSlotsL( const TInt aIpc );
                
    public: // New functions
        
        /**
        * Completes requests to ETel
        * @param aReqHandle: Registered req handle
        * @param aRet: return value
        */
        void ReqCompleted( TTsyReqHandle aReqHandle,
        	TInt aRet );
        /**
        * Cache call connected envelope
        * @param aEnvelope Call connected envelope
        */
        void StoreCallConnectedEvent( const TDesC8& aEnvelope );
        
        /**
        * Set Status of SetUpCall command to CSatEventDownloadTsy class.
        * @param aStatus status of Set up Call proactive command
        */        
        void SetSetUpCallStatus( const TBool aStatus );
		
        /**
        * Set up events to report for USIM.
        * @param  aEvents: bit mask
        */
        void SetUpEventList( TUint32 aEvents );
    
        /**
        * Returns pointer to the message manager
        * @return CMmMessageManager*: pointer to the requested object
        */
        CMmMessageManagerBase* MessageManager();
        
        /**
        * Returns pointer to the Call Control TSY 
        * @return CSatCCTsy*: pointer to the requested object
        */
        CSatCCTsy* SatCCTsy();

		/**
        * Get pointer to SAT request handle
        * @return CSatTsyReqHandleStore*:
        */
		CSatTsyReqHandleStore* GetSatReqHandleStore();
        
        /**
        * Get pointer to request handle type
        * @return pointer to request handle type
        */
        TSatRequestTypes* GetReqHandleType();
        
        /**
        * Reports to CSatCCTsy whether call or SS has been activated by PCmd
        * @param aAddress: Destination address of proactive command or NULL
        */
        void StoreProactiveAddress( const TDesC8* aAddress );  
        
        /**
        * Set Internal TON and NPI
        * @param aTonNpi: TON and NPI
        */
        void SetTonNpi( const TUint8 aTonNpi );
        
        MmMessageManagerCallback* GetMessageManagerCallback();
  
  public: //from    MStkTsySatService   
       /**
         * From MStkTsySatService 
         * Dummy implementation for CompleteSendSmsMessage function
         *          
         * @param aError Error value
         * @return Return always KErrNone
         */ 
        virtual TInt CompleteSendSmsMessage( TInt aError );
    
        /**
         * From MStkTsySatService 
         * Dummy implementation for IsMoSmControlBySimActivated function
         *          
         * @return Return always EFalse
         */     
        virtual TBool IsMoSmControlBySimActivated();

#ifdef USING_CTSY_DISPATCHER
  public: //from    MStkTsyCallOrigin
       /**
         * From MStkTsyCallOrigin 
         * Dummy implementation for IsSimOriginatedCall function
         *          
         * @param aAddress The number whose origin is required.
         * @return ETrue if call originates from a Proactive Command
         */ 
	  	virtual TBool IsSimOriginatedCall( const TDesC8& aAddress );
#endif //USING_CTSY_DISPATCHER
    private:
    
    	/**
        * Handles extended client requests. Uses TRAP to ensure functioning on
        * memory allocation failure
        * @param aTsyReqHandle: TSY request handle
        * @param aIpc: Request IPC number
        * @param aPackage: Packaged request parameters
        * @return result of the request
        */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, const TDataPackage& aPackage );

		/**
        * Menu selection command handler from client application.
        * @param aTsyReqHandle: TSY request handle
		* @param aSelection: Packed menu item nr and help item indicator
        * @return status
        */
        TInt MenuSelectionL( TTsyReqHandle aTsyReqHandle, 
			RSat::TMenuSelectionV1Pckg* aSelection );

        /**
        * Send SMS No Logging command handler from client application.
        * @param aMsg pointer to msg
        * @param aTsyReqHandle TsyReqHandle
        * @return status
        */
        TInt SendMessageNoLogging( TDes8* aMsg, TTsyReqHandle aTsyReqHandle );     

    private:  // Data

        // A table for SAT request handles
        TTsyReqHandle iSatReqHandles[ESatMaxNumOfRequests];
        // Request handle type
        TSatRequestTypes iReqHandleType;
        // Request handle array type
        TSatRequestTypes iReqHandleArrayType;
        // Message manager
        CMmMessageManagerBase* iMessageManager;
        // SMS messaging class pointer for sending SMS messages
        CMmSmsTsy* iMmSmsTsy;
         // Store for request handles
        CSatTsyReqHandleStore* iSatReqHandleStore;

		// Pointer to the CCTsy class
        CSatCCTsy* iSatCCTsy;
		// Pointer to the IconTsy class
		CSatIconTsy* iSatIconTsy; 
		// Pointer to the NotificationsTsy class
		CSatNotificationsTsy* iSatNotificationsTsy;
		// Pointer to the DataDownloadTsy class
        CSatDataDownloadTsy* iSatDataDownloadTsy;
		// Pointer to the EventDownloadTsy class
		CSatEventDownloadTsy* iSatEventDownloadTsy;
		
		MMessageRouter& iMessageRouter;
		MCtsySatService& iSatService;
				
    };

#endif  // CSATTSY_H

// End of File
