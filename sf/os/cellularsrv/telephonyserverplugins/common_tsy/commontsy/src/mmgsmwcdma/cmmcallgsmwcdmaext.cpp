// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Description:
//




#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmcallgsmwcdmaextTraces.h"
#endif

#include "cmmcallgsmwcdmaext.h"
#include <featureuids.h>
#include "cmmphonetsy.h"
#include "cmmcalllist.h"
#include "cmmconferencecalltsy.h"
#include "cmmconferencecallgsmwcdmaext.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmmessagemanagerbase.h"

#include "cmmvoicecalltsy.h"

//csd includes
#include "cmmdatacalltsy.h"

 #include <et_tsy.h>

// ======== MEMBER FUNCTIONS ========

CMmCallGsmWcdmaExt::CMmCallGsmWcdmaExt()
    {
    }

void CMmCallGsmWcdmaExt::ConstructL()
    {
    // Create  normal and HSCSD data call related pointers!
    iDataCallCaps = new ( ELeave ) RMobileCall::TMobileCallDataCapsV1();
    iCSDParams = new ( ELeave ) RMobileCall::TMobileDataCallParamsV8();
    iHSCSDParams = new ( ELeave ) RMobileCall::TMobileHscsdCallParamsV8();
    iCurrentHSCSDInfo = new ( ELeave ) RMobileCall::TMobileCallHscsdInfoV8();

    // iCSDParams, iHSCSDParams and iCurrentHSCSDInfo:
    // Service and QoS values are set to only supported value (MO, defined in
    // MmTsy_Defaults.h). If there will be more supported caps in for example
    // continuation projects, then set here Service and QoS to Unspecified.
    iCSDParams->iService = RMobileCall::RMobileCall::EServiceDataCircuitAsync;;
    iCSDParams->iQoS = RMobileCall::EQoSNonTransparent;
    iCSDParams->iSpeed = RMobileCall::ESpeedUnspecified;
    iCSDParams->iProtocol = RMobileCall::EProtocolUnspecified;
    iCSDParams->iRLPVersion = RMobileCall::ERLPNotRequested;
    iCSDParams->iModemToMSWindowSize = 0x00;
    iCSDParams->iMSToModemWindowSize = 0x00;
    iCSDParams->iAckTimer = 0x00;
    iCSDParams->iRetransmissionAttempts = 0x00;
    iCSDParams->iResequencingPeriod = 0x00;
    iCSDParams->iV42bisReq = RMobileCall::EV42bisNeitherDirection;
    iCSDParams->iV42bisCodewordsNum = 0x00;
    iCSDParams->iV42bisMaxStringLength = 0x00; 
    
  
    RMobileCall::TMobileDataCallParamsV8* dataCallParamsV8 = static_cast<RMobileCall::TMobileDataCallParamsV8*>(iCSDParams);
    
    dataCallParamsV8->iBearerMode = RMobileCall::EMulticallNotSupported;
    dataCallParamsV8->iCallParamOrigin = RMobileCall::EOriginatorUnknown;
    dataCallParamsV8->iBCRepeatIndicator = RMobileCall::EBCAlternateMode;
   
    iHSCSDParams->iWantedAiur = RMobileCall::EAiurBpsUnspecified;
    iHSCSDParams->iWantedRxTimeSlots = 0x00;
    iHSCSDParams->iMaxTimeSlots = 0x00;
    iHSCSDParams->iCodings = 0x00;
    iHSCSDParams->iAsymmetry = RMobileCall::EAsymmetryNoPreference;
    iHSCSDParams->iUserInitUpgrade = KMultimodeTsyDataUserInitUpgrade;
    
    RMobileCall::TMobileHscsdCallParamsV8* hscsdCallParamsV8 = static_cast<RMobileCall::TMobileHscsdCallParamsV8*>(iHSCSDParams);
    hscsdCallParamsV8->iBearerMode = RMobileCall::EMulticallNotSupported;
    hscsdCallParamsV8->iCallParamOrigin = RMobileCall::EOriginatorUnknown;
    hscsdCallParamsV8->iIconId.iQualifier = RMobileCall::EIconQualifierNotSet;
    hscsdCallParamsV8->iIconId.iIdentifier = 0x00;
    hscsdCallParamsV8->iBCRepeatIndicator = RMobileCall::EBCAlternateMode;
       
    iCurrentHSCSDInfo->iAiur = RMobileCall::EAiurBpsUnspecified;
    iCurrentHSCSDInfo->iRxTimeSlots = 0x00;
    iCurrentHSCSDInfo->iTxTimeSlots = 0x00;
    iCurrentHSCSDInfo->iCodings = RMobileCall::ETchCodingUnspecified;
    iCurrentHSCSDInfo->iCallParamOrigin = RMobileCall::EOriginatorUnknown;
    iCurrentHSCSDInfo->iIconId.iIdentifier = 0x00;
    iCurrentHSCSDInfo->iIconId.iQualifier = RMobileCall::EIconQualifierNotSet;
    iCurrentHSCSDInfo->iParamsCallControlModified = EFalse;
    iCurrentHSCSDInfo->iBCRepeatIndicator = RMobileCall::EBCAlternateMode;
    
    iMobileCallCaps.iCallControlCaps = 0x00000000 | RCall::KCapsDial;

    if ( RMobilePhone::EVoiceService == iSymbianCallMode )
        {
        iMobileCallCaps.iCallControlCaps |= RCall::KCapsVoice;
        }
    else if ( RMobilePhone::ECircuitDataService == iSymbianCallMode )
        {
        iMobileCallCaps.iCallControlCaps |=
            RCall::KCapsData | RCall::KCapsConnect;
        }
    else if ( RMobilePhone::EFaxService == iSymbianCallMode )
        {
        iMobileCallCaps.iCallControlCaps |= RCall::KCapsFax;
        }
    else
        {
        iMobileCallCaps.iCallControlCaps = 0x00000000;
        }

    iMobileCallCaps.iCallEventCaps = 0x000000ff;

    iMobileCallInfo.iForwarded = EFalse;
    iMobileCallInfo.iBCRepeatIndicator = RMobileCall::EBCAlternateMode;
 
    TInt err = iFeatureControl.Open();
	iUsingFeatureManager = (err == KErrNone);
	
	iSystemStatePluginHandler = iMmCallTsy->Phone()->SystemStatePluginHandler();

	ResetInternalAttributes();
    }

CMmCallGsmWcdmaExt* CMmCallGsmWcdmaExt::NewL(
    CMmCallTsy* aMmCallTsy,
    RMobilePhone::TMobileService aMode,
    CMmMessageManagerBase* aMessageManager )
    {
    CMmCallGsmWcdmaExt* callGsmWcdmaExt = new( ELeave ) CMmCallGsmWcdmaExt();

    CleanupStack::PushL( callGsmWcdmaExt );
    callGsmWcdmaExt->iMmCallTsy = aMmCallTsy;
    callGsmWcdmaExt->iSymbianCallMode = aMode;
    callGsmWcdmaExt->iMessageManager = aMessageManager;
    callGsmWcdmaExt->ConstructL();
    CleanupStack::Pop();

    return callGsmWcdmaExt;
    }
    
CMmCallGsmWcdmaExt::~CMmCallGsmWcdmaExt()
    {
    iFeatureControl.Close();
    if ( iDataCallCaps )
        {
        delete iDataCallCaps;
        }
    iDataCallCaps = NULL;
    if ( iCSDParams )
        {
        delete iCSDParams;
        }
    iCSDParams = NULL;
    if ( iHSCSDParams )
        {
        delete iHSCSDParams;
        }
    iHSCSDParams = NULL;
    if ( iCurrentHSCSDInfo )
        {
        delete iCurrentHSCSDInfo;
        }
    iCurrentHSCSDInfo = NULL;

    iMmCallTsy = NULL;
    iMessageManager = NULL;
    
    }
    
// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::ResetInternalAttributes
// Resets GSM call internal attributes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::ResetInternalAttributes()
    {
    // Add only attributes that can be set in call object construction 
    // (during ConstructL), in HangUp or in call object destruction 
    // (during ~CMmCallGsmWcdmaExt).
    iDataCallMode = EDataCallModeUnknown;

    iBearerService.iBearerCaps =
        RCall::KBearerCapsCompressionNone | RCall::KBearerCapsProtocolUnknown;
    iBearerService.iBearerSpeed = RCall::EBearerDataUnknown;

    // Set data call caps to default
    // Supported capabilities defined in MmTsy_Defaults.h, see also etelmm.h 
    iDataCallCaps->iSpeedCaps = KMultimodeTsyDataSpeedCaps;
    iDataCallCaps->iProtocolCaps = KMultimodeTsyDataProtocolCaps;
    iDataCallCaps->iServiceCaps = KMultimodeTsyDataServiceCaps;
    iDataCallCaps->iQoSCaps = KMultimodeTsyDataQoSCaps;
    iDataCallCaps->iHscsdSupport = KMultimodeTsyDataHscsdSupport;

    // additional flags for videotelephony
	if ( (iUsingFeatureManager) && (iFeatureControl.FeatureSupported(NFeature::KCsVideoTelephony) == KFeatureSupported) )
		{
		iDataCallCaps->iSpeedCaps |= KMultimodeTsyDataSpeedCapsVT;
	    iDataCallCaps->iProtocolCaps |= KMultimodeTsyDataProtocolCapsVT;
    	iDataCallCaps->iServiceCaps |= KMultimodeTsyDataServiceCapsVT;
		}
	
    // If Hscsd support is false, all the following iDataCallCaps are invalid
    // Detailed in GSM 05.02 V8.10.0 Appendix B
    // Multislot class
    iDataCallCaps->iMClass = KMultimodeTsyDataMClassDefValue;
    // Max number of Rx slots
    iDataCallCaps->iMaxRxTimeSlots = KMultimodeTsyDataMaxRxTSDefValue;
    // Max number of Tx slots
    iDataCallCaps->iMaxTxTimeSlots = KMultimodeTsyDataMaxTxTSDefValue;
    // Max number of Sum slots
    iDataCallCaps->iTotalRxTxTimeSlots = KMultimodeTsyDataTotalRxTxTSDefValue;
    iDataCallCaps->iCodingCaps = KMultimodeTsyDataTchCodingsCaps;
    iDataCallCaps->iAsymmetryCaps = KMultimodeTsyDataAsymmetryCaps;
    // Not supporting automatic service level upgrading
    iDataCallCaps->iUserInitUpgrade = KMultimodeTsyDataUserInitUpgrade;  
    iDataCallCaps->iRLPVersionCaps = KMultimodeTsyDataRLPVersionCaps;
    iDataCallCaps->iV42bisCaps = KMultimodeTsyDataV42bisCaps; 
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::DialL
// GSM specific Dial method for outgoing call
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::DialL(
    RMobilePhone::TMobileService aCallMode,
    const TDesC8* aCallParams,
    const TDesC* aTelNumber,
    TInt aExtensionId )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_DIALL_1, "TSY: CMmCallGsmWcdmaExt::DialL extensionid %d", aExtensionId );

    // Reset iCallParams extended members 
    iCallParams.iAlphaId.Zero();
    iCallParams.iIconId.iQualifier = RMobileCall::EIconQualifierNotSet;
    iCallParams.iIconId.iIdentifier = 0x00;
    iCallParams.iBCRepeatIndicator = RMobileCall::EBCAlternateMode;
    iCallParams.iBearerCap2.Zero();
    iCallParams.iBearerCap1.Zero();
    iCallParams.iSubAddress.Zero();
    iCallParams.iCallParamOrigin = RMobileCall::EOriginatorUnknown;
    iCallParams.iBearerMode = RMobileCall::EMulticallNotSupported;
    iCallParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
    iCallParams.iCug.iExplicitInvoke = EFalse;
    iCallParams.iCug.iCugIndex = 0xFFFF;
    iCallParams.iCug.iSuppressPrefCug = EFalse;
    iCallParams.iCug.iSuppressOA = EFalse;
    iCallParams.iAutoRedial = EFalse;
    
    //use base class to determine callparam version
    RCall::TCallParamsPckg* callParamsPckg = 
        reinterpret_cast<RCall::TCallParamsPckg*>(const_cast<TDesC8*>( aCallParams ) ); 

    RCall::TCallParams& callParams = ( *callParamsPckg )();
    
    TInt extensionId( callParams.ExtensionId() );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_DIALL_2, "TSY: CMmCallGsmWcdmaExt::DialL callParams.extensionId %d ", extensionId);

    if( ( RMobileCall::KETelMobileCallParamsV1 == extensionId ) || 
    		( RMobileCall::KETelMobileCallParamsV2 == extensionId ))
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_DIALL_3, "TSY: CMmCallGsmWcdmaExt::DialL callparams V1, V2");

        iCallParams.iSpeakerControl = callParams.iSpeakerControl;

        iCallParams.iSpeakerVolume = callParams.iSpeakerVolume;
        iCallParams.iInterval = callParams.iInterval;
        iCallParams.iWaitForDialTone = callParams.iWaitForDialTone;
        RMobileCall::TMobileCallParamsV1Pckg* paramsPckgV1 = 
            reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*>( 
            const_cast<TDesC8*>( aCallParams ) );
        RMobileCall::TMobileCallParamsV1& params1 = (*paramsPckgV1)();
        iCallParams.iIdRestrict = params1.iIdRestrict;
        iCallParams.iAutoRedial = params1.iAutoRedial;
        iCallParams.iCug.iExplicitInvoke = params1.iCug.iExplicitInvoke;
        iCallParams.iCug.iCugIndex = params1.iCug.iCugIndex;
        iCallParams.iCug.iSuppressOA = params1.iCug.iSuppressOA;
        iCallParams.iCug.iSuppressPrefCug = params1.iCug.iSuppressPrefCug;        

        if ( RMobileCall::KETelMobileCallParamsV2 == extensionId ) 
            {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_DIALL_4, "TSY: CMmCallGsmWcdmaExt::DialL callparams V2");
            RMobileCall::TMobileCallParamsV2Pckg* paramsPckgV2 = 
                reinterpret_cast<RMobileCall::TMobileCallParamsV2Pckg*>( 
                const_cast<TDesC8*>( aCallParams ) );
            
            RMobileCall::TMobileCallParamsV2& params2 = (*paramsPckgV2)();
            iCallParams.iBearerMode = params2.iBearerMode;
            }
        }

    else if( RMobileCall::KETelMobileCallParamsV7 == extensionId )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_DIALL_5, "TSY: CMmCallGsmWcdmaExt::DialL callparams V7");
        RMobileCall::TMobileCallParamsV7Pckg* paramsPckgV7 = 
            reinterpret_cast<RMobileCall::TMobileCallParamsV7Pckg*>( 
            const_cast<TDesC8*>( aCallParams ) );

        iCallParams = ( *paramsPckgV7 )();
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_DIALL_6, "TSY: CMmCallGsmWcdmaExt::DialL callparams version unknown");
        iCallParams.iSpeakerControl = callParams.iSpeakerControl;
        iCallParams.iSpeakerVolume = callParams.iSpeakerVolume;
        iCallParams.iInterval = callParams.iInterval;
        iCallParams.iWaitForDialTone = callParams.iWaitForDialTone;
        //default unknown values
        iCallParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
        iCallParams.iCug.iExplicitInvoke = EFalse;
        iCallParams.iCug.iCugIndex = 0xFFFF;
        iCallParams.iCug.iSuppressPrefCug = EFalse;
        iCallParams.iCug.iSuppressOA = EFalse;
        iCallParams.iAutoRedial = EFalse;
        }

    iMobileCallInfo.iDialledParty.iTelNumber.Copy( *aTelNumber );
    iMobileCallInfo.iDialledParty.iNumberPlan = 
        RMobilePhone::EUnknownNumberingPlan;
    iMobileCallInfo.iDialledParty.iTypeOfNumber = 
        RMobilePhone::EUnknownNumber;
    iMobileCallInfo.iService = aCallMode;
    iMobileCallInfo.iValid |= RMobileCall::KCallDialledParty;
    iMobileCallInfo.iValid |= RMobileCall::KCallAlternating;
    iMobileCallInfo.iAlternatingCall = 
        RMobilePhone::EAlternatingModeUnspecified;
    
    // Reset old info
    iMobileCallInfo.iRemoteParty.iDirection = RMobileCall::EDirectionUnknown;
    
    //Create package
    CCallDataPackage package;
    //Set call id and call mode
    package.SetCallIdAndMode( iMmCallTsy->CallId(), aCallMode );

    //Send request to the Domestic OS layer.
    if ( KETelExt3rdPartyV1 == aExtensionId )
    	{
        //Pack call parameters and mobile call info
        // Use argument parameters since TMobileCallParamsV7 does not inherit from 3rd party params.
        package.PackData(const_cast<TDesC8*>(aCallParams), &iMobileCallInfo);
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_DIALL_7, "TSY: CMmCallGsmWcdmaExt::DialL: KETelExt3rdPartyV1");
    	//if it is a 3rd party client
    	return iMessageManager->HandleRequestL( EMobileCallDialISV, 
            &package );
    	}
        
//if not a 3rd party client and dial no fdn check used    	
    else if ( KMultimodeCallTypeIDNoFdnCheck == aExtensionId )
        {
        //Pack call parameters and mobile call info
        TPckg<RMobileCall::TMobileCallParamsV7> pckgToSend(iCallParams); 
        package.PackData(  &pckgToSend , &iMobileCallInfo );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_DIALL_8, "TSY: CMmCallGsmWcdmaExt::DialL: KMultimodeCallTypeIDNoFdnCheck");
        return iMessageManager->HandleRequestL( 
            EMobileCallDialNoFdnCheck, &package );
        }    
    else
        {
        //Pack call parameters and mobile call info
        TPckg<RMobileCall::TMobileCallParamsV7> pckgToSend(iCallParams); 
        package.PackData(  &pckgToSend , &iMobileCallInfo );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_DIALL_9, "TSY: CMmCallGsmWcdmaExt::DialL: EEtelCallDial");
    	return iMessageManager->HandleRequestL( EEtelCallDial, &package );
    	}

    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::AnswerIncomingCallL
// Answers incoming call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::AnswerIncomingCallL(
    TInt aCallId,
    TInt aExtensionId )
    {
    //Create package
    CCallDataPackage package;

    //Set call id and call mode
    package.SetCallIdAndMode( aCallId, iSymbianCallMode );

	//Send request to the Domestic OS layer.
	if ( KETelExt3rdPartyV1 == aExtensionId )
		{
		//if it is a 3rd party client
		return iMessageManager->HandleRequestL( EMobileCallAnswerISV, 
            &package );			
		}
	else
		{
		//if not a 3rd party client
		return iMessageManager->HandleRequestL( EEtelCallAnswer, &package );		
		}
    
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::GetMobileCallCapsV1
// Return GSM specific mobile call capabilities (V1).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::GetMobileCallCapsV1(
    RMobileCall::TMobileCallCapsV1& aCallCapsV1 )
    {
    aCallCapsV1 = iMobileCallCaps;  
    }


// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::SetGSMCallCaps
// Set GSM specific mobile call caps
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::SetGSMCallCaps(
    const RMobileCall::TMobileCallCapsV1* aCaps )
    {
    iMobileCallCaps = *aCaps;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::AddGSMCallCaps
// Adds requested caps to GSM specific mobile call caps
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::AddGSMCallCaps(
    TUint32 aCaps )
    {
    iMobileCallCaps.iCallControlCaps |= aCaps;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::RemoveGSMCallCaps
// Removes requested caps from GSM specific mobile call caps
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::RemoveGSMCallCaps(
    TUint32 aCaps )
    {
    iMobileCallCaps.iCallControlCaps &= ~aCaps;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::CompleteNotifyStatusChange
// Updates the dynamic capabilities of the GSM extension
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::CompleteNotifyStatusChange(
    RMobileCall::TMobileCallStatus aCallStatus )
    {
    TInt ret( KErrNone );
    TBool singleCallGoingOn( EFalse );
    TBool twoCallsGoingOn( EFalse );
    TBool conferenceGoingOn( EFalse );
    TInt numberOfNonIdleCallObjects( 0 );

    // Get CMmPhone for information gathering
    CMmPhoneTsy* mmPhone = iMmCallTsy->Phone();
    // Get CallList
    CMmCallList* callList = mmPhone->CallList();
    // Get Conference call object
    CMmConferenceCallTsy* mmConference = mmPhone->ConferenceCall();
    // Create a pointer for call object
    CMmCallTsy* mmCall = NULL;
    
    //Save previous caps
    TUint32 oldControlCaps = iMobileCallCaps.iCallControlCaps;

    //Count number of call objects that are not in idle state
    for ( TInt i = 0; i < callList->GetNumberOfObjects(); i++ )
        {
        mmCall = callList->GetMmCallByIndex( i );
        if ( RMobileCall::EStatusIdle != mmCall->MobileCallStatus() )
            {
            numberOfNonIdleCallObjects++;
            }
        }

    //if there is only one non-idle call
    if ( numberOfNonIdleCallObjects == 1 )
        {
        singleCallGoingOn = ETrue;
        }
    //multicall situation
    else
        {
        TBool confInUse( EFalse );

        //if conference call object exists
        if ( mmConference )
            {
            //if conference call is not in idle state
            //or if CreateConference has been requested
            if ( ( mmConference->Status() !=
                    RMobileConferenceCall::EConferenceIdle ) ||
                mmConference->ServiceRequested(
                    CMmConferenceCallTsy::EMultimodeConferenceCallCreateConference ) )
                {
                //conference is going on, act according the situation
                conferenceGoingOn = ETrue;
                confInUse = ETrue;
                }
            }
        //if conference call is not active
        if ( !confInUse )
            {
            //conference call is not in use. There are 2 calls in different
            //states.
            twoCallsGoingOn = ETrue;
            }
        }

    // Set Mobile Call Control + Core Caps
    // Switch according to the call status
    switch ( aCallStatus )
        {
        // Clear caps for idle status
        case RMobileCall::EStatusIdle:
            iMobileCallInfo.iValid = 0x00000000;
            
            // Set Idle caps
            iMobileCallCaps.iCallControlCaps |= 
                RCall::KCapsDial;

            // Data call has also connect.
            // Remove dataport caps.
            if ( RMobilePhone::ECircuitDataService == iSymbianCallMode )
                {
                iMobileCallCaps.iCallControlCaps |= 
                    RCall::KCapsConnect;
                iMobileCallCaps.iCallControlCaps &= ~(
                    RCall::KCapsLoanDataPort | 
                    RCall::KCapsRecoverDataPort );
                }

            // Clear possible old caps
            //Caps are set in 2 phases because defined in different
            //classes (lint complains about different types)
            iMobileCallCaps.iCallControlCaps &= ~(
                RCall::KCapsAnswer |
                RCall::KCapsHangUp );
            iMobileCallCaps.iCallControlCaps &= ~(
                RMobileCall::KCapsHold | 
                RMobileCall::KCapsResume | 
                RMobileCall::KCapsSwap | 
                RMobileCall::KCapsDeflect |
                RMobileCall::KCapsTransfer |
                RMobileCall::KCapsJoin |
                RMobileCall::KCapsOneToOne |
                RMobileCall::KCapsSwitchAlternatingCall );

            if ( conferenceGoingOn )
                {
                //reset the enable Join flag
                iEnableJoinFlag = EFalse;
                CMmCallGsmWcdmaExt* mmCallGsmWcdmaExt = NULL;

                //loop through all call objects updating GoOneToOne caps
                for ( TInt i = 0; i < callList->GetNumberOfObjects(); i++ )
                    {
                    mmCall = callList->GetMmCallByIndex( i );

                    if ( mmCall->MobileCallStatus() == RMobileCall::EStatusConnected 
                        && mmConference)
                        {
                        if ( mmConference->IsGoOneToOneSupported( mmCall->CallId() ) )
                            {
                            // Fetch the active call extension
                            mmCallGsmWcdmaExt = REINTERPRET_CAST( 
                                CMmCallGsmWcdmaExt*, mmCall->ActiveCallExtension() );

                            //add GoOneToOne caps
                            mmCallGsmWcdmaExt->AddGSMCallCaps( 
                                RMobileCall::KCapsOneToOne );

                            //complete caps change notification
                            mmCall->CompleteNotifyMobileCallCapsChange( KErrNone );
                            }
                        }
                    }
                }
            break;
        case RMobileCall::EStatusDialling:
            //activate HangUp caps
            iMobileCallCaps.iCallControlCaps |=
                RCall::KCapsHangUp;

            iMobileCallCaps.iCallControlCaps &= ~(
                RCall::KCapsDial | RCall::KCapsConnect );

            break;
        case RMobileCall::EStatusRinging:
            //Caps are set in 2 phases because defined in different
            //classes (lint complains about different types)
            iMobileCallCaps.iCallControlCaps |=
                RCall::KCapsHangUp;
            iMobileCallCaps.iCallControlCaps |=
                RMobileCall::KCapsDeflect;

            //if there is no held and active calls
            if ( !( ( callList->GetMmCallByStatus( 
                        RMobileCall::EStatusHold ) != NULL ) &&
                ( callList->GetMmCallByStatus( 
                        RMobileCall::EStatusConnected ) != NULL ) ) )
                {
                iMobileCallCaps.iCallControlCaps |= 
                    RCall::KCapsAnswer;
                }
                
            if ( RMobilePhone::ECircuitDataService == iSymbianCallMode )
                {
                if ( ( callList->GetMmCallByStatus(
                        RMobileCall::EStatusConnected ) == NULL ) &&
                     ( callList->GetMmCallByStatus(
                        RMobileCall::EStatusHold ) == NULL ) &&
                     ( callList->GetMmCallByStatus(
                        RMobileCall::EStatusDisconnecting ) == NULL ) &&
                     ( callList->GetMmCallByStatus(
                        RMobileCall::EStatusDisconnectingWithInband ) == NULL ) )
                    {
                    iMobileCallCaps.iCallControlCaps |=
                        RCall::KCapsAnswer;
                    // Set call caps
                    TUint32 callCaps = iMmCallTsy->CallCaps();
                    callCaps |= RCall::KCapsAnswer;
                    iMmCallTsy->SetCallCaps( callCaps );
                    }
                }
            else
                {
                if ( !( ( callList->GetMmCallByStatus(
                        RMobileCall::EStatusHold ) != NULL ) &&
                    ( callList->GetMmCallByStatus(
                        RMobileCall::EStatusConnected ) != NULL ) ) )
                    {
                    iMobileCallCaps.iCallControlCaps |=
                        RCall::KCapsAnswer;
                    }
                }               

            //disable Dial caps
            iMobileCallCaps.iCallControlCaps &= ~(
                RCall::KCapsDial | RCall::KCapsConnect );

            break;
        case RMobileCall::EStatusAnswering:
            //activate hangup caps. EStatusRinging may have been missed
            //so this needs to be activated.
            iMobileCallCaps.iCallControlCaps |= RCall::KCapsHangUp;

            //remove dial, answer and deflect caps
            //Caps are set in 2 phases because defined in different
            //classes (lint complains about different types)
            iMobileCallCaps.iCallControlCaps &= ~(
                RCall::KCapsDial |
                RCall::KCapsAnswer );
            iMobileCallCaps.iCallControlCaps &= ~(
                RMobileCall::KCapsDeflect );

            //if conference call is going on
            if ( conferenceGoingOn )
                {
                //enable Join flag when call enters the connected state
                iEnableJoinFlag = ETrue;
                }
            break;
        case RMobileCall::EStatusConnecting:
            //if conference call is going on
            if ( conferenceGoingOn )
                {
                //enable Join flag when call enters the connected state
                iEnableJoinFlag = ETrue;

                //loop through all call objects searching held calls
                for ( TInt i = 0; i < callList->GetNumberOfObjects(); i++ )
                    {
                    mmCall = callList->GetMmCallByIndex( i );

                    if ( mmCall->MobileCallStatus() == 
                            RMobileCall::EStatusHold )
                        {
                        //If status is Hold, disable GoOneToOne and Dial caps
                        //Caps are set in 2 phases because defined in different
                        //classes (lint complains about different types)
                        iMobileCallCaps.iCallControlCaps &= ~(
                            RCall::KCapsDial );
                        iMobileCallCaps.iCallControlCaps &= ~(
                            RMobileCall::KCapsOneToOne );
                        }
                    }
                }
            else if ( twoCallsGoingOn )
                {
                //if there is a call on Hold state
                if ( callList->GetMmCallByStatus( 
                        RMobileCall::EStatusHold ) != NULL )
                    {
                    //enable transfer caps
                    iMobileCallCaps.iCallControlCaps |=
                        RMobileCall::KCapsTransfer;
                    }
                }
            break;
        case RMobileCall::EStatusDisconnecting:
        case RMobileCall::EStatusDisconnectingWithInband:
            //disable some caps
            iMobileCallCaps.iCallControlCaps &= ~(
                RMobileCall::KCapsHold |
                RMobileCall::KCapsResume |
                RMobileCall::KCapsSwap |
                RMobileCall::KCapsDeflect |
                RMobileCall::KCapsTransfer |
                RMobileCall::KCapsJoin |
                RMobileCall::KCapsOneToOne |
                RMobileCall::KCapsSwitchAlternatingCall |
                RMobileCall::KCapsActivateCCBS );
            // Remove dataport caps.
            if ( RMobilePhone::ECircuitDataService == iSymbianCallMode )
                {
                iMobileCallCaps.iCallControlCaps &= ~(RCall::KCapsLoanDataPort);
                }
            break;
        case RMobileCall::EStatusReconnectPending:
        case RMobileCall::EStatusWaitingAlternatingCallSwitch:
            //Clear useless caps
            //Caps are set in 2 phases because defined in different
            //classes (lint complains about different types)
            iMobileCallCaps.iCallControlCaps &= ~(
                RCall::KCapsHangUp );
            iMobileCallCaps.iCallControlCaps &= ~(
                RMobileCall::KCapsHold |
                RMobileCall::KCapsResume |
                RMobileCall::KCapsSwap |
                RMobileCall::KCapsDeflect |
                RMobileCall::KCapsTransfer |
                RMobileCall::KCapsJoin |
                RMobileCall::KCapsOneToOne |
                RMobileCall::KCapsSwitchAlternatingCall );
            break;
        case RMobileCall::EStatusUnknown:
        case RMobileCall::EStatusConnected:
        case RMobileCall::EStatusHold:
        case RMobileCall::EStatusTransferring:
        case RMobileCall::EStatusTransferAlerting:
        // Default: do nothing
        default:
            if ( singleCallGoingOn )
                {
                UpdateSingleCallControlCaps( aCallStatus );
                }
            else if ( twoCallsGoingOn )
                {
                UpdateDoubleCallControlCaps( aCallStatus );
                }
            else if ( conferenceGoingOn )
                {
                UpdateMultipleCallControlCaps( 
                    aCallStatus );
                }
            break;
        }

    if ( oldControlCaps == iMobileCallCaps.iCallControlCaps )
        {
        ret = KErrNotFound;
        }
    else
        {
        ret =  KErrNone;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::UpdateMultipleCallControlCaps
// This method updates call capabilities in case that conference call is going 
// on.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::UpdateMultipleCallControlCaps(
    RMobileCall::TMobileCallStatus aCallStatus )
    {
    // Get CMmPhone for information gathering
    CMmPhoneTsy* mmPhone = iMmCallTsy->Phone();
    // Get CallList
    CMmCallList* callList = mmPhone->CallList();

    TInt numberOfCallsInConfererence = 
        mmPhone->ConferenceCall()->NumberOfCallsInConference();

    // Set Mobile Call Control + Core Caps
    // Switch according to the call status
    switch ( aCallStatus )
        {
        // Enable caps for an active call
        case RMobileCall::EStatusConnected:
            //Clear useless caps
            //Caps are set in 2 phases because defined in different
            //classes (lint complains about different types)
            iMobileCallCaps.iCallControlCaps &= ~(
                RCall::KCapsDial |
                RCall::KCapsAnswer );
            iMobileCallCaps.iCallControlCaps &= ~(
                RMobileCall::KCapsTransfer |
                RMobileCall::KCapsHold |
                RMobileCall::KCapsResume |
                RMobileCall::KCapsDeflect );

            if ( NULL == callList->GetMmCallByStatus( 
                 RMobileCall::EStatusHold ) )
                {
                //Go one to one is possible
                iMobileCallCaps.iCallControlCaps |= 
                    RMobileCall::KCapsOneToOne;
                }

            //This is a case where conference call is going on and a new call 
            //has entered the connected state -> Join and Swap caps might be 
            //enabled in this situation
            if ( iEnableJoinFlag )
                {
                //if there are calls on hold status and more participants can 
                //be added to conference call
                if ( callList->GetMmCallByStatus( 
                    RMobileCall::EStatusHold ) != NULL &&
                    numberOfCallsInConfererence < KMaxCallsInConference )
                    {
                    //This call is not part of conference call, Join flag can
                    //be enabled. Only the call that is not part of conference
                    //can have Swap caps enabled.
                    iMobileCallCaps.iCallControlCaps |= 
                        RMobileCall::KCapsJoin;
                    }
                //reset the flag
                iEnableJoinFlag = EFalse;
                } 
            break;
        case RMobileCall::EStatusHold:
            //GoOneToOne is not possible in Hold state, disable it.
            iMobileCallCaps.iCallControlCaps &= ~(    
                RMobileCall::KCapsOneToOne );
            break;
        case RMobileCall::EStatusUnknown:
        case RMobileCall::EStatusIdle:
        case RMobileCall::EStatusDialling:
        case RMobileCall::EStatusRinging:
        case RMobileCall::EStatusAnswering:
        case RMobileCall::EStatusConnecting:
        case RMobileCall::EStatusDisconnecting:
        case RMobileCall::EStatusDisconnectingWithInband:
        case RMobileCall::EStatusReconnectPending:
        case RMobileCall::EStatusWaitingAlternatingCallSwitch:
        case RMobileCall::EStatusTransferring:
        case RMobileCall::EStatusTransferAlerting:
        // Default: do nothing
        default:
            break;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::UpdateDoubleCallControlCaps
// This method updates call capabilities in case that there are two calls in 
// non-idle state and they are not in a conference.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::UpdateDoubleCallControlCaps(
    RMobileCall::TMobileCallStatus aCallStatus )
    {
    // Get CMmPhone for information gathering
    CMmPhoneTsy* mmPhone = iMmCallTsy->Phone();
    // Get CallList
    CMmCallList* callList = mmPhone->CallList();

    // Set Mobile Call Control + Core Caps
    // Switch according to the call status
    switch ( aCallStatus )
        {
        // Enable caps for an active call
        case RMobileCall::EStatusConnected:
            //Clear useless caps
            //Caps are set in 2 phases because defined in different
            //classes (lint complains about different types)
            iMobileCallCaps.iCallControlCaps &= ~(
                RCall::KCapsAnswer |
                RCall::KCapsDial );
            iMobileCallCaps.iCallControlCaps &= ~(
                RMobileCall::KCapsTransfer |
                RMobileCall::KCapsJoin |
                RMobileCall::KCapsResume |
                RMobileCall::KCapsSwap |
                RMobileCall::KCapsDeflect |
                RMobileCall::KCapsHold );

            //if there is a call on hold
            if ( callList->GetMmCallByStatus( 
                    RMobileCall::EStatusHold ) != NULL )
                {
                //enable swap, join and transfer caps
                iMobileCallCaps.iCallControlCaps |= (
                    RMobileCall::KCapsSwap |
                    RMobileCall::KCapsJoin |
                    RMobileCall::KCapsTransfer );
                }
            //if there is waiting call
            else if ( callList->GetMmCallByStatus( 
                    RMobileCall::EStatusRinging ) != NULL )
                {
                //enable swap, join and transfer caps
                iMobileCallCaps.iCallControlCaps |= 
                    RMobileCall::KCapsHold;
                }

            break;
        case RMobileCall::EStatusHold:
            //clear useless caps
            iMobileCallCaps.iCallControlCaps &= ~(
                RMobileCall::KCapsOneToOne |
                RMobileCall::KCapsTransfer |
                RMobileCall::KCapsSwap |
                RMobileCall::KCapsHold |
                RMobileCall::KCapsJoin |
                RMobileCall::KCapsResume );
            
            //if there is a call on Connected state
            if ( callList->GetMmCallByStatus( 
                    RMobileCall::EStatusConnected ) != NULL )
                {
                //enable swap, join and transfer caps
                iMobileCallCaps.iCallControlCaps |= 
                    RMobileCall::KCapsSwap |
                    RMobileCall::KCapsJoin |
                    RMobileCall::KCapsTransfer;
                }
            //if there is waiting call
            else if ( callList->GetMmCallByStatus( 
                    RMobileCall::EStatusRinging ) != NULL )
                {
                //enable swap, join and transfer caps
                iMobileCallCaps.iCallControlCaps |= 
                    RMobileCall::KCapsResume;
                }

            //if there is a call on Connecting state
            if ( callList->GetMmCallByStatus( 
                    RMobileCall::EStatusConnecting ) != NULL )
                {
                //enable transfer caps
                iMobileCallCaps.iCallControlCaps |=
                    RMobileCall::KCapsTransfer;
                }
            break;
        case RMobileCall::EStatusUnknown:
        case RMobileCall::EStatusIdle:
        case RMobileCall::EStatusDialling:
        case RMobileCall::EStatusRinging:
        case RMobileCall::EStatusAnswering:
        case RMobileCall::EStatusConnecting:
        case RMobileCall::EStatusDisconnecting:
        case RMobileCall::EStatusDisconnectingWithInband:
        case RMobileCall::EStatusReconnectPending:
        case RMobileCall::EStatusWaitingAlternatingCallSwitch:
        case RMobileCall::EStatusTransferring:
        case RMobileCall::EStatusTransferAlerting:
        // Default do nothing
        default:
            break;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::UpdateSingleCallControlCaps
// This method updates call capabilities in case that there is only one 
// non-idle call
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::UpdateSingleCallControlCaps(
    RMobileCall::TMobileCallStatus aCallStatus )
    {
    // Set Mobile Call Control + Core Caps
    // Switch according to the call status
    switch ( aCallStatus )
        {
        case RMobileCall::EStatusConnected:
            //Disable some caps for an active call
            //Caps are set in 2 phases because defined in different
            //classes (lint complains about different types)
            iMobileCallCaps.iCallControlCaps &= ~(
                RCall::KCapsAnswer |
                RCall::KCapsDial );
            iMobileCallCaps.iCallControlCaps &= ~(
                RMobileCall::KCapsTransfer |
                RMobileCall::KCapsResume |
                RMobileCall::KCapsSwap |
                RMobileCall::KCapsDeflect |
                RMobileCall::KCapsJoin |
                RMobileCall::KCapsOneToOne );

            //activate HangUp caps
            iMobileCallCaps.iCallControlCaps |=
                RCall::KCapsHangUp;

            // Data call can't be controlled with swap, resume & join
            if ( RMobilePhone::ECircuitDataService != iSymbianCallMode 
                && RMobilePhone::EFaxService != iSymbianCallMode )
                {
                iMobileCallCaps.iCallControlCaps |= 
                    RMobileCall::KCapsHold;
                }
            break;
        case RMobileCall::EStatusHold:
            //disable some caps first
            iMobileCallCaps.iCallControlCaps &= ~(
                RMobileCall::KCapsOneToOne |
                RMobileCall::KCapsTransfer |
                RMobileCall::KCapsSwap |
                RMobileCall::KCapsHold |
                RMobileCall::KCapsJoin |
                RMobileCall::KCapsOneToOne |
                RMobileCall::KCapsResume );

            //activate HangUp caps
            iMobileCallCaps.iCallControlCaps |=
                RCall::KCapsHangUp;

            // Data call can't be controlled with swap, resume & join
            if ( RMobilePhone::ECircuitDataService != iSymbianCallMode 
                && RMobilePhone::EFaxService != iSymbianCallMode )
                {
                iMobileCallCaps.iCallControlCaps |= 
                    RMobileCall::KCapsResume;
                }
            break;
        case RMobileCall::EStatusUnknown:
        case RMobileCall::EStatusIdle:
        case RMobileCall::EStatusDialling:
        case RMobileCall::EStatusRinging:
        case RMobileCall::EStatusAnswering:
        case RMobileCall::EStatusConnecting:
        case RMobileCall::EStatusDisconnecting:
        case RMobileCall::EStatusDisconnectingWithInband:
        case RMobileCall::EStatusReconnectPending:
        case RMobileCall::EStatusWaitingAlternatingCallSwitch:
        case RMobileCall::EStatusTransferring:
        case RMobileCall::EStatusTransferAlerting:
        // Default do nothing
        default:
            break;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::GetCallParams
// Returns the GSM specific call parameters in parameter.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::GetCallParams(
    TDes8* aParams )
    {
    RCall::TCallParamsPckg* callParamsPckg = 
        reinterpret_cast<RCall::TCallParamsPckg*>( aParams ); 
    RCall::TCallParams& callParams = ( *callParamsPckg )();

    TInt extensionId( callParams.ExtensionId() );

    if ( RMobilePhone::ECircuitDataService != iSymbianCallMode )
        {
        if ( RMobileCall::KETelMobileCallParamsV1 == extensionId ||
             RMobileCall::KETelMobileDataCallParamsV1 == extensionId ||
             RMobileCall::KETelMobileDataCallParamsV2 == extensionId ||
             RMobileCall::KETelMobileDataCallParamsV8 == extensionId ||
             RMobileCall::KETelMobileHscsdCallParamsV1 == extensionId ||
             RMobileCall::KETelMobileHscsdCallParamsV2 == extensionId ||
             RMobileCall::KETelMobileHscsdCallParamsV7 == extensionId ||
             RMobileCall::KETelMobileHscsdCallParamsV8 == extensionId )
            {
            RMobileCall::TMobileCallParamsV1Pckg* paramsPckgV1 =
                reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*>(
                    aParams );
            RMobileCall::TMobileCallParamsV1& params = ( *paramsPckgV1 )();

                    
            //Parameter of base class TMobileCallParamsV1
            params.iIdRestrict = iCallParams.iIdRestrict;
            params.iCug = iCallParams.iCug;
            params.iAutoRedial = iCallParams.iAutoRedial;
            }
        else if ( RMobileCall::KETelMobileCallParamsV2 == extensionId )
            {
            RMobileCall::TMobileCallParamsV2Pckg* paramsPckgV2 =
                reinterpret_cast<RMobileCall::TMobileCallParamsV2Pckg*>(
                    aParams );
            RMobileCall::TMobileCallParamsV2& params = ( *paramsPckgV2 )();

            //Parameter of base class TMobileCallParamsV1
            params.iIdRestrict = iCallParams.iIdRestrict;
            params.iCug = iCallParams.iCug;
            params.iAutoRedial = iCallParams.iAutoRedial;
            //Parameter of class TMobileCallParamsV2
            params.iBearerMode = iCallParams.iBearerMode; 
            }
        else if ( RMobileCall::KETelMobileCallParamsV7 == extensionId )
            {
              
            RMobileCall::TMobileCallParamsV7Pckg* paramsPckgV7 =
                reinterpret_cast<RMobileCall::TMobileCallParamsV7Pckg*>(
                    aParams );
            RMobileCall::TMobileCallParamsV7& params = ( *paramsPckgV7 )();
            
            //Parameter of base class TMobileCallParamsV1
            params.iIdRestrict = iCallParams.iIdRestrict;
            params.iCug = iCallParams.iCug;
            params.iAutoRedial = iCallParams.iAutoRedial;
            
            //Parameter of class TMobileCallParamsV2
            params.iBearerMode = iCallParams.iBearerMode;
            
            //Paremeters of class TMobileCallParamsV7
    		params.iCallParamOrigin = iCallParams.iCallParamOrigin;
    		params.iSubAddress = iCallParams.iSubAddress;
    		params.iBearerCap1 = iCallParams.iBearerCap1;
    		params.iBearerCap2 = iCallParams.iBearerCap2;
    		params.iBCRepeatIndicator = iCallParams.iBCRepeatIndicator;
    		params.iIconId.iQualifier = iCallParams.iIconId.iQualifier;
     		params.iIconId.iIdentifier = iCallParams.iIconId.iIdentifier;
    	 	params.iAlphaId = iCallParams.iAlphaId;
	 	
            }
        else
        	{
        	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_GETCALLPARAMS_1, "TSY: CMmCallGsmWcdmaExt::GetCallParams callparams version unknown");
        	}

        }
    else
    	{
	    if ( RMobileCall::KETelMobileCallParamsV1 == extensionId ||
	    	 RMobileCall::KETelMobileCallParamsV2 == extensionId ||
	    	 RMobileCall::KETelMobileCallParamsV7 == extensionId ||
	         RMobileCall::KETelMobileDataCallParamsV1 == extensionId ||
	         RMobileCall::KETelMobileDataCallParamsV2 == extensionId ||
	         RMobileCall::KETelMobileDataCallParamsV8 == extensionId ||
	         RMobileCall::KETelMobileHscsdCallParamsV1 == extensionId ||
	         RMobileCall::KETelMobileHscsdCallParamsV2 == extensionId ||
	         RMobileCall::KETelMobileHscsdCallParamsV7 == extensionId ||
	         RMobileCall::KETelMobileHscsdCallParamsV8 == extensionId )
	        {
	        RMobileCall::TMobileCallParamsV1Pckg* paramsPckgV1 =
                reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*>(
                    aParams );
            RMobileCall::TMobileCallParamsV1& params = ( *paramsPckgV1 )();

            //Parameter of base class TMobileCallParamsV1
            params.iIdRestrict = iCSDParams->iIdRestrict;
            params.iCug = iCSDParams->iCug;
            params.iAutoRedial = iCSDParams->iAutoRedial;
	                
	        }
	    
	    if ( RMobileCall::KETelMobileDataCallParamsV1 == extensionId ||
	    	 RMobileCall::KETelMobileDataCallParamsV2 == extensionId ||
	    	 RMobileCall::KETelMobileDataCallParamsV8 == extensionId ||
	    	 RMobileCall::KETelMobileHscsdCallParamsV1 == extensionId ||
 	         RMobileCall::KETelMobileHscsdCallParamsV2 == extensionId ||
 	         RMobileCall::KETelMobileHscsdCallParamsV7 == extensionId ||
 	         RMobileCall::KETelMobileHscsdCallParamsV8 == extensionId )
	        {
	        RMobileCall::TMobileDataCallParamsV1Pckg* paramsPckgV1 =
	            reinterpret_cast<RMobileCall::TMobileDataCallParamsV1Pckg*>(
	                aParams );
	        RMobileCall::TMobileDataCallParamsV1& dataParams = ( *paramsPckgV1 )();
	        
	        CopyMobileDataCallParams( dataParams, *iCSDParams );
	                
	        }
	    
	    if ( RMobileCall::KETelMobileDataCallParamsV2 == extensionId ||
	    	 RMobileCall::KETelMobileDataCallParamsV8 == extensionId)
	        {
	        RMobileCall::TMobileDataCallParamsV2Pckg* paramsPckgV2 =
	            reinterpret_cast<RMobileCall::TMobileDataCallParamsV2Pckg*>(
	                aParams );
	        RMobileCall::TMobileDataCallParamsV2& dataParams2 = ( *paramsPckgV2 )();
	
	        RMobileCall::TMobileDataCallParamsV2* dataCallParamsV2 = static_cast<RMobileCall::TMobileDataCallParamsV2*>(iCSDParams);
	        
	        //Parameter of class TMobileDataCallParamsV2
	        dataParams2.iBearerMode = dataCallParamsV2->iBearerMode;
	        }
	    
	    if ( RMobileCall::KETelMobileDataCallParamsV8 == extensionId )
	        {
	        RMobileCall::TMobileDataCallParamsV8Pckg* paramsPckgV8 =
	           reinterpret_cast<RMobileCall::TMobileDataCallParamsV8Pckg*>(
	              aParams );
	        RMobileCall::TMobileDataCallParamsV8& dataParams8 = ( *paramsPckgV8 )();
	        
	        RMobileCall::TMobileDataCallParamsV8* dataCallParamsV8 = static_cast<RMobileCall::TMobileDataCallParamsV8*>(iCSDParams);
	
	        //Parameters of class TMobileDataCallParamsV8
	        dataParams8.iSubAddress = dataCallParamsV8->iSubAddress;
	        dataParams8.iBearerCap1 = dataCallParamsV8->iBearerCap1;
	        dataParams8.iBearerCap2 = dataCallParamsV8->iBearerCap2;
	        dataParams8.iBCRepeatIndicator = dataCallParamsV8->iBCRepeatIndicator;
	        dataParams8.iCallParamOrigin = dataCallParamsV8->iCallParamOrigin;
	        dataParams8.iIconId.iQualifier = dataCallParamsV8->iIconId.iQualifier;
	        dataParams8.iIconId.iIdentifier = dataCallParamsV8->iIconId.iIdentifier;
	        dataParams8.iAlphaId = dataCallParamsV8->iAlphaId;
	        
	        }
	    
	    if ( RMobileCall::KETelMobileHscsdCallParamsV1 == extensionId ||
	    	 RMobileCall::KETelMobileHscsdCallParamsV2 == extensionId ||
	    	 RMobileCall::KETelMobileHscsdCallParamsV7 == extensionId ||
	    	 RMobileCall::KETelMobileHscsdCallParamsV8 == extensionId)
	        {
	        RMobileCall::TMobileHscsdCallParamsV1Pckg* paramsPckgV1 =
	            reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV1Pckg*>(
	                aParams );
	        RMobileCall::TMobileHscsdCallParamsV1& hscsdParams =
	            ( *paramsPckgV1 )();
	
	        //Parameter of base class TMobileHscsdCallParamsV1
	        hscsdParams.iWantedAiur = iHSCSDParams->iWantedAiur;
	        hscsdParams.iWantedRxTimeSlots = iHSCSDParams->iWantedRxTimeSlots;
	        hscsdParams.iMaxTimeSlots = iHSCSDParams->iMaxTimeSlots;
	        hscsdParams.iCodings = iHSCSDParams->iCodings;
	        hscsdParams.iAsymmetry = iHSCSDParams->iAsymmetry;
	        hscsdParams.iUserInitUpgrade = iHSCSDParams->iUserInitUpgrade;
	        }
	    
	    if ( RMobileCall::KETelMobileHscsdCallParamsV2 == extensionId ||
	    	 RMobileCall::KETelMobileHscsdCallParamsV7 == extensionId ||
		   	 RMobileCall::KETelMobileHscsdCallParamsV8 == extensionId)
	        {
	        RMobileCall::TMobileHscsdCallParamsV2Pckg* paramsPckgV2 =
	            reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV2Pckg*>(
	                aParams );
	        RMobileCall::TMobileHscsdCallParamsV2& hscsdParams2 =
	            ( *paramsPckgV2 )();
	
	        //Parameter of class TMobileHscsdCallParamsV2
	        RMobileCall::TMobileHscsdCallParamsV2* hscsdCallParamsV2 = static_cast<RMobileCall::TMobileHscsdCallParamsV2*>(iHSCSDParams);
	        hscsdParams2.iBearerMode = hscsdCallParamsV2->iBearerMode;
	        }
	    
	    if ( RMobileCall::KETelMobileHscsdCallParamsV7 == extensionId ||
	         RMobileCall::KETelMobileHscsdCallParamsV8 == extensionId)
	        {
	        RMobileCall::TMobileHscsdCallParamsV7Pckg* paramsPckgV7 =
	            reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV7Pckg*>(
	                aParams );
	        RMobileCall::TMobileHscsdCallParamsV7& hscsdParams7 =
	            ( *paramsPckgV7 )();
	
	        RMobileCall::TMobileHscsdCallParamsV7* hscsdCallParamsV7 = static_cast<RMobileCall::TMobileHscsdCallParamsV7*>(iHSCSDParams);
	                
	        //Parameter of class TMobileHscsdCallParamsV7
	        hscsdParams7.iCallParamOrigin = hscsdCallParamsV7->iCallParamOrigin;
	        hscsdParams7.iIconId.iQualifier = hscsdCallParamsV7->iIconId.iQualifier;
		    hscsdParams7.iIconId.iIdentifier = hscsdCallParamsV7->iIconId.iIdentifier;
	        hscsdParams7.iAlphaId = hscsdCallParamsV7->iAlphaId;
	        }
	    
	    if ( RMobileCall::KETelMobileHscsdCallParamsV8 == extensionId )
	        {
	        RMobileCall::TMobileHscsdCallParamsV8Pckg* paramsPckgV8 =
	            reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV8Pckg*>(
	                aParams );
	        RMobileCall::TMobileHscsdCallParamsV8& hscsdParams8 =
	            ( *paramsPckgV8 )();
	
	        RMobileCall::TMobileHscsdCallParamsV8* hscsdCallParamsV8 = static_cast<RMobileCall::TMobileHscsdCallParamsV8*>(iHSCSDParams);
	        
	        //Parameter of class TMobileHscsdCallParamsV8
	        hscsdParams8.iSubAddress = hscsdCallParamsV8->iSubAddress;
	        hscsdParams8.iBearerCap1 = hscsdCallParamsV8->iBearerCap1;
	        hscsdParams8.iBearerCap2 = hscsdCallParamsV8->iBearerCap2;
	        hscsdParams8.iBCRepeatIndicator = hscsdCallParamsV8->iBCRepeatIndicator;
	          
	        }
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_GETCALLPARAMS_2, "TSY: CMmCallGsmWcdmaExt::GetCallParams callparams version unknown");
	    
    	}
       
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::DialEmergencyCallL
// GSM specific DialEmergencyCall method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::DialEmergencyCall(
        TCtsySsmCallbackData& aCallbackData )
    {    
    // store stateInfo and Emergency number     
    // to have access to them inside callback
    TRfStateInfo stateInfo = iMmCallTsy->Phone()->GetRfStateInfo();
                
    if( stateInfo != ERfsStateInfoNormal )
        {
        //the method in callTsy has already checked that
        // emergency calls in offline are enabled
        //otherwise this code woulnd't be reached
					
        //set the RF to normal state so the emergency call can be dialed        
        SetRfState( ERfsStateInfoNormal, this, aCallbackData );        
        }
     else 
     	{
     	// if there is no need to change RF state 
     	// then call SsmPluginCallback immediately
     	SsmPluginCallback(KErrNone, aCallbackData);
     	}   
     	
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::HoldL
// This method can be used to hold an active call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::HoldL(
    TInt aCallId )
    {    
    TInt ret( KErrNone );

    CMmConferenceCallTsy* mmConference = 
    iMmCallTsy->Phone()->ConferenceCall();

    if ( NULL != mmConference )
        {
        if ( RMobileConferenceCall::EConferenceIdle != 
            mmConference->Status() )
            {
            ret = KErrNotReady;
            }
        }

    if ( KErrNone == ret )
        {
        //Create package
        CCallDataPackage package;
        //Set call id and call mode
        package.SetCallIdAndMode( aCallId, iSymbianCallMode );
        //Send request to the Domestic OS layer.
        ret = iMessageManager->HandleRequestL( EMobileCallHold, &package );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::ResumeL
// This method can be used to resume a held call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::ResumeL(
    TInt aCallId )
    {
    TInt ret( KErrNone );

    CMmConferenceCallTsy* mmConference = 
    iMmCallTsy->Phone()->ConferenceCall();

    if ( NULL != mmConference )
        {
    
        if ( RMobileConferenceCall::EConferenceIdle != 
            mmConference->Status() )
            {
            ret = KErrNotReady;
            }
        }

    if ( KErrNone == ret )
        {
        //Create package
        CCallDataPackage package;
        //Set call id and call mode
        package.SetCallIdAndMode( aCallId, iSymbianCallMode );
        //Send request to the Domestic OS layer.
        ret = iMessageManager->HandleRequestL( EMobileCallResume, &package );
        }    

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::SwapL
// This method can be used to swap between active and held calls.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
#ifndef USING_CTSY_DISPATCHER
TInt CMmCallGsmWcdmaExt::SwapL(
    TInt aCallId )
    {
    TInt ret( KErrNone );

	//Create package
    CCallDataPackage package;
    //Set call id and call mode
    package.SetCallIdAndMode( aCallId, iSymbianCallMode );
    //Send request to the Domestic OS layer.
    ret = iMessageManager->HandleRequestL( EMobileCallSwap, &package );

    return ret;     
    }
#endif //USING_CTSY_DISPATCHER

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::GoOneToOneL
// This method requests a private communication to the remote party of one 
// call within a conference call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::GoOneToOneL(
    TInt aCallId )
    {
    TInt ret( KErrNotSupported );

    CMmConferenceCallTsy* mmConference = 
        iMmCallTsy->Phone()->ConferenceCall();

    if ( mmConference )
        {
        //Check if GoOneToOne is supported
        TBool isSupported = mmConference->IsGoOneToOneSupported( aCallId );

        //if GoOneToOne is possible
        if ( isSupported )
            {
            //Create package
            CCallDataPackage package;
            //Set call id and call mode
            package.SetCallIdAndMode( aCallId, iSymbianCallMode );
            //Send request to the Domestic OS layer.
			ret = iMessageManager->HandleRequestL(
                EMobileCallGoOneToOne, &package );

            // Inform the Conference Call GSM extension about pending 
            // GoOneToOne operation
            if ( KErrNone == ret )
                {
                CMmConferenceCallExtInterface* confCallExt =
                    mmConference->GetActiveConferenceCallExtension();
                if ( confCallExt )
                    {
                    // This cast is safe: As we are here in GSM/WCDMA Call 
                    // extension, we know that the active Conference Call 
                    // extension is also GSM/WCDMA
                    static_cast<CMmConferenceCallGsmWcdmaExt*>( confCallExt )->
                        SetGoOneToOneHandlingStarted( ETrue, aCallId );
                    }
                }
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::DeflectL
// This method deflects incoming call to another destination. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::DeflectL(
    const TDataPackage& aData )
    {
    TInt ret( KErrNone );
    
    RMobileCall::TMobileCallDeflect* deflectType =
        reinterpret_cast<RMobileCall::TMobileCallDeflect*>( aData.Ptr1() );
    RMobilePhone::TMobileAddress* destination =
        reinterpret_cast<RMobilePhone::TMobileAddress*>( aData.Ptr2() );

    if ( ( ( RMobileCall::EDeflectUnspecified == *deflectType ) ||
        ( RMobileCall::EDeflectSuppliedNumber == *deflectType ) ) &&
        ( 0 < destination->iTelNumber.Length() ) )
        // Type EDeflectUnspecified is accepted if destination number is
        // supplied
        {
        //Create package
        CCallDataPackage package;
        //Set call id and call mode
        package.SetCallIdAndMode( iMmCallTsy->CallId(), iSymbianCallMode );
        //Send request to the Domestic OS layer.
		ret = iMessageManager->HandleRequestL( EMobileCallDeflect, &package );
        }
    else
        {
        // other deflect types (EDeflectRegisteredNumber and EDeflectVoicemail)
        // are not supported
        ret = KErrNotSupported;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::TransferL
// This method transfers call to another destination.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
#ifndef USING_CTSY_DISPATCHER
TInt CMmCallGsmWcdmaExt::TransferL()
    {
    TInt ret( KErrNone );

    // Retrieve possible held call
    CMmCallTsy* mmCall = iMmCallTsy->Phone()->CallList()->GetMmCallByStatus( 
        RMobileCall::EStatusHold );

    if ( mmCall )
        {
        // Held call Id
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_TRANSFERL_1, "TSY: CMmCallGsmWcdmaExt::Transfer, CallId: %d", mmCall->CallId() );

        //Create package
        CCallDataPackage package;
        //Set call id and call mode
        package.SetCallIdAndMode( mmCall->CallId(), iSymbianCallMode );
        //Send request to the Domestic OS layer.
		ret = iMessageManager->HandleRequestL( 
            EMobileCallTransfer, &package );
        }
    else 
        {
        ret = KErrNotReady;
        }

    return ret;
    }
#endif //USING_CTSY_DISPATCHER

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::FillMobileCallInfo
// Fills the GSM specific Mobile Call Information in the 
// RMobileCall::TMobileCallInfoV1 given as parameter.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::FillMobileCallInfo(
		RMobileCall::TMobileCallInfoV1* aInfoV1)
    {
    aInfoV1->iAlternatingCall = iMobileCallInfo.iAlternatingCall;
    aInfoV1->iRemoteParty =     iMobileCallInfo.iRemoteParty;
    aInfoV1->iForwarded =       iMobileCallInfo.iForwarded;
    aInfoV1->iDialledParty =    iMobileCallInfo.iDialledParty;
    aInfoV1->iService =         iMobileCallInfo.iService;
    aInfoV1->iValid |=          iMobileCallInfo.iValid;
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::FillMobileCallInfoV3
// Fills the GSM specific Mobile Call Information in the 
// RMobileCall::TMobileCallInfoV3 given as parameter.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::FillMobileCallInfoV3(
    RMobileCall::TMobileCallInfoV3* aInfoV3 )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_FILLMOBILECALLINFOV3_1, "TSY: CMmCallGsmWcdmaExt::FillMobileCallInfoV3 - extensionid=%d", aInfoV3->ExtensionId() );
    
    // V3 info parameters
    aInfoV3->iSecurity = iMobileCallInfo.iSecurity;
    
    return KErrNone;
    }    
// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::FillMobileCallInfoV7
// Fills the GSM specific Mobile Call Information in the 
// RMobileCall::TMobileCallInfoV7 given as parameter.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::FillMobileCallInfoV7(
    RMobileCall::TMobileCallInfoV7* aInfoV7 )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_FILLMOBILECALLINFOV7_1, "TSY: CMmCallGsmWcdmaExt::FillMobileCallInfoV7 - extensionid=%d", aInfoV7->ExtensionId() );
        
    // V7 info parameters
    // TCallParamOrigin iCallParamOrigin;    
    aInfoV7->iCallParamOrigin =           iMobileCallInfo.iCallParamOrigin;
    // TIconId iIconId;
    aInfoV7->iIconId.iIdentifier =        iMobileCallInfo.iIconId.iIdentifier;
    aInfoV7->iIconId.iQualifier =         iMobileCallInfo.iIconId.iQualifier;
    // TAlphaIdBuf iAlphaId;
    aInfoV7->iAlphaId =                   iMobileCallInfo.iAlphaId;
    // TBool iParamsCallControlModified;
    aInfoV7->iParamsCallControlModified = iMobileCallInfo.iParamsCallControlModified;

    return KErrNone;
    }     

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::FillMobileCallInfoV8
// Fills the GSM specific Mobile Call Information in the 
// RMobileCall::TMobileCallInfoV8 given as parameter.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::FillMobileCallInfoV8(
    RMobileCall::TMobileCallInfoV8* aInfoV8 )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_FILLMOBILECALLINFOV8_1, "TSY: CMmCallGsmWcdmaExt::FillMobileCallInfoV8 - extensionid=%d", aInfoV8->ExtensionId() );
   
    // V8 info parameters
    aInfoV8->iSubAddress = iMobileCallInfo.iSubAddress;
    aInfoV8->iBearerCap1 = iMobileCallInfo.iBearerCap1;
    aInfoV8->iBearerCap2 = iMobileCallInfo.iBearerCap2;
    aInfoV8->iBCRepeatIndicator = iMobileCallInfo.iBCRepeatIndicator;
    
    return KErrNone;
    }     



// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::GetRemotePartyInfo
// Gets remote party information
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::GetRemotePartyInfo(
    RMobileCall::TMobileCallRemotePartyInfoV1Pckg* aInfoV1Pckg )
    {
    RMobileCall::TMobileCallRemotePartyInfoV1Pckg* infoPckgV1 = 
        reinterpret_cast<RMobileCall::TMobileCallRemotePartyInfoV1Pckg*>( aInfoV1Pckg );
    RMobileCall::TMobileCallRemotePartyInfoV1& infoV1 = ( *infoPckgV1 )();

    infoV1 = iMobileCallInfo.iRemoteParty;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::SetRfState
// This method sets RF state
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::SetRfState(
    TRfStateInfo aMtcState, 
    MCtsySsmPluginCallback* aSsmPluginCallback,
    TCtsySsmCallbackData& aCallbackData  )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_SETRFSTATE_1, "TSY: CMmCallGsmWcdmaExt::SetRfState - state=%d", aMtcState );

    TInt err ( KErrNotSupported );  
    if ( (iUsingFeatureManager) && (iFeatureControl.FeatureSupported(NFeature::KEmergencyCallsEnabledInOfflineMode ) == KFeatureSupported) )
        {
        if (!iSystemStatePluginHandler)
        	{
        	TRAP(err, iSystemStatePluginHandler = CCtsySystemStatePluginHandler::NewL());
        	if(KErrNone != err)
        		{
        		// pass this error to callback
        		SsmPluginCallback(err, aCallbackData);
        		return;
        		}
        	}
        	        	
        if ( ERfsStateInfoNormal == aMtcState )
       		{
       		iSystemStatePluginHandler->ActivateRfForEmergencyCall(aSsmPluginCallback, aCallbackData);
       		}
       	else if ( ERfsStateInfoInactive == aMtcState )
       		{
       		iSystemStatePluginHandler->DeactivateRfAfterEmergencyCall();
       		}
       	else
       		{
       		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_SETRFSTATE_2, "TSY: CMmCallGsmWcdmaExt::SetRfState - bad state");
       		// pass this error to callback
       		SsmPluginCallback (KErrArgument, aCallbackData);
       		}        		
        }  
     else
     	{
     	SsmPluginCallback (KErrNotSupported, aCallbackData);
     	} 
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::SetMobileCallData
// This method sets GSM specific mobile call information
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::SetMobileCallData(
    CMmDataPackage* aDataPackage,
    RMobileCall::TMobileCallDirection aCallDirection )
    {
    RMobileCall::TMobileCallInfoV1* mobileCallInfo;
    aDataPackage->UnPackData( &mobileCallInfo );

    // get extensionid from the recieved data
    TInt extensionId( mobileCallInfo->ExtensionId() );    
    
    //Set call direction info
    iMobileCallInfo.iRemoteParty.iDirection = aCallDirection;

    //call mode should be always available
    iMobileCallInfo.iService = mobileCallInfo->iService;

    //is this an emergency call
    if ( RMobileCall::KCallEmergency & mobileCallInfo->iValid )
        {
        iMobileCallInfo.iEmergency = mobileCallInfo->iEmergency;
        }

    if ( 0 < mobileCallInfo->iRemoteParty.iRemoteNumber.iTelNumber.Length() &&
        ( RMobilePhone::KMaxMobileTelNumberSize >=
            mobileCallInfo->iRemoteParty.iRemoteNumber.iTelNumber.Length() ) )
        {
        iMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Zero();
        iMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Copy(
            mobileCallInfo->iRemoteParty.iRemoteNumber.iTelNumber );

        iMobileCallInfo.iRemoteParty.iRemoteNumber.iNumberPlan =
            mobileCallInfo->iRemoteParty.iRemoteNumber.iNumberPlan;
        iMobileCallInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber =
            mobileCallInfo->iRemoteParty.iRemoteNumber.iTypeOfNumber;

        iMobileCallInfo.iRemoteParty.iRemoteIdStatus =
            mobileCallInfo->iRemoteParty.iRemoteIdStatus;
        }

    if ( RMobileCall::KCallRemoteParty & mobileCallInfo->iValid )
        {
        iMobileCallInfo.iRemoteParty.iRemoteIdStatus =
            mobileCallInfo->iRemoteParty.iRemoteIdStatus;
        }

    if ( 0 < mobileCallInfo->iRemoteParty.iCallingName.Length() &&
        ( RMobileCall::KCallingNameSize >=
            mobileCallInfo->iRemoteParty.iCallingName.Length() ) )
        {
        iMobileCallInfo.iRemoteParty.iCallingName.Zero();
        iMobileCallInfo.iRemoteParty.iCallingName.Copy(
            mobileCallInfo->iRemoteParty.iCallingName );

        iMobileCallInfo.iRemoteParty.iRemoteIdStatus =
            mobileCallInfo->iRemoteParty.iRemoteIdStatus;
        }

    if ( 0 < mobileCallInfo->iDialledParty.iTelNumber.Length() &&
        ( RMobilePhone::KMaxMobileTelNumberSize >=
            mobileCallInfo->iDialledParty.iTelNumber.Length() ) )
        {
        iMobileCallInfo.iDialledParty.iTelNumber.Zero();
        iMobileCallInfo.iDialledParty.iTelNumber.Copy(
            mobileCallInfo->iDialledParty.iTelNumber );

        iMobileCallInfo.iDialledParty.iNumberPlan =
            mobileCallInfo->iDialledParty.iNumberPlan;
        iMobileCallInfo.iDialledParty.iTypeOfNumber =
            mobileCallInfo->iDialledParty.iTypeOfNumber;
        }

    if ( RMobileCall::KCallForwarded & mobileCallInfo->iValid )
        {
        iMobileCallInfo.iForwarded = mobileCallInfo->iForwarded;
        }

    if ( RMobileCall::KCallAlternating & mobileCallInfo->iValid )
        {
        iMobileCallInfo.iAlternatingCall = mobileCallInfo->iAlternatingCall;
        }
    
    // If a Mobile Originated call, take Alpha ID and Icon ID values from iCallParams, if valid.
    if ( iMobileCallInfo.iRemoteParty.iDirection == RMobileCall::EMobileOriginated)
        {
        // Set the Alpha ID, if not set.
        if ( iMobileCallInfo.iAlphaId.Length() <= 0 && 
                iCallParams.iAlphaId.Length() > 0 &&
                iCallParams.iAlphaId.Length() <= RMobileCall::KAlphaIdMaxSize )
            {
            iMobileCallInfo.iAlphaId.Zero();
            iMobileCallInfo.iAlphaId.Copy(iCallParams.iAlphaId );
            iMobileCallInfo.iValid |= RMobileCall::KCallAlphaId;
            }
        // Set the Icon ID, if not set.
        if ( iMobileCallInfo.iIconId.iIdentifier == 0 && 
                iMobileCallInfo.iIconId.iQualifier == RMobileCall::EIconQualifierNotSet &&
                iCallParams.iIconId.iIdentifier > 0 &&
                iCallParams.iIconId.iQualifier != RMobileCall::EIconQualifierNotSet )
            {
            iMobileCallInfo.iIconId.iIdentifier = iCallParams.iIconId.iIdentifier;
            iMobileCallInfo.iIconId.iQualifier = iCallParams.iIconId.iQualifier;
            iMobileCallInfo.iValid |= RMobileCall::KCallIconId;
            }
        }
 
    // TMobileCallInfoV3
    if ( (KETelExtMultimodeV3 == extensionId) ||
    	 (KEtelExtMultimodeV7 == extensionId) ||
    	 (KEtelExtMultimodeV8 == extensionId))
        {    

        RMobileCall::TMobileCallInfoV3* mobileCallInfoV3 = static_cast<RMobileCall::TMobileCallInfoV3*>(mobileCallInfo);
        
        if(RMobileCall::KCallSecurity & mobileCallInfo->iValid)
        	{
        	// V3 parameters
	        iMobileCallInfo.iSecurity = mobileCallInfoV3->iSecurity;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_SETMOBILECALLDATA_1, "TSY: CMmCallGsmWcdmaExt::SetMobileCallData V3 params copied");
        	}
        }
        
    // TMobileCallInfoV7
    if ( (KEtelExtMultimodeV7 == extensionId) || (KEtelExtMultimodeV8 == extensionId))
        {
       
        // TMobileCallInfoV7 params
         
        RMobileCall::TMobileCallInfoV7* mobileCallInfoV7 = static_cast<RMobileCall::TMobileCallInfoV7*>(mobileCallInfo);
        
        if(RMobileCall::KCallParamOrigin & mobileCallInfo->iValid) 
        	{
        	// TCallParamOrigin iCallParamOrigin;   
        	iMobileCallInfo.iCallParamOrigin = mobileCallInfoV7->iCallParamOrigin;
        	}
            
        // Set the Alpha ID if call is not Mobile Originated.
        // If MO, the Alpha ID would be set above from iCallParams
        if(RMobileCall::KCallIconId & mobileCallInfo->iValid &&
                iMobileCallInfo.iRemoteParty.iDirection != RMobileCall::EMobileOriginated) 
        	{
        	// TIconId iIconId;
	        iMobileCallInfo.iIconId.iIdentifier = mobileCallInfoV7->iIconId.iIdentifier;
	        iMobileCallInfo.iIconId.iQualifier = mobileCallInfoV7->iIconId.iQualifier;
        	}
        
        // Set the Icon ID if call is not Mobile Originated.
        // If MO, Icon ID would be set above from iCallParams
        if(RMobileCall::KCallAlphaId & mobileCallInfo->iValid &&
                iMobileCallInfo.iRemoteParty.iDirection != RMobileCall::EMobileOriginated) 
           	{
           	// TAlphaIdBuf iAlphaId;
   	        iMobileCallInfo.iAlphaId.Zero();
   	        iMobileCallInfo.iAlphaId.Copy( mobileCallInfoV7->iAlphaId );
           	}
        
        if(RMobileCall::KCallParamsCallControlModified & mobileCallInfo->iValid) 
        	{
        	// TBool iParamsCallControlModified;
	        iMobileCallInfo.iParamsCallControlModified = mobileCallInfoV7->iParamsCallControlModified;
        	}
       
        
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_SETMOBILECALLDATA_2, "TSY: CMmCallGsmWcdmaExt::SetMobileCallData V7 params copied");
        }
        
    // TMobileCallInfoV8
	if ( KEtelExtMultimodeV8 == extensionId )
	    {
	    // TMobileCallInfoV8 params
	    
	    RMobileCall::TMobileCallInfoV8* mobileCallInfoV8 = static_cast<RMobileCall::TMobileCallInfoV8*>(mobileCallInfo);
	     
	
	    if(RMobileCall::KCallSubAddress & mobileCallInfo->iValid) 
        	{
        	iMobileCallInfo.iSubAddress = mobileCallInfoV8->iSubAddress;
        	}	     
	    
	    if(RMobileCall::KCallBearerCap1 & mobileCallInfo->iValid) 
        	{
        	 iMobileCallInfo.iBearerCap1 = mobileCallInfoV8->iBearerCap1;
        	}	 
	    
	    if(RMobileCall::KCallBearerCap2 & mobileCallInfo->iValid) 
        	{
        	iMobileCallInfo.iBearerCap2 = mobileCallInfoV8->iBearerCap2;
        	}	
	    
	    if(RMobileCall::KCallBCRepeatIndicator & mobileCallInfo->iValid) 
        	{
        	iMobileCallInfo.iBCRepeatIndicator = mobileCallInfoV8->iBCRepeatIndicator;
        	}	 
	    
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_SETMOBILECALLDATA_3, "TSY: CMmCallGsmWcdmaExt::SetMobileCallData V8 params copied");
	       }
	
	//update validity flags
	iMobileCallInfo.iValid |= mobileCallInfo->iValid;
    
    }

// CSD related methods

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::GetMobileDataCallCapsV1
// Return GSM specific data call capabilities (V1).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::GetMobileDataCallCapsV1(
    RMobileCall::TMobileCallDataCapsV1& aCallDataCapsV1 )
    {
    aCallDataCapsV1 = *iDataCallCaps;  
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::ReceivedMobileDataCallCaps
// Received GSM specific data call capabilities.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::ReceivedMobileDataCallCaps(
    RMobileCall::TMobileCallDataCapsV1* aCaps )
    {
    *iDataCallCaps = *aCaps;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::GetBearerServiceInfo
// Return GSM specific bearer service info.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::GetBearerServiceInfo(
    RCall::TBearerService* aBearerService )
    {
    *aBearerService = iBearerService;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::ReceivedBearerServiceInfo
// Copy received bearer service info.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::ReceivedBearerServiceInfo(
    RCall::TBearerService aBearerService )
    {
    iBearerService = aBearerService;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::SetDynamicHscsdParamsL
// Sets GSM specific HSCSD parameters into extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::SetDynamicHscsdParamsL(
    RMobileCall::TMobileCallAiur* aAiur,
    TInt* aRxTimeslots )
    {
    TInt ret( KErrNone );

    // Check if the HSCSD call is ok
    if ( iHSCSDParams )
        {
        iHSCSDParams->iWantedAiur = *aAiur;
        iHSCSDParams->iWantedRxTimeSlots = *aRxTimeslots;

        //Create package
        CCallDataPackage package;
        //Set call id and call mode
        package.SetCallIdAndMode(
            iMmCallTsy->CallId(), iSymbianCallMode );
        //Pack call parameters and mobile call info
        package.PackData( iHSCSDParams );
        //Send request to the Domestic OS layer.
		ret = iMessageManager->HandleRequestL(
            EMobileCallSetDynamicHscsdParams, &package );

        iInformationReceived = EFalse;
        }
    else 
        {
        ret = KErrNotFound;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::GetCurrentHscsdInfo
// Return GSM specific current HSCSD info.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::GetCurrentHscsdInfo(
    TDes8* aHSCSDInfo )
    {
    TInt ret( KErrNone );

    if ( iInformationReceived )
        {
        // Check if current HSCSD info is valid!
        if ( EDataCallModeHscsd == iDataCallMode && 
            iCurrentHSCSDInfo != NULL )
            {
            // Check if aHSCSDInfo parameter descriptor size is valid
	        if ( sizeof(RMobilePhone::TMultimodeTypePckg) <= aHSCSDInfo->Length() )
			   {
		       // using base class to solve correct info version
               RMobilePhone::TMultimodeTypePckg* infoParamsPckg =
                   reinterpret_cast<RMobilePhone::TMultimodeTypePckg*>(
                   aHSCSDInfo );
               
			   RMobilePhone::TMultimodeType& infoParams = ( *infoParamsPckg )();

               // get extensionid from the recieved data
               TInt extensionId( infoParams.ExtensionId() );

               if (KETelExtMultimodeV1 == extensionId )
                   {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_GETCURRENTHSCSDINFO_1, "TSY: CMmCallGsmWcdmaExt::GetCurrentHscsdInfo - KETelMobileHscsdCallParamsV1");
                   RMobileCall::TMobileCallHscsdInfoV1Pckg* hscsdInfoPckg =
                       REINTERPRET_CAST( RMobileCall::TMobileCallHscsdInfoV1Pckg*,
                       aHSCSDInfo );
                   RMobileCall::TMobileCallHscsdInfoV1& hscsdInfo =
                       ( *hscsdInfoPckg )();
                    
                   // Fill V1 information 
                   hscsdInfo.iAiur = iCurrentHSCSDInfo->iAiur;
                   hscsdInfo.iCodings = iCurrentHSCSDInfo->iCodings;
                   hscsdInfo.iRxTimeSlots = iCurrentHSCSDInfo->iRxTimeSlots;
                   hscsdInfo.iTxTimeSlots = iCurrentHSCSDInfo->iTxTimeSlots;
                   }
            
               else if (KEtelExtMultimodeV7 == extensionId )
                   {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_GETCURRENTHSCSDINFO_2, "TSY: CMmCallGsmWcdmaExt::GetCurrentHscsdInfo - KETelMobileHscsdCallParamsV7");
                   RMobileCall::TMobileCallHscsdInfoV7Pckg* hscsdInfoPckg7 =
                       ( RMobileCall::TMobileCallHscsdInfoV7Pckg* )aHSCSDInfo;
                   RMobileCall::TMobileCallHscsdInfoV7& hscsdInfoV7 =
                       ( *hscsdInfoPckg7 )();
                
                   // Fill V1 information 
                   hscsdInfoV7.iAiur = iCurrentHSCSDInfo->iAiur;
                   hscsdInfoV7.iCodings = iCurrentHSCSDInfo->iCodings;
                   hscsdInfoV7.iRxTimeSlots = iCurrentHSCSDInfo->iRxTimeSlots;
                   hscsdInfoV7.iTxTimeSlots = iCurrentHSCSDInfo->iTxTimeSlots;

                   // Fill V7 information
                   hscsdInfoV7.iAlphaId = iCurrentHSCSDInfo->iAlphaId;
                   hscsdInfoV7.iCallParamOrigin = iCurrentHSCSDInfo->iCallParamOrigin;
                   hscsdInfoV7.iIconId.iIdentifier =
                       iCurrentHSCSDInfo->iIconId.iIdentifier;
                   hscsdInfoV7.iIconId.iQualifier =
                       iCurrentHSCSDInfo->iIconId.iQualifier;   
                   hscsdInfoV7.iParamsCallControlModified = 
                       iCurrentHSCSDInfo->iParamsCallControlModified;
                   }     
            
               else if (KEtelExtMultimodeV8 == extensionId )
                   {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_GETCURRENTHSCSDINFO_3, "TSY: CMmCallGsmWcdmaExt::GetCurrentHscsdInfo - KETelMobileHscsdCallParamsV8");
                   RMobileCall::TMobileCallHscsdInfoV8Pckg* hscsdInfoPckg8 =
                       ( RMobileCall::TMobileCallHscsdInfoV8Pckg* )aHSCSDInfo;
                   RMobileCall::TMobileCallHscsdInfoV8& hscsdInfoV8 =
                       ( *hscsdInfoPckg8 )();
                
                   // Fill V1 information
                   hscsdInfoV8.iAiur = iCurrentHSCSDInfo->iAiur;
                   hscsdInfoV8.iCodings = iCurrentHSCSDInfo->iCodings;
                   hscsdInfoV8.iRxTimeSlots = iCurrentHSCSDInfo->iRxTimeSlots;
                   hscsdInfoV8.iTxTimeSlots = iCurrentHSCSDInfo->iTxTimeSlots;

                   // Fill V7 information
                   hscsdInfoV8.iAlphaId = iCurrentHSCSDInfo->iAlphaId;
                   hscsdInfoV8.iCallParamOrigin = iCurrentHSCSDInfo->iCallParamOrigin;
                   hscsdInfoV8.iIconId.iIdentifier =
                       iCurrentHSCSDInfo->iIconId.iIdentifier;
                   hscsdInfoV8.iIconId.iQualifier =
                       iCurrentHSCSDInfo->iIconId.iQualifier;   
                   hscsdInfoV8.iParamsCallControlModified = 
                       iCurrentHSCSDInfo->iParamsCallControlModified;                
                
                   // Fill V8 information
                   hscsdInfoV8.iBCRepeatIndicator = iCurrentHSCSDInfo->iBCRepeatIndicator;
                   hscsdInfoV8.iBearerCap1 = iCurrentHSCSDInfo->iBearerCap1;
                   hscsdInfoV8.iBearerCap2 = iCurrentHSCSDInfo->iBearerCap2;
                   hscsdInfoV8.iSubAddress = iCurrentHSCSDInfo->iSubAddress; 
                   }
                else
			       {
			       ret = KErrArgument; 
			       }
			    }
			 else
                {
                ret = KErrArgument;
                }    
            }

        // Not found, return error!
        else
            {
            ret = KErrEtelCallNotActive;
            }
        }
    else
        {
        ret = KErrNotReady;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::DialDataCallL
// Dial method for data calls!
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::DialDataCallL(
    RMobilePhone::TMobileService aCallMode,
    const TDesC8* aCallParams,
    const TDesC* aTelNumber )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_DIALDATACALLL_1, "TSY: CMmCallGsmWcdmaExt::DialDataCallL");
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_DIALDATACALLL_2, "TSY: CMmCallGsmWcdmaExt::DialDataCallL aCallMode: %d", aCallMode);

	iMobileCallInfo.iDialledParty.iTelNumber.Copy( *aTelNumber );
	iMobileCallInfo.iDialledParty.iNumberPlan =
		RMobilePhone::EUnknownNumberingPlan;
	iMobileCallInfo.iDialledParty.iTypeOfNumber =
		RMobilePhone::EUnknownNumber;
	iMobileCallInfo.iService = aCallMode;
	iMobileCallInfo.iValid |= RMobileCall::KCallDialledParty;
	iMobileCallInfo.iValid |= RMobileCall::KCallAlternating;
	iMobileCallInfo.iAlternatingCall = RMobilePhone::EAlternatingModeUnspecified;
	TBool defaultParams(false);
	if(aCallParams->Length() != 0)
		{
		RCall::TCallParamsPckg* paramsPckg =
        	reinterpret_cast<RCall::TCallParamsPckg*>(
        			const_cast<TDesC8*>( aCallParams ) );
            
        	RCall::TCallParams& dataParams = (*paramsPckg)();
        	
        	if ( ( RMobileCall::KETelMobileDataCallParamsV1 ==
            	dataParams.ExtensionId() ) ||
            	( RMobileCall::KETelMobileDataCallParamsV2 ==
            	dataParams.ExtensionId() ) ||
            	( RMobileCall::KETelMobileDataCallParamsV8 ==
            	dataParams.ExtensionId() ) ||
            	( RMobileCall::KETelMobileHscsdCallParamsV1 ==
            	dataParams.ExtensionId() ) ||
            	( RMobileCall::KETelMobileHscsdCallParamsV2 ==
            	dataParams.ExtensionId() ) ||
            	( RMobileCall::KETelMobileHscsdCallParamsV7 ==
            	dataParams.ExtensionId() )||
            	( RMobileCall::KETelMobileHscsdCallParamsV8 ==
            	dataParams.ExtensionId()))
        		{
        		// Normal data call
        		iDataCallMode = EDataCallModeNormal;
        		RMobileCall::TMobileDataCallParamsV1Pckg* paramsPckgV1 =
        		reinterpret_cast<RMobileCall::TMobileDataCallParamsV1Pckg*>(
        				const_cast<TDesC8*>( aCallParams ) );

        		RMobileCall::TMobileDataCallParamsV1& dataParamsV1 = (*paramsPckgV1)();
        		//Parameter of base class TMobileCallParamsV1
        		iCSDParams->iIdRestrict = dataParamsV1.iIdRestrict;
        		iCSDParams->iCug = dataParamsV1.iCug;
        		iCSDParams->iAutoRedial = dataParamsV1.iAutoRedial;

        		CopyMobileDataCallParams( *iCSDParams, dataParamsV1 );
        		// Set iHscsdSupport initially to EFalse. 
        		// Normal CSD does not support HSCSD features.
        		iDataCallCaps->iHscsdSupport = EFalse;
        		}
        	else
        	    {
                defaultParams = true;
                //default unknown values
                iCSDParams->iIdRestrict = RMobileCall::EIdRestrictDefault;
                iCSDParams->iCug.iExplicitInvoke = EFalse;
                iCSDParams->iCug.iCugIndex = 0xFFFF;
                iCSDParams->iCug.iSuppressPrefCug = EFalse;
                iCSDParams->iCug.iSuppressOA = EFalse;
                iCSDParams->iAutoRedial = EFalse;
                
                // Normal data call
                iDataCallMode = EDataCallModeNormal;
                
                // Set iHscsdSupport initially to EFalse. 
                // Normal CSD does not support HSCSD features.
                iDataCallCaps->iHscsdSupport = EFalse;
        	    }
   
        	if ( RMobileCall::KETelMobileDataCallParamsV2 ==
             	dataParams.ExtensionId()||
             	RMobileCall::KETelMobileDataCallParamsV8 ==
                          	dataParams.ExtensionId())
        		{
       
        		RMobileCall::TMobileDataCallParamsV2Pckg* paramsPckgV2 =
        			reinterpret_cast<RMobileCall::TMobileDataCallParamsV2Pckg*>(
        				const_cast<TDesC8*>( aCallParams ) );

        		RMobileCall::TMobileDataCallParamsV2& dataParams2 = (*paramsPckgV2)();
        
        		RMobileCall::TMobileDataCallParamsV2* dataCallParamsV2 = static_cast<RMobileCall::TMobileDataCallParamsV2*>(iCSDParams);
                   
        
        		dataCallParamsV2->iBearerMode = dataParams2.iBearerMode;
        		}
   
        	if ( RMobileCall::KETelMobileDataCallParamsV8 == dataParams.ExtensionId())
        		{
           
        		RMobileCall::TMobileDataCallParamsV8Pckg* paramsPckgV8 =
        			reinterpret_cast<RMobileCall::TMobileDataCallParamsV8Pckg*>(
        					const_cast<TDesC8*>( aCallParams ) );

        		RMobileCall::TMobileDataCallParamsV8& dataParams8 = (*paramsPckgV8)();
        
        		RMobileCall::TMobileDataCallParamsV8* dataCallParamsV8 = static_cast<RMobileCall::TMobileDataCallParamsV8*>(iCSDParams);
            
        
        		dataCallParamsV8->iCallParamOrigin = dataParams8.iCallParamOrigin;
        		dataCallParamsV8->iSubAddress = dataParams8.iSubAddress;
        		dataCallParamsV8->iBearerCap1 = dataParams8.iBearerCap1;
        		dataCallParamsV8->iBearerCap2 = dataParams8.iBearerCap2;
        		dataCallParamsV8->iBCRepeatIndicator = dataParams8.iBCRepeatIndicator;
        		dataCallParamsV8->iIconId.iQualifier = dataParams8.iIconId.iQualifier;
        		dataCallParamsV8->iIconId.iIdentifier = dataParams8.iIconId.iIdentifier;
        		dataCallParamsV8->iAlphaId = dataParams8.iAlphaId;
        
        		if ( dataParams8.iAlphaId.Length() > 0 )
        			{
        			iMobileCallInfo.iAlphaId.Copy(dataParams8.iAlphaId);
        			iMobileCallInfo.iValid |= RMobileCall::KCallAlphaId;
        			}
        
        		if ( dataParams8.iIconId.iQualifier != RMobileCall::EIconQualifierNotSet )
        			{
        			iMobileCallInfo.iIconId.iIdentifier = dataParams8.iIconId.iIdentifier;
        			iMobileCallInfo.iIconId.iQualifier = dataParams8.iIconId.iQualifier;
        			iMobileCallInfo.iValid |= RMobileCall::KCallIconId;
        			}
        
        		if ( dataParams8.iCallParamOrigin != RMobileCall::EOriginatorUnknown)
        			{
        			iMobileCallInfo.iCallParamOrigin = dataParams8.iCallParamOrigin;
        			iMobileCallInfo.iValid |= RMobileCall::KCallParamOrigin;
        			}
                
        		}

        	if ( RMobileCall::KETelMobileHscsdCallParamsV1 ==  dataParams.ExtensionId() ||
        			RMobileCall::KETelMobileHscsdCallParamsV2 ==  dataParams.ExtensionId() ||
        			RMobileCall::KETelMobileHscsdCallParamsV7 ==  dataParams.ExtensionId() ||
        			RMobileCall::KETelMobileHscsdCallParamsV8 ==  dataParams.ExtensionId())
        		{
        		// HSCSD data call
        		iDataCallMode = EDataCallModeHscsd;
        		RMobileCall::TMobileHscsdCallParamsV1Pckg* paramsPckgV1 =
        			reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV1Pckg*>(
        					const_cast<TDesC8*>( aCallParams ) );
        		RMobileCall::TMobileHscsdCallParamsV1& hscsdParams =
        			(*paramsPckgV1)();
        		*(static_cast<RMobileCall::TMobileHscsdCallParamsV1*>(iHSCSDParams)) = hscsdParams;
        		// Set iHscsdSupport to ETrue. 
        		// This call is HSCSD call and features are supported.
        		iDataCallCaps->iHscsdSupport = ETrue;
        		}
   
        	if ( RMobileCall::KETelMobileHscsdCallParamsV2 ==  dataParams.ExtensionId() ||
        			RMobileCall::KETelMobileHscsdCallParamsV7 ==  dataParams.ExtensionId() ||
        			RMobileCall::KETelMobileHscsdCallParamsV8 ==  dataParams.ExtensionId())
        		{
       
        		RMobileCall::TMobileHscsdCallParamsV2Pckg* paramsPckgV2 =
        			reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV2Pckg*>(
        					const_cast<TDesC8*>( aCallParams ) );
        		RMobileCall::TMobileHscsdCallParamsV2& hscsdParams2 =
        			(*paramsPckgV2)();
        		RMobileCall::TMobileHscsdCallParamsV2* hscsdCallParamsV2 = static_cast<RMobileCall::TMobileHscsdCallParamsV2*>(iHSCSDParams);
        
        		hscsdCallParamsV2->iBearerMode = hscsdParams2.iBearerMode;
        
        		}
    
        	if ( RMobileCall::KETelMobileHscsdCallParamsV7 ==  dataParams.ExtensionId() ||
        			RMobileCall::KETelMobileHscsdCallParamsV8 ==  dataParams.ExtensionId())
        		{
       
        		RMobileCall::TMobileHscsdCallParamsV7Pckg* paramsPckgV7 =
        			reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV7Pckg*>(
        					const_cast<TDesC8*>( aCallParams ) );
        		RMobileCall::TMobileHscsdCallParamsV7& hscsdParams7 =
        			(*paramsPckgV7)();
        
        		RMobileCall::TMobileHscsdCallParamsV7* hscsdCallParamsV7 = static_cast<RMobileCall::TMobileHscsdCallParamsV7*>(iHSCSDParams);
             
        		hscsdCallParamsV7->iCallParamOrigin = hscsdParams7.iCallParamOrigin;
        		hscsdCallParamsV7->iIconId.iIdentifier = hscsdParams7.iIconId.iIdentifier;
        		hscsdCallParamsV7->iIconId.iQualifier = hscsdParams7.iIconId.iQualifier;
        		hscsdCallParamsV7->iAlphaId = hscsdParams7.iAlphaId;
        		
        		if ( hscsdParams7.iAlphaId.Length() > 0 )
        			{
        			iMobileCallInfo.iAlphaId.Copy(hscsdParams7.iAlphaId);
        			iMobileCallInfo.iValid |= RMobileCall::KCallAlphaId;
        			}
        
        		if ( hscsdParams7.iIconId.iQualifier != RMobileCall::EIconQualifierNotSet )
        			{
        			iMobileCallInfo.iIconId.iIdentifier = hscsdParams7.iIconId.iIdentifier;
        			iMobileCallInfo.iIconId.iQualifier = hscsdParams7.iIconId.iQualifier;
        			iMobileCallInfo.iValid |= RMobileCall::KCallIconId;
        			}
        
        		if ( hscsdParams7.iCallParamOrigin != RMobileCall::EOriginatorUnknown)
        			{
        			iMobileCallInfo.iCallParamOrigin = hscsdParams7.iCallParamOrigin;
        			iMobileCallInfo.iValid |= RMobileCall::KCallParamOrigin;
        			}
        		}
        	if ( RMobileCall::KETelMobileHscsdCallParamsV8 ==  dataParams.ExtensionId() )
        		{
       
        		RMobileCall::TMobileHscsdCallParamsV8Pckg* paramsPckgV8 =
        			reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV8Pckg*>(
        					const_cast<TDesC8*>( aCallParams ) );
        		RMobileCall::TMobileHscsdCallParamsV8& hscsdParams8 =
        			(*paramsPckgV8)();
       
        		RMobileCall::TMobileHscsdCallParamsV8* hscsdCallParamsV8 = static_cast<RMobileCall::TMobileHscsdCallParamsV8*>(iHSCSDParams);
                     
        		hscsdCallParamsV8->iSubAddress = hscsdParams8.iSubAddress;
        		hscsdCallParamsV8->iBearerCap1 = hscsdParams8.iBearerCap1;
        		hscsdCallParamsV8->iBearerCap2 = hscsdParams8.iBearerCap2;
        		hscsdCallParamsV8->iBCRepeatIndicator = hscsdParams8.iBCRepeatIndicator;
        
        
        		}
		}
    SetDataCaps( aCallParams );

    //Create package
    CCallDataPackage package;
    //Set call id and call mode
    package.SetCallIdAndMode( iMmCallTsy->CallId(), aCallMode );
    if(defaultParams)
        {
        TPckg<RMobileCall::TMobileDataCallParamsV1> pckgToSend(*iCSDParams); 
        package.PackData(  &pckgToSend , &iMobileCallInfo );

        }
    else
        {
    //Pack call parameters and mobile call info
        package.PackData( const_cast<TDesC8*>( aCallParams ), &iMobileCallInfo );
        }
    
    if( KMultimodeCallTypeIDNoFdnCheck == iMmCallTsy->GetDialTypeId() )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_DIALDATACALLL_3, "TSY: CMmCallGsmWcdmaExt::DialDataCallL: EMobileCallDialNoFdnCheck");
        //Send request to the Domestic OS layer.
        return iMessageManager->HandleRequestL( EMobileCallDialNoFdnCheck, &package );
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_DIALDATACALLL_4, "TSY: CMmCallGsmWcdmaExt::DialDataCallL: EEtelCallDial");
        //Send request to the Domestic OS layer.
        return iMessageManager->HandleRequestL( EEtelCallDial, &package );
        }        

    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::AnswerIncomingDataCallL
// Answer incoming data call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::AnswerIncomingDataCallL(
    TInt aCallId )
    {
    // Set data call caps for incoming call. The caps are unknown at this 
    // point and they are set when the call is connected!
    iDataCallCaps->iServiceCaps = 0x00;
//    iDataCallCaps->iProtocolCaps = 0x00;
    iDataCallCaps->iQoSCaps = 0x00;
    iDataCallCaps->iSpeedCaps = 0x00;
    iDataCallCaps->iHscsdSupport = EFalse;

    // Initially always normal, will be set later to HSCSD if call is HSCSD
    iDataCallMode = EDataCallModeNormal;

    //Create package
    CCallDataPackage package;
    //Set call id and call mode
    package.SetCallIdAndMode( aCallId, iSymbianCallMode );

    //Send request to the Domestic OS layer.
    return iMessageManager->HandleRequestL( EEtelCallAnswer, &package );
    }
// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::Connect
// Sets correct data call attributes, depending on parameter extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::Connect(
    const TDesC8* aCallParams )
    {
    RMobileCall::TMobileDataCallParamsV1Pckg* paramsPckgV1 =
        reinterpret_cast<RMobileCall::TMobileDataCallParamsV1Pckg*>(
            const_cast<TDesC8*>( aCallParams ) );
    RMobileCall::TMobileDataCallParamsV1& dataParams = ( *paramsPckgV1 )();

    if ( ( RMobileCall::KETelMobileDataCallParamsV1 ==
            dataParams.ExtensionId() ) ||
        ( RMobileCall::KETelMobileHscsdCallParamsV1 ==
            dataParams.ExtensionId() )||
        ( RMobileCall::KETelMobileDataCallParamsV2 ==
            dataParams.ExtensionId() ) ||
        ( RMobileCall::KETelMobileHscsdCallParamsV2 ==
            dataParams.ExtensionId() ) ||
        ( RMobileCall::KETelMobileHscsdCallParamsV7 ==
            dataParams.ExtensionId() ) ||
        ( RMobileCall::KETelMobileHscsdCallParamsV8 ==
            dataParams.ExtensionId() ) ||
        ( RMobileCall::KETelMobileDataCallParamsV8 ==
            dataParams.ExtensionId() ))
            
        {
        // Normal data call
        iDataCallMode = EDataCallModeNormal;
        CopyMobileDataCallParams( *iCSDParams, dataParams );  
        // Set iHscsdSupport initially to EFalse. 
        // Normal CSD does not support HSCSD features.
        iDataCallCaps->iHscsdSupport = EFalse;
        }
   
    if ((RMobileCall::KETelMobileDataCallParamsV2 == dataParams.ExtensionId())||
    	(RMobileCall::KETelMobileDataCallParamsV8 == dataParams.ExtensionId()))
       {
       RMobileCall::TMobileDataCallParamsV2Pckg* dataParamsPckgV2 = (RMobileCall::TMobileDataCallParamsV2Pckg*)aCallParams;
       RMobileCall::TMobileDataCallParamsV2& dataParamsV2 = (*dataParamsPckgV2)();
       
       RMobileCall::TMobileDataCallParamsV2* dataCallParamsV2 = static_cast<RMobileCall::TMobileDataCallParamsV2*>(iCSDParams);
       
       dataCallParamsV2->iBearerMode = dataParamsV2.iBearerMode;  
       }

    if (RMobileCall::KETelMobileDataCallParamsV8 == dataParams.ExtensionId())
       {
       RMobileCall::TMobileDataCallParamsV8Pckg* dataParamsPckgV8 = (RMobileCall::TMobileDataCallParamsV8Pckg*)aCallParams;
       RMobileCall::TMobileDataCallParamsV8& dataParamsV8 = (*dataParamsPckgV8)();
       
       RMobileCall::TMobileDataCallParamsV8* dataCallParamsV8 = static_cast<RMobileCall::TMobileDataCallParamsV8*>(iCSDParams);
       
       dataCallParamsV8->iCallParamOrigin = dataParamsV8.iCallParamOrigin;
       dataCallParamsV8->iSubAddress = dataParamsV8.iSubAddress;
       dataCallParamsV8->iBearerCap1 = dataParamsV8.iBearerCap1; 
       dataCallParamsV8->iBearerCap2 = dataParamsV8.iBearerCap2;
       dataCallParamsV8->iBCRepeatIndicator = dataParamsV8.iBCRepeatIndicator;
       dataCallParamsV8->iIconId.iQualifier = dataParamsV8.iIconId.iQualifier;
       dataCallParamsV8->iIconId.iIdentifier = dataParamsV8.iIconId.iIdentifier;
       dataCallParamsV8->iAlphaId = dataParamsV8.iAlphaId;
       
       
       }
    
    if ( RMobileCall::KETelMobileHscsdCallParamsV1 ==
            dataParams.ExtensionId() )
       {
       // HSCSD data call
       iDataCallMode = EDataCallModeHscsd;
       RMobileCall::TMobileHscsdCallParamsV1Pckg* hscsdParamsPckgV1 =
           reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV1Pckg*>(
               const_cast<TDesC8*>( aCallParams ) );
       RMobileCall::TMobileHscsdCallParamsV1& hscsDataParams =
           ( *hscsdParamsPckgV1 )();
       *(static_cast<RMobileCall::TMobileHscsdCallParamsV1*>(iHSCSDParams)) = hscsDataParams;
         // Set iHscsdSupport to ETrue. 
       // This call is HSCSD call and features are supported.
       iDataCallCaps->iHscsdSupport = ETrue;
       }
   
    if ( RMobileCall::KETelMobileHscsdCallParamsV2 ==
            dataParams.ExtensionId() )
       {
       // HSCSD data call
       iDataCallMode = EDataCallModeHscsd;
       RMobileCall::TMobileHscsdCallParamsV2Pckg* hscsdParamsPckgV2 =
           reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV2Pckg*>(
               const_cast<TDesC8*>( aCallParams ) );
       RMobileCall::TMobileHscsdCallParamsV2& hscsDataParams2 =
           ( *hscsdParamsPckgV2 )();
       *(static_cast<RMobileCall::TMobileHscsdCallParamsV2*>(iHSCSDParams)) = hscsDataParams2;
         // Set iHscsdSupport to ETrue. 
       // This call is HSCSD call and features are supported.
       iDataCallCaps->iHscsdSupport = ETrue;
       }
    
    if ( RMobileCall::KETelMobileHscsdCallParamsV7 ==
            dataParams.ExtensionId() )
       {
       // HSCSD data call
       iDataCallMode = EDataCallModeHscsd;
       RMobileCall::TMobileHscsdCallParamsV7Pckg* hscsdParamsPckgV7 =
           reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV7Pckg*>(
               const_cast<TDesC8*>( aCallParams ) );
       RMobileCall::TMobileHscsdCallParamsV7& hscsDataParams7 =
           ( *hscsdParamsPckgV7)();
       *(static_cast<RMobileCall::TMobileHscsdCallParamsV7*>(iHSCSDParams)) = hscsDataParams7;
         // Set iHscsdSupport to ETrue. 
       // This call is HSCSD call and features are supported.
       iDataCallCaps->iHscsdSupport = ETrue;
       }
    
    if ( RMobileCall::KETelMobileHscsdCallParamsV8 ==
            dataParams.ExtensionId() )
       {
       // HSCSD data call
       iDataCallMode = EDataCallModeHscsd;
       RMobileCall::TMobileHscsdCallParamsV8Pckg* hscsdParamsPckgV8 =
           reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV8Pckg*>(
               const_cast<TDesC8*>( aCallParams ) );
       RMobileCall::TMobileHscsdCallParamsV8& hscsDataParams8 =
           ( *hscsdParamsPckgV8 )();
       *iHSCSDParams = hscsDataParams8;
         // Set iHscsdSupport to ETrue. 
       // This call is HSCSD call and features are supported.
       iDataCallCaps->iHscsdSupport = ETrue;
       }
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::ReceivedCallParams
// Stores new CALL/CSD parameters.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::ReceivedCallParams(
    const CMmDataPackage* aDataPackage )
    {
    TDesC8* callParamsDes = NULL;

    aDataPackage->UnPackData( &callParamsDes );

    RCall::TCallParamsPckg* callParamsPckg =
        reinterpret_cast<RCall::TCallParamsPckg*>( callParamsDes );
    RCall::TCallParams& callParams = ( *callParamsPckg )();

    TInt extensionId( callParams.ExtensionId() );

    if ( iCSDParams &&
       (( RMobileCall::KETelMobileDataCallParamsV1 == extensionId) ||
       ( RMobileCall::KETelMobileHscsdCallParamsV1 == extensionId )))
       {
       RMobileCall::TMobileDataCallParamsV1Pckg* paramsPckgV1 =
           reinterpret_cast<RMobileCall::TMobileDataCallParamsV1Pckg*>(
               callParamsDes );
       RMobileCall::TMobileDataCallParamsV1& dataParams = ( *paramsPckgV1 )();

       // Save all the CSD parameters here
       *(static_cast<RMobileCall::TMobileDataCallParamsV1*>(iCSDParams)) = dataParams;
       // Update data call caps
       SetDataCaps( callParamsDes );
       reinterpret_cast<CMmDataCallTsy*>( iMmCallTsy )->
           CompleteNotifyMobileDataCallCapsChange();
       }
   
    if ( iCSDParams && ( RMobileCall::KETelMobileDataCallParamsV2 == extensionId))
       {
       RMobileCall::TMobileDataCallParamsV2Pckg* paramsPckgV2 =
           reinterpret_cast<RMobileCall::TMobileDataCallParamsV2Pckg*>(
               callParamsDes );
       RMobileCall::TMobileDataCallParamsV2& dataParams2 = ( *paramsPckgV2 )();

       // Save all the CSD parameters here
       *(static_cast<RMobileCall::TMobileDataCallParamsV2*>(iCSDParams)) = dataParams2;
       // Update data call caps
       SetDataCaps( callParamsDes );
       reinterpret_cast<CMmDataCallTsy*>( iMmCallTsy )->
           CompleteNotifyMobileDataCallCapsChange();
       }
   
    if ( iCSDParams &&( RMobileCall::KETelMobileDataCallParamsV8 == extensionId))
       {
       RMobileCall::TMobileDataCallParamsV8Pckg* paramsPckgV8 =
            reinterpret_cast<RMobileCall::TMobileDataCallParamsV8Pckg*>(
                 callParamsDes );
       RMobileCall::TMobileDataCallParamsV8& dataParams8 = ( *paramsPckgV8)();

       // Save all the CSD parameters here
       *iCSDParams = dataParams8;
       // Update data call caps
       SetDataCaps( callParamsDes );
       reinterpret_cast<CMmDataCallTsy*>( iMmCallTsy )->
           CompleteNotifyMobileDataCallCapsChange();
       }
    
    if ( iHSCSDParams && (RMobileCall::KETelMobileHscsdCallParamsV1 == extensionId ) )
       {
       RMobileCall::TMobileHscsdCallParamsV1Pckg* hscsdParamsPckgV1 =
            reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV1Pckg*>(
                callParamsDes );
       RMobileCall::TMobileHscsdCallParamsV1& hscsdDataParams = ( *hscsdParamsPckgV1 )();

       // Save all the CSD parameters here
       *(static_cast<RMobileCall::TMobileHscsdCallParamsV1*>(iHSCSDParams)) = hscsdDataParams;
       // Update data call caps
       SetDataCaps( callParamsDes );
       reinterpret_cast<CMmDataCallTsy*>( iMmCallTsy )->
       	   CompleteNotifyMobileDataCallCapsChange();
       }
    
    if ( iHSCSDParams && (RMobileCall::KETelMobileHscsdCallParamsV2 == extensionId ) )
       {
       RMobileCall::TMobileHscsdCallParamsV2Pckg* hscsdParamsPckgV2 =
           reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV2Pckg*>(
               callParamsDes );
       RMobileCall::TMobileHscsdCallParamsV2& hscsdDataParams2 = ( *hscsdParamsPckgV2 )();

       // Save all the CSD parameters here
       *(static_cast<RMobileCall::TMobileHscsdCallParamsV2*>(iHSCSDParams)) = hscsdDataParams2;
       // Update data call caps
       SetDataCaps( callParamsDes );
       reinterpret_cast<CMmDataCallTsy*>( iMmCallTsy )->
       	   CompleteNotifyMobileDataCallCapsChange();
       }
   
    if ( iHSCSDParams && (RMobileCall::KETelMobileHscsdCallParamsV7 == extensionId ) )
       {
       RMobileCall::TMobileHscsdCallParamsV7Pckg* hscsdParamsPckgV7 =
           reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV7Pckg*>(
               callParamsDes );
       RMobileCall::TMobileHscsdCallParamsV7& hscsdDataParams7 = ( *hscsdParamsPckgV7 )();

       // Save all the CSD parameters here
       *(static_cast<RMobileCall::TMobileHscsdCallParamsV7*>(iHSCSDParams)) = hscsdDataParams7;
       // Update data call caps
       SetDataCaps( callParamsDes );
       reinterpret_cast<CMmDataCallTsy*>( iMmCallTsy )->
       	   CompleteNotifyMobileDataCallCapsChange();
       }
    
    if ( iHSCSDParams && (RMobileCall::KETelMobileHscsdCallParamsV8 == extensionId ) )
       {
       RMobileCall::TMobileHscsdCallParamsV8Pckg* hscsdParamsPckgV8 =
           reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV8Pckg*>(
               callParamsDes );
       RMobileCall::TMobileHscsdCallParamsV8& hscsdDataParams8 = ( *hscsdParamsPckgV8 )();

       // Save all the CSD parameters here
       *iHSCSDParams = hscsdDataParams8;
       // Update data call caps
       SetDataCaps( callParamsDes );
       reinterpret_cast<CMmDataCallTsy*>( iMmCallTsy )->
       	   CompleteNotifyMobileDataCallCapsChange();
       }

    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::SetDataCaps
// Sets data call service, protocol and QoS caps.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::SetDataCaps(
    const TDesC8* aCallParams )
    {
	iDataCallCaps->iSpeedCaps = 0x00;
    iDataCallCaps->iProtocolCaps = 0x00;
    iDataCallCaps->iQoSCaps = 0x00;
    iDataCallCaps->iServiceCaps = 0x00;
    if(aCallParams->Length() < sizeof(RMobileCall::TMobileDataCallParamsV1) )
    	{
    	return;
    	}
    RMobileCall::TMobileDataCallParamsV1Pckg* paramsPckgV1 =
        reinterpret_cast<RMobileCall::TMobileDataCallParamsV1Pckg*>(
            const_cast<TDesC8*>( aCallParams ) );
    RMobileCall::TMobileDataCallParamsV1& dataParams = ( *paramsPckgV1 )();

    // NOTE: Service and QoS values are set to only supported value in 
    // ResetInternalAttributes. If there will be more supported caps in for 
    // example continuation projects, then do Service and QoS here the same
    // way as Speed and Protocol are done.

    // Change the speed caps
    switch ( dataParams.iSpeed )
        {
        case RMobileCall::ESpeedAutobauding:
            iDataCallCaps->iSpeedCaps =
                RMobileCall::KCapsSpeedAutobauding;
            break;
        case RMobileCall::ESpeed9600:
            iDataCallCaps->iSpeedCaps =
                RMobileCall::KCapsSpeed9600;
            break;
        case RMobileCall::ESpeed14400:
            iDataCallCaps->iSpeedCaps =
                RMobileCall::KCapsSpeed14400;
            break;
        case RMobileCall::ESpeed19200:
            iDataCallCaps->iSpeedCaps =
                RMobileCall::KCapsSpeed19200;
            break;
        case RMobileCall::ESpeed28800:
            iDataCallCaps->iSpeedCaps =
                RMobileCall::KCapsSpeed28800;
            break;
        case RMobileCall::ESpeed38400:
            iDataCallCaps->iSpeedCaps =
                RMobileCall::KCapsSpeed38400;
            break;
        case RMobileCall::ESpeed43200:
            iDataCallCaps->iSpeedCaps =
                RMobileCall::KCapsSpeed43200;
            break;
        // For Multimedia call
        case RMobileCall::ESpeed64000:      
        	if ( (iUsingFeatureManager) && (iFeatureControl.FeatureSupported(NFeature::KCsVideoTelephony ) == KFeatureSupported) )
                {
                iDataCallCaps->iSpeedCaps =
                    RMobileCall::KCapsSpeed64000;
                }
            break;
        case RMobileCall::ESpeed2400:
        case RMobileCall::ESpeed4800:
        case RMobileCall::ESpeed32000:
        case RMobileCall::ESpeed33600:
        case RMobileCall::ESpeed48000:
        case RMobileCall::ESpeed56000:
        case RMobileCall::ESpeed57600:
        case RMobileCall::ESpeedUnspecified:
        default:
            // already set iDataCallCaps->iSpeedCaps = 0x00;
            break;
        }

    // Change the protocol caps
    switch ( dataParams.iProtocol )
        {
        case RMobileCall::EProtocolV32:
            iDataCallCaps->iProtocolCaps =
                RMobileCall::KCapsProtocolV32;
            break;
        case RMobileCall::EProtocolV34:
            iDataCallCaps->iProtocolCaps =
                RMobileCall::KCapsProtocolV34;
            break;
        case RMobileCall::EProtocolV110:
            iDataCallCaps->iProtocolCaps =
                RMobileCall::KCapsProtocolV110;
            break;
        case RMobileCall::EProtocolV120:
            iDataCallCaps->iProtocolCaps =
                RMobileCall::KCapsProtocolV120;
            break;
        // For Multimedia call
		case RMobileCall::EProtocolPstnMultimedia:	
			if ( (iUsingFeatureManager) && (iFeatureControl.FeatureSupported(NFeature::KCsVideoTelephony ) == KFeatureSupported) )
                {
                iDataCallCaps->iProtocolCaps =
                    RMobileCall::KCapsPstnMultimedia;
                }	
            break;
		case RMobileCall::EProtocolIsdnMultimedia:	
			if ( (iUsingFeatureManager) && (iFeatureControl.FeatureSupported(NFeature::KCsVideoTelephony ) == KFeatureSupported) )
                {
                iDataCallCaps->iProtocolCaps =
                    RMobileCall::KCapsIsdnMultimedia;
                }	
            break;
        case RMobileCall::EProtocolUnspecified:
        case RMobileCall::EProtocolV22bis:
        case RMobileCall::EProtocolX31FlagStuffing:
        case RMobileCall::EProtocolPIAFS:
        case RMobileCall::EProtocolBitTransparent:
        case RMobileCall::EProtocolPstnMultimediaVoiceFallback:
        default:
            // aldready set iDataCallCaps->iProtocolCaps = 0x00;
            break;
        }

    // Change the QoS caps
    switch ( dataParams.iQoS )
        {
        case RMobileCall::EQoSTransparent:
            iDataCallCaps->iQoSCaps = 
                RMobileCall::KCapsTransparent;
            break;
        case RMobileCall::EQoSNonTransparent:
            iDataCallCaps->iQoSCaps = 
                RMobileCall::KCapsNonTransparent;
            break;
        case RMobileCall::EQosTransparentPreferred:
            iDataCallCaps->iQoSCaps = 
                RMobileCall::KCapsTransparentPreferred;
            break;
        case RMobileCall::EQosNonTransparentPreferred:
            iDataCallCaps->iQoSCaps = 
                RMobileCall::KCapsNonTransparentPreferred;
            break;
        case RMobileCall::EQoSUnspecified:
        default:
            // aldready set iDataCallCaps->iQoSCaps = 0x00;
            break;
        }

    // Change the service caps
    switch ( dataParams.iService )
        {
        case RMobileCall::EServiceDataCircuitAsync:
            iDataCallCaps->iServiceCaps = 
                RMobileCall::KCapsDataCircuitAsynchronous;
            break;
        case RMobileCall::EServiceDataCircuitAsyncRdi:
            iDataCallCaps->iServiceCaps = 
                RMobileCall::KCapsDataCircuitAsynchronousRDI;
            break;
        case RMobileCall::EServiceDataCircuitSync:
            iDataCallCaps->iServiceCaps = 
                RMobileCall::KCapsDataCircuitSynchronous;
            break;
        case RMobileCall::EServiceDataCircuitSyncRdi:
            iDataCallCaps->iServiceCaps = 
                RMobileCall::KCapsDataCircuitSynchronousRDI;
            break;
        case RMobileCall::EServicePADAsyncUDI:                             
            iDataCallCaps->iServiceCaps = 
                RMobileCall::KCapsPADAsyncUDI;
            break;
        case RMobileCall::EServicePADAsyncRDI:
            iDataCallCaps->iServiceCaps = 
                RMobileCall::KCapsPADAsyncRDI;
            break;
        case RMobileCall::EServicePacketAccessSyncUDI:
            iDataCallCaps->iServiceCaps = 
                RMobileCall::KCapsPacketAccessSyncUDI;
            break;
        case RMobileCall::EServicePacketAccessSyncRDI:
            iDataCallCaps->iServiceCaps = 
                RMobileCall::KCapsPacketAccessSyncRDI;
            break;
        case RMobileCall::EServiceUnspecified:
        default :
            // aldready set iDataCallCaps->iServiceCaps = 0x00;
            break;
        }
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::CopyMobileDataCallParams
// Copies data call parameters to another variable without extension id.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::CopyMobileDataCallParams(
    RMobileCall::TMobileDataCallParamsV1& aDestination,
    const RMobileCall::TMobileDataCallParamsV1& aSource ) const
    {
    aDestination.iService = aSource.iService;
    aDestination.iSpeed = aSource.iSpeed;
    aDestination.iProtocol = aSource.iProtocol;
    aDestination.iQoS = aSource.iQoS;
    aDestination.iRLPVersion = aSource.iRLPVersion;
    aDestination.iModemToMSWindowSize = aSource.iModemToMSWindowSize;
    aDestination.iMSToModemWindowSize = aSource.iMSToModemWindowSize;
    aDestination.iAckTimer = aSource.iAckTimer;
    aDestination.iRetransmissionAttempts = aSource.iRetransmissionAttempts;
    aDestination.iResequencingPeriod = aSource.iResequencingPeriod;
    aDestination.iV42bisReq = aSource.iV42bisReq;
    aDestination.iV42bisCodewordsNum = aSource.iV42bisCodewordsNum;
    aDestination.iV42bisMaxStringLength = aSource.iV42bisMaxStringLength;
    aDestination.iUseEdge = aSource.iUseEdge; 
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::HscsdInfoReceived
// Set extension specific HSCSD info.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::ReceivedHscsdInfo(
    CMmDataPackage* aDataPackage )
    {
    RMobileCall::TMobileCallHscsdInfoV8* newHscsdInfo;
    RMobileCall::TMobileCallDataCapsV1* newDataCallCaps;

    aDataPackage->UnPackData( &newHscsdInfo, &newDataCallCaps );

    iInformationReceived = ETrue;

    if ( newDataCallCaps->iHscsdSupport )
        {
        iDataCallMode = EDataCallModeHscsd;
        }
    else
        {
        iDataCallMode = EDataCallModeNormal;
        }

    if ( newHscsdInfo->iCodings != iCurrentHSCSDInfo->iCodings ||
         newHscsdInfo->iAiur != iCurrentHSCSDInfo->iAiur ||
         newHscsdInfo->iTxTimeSlots != iCurrentHSCSDInfo->iTxTimeSlots ||
         newHscsdInfo->iRxTimeSlots != iCurrentHSCSDInfo->iRxTimeSlots ||
         newHscsdInfo->iCallParamOrigin != iCurrentHSCSDInfo->iCallParamOrigin ||
	     newHscsdInfo->iIconId.iIdentifier != iCurrentHSCSDInfo->iIconId.iIdentifier ||
	     newHscsdInfo->iIconId.iQualifier != iCurrentHSCSDInfo->iIconId.iQualifier ||
	     newHscsdInfo->iParamsCallControlModified != iCurrentHSCSDInfo->iParamsCallControlModified ||
	     newHscsdInfo->iSubAddress != iCurrentHSCSDInfo->iSubAddress ||
	     newHscsdInfo->iBearerCap1 != iCurrentHSCSDInfo->iBearerCap1 ||
	     newHscsdInfo->iBearerCap2 != iCurrentHSCSDInfo->iBearerCap2 ||
	     newHscsdInfo->iBCRepeatIndicator != iCurrentHSCSDInfo->iBCRepeatIndicator)
       	{
       	
       	       	
        *iCurrentHSCSDInfo = *newHscsdInfo;
        reinterpret_cast<CMmDataCallTsy*>( iMmCallTsy )->
            CompleteNotifyHscsdInfoChange();
        }

    if ( newDataCallCaps->iMaxRxTimeSlots != iDataCallCaps->iMaxRxTimeSlots ||
         newDataCallCaps->iMaxTxTimeSlots != iDataCallCaps->iMaxTxTimeSlots ||
         newDataCallCaps->iHscsdSupport != iDataCallCaps->iHscsdSupport  ||
         newDataCallCaps->iSpeedCaps != iDataCallCaps->iSpeedCaps )
        {
        //Update the caps for speed
        iDataCallCaps->iSpeedCaps = newDataCallCaps->iSpeedCaps;
        iDataCallCaps->iHscsdSupport = newDataCallCaps->iHscsdSupport;
        iDataCallCaps->iMaxRxTimeSlots = newDataCallCaps->iMaxRxTimeSlots;
        iDataCallCaps->iMaxTxTimeSlots = newDataCallCaps->iMaxTxTimeSlots;

        reinterpret_cast<CMmDataCallTsy*>( iMmCallTsy )->
            CompleteNotifyMobileDataCallCapsChange();
        }
    }

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::ReceivedMobileDataCallCaps
// Set extension specific data call capabilities.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::ReceivedMobileDataCallCaps(
    CMmDataPackage* aDataPackage )
    {
    RMobileCall::TMobileCallDataCapsV1* newDataCallCaps;

    aDataPackage->UnPackData( &newDataCallCaps );

    *iDataCallCaps = *newDataCallCaps;

    reinterpret_cast<CMmDataCallTsy*>( iMmCallTsy )->
        CompleteNotifyMobileDataCallCapsChange();
    }
   
    
// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::SsmPluginCallback
// This is callback
// This method is called from System State Manager (or from CMmCallGsmWcdmaExt, if there 
//  is no reason to change RF state), after RF state changing      
// ---------------------------------------------------------------------------
//
void CMmCallGsmWcdmaExt::SsmPluginCallback(TInt aResult, TCtsySsmCallbackData& aCallbackData)
	{
	TInt result (aResult);
	
	if ( KErrNone == aResult )
    	{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_SSMPLUGINCALLBACK_1, "TSY:CMmCallGsmWcdmaExt::SsmPluginCallback: Dialing...");
		
		//at this point the RF should be enabled in all cases
		TRAPD(err, result = iMessageManager->HandleRequestL(
         		EMobileCallDialEmergencyCall, &aCallbackData.iNumberPackage ));
        if (KErrNone != err)
        	{
        	result = err;
        	}

    	//if the RF state was inactive prior dialing the emergency call
        //change the status of the RF in phone to EEmergencyCallMade
        //this will enable that RF is set back to off when emergency call
        //is finished
        TRfStateInfo stateInfo = iMmCallTsy->Phone()->GetRfStateInfo();
        if ( result == KErrNone && stateInfo != ERfsStateInfoNormal )
        	{
            iMmCallTsy->Phone()->SetRfStatus( EEmergencyCallMade );            
            }

        }
    
    //pass answer from SSM further to CMmVoiceCallTsy::SsmPluginCallback()
    ((CMmVoiceCallTsy*)iMmCallTsy)->SsmPluginCallback(result, aCallbackData);		

	}

// End of File
