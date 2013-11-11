// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TCALLCONTROLTSYTESTHELPER_H
#define TCALLCONTROLTSYTESTHELPER_H

#include "ttsytesthelperbase.h"

	
class TCallControlTsyTestHelper : public TTsyTestHelperBase
	{
public:
	TCallControlTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep);

	void WaitForCallNotifyCapsChange( 
				RCall& aCall,
				TEtelRequestBase& aRequestStatus,
				RCall::TCaps& aCaps, 
				RCall::TCaps aWantedCaps,
				RCall::TCaps aUnwantedCaps,
				TInt aWantedStatus );

	void WaitForCallNotifyCallDurationChange( 
				RCall& aCall,
				TEtelRequestBase& aRequestStatus,
				TTimeIntervalSeconds& aTime, 
				TTimeIntervalSeconds aExpectedTime,
				TInt aWantedStatus );

	void WaitForCallNotifyHookChange( 
				RCall& aCall,
				TEtelRequestBase& aRequestStatus,
				RCall::THookStatus& aHookStatus, 
				RCall::THookStatus aExpectedHookStatus,
				TInt aWantedStatus );

	void WaitForCallNotifyStatusChange( 
				RCall& aCall,
				TEtelRequestBase& aRequestStatus,
				RCall::TStatus& aCallStatus, 
				RCall::TStatus& aExpectedCallStatus,
				TInt aWantedStatus );

	void WaitForLineNotifyCapsChange( 
				RLine& aLine,
				TEtelRequestBase& aRequestStatus,
				RLine::TCaps& aCaps, 
				RLine::TCaps aWantedCaps,
				RLine::TCaps aUnwantedCaps,
				TInt aWantedStatus );

	void WaitForLineNotifyCallAdded( 
				RLine& aLine,
				TEtelRequestBase& aRequestStatus,
				TName& aName, 
				TName aExpectedName,
				TInt aWantedStatus );

	void WaitForLineNotifyHookChange( 
				RLine& aLine,
				TEtelRequestBase& aRequestStatus,
				RCall::THookStatus& aHookStatus, 
				RCall::THookStatus aExpectedHookStatus,
				TInt aWantedStatus );

	void WaitForLineNotifyStatusChange( 
				RLine& aLine,
				TEtelRequestBase& aRequestStatus,
				RCall::TStatus& aLineStatus, 
				RCall::TStatus& aExpectedLineStatus,
				TInt aWantedStatus );

	void WaitForLineNotifyIncomingCall( 
				RLine& aLine,
				TEtelRequestBase& aRequestStatus,
				TName& aName, 
				TName aExpectedName,
				TInt aWantedStatus );

	void WaitForPhoneNotifyCapsChange( 
				RPhone& aPhone,
				TEtelRequestBase& aRequestStatus,
				RPhone::TCaps& aCaps, 
				RPhone::TCaps aWantedCaps,
				RPhone::TCaps aUnwantedCaps,
				TInt aWantedStatus );

	void WaitForPhoneNotifyModemDetected( 
				RPhone& aPhone,
				TEtelRequestBase& aRequestStatus,
				RPhone::TModemDetection& aDetection, 
				RPhone::TModemDetection aExpectedDetection,
				TInt aWantedStatus );

	void WaitForMobileCallNotifyAlternatingCallSwitch( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus );

	void WaitForMobileCallNotifyAudioToneEvent( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobileCall::TAudioToneV3> &aAudioToneV3,
				TInt aWantedStatus );

	void WaitForMobileCallNotifyCallEvent( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				RMobileCall::TMobileCallEvent& aEvent, 
				RMobileCall::TMobileCallEvent aExpectedEvent,
				TInt aWantedStatus );

	void WaitForMobileCallNotifyHscsdInfoChange( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobileCall::TMobileCallHscsdInfoV1> &aMobileCallHscsdInfoV1,
				TInt aWantedStatus );

	void WaitForMobileCallNotifyMobileCallCapsChange( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				TCmpRMobileCallTMobileCallCaps &aMobileCallCapsV1,
				TInt aWantedStatus );

	void WaitForMobileCallNotifyMobileCallStatusChange( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				RMobileCall::TMobileCallStatus& aStatus, 
				RMobileCall::TMobileCallStatus aExpectedStatus,
				TInt aWantedStatus );

	void WaitForMobileCallNotifyMobileDataCallCapsChange( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobileCall::TMobileCallDataCapsV1> &aMobileCallDataCapsV1,
				TInt aWantedStatus );

	void WaitForMobileCallNotifyUUSCapsChange( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus );

	void WaitForMobileCallNotifyVoiceFallback( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				TName& aCallName, 
				TName aExpectedCallName,
				TInt aWantedStatus );

	void WaitForMobileCallNotifyPrivacyConfirmation( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhonePrivacy& aPrivacySetting, 
				RMobilePhone::TMobilePhonePrivacy aExpectedPrivacySetting,
				TInt aWantedStatus );

	void WaitForMobileCallNotifyTrafficChannelConfirmation( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				RMobileCall::TMobileCallTch& aTchType, 
				RMobileCall::TMobileCallTch aExpectedTchType,
				TInt aWantedStatus );

	void WaitForMobileCallNotifyRemotePartyInfoChange( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobileCall::TMobileCallRemotePartyInfoV1> &aMobileCallRemotePartyInfoV1,
				TInt aWantedStatus );

	void WaitForMobileConferenceCallNotifyCapsChange( 
				RMobileConferenceCall& aMobileConferenceCall,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus );

	void WaitForMobileConferenceCallNotifyConferenceEvent( 
				RMobileConferenceCall& aMobileConferenceCall,
				TEtelRequestBase& aRequestStatus,
				RMobileConferenceCall::TMobileConferenceEvent& aEvent, 
				RMobileConferenceCall::TMobileConferenceEvent aExpectedEvent,
				TName &aCallName,
				TInt aWantedStatus );

	void WaitForMobileConferenceCallNotifyConferenceStatusChange( 
				RMobileConferenceCall& aMobileConferenceCall,
				TEtelRequestBase& aRequestStatus,
				RMobileConferenceCall::TMobileConferenceStatus& aStatus, 
				RMobileConferenceCall::TMobileConferenceStatus aExpectedStatus,
				TInt aWantedStatus );

	void WaitForMobileLineNotifyMobileLineStatusChange( 
				RMobileLine& aMobileLine,
				TEtelRequestBase& aRequestStatus,
				RMobileCall::TMobileCallStatus& aStatus, 
				RMobileCall::TMobileCallStatus aExpectedStatus,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyALSLineChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneALSLine& aALSLine, 
				RMobilePhone::TMobilePhoneALSLine aExpectedALSLine,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyAlternatingCallCapsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyAlternatingCallModeChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneAlternatingCallMode& aMode, 
				RMobilePhone::TMobilePhoneAlternatingCallMode aExpectedMode,
				RMobilePhone::TMobileService &aFirstService,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyDTMFCapsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyDTMFEvent( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneDTMFEvent& aEvent, 
				RMobilePhone::TMobilePhoneDTMFEvent& aExpectedEvent,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyFdnStatusChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneFdnStatus& aFdnStatus, 
				RMobilePhone::TMobilePhoneFdnStatus aExpectedFdnStatus,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyIncomingCallTypeChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneIncomingCallType &aCallType,
				TCmpBase<RMobileCall::TMobileDataCallParamsV1> &aMobileDataCallParamsV1,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyIndicatorChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TUint32& aIndicator, 
				TUint32 aExpectedIndicator,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyMessageWaiting( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TInt& aCount, 
				TInt aExpectedCount,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyMulticallIndicatorChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneMulticallIndicator& aMulticallIndicator, 
				RMobilePhone::TMobilePhoneMulticallIndicator aExpectedMulticallIndicator,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyMulticallParamsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneMulticallSettingsV1> &aMobilePhoneMulticallSettingsV1,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyMultimediaCallPreferenceChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneMultimediaSettings& aMmSettings, 
				RMobilePhone::TMobilePhoneMultimediaSettings aExpectedMmSettings,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyStopInDTMFString( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyAirTimeDurationChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TTimeIntervalSeconds& aTime, 
				TTimeIntervalSeconds aExpectedTime,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyCostInfoChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneCostInfoV1> &aMobilePhoneCostInfoV1,
				TInt aWantedStatus );
				
	void CheckForValidCallInfo(RMobileCall::TMobileCallInfoV1& aCallInfo, TPtrC aNumber);
	void CheckForValidCallInfo(RMobileCall::TMobileCallInfoV1& aCallInfo, TInt aRequiredExitCode=KErrNone, TBool aIsCallConnected=ETrue);

	}; // class TCallControlTsyTestHelper 

#endif
