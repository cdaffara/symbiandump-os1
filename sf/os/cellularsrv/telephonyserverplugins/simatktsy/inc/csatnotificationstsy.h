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
* Name        : CSatNotificationsTsy.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Notifications-related functionality of Sat Tsy
* Version     : 1.0
*
*/




#ifndef CSATNOTIFICATIONSTSY_H
#define CSATNOTIFICATIONSTSY_H


//  INCLUDES
#include <e32base.h>
#include <etelsat.h>
#include <et_tsy.h>
#include "CSatTimer.h"				// Timer class
#include "TSatUtility.h"
#include "TTlv.h"

// CONSTANTS
const TUint8 KSmsHeaderIndicatorMask              = 0x40;

const TUint8 KPlusMarkCharacterCode               = 0x2B;
const TUint8 KRefreshMasterFileHeader             = 0x3F;
const TUint8 KRefresh1StLevelDedicatedFileHeader  = 0x7F;
const TUint8 KRefresh2StLevelDedicatedFileHeader  = 0x5F;

const TUint8 KMask3                               = 0x03;

const TUint8 KMaskC0                              = 0xC0;
const TUint8 KMaskF3                              = 0xF3;
const TUint8 KMaskFB                              = 0xFB;
const TUint8 KTPDUFieldStart                      = 0x04;
const TUint8 KTonNpiBit8                          = 0x80;

const TUint8 KSmsVPFEnhanced                      = 0x08;
const TUint8 KSmsVPFRelative                      = 0x10;
const TUint8 KSmsVPFAbsolute                      = 0x18;
const TUint8 KSATSmsMTISubmitOrSubmitReport       = 0x01;

const TUint8 KNoCause                             = 0x00;

const TUint8 KOnlyIfNotBusy                       = 0x00;
const TUint8 KOnlyIfNotBusyWithRedial             = 0x01;
const TUint8 KHoldOtherCalls                      = 0x02;
const TUint8 KHoldOtherCallsWithRedial            = 0x03;
const TUint8 KDisconnectOtherCalls                = 0x04;
const TUint8 KDisconnectOtherCallsWithRedial      = 0x05;

const TUint8 KMTCall                              = 0x00;
const TUint8 KCallConnected                       = 0x01;
const TUint8 KCallDisconnected                    = 0x02;
const TUint8 KLocationStatus                      = 0x03;
const TUint8 KUserActivity                        = 0x04;
const TUint8 KIdleScreenAvailable                 = 0x05;
const TUint8 KCardReaderStatus                    = 0x06;
const TUint8 KLanguageSelection                   = 0x07;
const TUint8 KBrowserTermination                  = 0x08;
const TUint8 KDataAvailable                       = 0x09;
const TUint8 KChannelStatus                       = 0x0A;
const TUint8 KAccessTechnologyChange	          = 0x0B;
const TUint8 KDisplayParamsChanges	              = 0x0C;
const TUint8 KLocalConnection               	  = 0x0D;

const TUint8 KDefaultPollInterval                 = 0x00;
const TUint8 KMinPollInterval                     = 0x05;
const TUint8 KMaxLegacyPollInterval               = 0x19;
const TUint8 KMaxPollInterval                     = 0xFF;

const TUint8 KMinTimerValue                       = 0;
const TUint  KMaxTimerValueInSeconds              = 86400;
const TUint8 KDcsCbs7bitDefault                   = 0x0F;

const TUint8 KLaunchBrowserCmdQualifierNotUsed    = 0x01;
const TUint8 KLaunchBrowserCmdQualifierReserved   = 0x04;
const TUint8 KCsdBearer                           = 0x01;
const TUint8 KGprsBearer                          = 0x03;

const TUint8 KSpecificLanguage                    = 0x01;
const TUint8 KBipCsdBearer                        = 0x01;
const TUint8 KBipGprsBearer                       = 0x02;
const TUint8 KBipDefaultBearer                    = 0x03;
const TUint8 KBipLocalLinkBearer                  = 0x04;
const TUint8 KBipSendDataImmediately              = 0x01;

const TUint8 KMEIsInTheIdleState                  = 0x00;

const TUint8 KMaxCommandDetailsLength			  = 5;

// Maksimum value for get input string
const TUint8 KGetInputStringMaxSize               = 0xEF;

// MACROS
    // None

// DATA TYPES
    // None

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATIONS
class CSatTsy;
class CSatTimer;
class CMmMessageManagerBase;
class CSatDataPackage;
class CSatTsyReqHandleStore;
class CSatNotifyDisplayText;
class CSatNotifyGetInkey;
class CSatNotifyGetInput;
class CSatNotifyPlayTone;
class CSatNotifySetUpMenu;
class CSatNotifySelectItem;
class CSatNotifySendSm;
class CSatNotifySendSs;
class CSatNotifySetUpEventList;
class CSatNotifySendUssd;
class CSatNotifySetUpIdleModeText;
class CSatNotifyLaunchBrowser;
class CSatNotifyPollInterval;
class CSatNotifyCallControlRequest;
class CSatNotifyTimerMgmt;
class CSatNotifyLanguageNotification;
class CSatNotifySendDtmf;
class CSatNotifyPollingOff;
class CSatNotifySetUpCall;
class CSatNotifySimSessionEnd;
class CSatNotifyLocalInfo;
class CSatNotifyRefresh;
class CSatNotifyOpenChannel;
class CSatNotifyGetChannelStatus;
class CSatNotifyMoSmControlRequest;
class CSatNotifyCloseChannel;
class CSatNotifySendData;
class CSatNotifyReceiveData;
class CSatNotifyMoreTime;

// CLASS DECLARATION
    // None

// DESCRIPTION
/**
* Each SAT proactive command has its own class (inherited from 
* MSatNotificationsBase), which holds the information
* relevant to the command in question. All proactive command classes are
* created when CSatNotificationsTsy object is created. This way each proactive
* command is handled by its own notify class. The proactive commands are
* encapsulated to individual classes in order to simplify the design and
* updating.
*
* CSatNotificationsTsy contains Sat proactive command handling functionality. 
* 
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatNotificationsTsy ) : public CBase
    {
    public: // Enumerations
    
		enum TCachedSimAtkFiles
			{
			ECacheEFSST = 0x01,  // EF-SST, (U)SIM Service table
			ECacheEFCBMID = 0x02,// EF-CBMID, CB DataDownload message IDs
			// Next 0x04, 0x08 etc..
			};

		/**
        * The data in this structure is updated each time a notification 
        * message is received from DOS. It provides the data needed for
        * sending the terminal response.
        * Additional info for NotifySetUpEventList, 
        * CSatNotifyLocalInfo::CompleteTerminalRespL()
        * and CSatNotifyRefresh::CompleteTerminalRespDataL()
        * ServiceType for CSatNotifyRefresh::CompleteNotifyL()
        */
        struct TTerminalRespData 
            {
            // Proactive command number
            TUint8 iPCmdNumber;   
            // Command details tlv     
            TBuf8<KMaxCommandDetailsLength>iCommandDetails;
            // General result for NotifySetUpEventList    
            TUint8 iGeneralResult;  
            // Additional info   
            TBuf<RSat::KAdditionalInfoMaxSize> iAdditionalInfo; 
            };
 
        
        /*
        * This structure contains the address data of the service center
        * needed for MO-SMS Control envelope.
        */
        struct TAddressData 
            {
            TPtrC8 iRPDestinationAddr;	// RP Destination Address
            TPtrC8 iTPDestinationAddr;	// TP Destination Address 
            };
    
        /**
        * The data in this structure is used for a response to the NAA for a
        * MO SM Control notification request. The possible values of the
        * result are defined in TSatUtility.h
        */
        struct TMoSmCtrlData 
            { 
            TAddressData iAddressData;  // RP and TP Destination addresses 
            RSat::TAlphaId*	iAlphaId;	// Pointer to the Alpha ID object
            TBool iIsAlphaIdEmpty;		// Declares id Alpha ID is empty
            TUint8 iResult;				// MO SM Ctrl Result, 
            };

        
        /*
	    * This structure contains local information.
		* Used in event handling and by CSatNotifyLocalInfo
        */
	    struct TLocalInformation
	        {
            TUint16 iLocationAreaCode;      // Location area code 
            TUint16 iCellId;                // Cell Id
            TBuf8<KOperatorCodeLength> iOperatorCode;   // Operator code
	        };
        
        /**
        * This Structure provides Refresh specific information
        * Used: CSatNotifyRefresh::RefreshAllowed()
        */
        struct TRefreshInfo
            {
            TBuf8<RSat::KRefreshedFileListMaxSize> iFileList;
            TBuf8<RSat::KAidMaxSize> iAid;
            TUint16 iInternalCache;
            };
    	/**
        * This Structure provides Ss and Ussd status
        * Used: CSatNotifyLaunchBrowser::SetSsStatus()
        */
		enum TSsStatus
			{
			ENotBusy,
			ESsBusy,
			EUssdBusy
			};
    public:

        /**
        * Two-phased constructor.
        * @param aSatTsy: Pointer to Sat TSY
        * @param aSatReqHandleStore: Pointer to request handle store
        * @return created object 
        */
        static CSatNotificationsTsy* NewL( CSatTsy* aSatTsy, 
			   CSatTsyReqHandleStore* aSatReqHandleStore );

        /**
        * C++ Destructor.
        */
        ~CSatNotificationsTsy();
    
    private:

        /**
        * By default C++ constructor is private.
        * @param aSatTsy: Pointer to Sat TSY
        * @param aSatReqHandleStore: Pointer to request handle store
        */
        CSatNotificationsTsy( CSatTsy* aSatTsy, 
			   CSatTsyReqHandleStore* aSatReqHandleStore );

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL();
    
    public: // New functions

        /**
        * Handles extended client requests.
        * @param aTsyReqHandle: Telephony service request handle
        * @param aIpc: Request IPC number
        * @param aPackage: Packed request parameters
        * @return Result of the request
        */
        TInt DoExtFuncL(
                const TTsyReqHandle aTsyReqHandle, 
                const TInt aIpc, 
                const TDataPackage& aPackage );

        /**
        * Cancels request that's IPC number and request handle are given in
        * parameters
        * @param aIpc: Request IPC number
        * @param aTsyReqHandle: TSY request handle
        * @return Result of the request
        */
        TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
        * Sets PollingOff/PollInterval response data, calls CreateTerminalRespL
        * @param aDataPackage: Data
        * @param aErrorCode: error code possible values KErrNone/KErrArgument
        */
        void SetPollingResultL( CSatDataPackage* aDataPackage, 
                               TInt aErrorCode );

        /**
        * Set Polling Interval, sending a request to DOS
        * @param aValue: Polling interval in seconds
        */
        void SetPollingIntervalL( TUint8 aValue );

        /**
        * Timer Expiration handler
        * @param aTimerId: Timer identification
        * @param aTimerValue: Timer value
        * @return Success/Failure value
        */
        TInt TimerExpirationL( TInt aTimerId, TUint32 aTimerValue );

        /**
        * Returns pointer to SatTimer
        * @return Pointer to CSatTimer
        */
        CSatTimer* GetSatTimer();

        /**
        * Returns pointer to CSatNotifySetUpCall
        * @return Pointer to CSatNotifySetUpCall
        */
        CSatNotifySetUpCall* SatNotifySetUpCall();

        /**
        * Sets proactive command on going status
        * @param aDataPackage: Packed data
        * @param aResult: result
        */  
        void SetProactiveCommandOnGoingStatus( CSatDataPackage* aDataPackage,
		                                        TInt aResult );
		                                                
        /**
        * Completes the notification request received from client
        * @param aDataPackage: Packed Bertlv data
        * @param aResult: result
        * @return KErrNone       
        */  		
        TInt CompletePCmdL( CSatDataPackage* aDataPackage, TInt aResult );        
        
        /*
        * Returns iIsMoSmsCtrlActivated
        * This is used by CSatTsy
        * @return ETrue if MO-SMS Control is activated
        */
        TBool IsMoSmControlActivated();

        /**
        * Set PollingOff value
        * @param aPollingOff: Polling off value
        */        
        void SetPollingOffStatus( TBool aPollingOff );
        
        /**
        * Returns iPollingOff
        * This is used by CSatNotifyPollingOff and CSatNotifyPollInterval
        * @return ETrue if Polling is ON
        */        
        TBool PollingOffStatus( );
        
        /**
        * Add Location Information simple TLV to TTlv
        * @param aTlv: Target for location information
        */
        void AddLocationInformationToTlv( TTlv& aTlv );
        
        /**
        * Passes Alpha Id for notification completion method
        * @param aAlphaId: Reference to the Alpha ID
        * @param aResult: Result of the call control request
        */
        void NotifyClientForAlphaId( RSat::TAlphaIdBuf& aAlphaId,
	        RSat::TControlResult aResult );
	        
	   	/**
        * Returns pointer to the request handle store
        */
        CSatTsyReqHandleStore* RequestHandleStore();

    protected:

        /**
        * Notifies DOS of being ready to receive indications, reguests
        * the wanted msg from pending state if exists
        * @param aPCmdMsgType: Notification msg type to be received.
        * @return success/failure if msg was found
        */
        TInt NotifySatReadyForNotification( TUint8 aPCmdMsgType );

	private:

        /**
        * Terminal response command handler from client application.
        * @param aPCmd: Proactive command Id
        * @param aRsp: Response structure
        * @param aTsyReqHandle: Telephony service request handle
        * @return KErrNone, KErrGeneral or KErrNotSupported
        */
        TInt TerminalResponseL( RSat::TPCmd* aPCmd, TDes8* aRsp, 
            TTsyReqHandle aTsyReqHandle );

        /**
        * Completes terminal response
        * @param TInt aErrorCode: error/success code
        */
        void CompleteTerminalResponse( TInt aErrorCode );

        /**
        * Test if a command was performed successfully
        * @param aGeneralResult: General result value
        * @return ETrue if command performed successfully, else EFalse
        */
        TBool CommandPerformedSuccessfully( TUint8 aGeneralResult );

        /**
        * Cell broadcast data download from client application
        * @param aTsyReqHandle: Telephony service request handle
        * @return KErrNone
        */
        TInt NotifyCbDownload( TTsyReqHandle aTsyReqHandle );

        /**
        * Sms-Pp data download from client application
        * @param aTsyReqHandle: Telephony service request handle
        * @return KErrNone
        */
        TInt NotifySmsPpDownload( TTsyReqHandle aTsyReqHandle );

    protected: // Data

        // Pointer to Sat TSY
        CSatTsy* iSatTsy;
        // Timer class object
        CSatTimer* iSatTimer;

    private:    // Data

        // Proactive command class pointers
        // created when this class is created, all classes
        // need pointers to this class
        CSatNotifyDisplayText*         iNotifyDisplayText;
        CSatNotifyGetInkey*            iNotifyGetInkey;
        CSatNotifyGetInput*            iNotifyGetInput;
        CSatNotifyPlayTone*            iNotifyPlayTone;
        CSatNotifySetUpMenu*           iNotifySetUpMenu;
        CSatNotifySelectItem*          iNotifySelectItem;
        CSatNotifySendSm*              iNotifySendSm;
        CSatNotifySendSs*              iNotifySendSs;
        CSatNotifySetUpEventList*      iNotifySetUpEventList;
		CSatNotifySendUssd*            iNotifySendUssd;
		CSatNotifySetUpIdleModeText*   iNotifySetUpIdleModeText;
		CSatNotifyLaunchBrowser*       iNotifyLaunchBrowser;
		CSatNotifyPollInterval*        iNotifyPollInterval;
		CSatNotifyCallControlRequest*  iNotifyCallControlRequest;
        CSatNotifySetUpCall*           iNotifySetUpCall;
        CSatNotifyRefresh*             iNotifyRefresh;
        CSatNotifySimSessionEnd*       iNotifySimSessionEnd;
        CSatNotifySendDtmf*            iNotifySendDtmf;
        CSatNotifyPollingOff*          iNotifyPollingOff;
        CSatNotifyLocalInfo*           iNotifyLocalInfo;
        CSatNotifyTimerMgmt*           iNotifyTimerMgmt;
        CSatNotifyLanguageNotification* iNotifyLanguageNotification;
        CSatNotifyOpenChannel*		   iNotifyOpenChannel;
        CSatNotifyMoSmControlRequest*  iNotifyMoSmControlRequest;
        CSatNotifyGetChannelStatus*	   iNotifyGetChannelStatus;
        CSatNotifyCloseChannel*		   iNotifyCloseChannel;
        CSatNotifySendData*			   iNotifySendData;
        CSatNotifyReceiveData*		   iNotifyReceiveData;
        CSatNotifyMoreTime*			   iNotifyMoreTime;	
        
        // Terminal response data class
        TTerminalRespData iTerminalRespData;
        // Request handle store
        CSatTsyReqHandleStore* iSatReqHandleStore;
        // Message manager
        CMmMessageManagerBase* iMessageManager;
        // Flag is ETrue after Polling Off, EFalse after Polling Interval PCmd
        TBool iPollingOff;        
        // Flag is set true if requested Poll Interval 
        // is longer than KMaxLegacyPollInterval
        TBool iLongPollIntervalReq;

    private:    // Friend classes

        // Notification classes
        friend class CSatNotifyDisplayText;
        friend class CSatNotifyGetInkey;
        friend class CSatNotifyGetInput;
        friend class CSatNotifyPlayTone;
        friend class CSatNotifySetUpMenu;
        friend class CSatNotifySelectItem;
        friend class CSatNotifySendSm;
        friend class CSatNotifySendSs;
		friend class CSatNotifySetUpEventList;
        friend class CSatNotifySendUssd;
		friend class CSatNotifySetUpIdleModeText;
		friend class CSatNotifyLaunchBrowser;
		friend class CSatNotifyPollInterval;
		friend class CSatNotifyCallControlRequest;
        friend class CSatNotifySetUpCall;
        friend class CSatNotifyRefresh;
        friend class CSatNotifySimSessionEnd;
        friend class CSatNotifySendDtmf;
        friend class CSatNotifyPollingOff;
        friend class CSatNotifyLocalInfo;
        friend class CSatNotifyTimerMgmt;
        friend class CSatNotifyLanguageNotification;
		friend class CSatNotifyOpenChannel;
		friend class CSatNotifyGetChannelStatus;
        friend class CSatNotifyMoSmControlRequest;
		friend class CSatNotifyCloseChannel;
		friend class CSatNotifySendData;
		friend class CSatNotifyReceiveData;
		friend class CSatNotifyMoreTime;
    };             

#endif // CSATNOTIFICATIONSTSY_H
           
// End of File
