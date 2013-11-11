// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//

#ifndef SATMESSHANDLER_H
#define SATMESSHANDLER_H

//  INCLUDES
#include <e32base.h>
#include "isi_sim.h"
#include "isi_atk.h"
#include <adaptation_conf.h>

#ifndef AFF_ISC_API_IN_USE
#include "pn_obj_auto.h"
#endif

// CONSTANTS
const TUint8 KTerminalResponseClass = 0xA0;
const TUint8 KTerminalResponseIns = 0x14;
const TUint8 KTerminalResponseP1 = 0x00;
const TUint8 KTerminalResponseP2 = 0x00;
const TUint8 KOneByteLengthLimit = 0x7F;
const TUint8 KFilePathLength = 8;
const TUint16 KFileMaxLength = 0xFFFF;
const TUint8 MEIsInTheIdleState = 0x00;
// SAT related traid constant must be defined, because
// otherwise it may happen that sat simservicetable req
// is directed to phonemesshander.
const TUint8 KSatSimServiceTableReqTraId = 0xaa;

// FORWARD DECLARATIONS
class CMmPhoNetSender;
class CTsySatMessaging;

// CLASS DECLARATION

/**
*  SAT message handler class.
*  Handles message sending and receiving to and from phonet.
*  
*/
class CSatMessHandler : public CBase 
    {
    public:  // Constructors and destructor

        /**
         * C++ default constructor.
         * @param aTsySatMessaging pointer to TsySatMessaging class
         * @param aPnSend Phonetsender for passing messages to phonet
         */
        CSatMessHandler( CTsySatMessaging* aTsySatMessaging, 
            CMmPhoNetSender* aPnSend );

        /**
         * Two-phased constructor.
         * @param aTsySatMessaging pointer to TsySatMessaging class
         * @param aPnSend Phonetsender for passing messages to phonet
         */
        static CSatMessHandler* NewL( CTsySatMessaging* aTsySatMessaging,
            CMmPhoNetSender* aPnSend );

        /**
         * Destructor.
         */
        virtual ~CSatMessHandler();

        // New functions

        /**
         * Display text response method.
         *          
         * @param aTransId Unique transaction ID
         * @param aCommandDetails command details tlv
         * @param aGeneralResult result of the proactive command display text
         * @param aAdditionalInfo Additional info response might give
         * @return phonet sender status
         */
        TInt DisplayTextTerminalRespL( TUint8 aTransId, 
            TDes8& aCommandDetails, 
            TUint8 aGeneralResult,
            TUint8 aAdditionalInfo );

        /**
         * Get Inkey response method.
         *          
         * @param aTransId Unique transaction ID
         * @param aCommandDetails command details tlv
         * @param aGeneralResult result of the proactive command display text
         * @param aDcs Data Coding Scheme
         * @param aAdditionalInfo Additional info response might give
         * @return phonet sender status
         */
        TInt GetInkeyTerminalRespL( TUint8 aTransId,
            TDes8& aCommandDetails,
            TUint8 aGeneralResult,
            TUint8 aDcs,
            TDes16& aAdditionalInfo );

        /**
         * Get Input response method.
         *          
         * @param aTransId Unique transaction ID
         * @param aCommandDetails Command details tlv
         * @param aGeneralResult result of the proactive command Get Input
         * @param aAdditionalInfo Additional info response might give
         * @param aTextString text string provided by the user
         * @param aDataCodingScheme data coding scheme for text string
         * @return phonet sender status
         */
        TInt GetInputTerminalRespL( TUint8 aTransId,
            TDes8& aCommandDetails,
            TUint8 aGeneralResult,
            TUint8 aAdditionalInfo,
            TDesC16& aTextString,
            TUint8 aDataCodingScheme );

        /**
         * Play Tone response method.
         *          
         * @param Unique transaction ID
         * @param aCommandDetails 
         * @param aGeneralResult result of the proactive command
         * @param aAdditionalInfo Additional info given in some cases
         * @return phonet sender status
         */
        TInt PlayToneTerminalRespL( TUint8 aTransId,
            TDes8& aCommandDetails, 
            TUint8 aGeneralResult, 
            TUint8 aAdditionalInfo );

        /**
         * Setup Menu response method.
         *          
         * @param aTransId Unique transaction ID
         * @param aCommandDetails command details tlv
         * @param aGeneralResult result of the proactice command Setup Menu
         * @param aAdditionalInfo Additional info response might give
         * @return phonet sender status
         */
        TInt SetUpMenuTerminalRespL( TUint8 aTransId,
            TDes8& aCommandDetails, 
            TUint8 aGeneralResult, 
            TUint8 aAdditionalInfo );

        /**
         * Select Item response method.
         *          
         * @param aTransId Unique transaction ID
         * @param aCommandDetails command details tlv
         * @param aGeneralResult result of the proactice command Select Item
         * @param aAdditionalInfo Additional info response might give
         * @return phonet sender status
         */
        TInt SelectItemTerminalRespL( TUint8 aTransId, 
            TDes8& aCommandDetails, 
            TUint8 aGeneralResult, 
            TUint8 aAdditionalInfo );

        /**
         * Send SM response method.
         *          
         * @param aPCmdNumber Unique transaction ID
         * @param aCommandDetails command details tlv
         * @param aGeneralResult result of the proactice command Send SM
         * @param aAdditionalInfo Additional info response might give
         * @return phonet sender status
         */
        TInt SendSmTerminalRespL( TUint8 aTransId, 
            TDes8& aCommandDetails,
            TUint8 aGeneralResult, 
            TUint8 aAdditionalInfo );

        /**
         * Send SS response method.
         *          
         * @param aTransId Unique transaction ID
         * @param aCommandDetails Command details TLV
         * @param aGeneralResult result of the proactive command Send SS
         * @param aAdditionalInfo Additional info response might give
         * @return phonet sender status
         */
        TInt SendSsTerminalRespL( TUint8 aTransId, 
            TDes8& aCommandDetails, 
            TUint8 aGeneralResult,
            TDesC8& aAdditionalInfo );

        /**
         * Send DTMF response method.
         *          
         * @param aTransId Unique transaction ID
         * @param aPCmdNumber Command number
         * @param aGeneralResult result of the proactice command Send SS
         * @param aAdditionalInfo Additional info response might give
         * @return phonet sender status
         */
        TInt SendDtmfTerminalRespL( TUint8 aTransId, 
            TDes8& aCommandDetails, 
            TUint8 aGeneralResult, 
            TUint8 aAdditionalInfo );

        /**
         * Send USSD response method.
         *          
         * @param aPCmdNumber Unique transaction ID
         * @param aGeneralResult result of the proactice command Send USSD
         * @param aAdditionalInfo Additional info response might give
         * @param aText input data, UCS2 string
         * @param aDCS data coding scheme
         * @return phonet sender status
         */
        TInt SendUssdTerminalRespL( TUint8 aTransId, 
            TDes8& aCommandDetails, 
            TUint8  aGeneralResult, 
            TDesC8& aAdditionalInfo, 
            TDesC& aText, 
            TUint8 aDCS );
        
        /**
         * Polling Interval response method
         *          
         * @param aPCmdNumber Unique transaction ID
         * @param aCommandDetails command details tlv
         * @param aGeneralResult result of the proactive command
         * @param aAdditionalInfo Additional info response might give
         * @param aNumOfUnits time interval
         * @return phonet sender status
         */
        TInt PollIntervalTerminalRespL( TUint8 aTransId, 
            TDes8& aCommandDetails, 
            TUint8 aGeneralResult, 
            TUint8 aAdditionalInfo,
            TUint8 aNumOfUnits );

        /**
         * Setup Call response method.
         *          
         * @param aTransId Unique transaction ID
         * @param aCommandDetails Command details TLV
         * @param aGeneralResult result of the proactive command Setup Call
         * @param aAdditionalInfo Additional info response might give
         * @return phonet sender status
         */
        TInt SetUpCallTerminalRespL( TUint8 aTransId, 
            TDes8& aCommandDetails, 
            TUint8 aGeneralResult, 
            TUint8 aAdditionalInfo );

        /**
         * Refresh response method.
         *          
         * @param aPCmdNumber Unique transaction ID
         * @param aGeneralResult result of the proactice command Refresh
         * @param aAdditionalInfo Additional info response might give
         * @return phonet sender status
         */
        TInt RefreshTerminalRespL( TUint8 aTransId, 
            TDes8& aCommandDetails, 
            TUint8 aGeneralResult, 
            TUint8 aAdditionalInfo );

        /**
         * Menu Selection method.
         *          
         * @param aTransId Unique transaction ID
         * @param aItemIdentifier identifier of the selected menu item
         * @param aHelp help requested by the user indicator
         * @return phonet sender status
         */
        void MenuSelectionIndL( TUint8 aTransId, 
            TUint8 aItemIdentifier, 
            TUint8 aHelp );

        /**
         * Handles the proactive commands from phonet
         *          
         * @param aIsiMsg proactive command
         */
        virtual void SatMessageReceivedL( CIsiMsg* aIsiMsg );

        /**
         * Launch Browser terminal response method
         *          
         * @param aTransId transaction id
         * @param aPCmdNumber command number
         * @param aGeneralResult
         * @param aAdditionalInfo
         * @param aCmdQualifier
         * @return phonet sender status
         */
        virtual TInt LaunchBrowserTerminalRespL( TUint8 aTransId,
            TDes8& aCommandDetails,         
            TUint8 aGeneralResult,      
            TUint8 aAdditionalInfo );

        /**
         * Converts Symbian OS general result to DOS general result
         *          
         * @param aPCmdNumber Unique transaction ID
         * @param TUint8 aAtkResult result of the proactice command
         * @return phonet sender status
         */
        TUint8 ConvertGeneralResult( TUint8 aGeneralResult );

        /**
         * Test if a command was performed successfully
         *          
         * @param aGeneralResult
         * @return ETrue if command performed successfully, else EFalse
         */
        TBool CommandPerformedSuccessfully( TUint8 aGeneralResult );

        /**
         * SetupIdleModeText Terminal Response
         *          
         * @param aTransId Unique transaction ID
         * @param aCommandDetails command details tlv
         * @param aGeneralResult result of the proactive command display text
         * @param aAdditionalInfo Additional info response might give
         * @return phonet sender status
         */
        TInt SetUpIdleModeTextTerminalRespL( TUint8 aTransId,
            TDes8& aCommandDetails,
            TUint8 aGeneralResult,
            TUint8 aAdditionalInfo );

        /**
         * SetUpEventList response method.
         *          
         * @param aTransId Unique transaction ID
         * @param aCommandDetails command details tlv
         * @param aGeneralResult result of the proactive command display text
         * @param aAdditionalInfo Additional info response might give
         * @return phonet sender status
         */
        TInt SetUpEventListTerminalRespL( TUint8 aTransId, 
            TDes8& aCommandDetails, 
            TUint8 aGeneralResult,
            TUint8 aAdditionalInfo );

        /**
         * PollingOff response method.
         *          
         * @param aPCmdNumber Unique transaction ID
         * @param aCommandDetails command details tlv
         * @param aAdditionalInfo Additional info response might give
         * @return phonet sender status
         */
        TInt PollingOffTerminalRespL( TUint8 aTransId, 
            TDes8& aCommandDetails,
            TUint8 aGeneralResult,
            TUint8 aAdditionalInfo );

        /**
         * LocalInfo response method.
         *          
         * @param aTransId Unique transaction ID
         * @param aCommandDetails Command details TLV
         * @param aGeneralResult result of the proactice command
         * @param aAdditionalInfo Additional info response might give
         * @return phonet sender status
         */
        TInt LocalInfoTerminalRespL( TUint8 aTransId, 
            TDes8& aCommandDetails,
            TUint8 aGeneralResult,
            TDes& aAdditionalInfo );

        /**
         * Timer management response method.
         *          
         * @param aTransId Unique transaction ID
         * @param aCommandDetails Command details TLV
         * @param aGeneralResult result of the proactive command
         * @param aTimerValue[3] timer value in hours, minutes and seconds
         * @param aTimerId timer identifier 
         * @param aAdditionalInfo Additional info response might give
         * @return phonet sender status
         */
        TInt TimerMgmtTerminalRespL( TUint8 aTransId, 
            TDes8& aCommandDetails,
            TUint8 aGeneralResult,
            TUint8 aTimerValue[3],   
            TUint8 aTimerId,
            TUint8 aAdditionalInfo );

        /**
         * Timer expiration indication
         *          
         * @param aTransId Unique transaction ID
         * @param aTimerId timer identification
         * @param aTimerValue[3] timer value
         * @return TInt
         */
        TInt TimerExpirationInd ( TUint8 aTransId,
            TUint8 aTimerId,
            TUint8 aTimerValue[3] );

        /**
         * More time response
         *          
         * @param aTransId Unique transaction ID
         * @param aCommandDetails Command details TLV
         * @param aGeneralResult result of the proactive command
         * @return phonet sender status 
         */
        TInt MoreTimeTerminalRespL( TUint8 aTransId,
            TDes8& aCommandDetails,
            TUint8 aGeneralResult );

        /**
         * SendTerminalResponseL
         *          
         * @param aCommandDetails Command details TLV
         * @param aBerTlvSpecificData data to be sent
         * @return phonet sender status                        
         */
        TInt SendTerminalResponseL( TDes8& aCommandDetails,
            const TDesC8& aBerTlvSpecificData,
            TUint8 aTransId = 0 );

        /**
         * SimReadFieldReq
         *          
         * @param aTransId Unique transaction ID
         * @param aOffset
         * @param aLength
         * @param aSimFilePath
         * @return phonet sender status                        
         */
        TInt SimReadFieldReq( TUint8 aTransId, 
            TUint16 aOffset,
            TUint16 aLength,
            TDes8* aSimFilePath );
                
        /**
         * SimReadFieldResp
         *          
         * @param aIsiMsg
         */
        void SimReadFieldResp( CIsiMsg* aIsiMsg );

        /**
         * SendAtkEnvelopeNtf
         *          
         * @param aBerTlv TLV data of the envelope command 
         * @return phonet sender status
         */
        TInt SendAtkEnvelopeNtf( TUint8 aTransId, 
            const TDesC8& aBerTlv );
        
        /**
         * ReSendAtkEnvelopeNtf
         *          
         * @param aIsiMsg response to envelope 
         * @return KErrNone/KErrNotReady, no resending or resending.
         */
        TInt ReSendAtkEnvelopeNtf( CIsiMsg* aIsiMsg );
        
        /**
         * Send a Sms-Pp report with subblocks 
         *          
         * @param aTransId Unique transaction Id
         * @param aData data to be appended to the request
         */
        void SendSmsPpReportReq( TUint8 aTransId, 
            TDesC8& aData );

        /**
         * Sends atk event resp without sub blocks
         *          
         * @param aTransId Unique transaction Id
         * @param aError success code
         * @param aReceiverObject receiver guardian
         */
        TInt SendAtkEventCcResp( TUint8 aTransId, 
            TUint8 aError, 
            TUint8 aReceiverObject );

        /**
         * Sends atk event resp with sub blocks
         *          
         * @param aTransId Unique transaction Id
         * @param aData data to be sent
         * @param aReceiverObject receiver guardian
         */
        TInt SendAtkEventCcResp( TUint8 aTransId, 
            TDesC8& aData,
            TUint8 aReceiverObject );

        /**
         * Sets header of the message for atk messages
         *          
         * @param aMsg message to be sent
         * @param aReceiverObj receiver object of the message
         */
#ifndef AFF_ISC_API_IN_USE
        void SetAtkMsgHeaderInformation( CPnMsg& aMsg,
            TInt aReceiverObj = PN_OBJ_SIMSON_SERV );
#else
        void SetAtkMsgHeaderInformation( CPnMsg& aMsg,
            TInt aReceiverObj = 0x35 );
#endif

        /**
         * Parse message and read from it operator code,
         * locationAreaCode and cell id.
         *          
         * @param aIsiMsg received message
         */
        void NetRegStatusInd( CIsiMsg* aIsiMsg );

        /**
         * activate call guardians
         *          
         * @param aTransId Unique transaction Id
         * @param aCCstatus Is the CC supported in SIM or not
         * @return phonet sender status 
         */
        TInt NotifyGuardiansAboutCCstatusInCard( TUint8 aTransId,
            TUint8 aCCstatus );

        /**
         * Read sim table if CC is supported
         *          
         * @return phonet sender status 
         */
        TInt SimReadTableReq();

        /**
         * Read sim table response
         *          
         * @param aIsiMsg response to envelope 
         */
        void SimReadTableResp( CIsiMsg* aIsiMsg );

        /**
         * Request to allow notification of SMS-PP Data download messages
         *          
         * @param aTransactionId Transaction identifier
         * @return TInt
         */
        TInt SmsPpRoutingReq ( TUint8 aTransactionId );

        /**
         * Breaks stub message
         *          
         * @param aIsiMsg
         */
        void InfoSerialNumberReadResp( CIsiMsg* aIsiMsg );
                                           
        /**
         * Send message to Phonet.
         *          
         * @param aTransId a unique transaction id
         * @param aCellInfoType 
         * @return Phonet sender status
         */
        TInt NetNeighbourCellsReq( TUint8 aTransId, 
            TUint8 aCellInfoType );

        /**
         * Breaks stub message
         *          
         * @param aIsiMsg
         */
        void NetNeighbourCellResp( CIsiMsg* aIsiMsg );

        /**
         * Send stub message
         *          
         * @param aTransId
         * @param aServiceType type of service
         * @param aMsg contains specific data required in the request
         * @return Phonet sender status
         */
        TInt SimAtkReq( TUint8 aTransId, 
            TUint8 aServiceType, 
            const TDesC8& aMsg );

        /**
         * Breaks stub message
         *          
         * @param aIsiMsg received message
         */
        void SimAtkRespL( CIsiMsg* aIsiMsg );
        
        /**
         * Set Polling Interval, sending a request to sim server
         *          
         * @param aTraId Transaction Id
         * @param aValue in seconds
         */
        void SetPollingInterval( TUint8 aTraId, 
            TUint8 aValue );

        /**
         * Method to check if IMEI is successfully received
         *          
         * @return KErrNone or KErrNotFound
         */
        TInt ImeiStatus();
       
        /**
         * Method to check SIM server response to the refres request
         *          
         * @param aStatus SIM server status
         * @param aServiceType
         */
        void RefreshResultL( TUint8 aStatus, 
            TUint8 aServiceType );

        /**
         * Method to check SIM response to a set polling request
         * Called by SimAtkRespL
         *          
         * @param aStatus SIM status
         * @param aTransId Transaction Id
         * @param aInterval obtained from the message of the response
         */
        void SetPollingResult( TUint8 aStatus,
            TUint8 aTransId,
            TUint8 aInterval );

        /**
         * Sends message to phonet
         *          
         * @param aTransId Transaction ID
         * @param aRefreshType Type of refresh
         * @return Phonet sender status
         */
        TInt NetSimRefreshReq( TUint8 aTransId,
            TUint8 aRefreshType );

        /**
         * Breaks stub message
         *          
         * @param aIsiMsg
         * @return Success code of refresh request
         */
        void NetSimRefreshRespL( CIsiMsg* aIsiMsg );

        /**
         * Method to check that proactive command is in correct format.
         *          
         * @param aIsiMsg 
         * @return KErrNone or KErrCorrupt
         */
        TInt CheckProactiveCommandL( CIsiMsg* aIsiMsg );

        /**
         * Send message
         *          
         * @param aTransId
         * @param aRoutingCommand
         * @return Phonet sender return value
         */
        TInt SmsGsmTempCbRoutingReq( TUint8 aTransId, 
            TUint8 aRoutingCommand );

        /**
         * Breaks message
         *          
         * @param aIsiMsg
         */
        void SmsGsmTempCbRoutingResp( CIsiMsg* aIsiMsg );

        /**
         * Creates InfoSerialNumberReadReq message and sends it to Phonet
         *          
         * @param aTransId unique transaction id
         * @param aTarget requested data
         * @return Error value 
         */
        TInt InfoSerialNumberReadReq( TUint8 aTransId, 
            TUint8 aTarget );

        /**
         * Creates message and sends it to Phonet
         *          
         * @param aTransId unique transaction id
         * @return Error value 
         */
        TInt NetRegStatusGetReq( TUint8 aTransId );

        /**
         * Breaks stub message
         *          
         * @param aIsiMsg
         */
        void NetRegStatusGetResp( CIsiMsg* aIsiMsg );

        /**
         * Method to check Location information status
         *          
         * @return ETrue or EFalse
         */
        TBool LocationInformationStatus() const
            {
            return iLocationInfoReceived;
            }

        /**        
         * Returns locations area code (LAC)
         *          
         * @return Location Area Code
         */
        TUint16 GetLocationAreaCode() const
            {
            return iLocationAreaCode;
            }

        /**
         * Returns Cell ID
         *          
         * @return Cell ID
         */
        TUint16 GetCellId() const
            {
            return iCellId;
            }

        /**
         * Returns Operator code
         *          
         * @return Operator Code
         */
        const TDesC8& GetOperatorCode() const
            {
            return iOperatorCode;
            }

        /**
         * Queries the default bearer for voice call
         *          
         * @return phonet sender status 
         */
        TInt GetBearerCapability();

        /**
         * Checks if received number is emergency number or not
         *          
         * @param aNumber number to be checked
         * @param aTransId unique transaction id
         * @return 
         */
        TInt CheckEmergencyNumber( TBuf8<123> aNumber, TUint8 aTransId );

        /** 
         * Breaks message
         *          
         */
        void SimServerReadyInd();

        /** 
         * Set refresh status flag.
         *          
         * @param aStatus Refresh is ongoing EFalse or ETrue
         */
        void SetRefreshStatus( TBool aStatus );
        
        /**
         * Sends message to phonet
         *
         * @param aTransId Transaction ID
         * @param TUint8 aOperation: Gss operation
         * @return Phonet sender status
         */
        TInt GssCsServiceReq( TUint8 aTransId,
            TUint8 aOperation );

        /**
         * Breaks message
         *          
         * @param aIsiMsg
         */
        void GssCsServiceResp( CIsiMsg* aIsiMsg );

        /**
         * Language Notification response
         *          
         * @param aTransId Unique transaction ID
         * @param aCommandDetails Command details TLV
         * @param aGeneralResult result of the proactive command
         * @param aLanguage 
         * @return phonet sender status 
         */
        TInt LanguageNotificationTerminalRespL( 
            TUint8 aTransId,
            TDes8& aCommandDetails,
            TUint8 aGeneralResult,
            TUint8 aAdditionalInfo );

        /** 
         * Breaks message
         *          
         * @param aIsiMsg received indication
         */
        void NetTimeInd( CIsiMsg* aIsiMsg );

        /** 
         * Returns time zone information received from network.
         *          
         * @return Time zone
         */
        TUint8 GetTimeZone() const;

    private:

        /**
         * Copy constructor, usage not allowed
         * @param aRhs Satmesshandler reference
         */
        CSatMessHandler( const CSatMessHandler& aRhs );

        /**
         * Assignment operator, usage not allowed
         * @param Satmesshandler reference
         * @return aRhs SatMessHandler reference
         */
        CSatMessHandler& operator = ( const CSatMessHandler& aRhs );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    public: // Data
        
        /**
         * Flag is true after a Polling Off, and false after 
         * a polling interval pcmd
         */
        TBool iPollingOff;

    private: // Data

        /**
         * pointer to SAT messaging object
         * Own.
         */
        CTsySatMessaging* iTsySatMessaging;    
        
        /**
         * pointer to Phonet Sender
         * Own.
         */
        CMmPhoNetSender* iPnSend;
        
        /**
         * Storage for terminal resp transaction id..
         */
        TInt iTerminalRespTraId;
        
        /**
         * Variable for storing card id
         */
        TUint8 iCardId;
        
        /**
         * For storing location area code
         */
        TUint16 iLocationAreaCode;
        
        /**
         * For storing cell id
         */
        TUint16 iCellId;
        
        /**
         * For checking if network information has been received
         */
        TBool iLocationInfoReceived;
        
        /**
         * For storing operator code.
         */
        TBuf8<3> iOperatorCode;
        
        /**
         * For storing IMEI code.
         */
        TBuf8<16> iIMEI;
        
        /**
         * For storing NMR
         */
        TBuf8<16> iNMR;
        
        /**
         * For storing BCCH channel list
         */
        TBuf<32> iBCCHChannelList;
        
        /**
         * For checking if IMEI code has been received
         */
        TBool iSerialNumberReadReceived;
        
        /**
         * Flag
         */
        TBool iInformSmsServerAboutCbmidIsOngoing;
        
        /**
         * Structure for storing envelope information for possible
         * need of resending
         */
        struct TEnvelopeStruct
            {
            // Transaction id of Envelope
            TUint8 iTransId;

            // BerTlv information
            TBuf8<257> iData;
            };
        
        /**
         * Variable for storing sent envelopes for possible need of resend
         */
        RArray<TEnvelopeStruct> *iEnvelope;
        
        /**
         * Refresh proactive command is currently active flag.
         */
        TBool iRefreshRequestIsOn;
        
        /**
         * Variable for storing Timing Advance
         */
        TUint8 iTimingAdvance;
        
        /**
         * Variable for storing ME Status
         */
        TUint8 iMEStatus;
        
        /**
         * Save the SimReadField transaction id used by GetIcon.
         */
        TInt iGetIconSimReadFieldTraId;
        
        /**
         * Save the SimReadField transaction id used by DataDownload.
         */
        TInt iDataDownloadSimReadFieldTraId;
        
        /**
         * Time zone information from Network. 0xFF if not received.
         */
        TUint8 iTimeZone;
    };

#endif    // SATMESSHANDLER_H   

// End of File
