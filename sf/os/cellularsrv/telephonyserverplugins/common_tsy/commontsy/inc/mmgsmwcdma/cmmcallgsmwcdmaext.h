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
*
*/



#ifndef CMMCALLGSMWCDMAEXT_H
#define CMMCALLGSMWCDMAEXT_H

//  INCLUDES
#include <featmgr/featurecontrol.h>
#include "cmmcallextinterface.h"
#include "CMmPrivateUtility.h"

#include "ctsysystemstatepluginhandler.h"

// FORWARD DECLARATIONS
class CMmCallTsy;
class CMmMessageManagerBase;

// CLASS DECLARATION

/**
*  CMmCallGsmWcdmaExt contains mode-dependent phone functionality for GSM
*  Calls. Extension request handles are stored in MM Call objects.
*/
NONSHARABLE_CLASS( CMmCallGsmWcdmaExt ) : public CMmCallExtInterface, public MCtsySsmPluginCallback
    {
    public: // Constructors and destructor
        
        /**
         * Two-phased constructor.
         *          
         *
         * @param aMmCallTsy Pointer to the created object
         * @param aMmCallMessHandler Pointer to call message handler
         * @param aMode Call mode
         * @param aMessageManager Pointer to the Message Manager
         * @return Pointer to the created object
         */
        static CMmCallGsmWcdmaExt* NewL( CMmCallTsy* aMmCallTsy,
            RMobilePhone::TMobileService aMode, 
            CMmMessageManagerBase* aMessageManager );

        
        /**
         * Destructor.
         *          
         *
         */
        ~CMmCallGsmWcdmaExt();
        
        /**
         * This method sets RF state
         *          
         *
         * @param aRfState New RF state
         * @param aSsmPluginCallback pointer to callback interface        
         */
        void SetRfState( TRfStateInfo aRfState, MCtsySsmPluginCallback* aSsmPluginCallback, TCtsySsmCallbackData& aCallbackData );


        /**
         * From CMmCallExtInterface Dials a voice call
         *          
         *
         * @param aAlsLineInfo ALS line information
         * @param aCallParams Call parameters
         * @param aTelNumber Phone number to be called
         * @param aExtensionId 
         * @return KErrNone or error code
         */
        TInt DialL( RMobilePhone::TMobileService aAlsLineInfo,
            const TDesC8* aCallParams,
            const TDesC* aTelNumber,
            TInt aExtensionId );

        /**
         * From CMmCallExtInterface Answer incoming call
         *          
         *
         * @param aCallId Call ID of the call to be answered
         * @param aExtensionId 
         * @return KErrNone or error code
         */
        TInt AnswerIncomingCallL( TInt aCallId, TInt aExtensionId );

        /**
         * From CMmCallExtInterface Get GSM specific call parameters
         *          
         *
         * @param aParams Pointer to parameters package
         */
        void GetCallParams( TDes8* aParams );

        /**
         * From CMmCallExtInterface This method is used to hold an active call
         *
         * @param aCallId Call Id of the operation's target call
         * @return KErrNone or error code
         */
        TInt HoldL( TInt aCallId );

        /**
         * From CMmCallExtInterface This method is used to resume a held call
         *          
         *
         * @param aCallId Call Id of the operation's target call
         * @return  KErrNone or error code
         */
        TInt ResumeL( TInt aCallId );

        /**
         * From CMmCallExtInterface This method is used to swap between active 
         * and held calls
         *          
         *
         * @param aCallId Call Id of the operation's target call
         * @return KErrNone or error code
         */
        TInt SwapL( TInt aCallId );

        /**
         * From CMmCallExtInterface This method requests a private communication
         * to the remote party of one call within a conference call.
         *          
         *
         * @param aCallId Call Id of the operation's target call
         * @return KErrNone or error code
         */
        TInt GoOneToOneL( TInt aCallId );

        /**
         * From CMmCallExtInterface Dial an emergency call
         *          
         *         
         * @param aCallbackData 
         * @return KErrNone or error code
         */
        TInt DialEmergencyCall(TCtsySsmCallbackData& aCallbackData );  
       

        /**
         * From CMmCallExtInterface Deflect an incoming call to another 
         * destination
         *          
         *
         * @param aData Packaged parameters
         * @return KErrNone or error code
         */
        TInt DeflectL( const TDataPackage& aData );

        /**
         * From CMmCallExtInterface Informs GSM extension about the current 
         * status.
         *          
         *
         * @param aCallStatus Current call status
         * @return KErrNone or error code
         */
        TInt CompleteNotifyStatusChange( 
            RMobileCall::TMobileCallStatus aCallStatus );

         /**
         * From CMmCallExtInterface Get GSM specific mobile call capabilities.
         *          
         *
         * @param aCallCapsV1: a reference to the parameter object to be filled
         */
        void GetMobileCallCapsV1( 
            RMobileCall::TMobileCallCapsV1& aCallCapsV1 );


        /**
         * From CMmCallExtInterface Set GSM specific mobile call capabilities.
         *          
         *
         * @return aCaps New call capabilities
         */
        void SetGSMCallCaps( 
            const RMobileCall::TMobileCallCapsV1* aCaps );

        /**
         * From CMmCallExtInterface Adds requested caps to GSM specific mobile 
         * call capabilities.
         *          
         *
         * @param aCaps Call capabilities to be added
         */
        void AddGSMCallCaps( TUint32 aCaps );

        /**
         * From CMmCallExtInterface Removes requested caps from GSM specific 
         * mobile call capabilities.
         *          
         *
         * @param aCaps Call capabilities to be removed
         */
        void RemoveGSMCallCaps( TUint32 aCaps );

        /**
         * From CMmCallExtInterface Transfer call to another destination
         *          
         *
         * @return KErrNone or error code
         */
        TInt TransferL();

        /**
         * From CMmCallExtInterface Resets call internal attributes.
         *          
         *
         */
        void ResetInternalAttributes();

        /**
         * From CMmCallExtInterface Fill mobile call information in the infoV1 
         * given as parameter
         *          
         *
         * @return aInfoV1 Current Mobile Call Information
         * @return KErrNone or error code
         */
        TInt FillMobileCallInfo( 
           RMobileCall::TMobileCallInfoV1* aInfoV1 );
        	

        /**
         * From CMmCallExtInterface Fill mobile call information in the infoV3 
         * given as parameter
         *          
         * 
         * @return aInfoV3 Current Mobile Call Information
         * @return KErrNone or error code
         */
        TInt FillMobileCallInfoV3( 
            RMobileCall::TMobileCallInfoV3* aInfoV3 );     
    
        /**
         * From CMmCallExtInterface Fill mobile call information in the infoV7 
         * given as parameter
         *          
         * 
         * @return aInfoV7 Current Mobile Call Information
         * @return KErrNone or error code
         */
        TInt FillMobileCallInfoV7( 
            RMobileCall::TMobileCallInfoV7* aInfoV7 );    
        
        /**
         * From CMmCallExtInterface Fill mobile call information in the infoV8 
         * given as parameter
         *          
         * 
         * @return aInfoV8 Current Mobile Call Information
         * @return KErrNone or error code
         */        
        TInt FillMobileCallInfoV8(
            RMobileCall::TMobileCallInfoV8* aInfoV8);

         /**
         * From CMmCallExtInterface Fills in the remote party info in the 
         * infoV1Pckg given as parameter
         *          
         *
         * @return aInfoV1Pckg Current remote party information
         */
        TInt GetRemotePartyInfo( 
            RMobileCall::TMobileCallRemotePartyInfoV1Pckg* aInfoV1Pckg );

        /**
         * From CMmCallExtInterface Sets mobile call data
         *          
         *
         * @param aDataPackage New mobile call data
         * @param aCallDirection New call direction
         */
        void SetMobileCallData(
            CMmDataPackage* aDataPackage,
            RMobileCall::TMobileCallDirection aCallDirection );

        //CSD/HSCSD methods

        /**
         * Return GSM specific data call capabilities.
         *          
         *
         * @param aCallDataCapsV1: a reference to the parameter object to be filled
         */
        void GetMobileDataCallCapsV1( 
            RMobileCall::TMobileCallDataCapsV1& aCallDataCapsV1 );

        /**
         * Received GSM specific data call capabilities.
         *          
         *
         * @param aCaps: New data call caps
         */
        void ReceivedMobileDataCallCaps(
            RMobileCall::TMobileCallDataCapsV1* aCaps );

        /**
         * Set GSM specific dynamic HSCSD parameters.
         *          
         *
         * @param RMobileCall::TMobileCallAiur* aAiur: Wanted rate
         * @param TInt* aRxTimeslots: Wanted RX timeslots
         * @return: KErrNone or KErrNotFound
         */
        TInt SetDynamicHscsdParamsL(
            RMobileCall::TMobileCallAiur* aAiur,
            TInt* aRxTimeslots );

        /** 
         * Return GSM specific current HSCSD info.
         *          
         *
         * @param  Current HSCSD info
         * @return KErrNone or KErrETelCallNotActive
         */
        TInt GetCurrentHscsdInfo( TDes8* aHSCSDInfo );

        /**
         * Return GSM specific bearer service info.
         *          
         *
         * @param RCall::TBearerService* aBearerService: Bearer service info
         * @return : KErrNone
         */
        TInt GetBearerServiceInfo( 
            RCall::TBearerService* aBearerService );

        /**
         * Dial for data calls.
         *          
         *
         * @parama CallMode: call mode
         * @parama CallParams: call parameters
         * @param  aTelNumber: phone number to be called
         * @return : KErrNone or error code from DP
         */
        TInt DialDataCallL(
            RMobilePhone::TMobileService aCallMode,
            const TDesC8* aCallParams,
            const TDesC* aTelNumber );

        /**
         * From CMmCallExtInterface Answer incoming data call
         *          
         *
         * @param aCallId Call ID of the call to be answered
         * @return KErrNone or error code
         */
        TInt AnswerIncomingDataCallL( TInt aCallId );

        /**
         * Copy received bearer service info.
         *          
         *
         * @param  Bearer service info
         */
        void ReceivedBearerServiceInfo( 
            RCall::TBearerService aBearerService );

        /**
         * Sets correct data call attributes.
         *          
         *
         * @param  Call parameters
         */
        void Connect( const TDesC8* aCallParams );

        /**
         * Stores new CALL/CSD parameters.
         *          
         *
         * @param aDataPackage: call parameters
         */
        void ReceivedCallParams(
            const CMmDataPackage* aDataPackage );

        /**
         * Copies data call parameters to another variable without extension 
         * id
         *          
         *         
         * @param destination  parameter
         * @param source parameter
         */
        void CopyMobileDataCallParams( 
            RMobileCall::TMobileDataCallParamsV1& aDestination,
            const RMobileCall::TMobileDataCallParamsV1& aSource ) const;

        /**
         * Received extension specific HSCSD info.
         *          
         *
         * @param aDataPackage
         */
        void ReceivedHscsdInfo(
            CMmDataPackage* aDataPackage );

        /**
         * Received extension specific data call capabilities.
         *          
         *
         * @param New data call capabilities and
         * data call mode 
         */
        void ReceivedMobileDataCallCaps(
            CMmDataPackage* aDataPackage );
   
   public: // Functions from MCtsySsmPluginCallback
   
        /** CMmCallGsmWcdmaExt::SsmPluginCallback
		 * This is callback
		 * This method is called from System State Manager (or from CMmCallGsmWcdmaExt, if there 
		 * is no reason to change RF state), after RF state changing      
         *
         * @param aResult -- result returned either from SSM
         */
         void SsmPluginCallback(TInt aResult, TCtsySsmCallbackData& aCallbackData);    


    private: // Constructors and destructor

        /**
         * C++ default constructor.
         *          
         *
         */
        CMmCallGsmWcdmaExt();

        /**
         * Second phase constructor of CMmCallGsmWcdmaExt.
         *          
         *
         */
        void ConstructL();

        /**
         * This method updates call capabilities in case that there is only 
         *  one non-idle call
         *           
         *
         * @param aCallStatus Current status of this call
         * @return KErrNone or error code
         */
        TInt UpdateSingleCallControlCaps( 
            RMobileCall::TMobileCallStatus aCallStatus );

        /**
         * This method updates call capabilities in case that there are two
         * calls in non-idle state and they are not in a conference.
         *          
         *
         * @param aCallStatus Current status of this call
         * @return KErrNone or error code
         */
        TInt UpdateDoubleCallControlCaps( 
            RMobileCall::TMobileCallStatus aCallStatus );

        /**
         * This method updates call capabilities in case that conference call
         * is going on.
         *          
         *
         * @param aCallStatus Current status of this call
         * @return KErrNone or error code
         */
        TInt UpdateMultipleCallControlCaps( 
            RMobileCall::TMobileCallStatus aCallStatus);

        //CSD/HSCSD methods
        /**
         * Sets data call service, protocol and QoS caps in CSD/HSCSD 
         *          
         *
         * @param const  aCallParams: Call parameters
         * @return KErrNone
         */
        void SetDataCaps( const TDesC8* aCallParams );
        

    private:    // Data

        /**
         * Pointer to the mode independent call object
         */
        CMmCallTsy* iMmCallTsy;

        /**
         * call mode
         */
        RMobilePhone::TMobileService iSymbianCallMode;

        /**
         * Call parameters
         */
        RMobileCall::TMobileCallParamsV7 iCallParams;

        /**
         * Dynamic call capabilities
         */
        RMobileCall::TMobileCallCapsV1 iMobileCallCaps;

        /**
         * Current mobile call info
         */
        RMobileCall::TMobileCallInfoV8 iMobileCallInfo;

        /**
         * Tells if Join flag should be enabled for this calls when it enters 
         * the connected state
         */
        TBool iEnableJoinFlag;

        /**
         * pointer to the message manager
         * Not own.
         */
        CMmMessageManagerBase* iMessageManager;

        /**
         * Data call mode (CSD or HSCSD)
         */
        TDataCallMode iDataCallMode;

        /**
         * Bearer service information for data call
         */
        RCall::TBearerService iBearerService;

        /**
         * Dynamic data call caps
         */
        RMobileCall::TMobileCallDataCapsV1* iDataCallCaps;

        /**
         * CSD parameters
         */
        RMobileCall::TMobileDataCallParamsV1* iCSDParams;

        /**
         * HSCSD parameters
         */
        RMobileCall::TMobileHscsdCallParamsV1* iHSCSDParams;

        /**
         * Current HSCSD call info
         */
        RMobileCall::TMobileCallHscsdInfoV8* iCurrentHSCSDInfo;

        /**
         * Tells if HSCSD info has been received or not
         */
        TBool iInformationReceived;

		/**
		 * Used to query for supported features
		 */
		RFeatureControl iFeatureControl;

		/**
		 * Tells if Feature Manager is available. Will be EFalse if 
		 * RFeatureControl::Open fails to connect to the Feature Manager
		 */
		TBool iUsingFeatureManager;
		
		// Pointer to SystemStatePlugin
		CCtsySystemStatePluginHandler* iSystemStatePluginHandler;
    };

#endif      // CMMCALLGSMWCDMAEXT_H   
            
// End of File
