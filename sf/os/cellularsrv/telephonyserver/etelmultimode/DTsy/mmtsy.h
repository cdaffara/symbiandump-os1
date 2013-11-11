// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalAll
*/


#ifndef __MMTSY_H
#define __MMTSY_H

#include <e32base.h>
#include <et_phone.h>
#include "ETELMM.H"
#include "mmlist.h"
#include "coretsy.H"
#include "mmretrieve.h"

/**************************************************************************/
//
//	Utility classes used by dummy Multimode TSY
//
/**************************************************************************/

class CListReadAllAttempt : public CBase
	{
public:
	static CListReadAllAttempt* NewL(RMobilePhone::TClientId* aId, TTsyReqHandle aReqHandle);
	~CListReadAllAttempt();

protected:
	CListReadAllAttempt(RMobilePhone::TClientId* aId, TTsyReqHandle aReqHandle);

private:
	void ConstructL();

public:
	CBufBase* iListBuf;
	RMobilePhone::TClientId iClient;
	TTsyReqHandle iReqHandle;
	};

class CPhoneDMmTsy;

/**************************************************************************/
//
//	CCallDMmTsy
//
/**************************************************************************/

class CCallDMmTsy : public CCallDummyBase
	{
public:
	static CCallDMmTsy* NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac);
	CCallDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CCallDMmTsy();
public:
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
private:
//	static CCallDMmTsy* This(TAny* aPtr);

	TInt GetMobileDataCallCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps);
	TInt NotifyMobileDataCallCapsChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps);
	TInt NotifyMobileDataCallCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);

	static void InitialiseDataCallCaps(TDes8* aCallCaps);

	TInt GetMobileDataCallRLPRange(const TTsyReqHandle aTsyReqHandle, TInt* aRLPVersion, TDes8* aRLPRange);
	TInt GetMobileDataCallRLPRangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt SetDynamicHscsdParams(const TTsyReqHandle aTsyReqHandle, TInt* aAiur, TInt* aRxTimeslots);
	TInt SetDynamicHscsdParamsCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetCurrentHscsdInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aHSCSDInfo);
	TInt NotifyHscsdInfoChange(const TTsyReqHandle aTsyReqHandle, TDes8* aHSCSDInfo);
	TInt NotifyHscsdInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifyVoiceFallback(const TTsyReqHandle aTsyReqHandle, TDes* aCallName);
	TInt NotifyVoiceFallbackCancel(const TTsyReqHandle aTsyReqHandle);

	static void InitialiseHscsdInfo(RMobileCall::TMobileCallHscsdInfoV1& aHSCSDInfo);

	TInt SwitchAlternatingCall(const TTsyReqHandle aTsyReqHandle);
	TInt SwitchAlternatingCallCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyAlternatingCallSwitch(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyAlternatingCallSwitchCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetMobileCallCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps);
	TInt NotifyMobileCallCapsChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps);
	TInt NotifyMobileCallCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetMobileCallStatus(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus);
	TInt NotifyMobileCallStatusChange(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus);
	TInt NotifyMobileCallStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt Hold(const TTsyReqHandle aTsyReqHandle);
	TInt HoldCancel(const TTsyReqHandle aTsyReqHandle);

	TInt Resume(const TTsyReqHandle aTsyReqHandle);
	TInt ResumeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt Swap(const TTsyReqHandle aTsyReqHandle);
	TInt SwapCancel(const TTsyReqHandle aTsyReqHandle);

	TInt Deflect(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallDeflect* aDeflectType, RMobilePhone::TMobileAddress* aDestination);
	TInt DeflectCancel(const TTsyReqHandle aTsyReqHandle);

	TInt Transfer(const TTsyReqHandle aTsyReqHandle);
	TInt TransferCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GoOneToOne(const TTsyReqHandle aTsyReqHandle);
	TInt GoOneToOneCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifyCallEvent(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallEvent* aEvent);
	TInt NotifyCallEventCancel(const TTsyReqHandle aTsyReqHandle);

	TInt DialNoFdnCheck(const TTsyReqHandle aTsyReqHandle, TDesC8* aCallParams, TDesC* aNumber);
	TInt DialNoFdnCheckCancel(const TTsyReqHandle aTsyReqHandle);

	TInt DialCallControl(const TTsyReqHandle aTsyReqHandle, TDesC8* aCallParams, TDesC* aNumber);
	TInt DialCallControlCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt SetPrivacySetting(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePrivacy* aPrivacySetting);
	TInt NotifyPrivacyConfirmation(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePrivacy* aPrivacySetting);
	TInt NotifyPrivacyConfirmationCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt SetTrafficChannel(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallTch* aTchRequest);
	TInt NotifyTrafficChannelConfirmation(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallTch* aTchType);
	TInt NotifyTrafficChannelConfirmationCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetMobileCallInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aInfo);
	TInt GetMobileCallInfoCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt NotifyRemotePartyInfoChange(const TTsyReqHandle aTsyReqHandle, TDes8* aRemotePartyInfo);
	TInt NotifyRemotePartyInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt DialEmergencyCall(const TTsyReqHandle aTsyReqHandle, TDesC* aNumber);
	TInt DialEmergencyCallCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt ActivateCCBS(const TTsyReqHandle aTsyReqHandle, TInt* aIndex);
	TInt ActivateCCBSCancel(const TTsyReqHandle aTsyReqHandle);
	TInt RejectCCBS(const TTsyReqHandle aTsyReqHandle);

	TInt GetUUSCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifyUUSCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifyUUSCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
    TInt ValidateCallParams(TDesC8* aCallParams);
	TInt ActivateUUS(const TTsyReqHandle aTsyReqHandle, TDes8* aUUIRequest);
	TInt ActivateUUSCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SendUUI(const TTsyReqHandle aTsyReqHandle, TBool* aMore, TDesC16* aUUI);
	TInt SendUUICancel(const TTsyReqHandle aTsyReqHandle);
	TInt ReceiveUUI(const TTsyReqHandle aTsyReqHandle, TDes16* aUUI);
	TInt ReceiveUUICancel(const TTsyReqHandle aTsyReqHandle);
	TInt HangupWithUUI(const TTsyReqHandle aTsyReqHandle, TDesC16* aUUI);
	TInt HangupWithUUICancel(const TTsyReqHandle aTsyReqHandle);
	TInt AnswerIncomingCallWithUUI(const TTsyReqHandle aTsyReqHandle, TDesC8* aCallParams, TDesC16* aUUI);
	TInt AnswerIncomingCallWithUUICancel(const TTsyReqHandle aTsyReqHandle);
	TInt AnswerMultimediaCallAsVoice(const TTsyReqHandle aTsyReqHandle, TDesC8* aCallParams, TDes16* aCallName);
	TInt AnswerMultimediaCallAsVoiceCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyAudioToneEvent(const TTsyReqHandle aTsyReqHandle, TDes8* aAudioTone);
	TInt NotifyAudioToneEventCancel(const TTsyReqHandle aTsyReqHandle);

	TInt DialISV(const TTsyReqHandle aTsyReqHandle, TDesC8* aCallParams, TDesC* aNumber);
    TInt DialISVCancel(const TTsyReqHandle aTsyReqHandle);
	TInt AnswerIncomingCallISV(const TTsyReqHandle aTsyReqHandle, TDesC8* aCallParams);
    TInt AnswerIncomingCallISVCancel(const TTsyReqHandle aTsyReqHandle);

private: // data
	TInt iNotifyMobileCallStatusChange;
	TInt iNotifyCallEvent;
	TInt iNotifyMobileCallCapsChange;
	TInt iNotifyRemotePartyInfoChange;
	TInt iNotifyMobileDataCallCapsChange;
	TInt iNotifyHscsdParamsChange;
	TInt iNotifyHscsdInfoChange;
	TInt iNotifyPrivacyConfirmation;
	TInt iNotifyTrafficChannelConfirmation;
	TInt iNotifyVoiceFallback;
	TInt iNotifyAlternatingCallSwitch;
	TInt iNotifyUUSCapsChange;
	TInt iCallTestCount;
	CPhoneDMmTsy *iPhone;
	TInt iNotifyAudioToneEvent;
protected:
	};

/**************************************************************************/
//
//	CLineDMmTsy
//
/**************************************************************************/

class CLineDMmTsy : public CLineDummyBase
	{
public:
	static CLineDMmTsy* NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac);
	CLineDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CLineDMmTsy();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	virtual TInt GetCaps(const TTsyReqHandle aTsyReqHandle,RLine::TCaps* aCaps);

private:
	TInt GetMobileLineStatus(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus);
	TInt NotifyMobileLineStatusChange(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallStatus* aStatus);
	TInt NotifyMobileLineStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);

private:// data
	//
	TInt iNotifyMobileLineStatusChange;
	TInt iCallObjectCount;
	CPhoneDMmTsy *iPhone;
protected:
	};

/**************************************************************************/
//
//	CPhoneDMmTsy
//
/**************************************************************************/

class CPhoneDMmTsy : public CPhoneDummyBase
	{
public:
	static CPhoneDMmTsy* NewL(CPhoneFactoryDummyBase* aFac);
	CPhoneDMmTsy(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CPhoneDMmTsy();
public:
	virtual void Init();
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);

	// methods for the list of aos which correspond to the outstanding delayed asyncrhonous reqs
	TInt RemoveDelayedReq(const TTsyReqHandle aTsyReqHandle);
	void AddDelayedReq(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject);
	// methods for the list of aos which correspond to the outstanding location services delayed asyncrhonous reqs
	void AddDelayedReq(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject,TInt aPriority);
private:
	void RegisterDelayedReqL(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject);
	void RegisterDelayedReqL(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject,TInt aPriority);
private:
	static CPhoneDMmTsy* This(TAny* aPtr);

	TInt GetMultimodeCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt GetPhoneStoreInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aGetInfo, TDes* aName);
	TInt GetPhoneStoreInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aGetInfo, RMobilePhone::TPhoneStoreNameAndIccType* aTable);
	TInt GetPhoneStoreInfoCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetIccAccessCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifyIccAccessCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifyIccAccessCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetIccIdentity(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TIccIdentity* aIdentity);
	TInt GetIccIdentityCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetCustomerServiceProfile(const TTsyReqHandle aTsyReqHandle, TDes8* aFile);
	TInt GetCustomerServiceProfileCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetServiceTable(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneServiceTable* aTable, TDes8* aFile);
	TInt GetServiceTableCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetBatteryCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt GetBatteryInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneBatteryInfoV1* aInfo);
	TInt GetBatteryInfoCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyBatteryInfoChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneBatteryInfoV1* aInfo);
	TInt NotifyBatteryInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetSignalCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt GetSignalStrength(const TTsyReqHandle aTsyReqHandle, TInt32* aSignalStrength, TInt8* aBar);
	TInt GetSignalStrengthCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySignalStrengthChange(const TTsyReqHandle aTsyReqHandle, TInt32* aSignalStrength, TInt8* aBar);
	TInt NotifySignalStrengthChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetIndicatorCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aActionCaps, TUint32* aIndCaps);
	TInt GetIndicator(const TTsyReqHandle aTsyReqHandle, TUint32* aIndicator);
	TInt GetIndicatorCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyIndicatorChange(const TTsyReqHandle aTsyReqHandle, TUint32* aIndicator);
	TInt NotifyIndicatorChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetIdentityCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt GetPhoneId(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneIdentityV1* aId);
	TInt GetPhoneIdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetSubscriberId(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSubscriberId* aId);
	TInt GetSubscriberIdCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetDTMFCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifyDTMFCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifyDTMFCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifyStopInDTMFString(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyStopInDTMFStringCancel(const TTsyReqHandle aTsyReqHandle);

	TInt SendDTMFTones(const TTsyReqHandle aTsyReqHandle, TDesC* aTones);
	TInt SendDTMFTonesCancel(const TTsyReqHandle aTsyReqHandle);

	TInt ReadDTMFTones(const TTsyReqHandle aTsyReqHandle, TDes* aTones);
	TInt ReadDTMFTonesCancel(const TTsyReqHandle aTsyReqHandle);

	TInt StartDTMFTone(const TTsyReqHandle aTsyReqHandle, TChar* aTone);

	TInt StopDTMFTone(const TTsyReqHandle aTsyReqHandle);

	TInt ContinueDTMFStringSending(const TTsyReqHandle aTsyReqHandle, TBool *aContinue);

	TInt GetNetworkCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);

	TInt GetCurrentMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNetworkMode* aMode);
	TInt NotifyModeChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNetworkMode* aMode);
	TInt NotifyModeChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetHomeNetwork(const TTsyReqHandle aTsyReqHandle, TDes8* aNetworkInfoPckg);
	TInt GetHomeNetworkCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetCurrentNetwork(const TTsyReqHandle aTsyReqHandle, TDes8* aNetworkInfoPckg, RMobilePhone::TMobilePhoneLocationAreaV1* aArea);
	TInt GetCurrentNetworkCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyCurrentNetworkChange(const TTsyReqHandle aTsyReqHandle, TDes8* aNetworkInfoPckg, RMobilePhone::TMobilePhoneLocationAreaV1* aArea);
	TInt NotifyCurrentNetworkChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetCellInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aCellInfoPckg);
	TInt GetCellInfoCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyCellInfoChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCellInfoPckg);
	TInt NotifyCellInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetNetworkRegistrationStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneRegistrationStatus* aStatus);
	TInt GetNetworkRegistrationStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyNetworkRegistrationStatusChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneRegistrationStatus* aStatus);
	TInt NotifyNetworkRegistrationStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetDetectedNetworksPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt ProcessGetDetectedNetworksPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt GetDetectedNetworksPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetDetectedNetworksCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetDetectedNetworksV2Phase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt ProcessGetDetectedNetworksV2Phase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt GetDetectedNetworksV2Phase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetDetectedNetworksV2Cancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetDetectedNetworksV5Phase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt ProcessGetDetectedNetworksV5Phase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt GetDetectedNetworksV5Phase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetDetectedNetworksV5Cancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetDetectedNetworksV8Phase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt ProcessGetDetectedNetworksV8Phase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt GetDetectedNetworksV8Phase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetDetectedNetworksV8Cancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetNetworkSelectionSetting(const TTsyReqHandle aTsyReqHandle, TDes8 *aSetting);
	TInt SetNetworkSelectionSetting(const TTsyReqHandle aTsyReqHandle, TDes8* aSetting);
	TInt SetNetworkSelectionSettingCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyNetworkSelectionSettingChange(const TTsyReqHandle aTsyReqHandle, TDes8* aSetting);
	TInt NotifyNetworkSelectionSettingChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SelectNetwork(const TTsyReqHandle aTsyReqHandle, TBool* aIsManual, RMobilePhone::TMobilePhoneNetworkManualSelection* aManualSelection);
	TInt SelectNetworkCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetNetworkTimeInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNITZ* aDateTime);
	TInt NotifyNetworkTimeInfoChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNITZ* aDateTime);
	TInt NotifyNetworkTimeInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetServiceProviderName(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneServiceProviderNameV2* aName);
	TInt GetServiceProviderNameCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetDefaultPrivacy(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePrivacy* aSetting);
	TInt SetDefaultPrivacy(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePrivacy* aSetting);
	TInt SetDefaultPrivacyCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyDefaultPrivacyChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePrivacy* aSetting);
	TInt NotifyDefaultPrivacyChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetCallServiceCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifyCallServiceCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifyCallServiceCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt ProgramFeatureCodeString(const TTsyReqHandle aTsyReqHandle, TDesC* aFCString, RMobilePhone::TNetworkServiceAndAction *aFeatureCode);
	TInt ProgramFeatureCodeStringCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetFeatureCodeString(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TNetworkServiceAndAction *aFeatureCode, TDes *aFCString);
	TInt GetFeatureCodeStringCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SendNetworkServiceRequest(const TTsyReqHandle aTsyReqHandle, TDesC* aString);
	TInt SendNetworkServiceRequestCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetCallForwardingStatusPhase1(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneCFList::TGetCallForwardingRequest* aReqData, TInt* aBufSize);
	TInt ProcessGetCallForwardingStatusPhase1L(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneCFList::TGetCallForwardingRequest* aReqData, TInt* aBufSize);
	TInt GetCallForwardingStatusPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetCallForwardingStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyCallForwardingStatusChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneCFCondition* aCondition);
	TInt NotifyCallForwardingStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetCallForwardingStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneCFCondition* aCondition, RMobilePhone::TMobilePhoneCFChangeV1* aInfo);
	TInt SetCallForwardingStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyCallForwardingActive(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobileService* aServiceGroup, RMobilePhone::TMobilePhoneCFActive* aActive);
	TInt NotifyCallForwardingActiveCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetIdentityServiceStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TIdServiceAndLocation *aInfo,
		RMobilePhone::TMobilePhoneIdServiceStatus* aStatus);
	TInt GetIdentityServiceStatusCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetCallBarringPhase1(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneCBList::TGetCallBarringRequest* aReqData, TInt* aBufSize);
	TInt ProcessGetCallBarringPhase1L(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneCBList::TGetCallBarringRequest* aReqData, TInt* aBufSize);
	TInt GetCallBarringPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetCallBarringCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt SetCallBarringStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneCBCondition *aCondition, RMobilePhone::TMobilePhoneCBChangeV1* aInfo);
	TInt SetCallBarringStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyCallBarringStatusChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneCBCondition* aCondition);
	TInt NotifyCallBarringStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetCallBarringPassword(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePasswordChangeV1* aPassword);
	TInt SetCallBarringPasswordCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetSSPassword(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePasswordChangeV2* aPassword, TInt* aService);
	TInt SetSSPasswordCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetCallWaitingPhase1(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneCWList::TGetCallWaitingRequest* aReqData, TInt* aBufSize);
	TInt ProcessGetCallWaitingPhase1L(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneCWList::TGetCallWaitingRequest* aReqData, TInt* aBufSize);
	TInt GetCallWaitingPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetCallWaitingCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetCallWaitingStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobileService* aServiceGroup, RMobilePhone::TMobilePhoneServiceAction *aAction);
	TInt SetCallWaitingStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyCallWaitingStatusChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCWStatus);
	TInt NotifyCallWaitingStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetCCBSStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneCCBSStatus* aCcbsStatus, RMobilePhone::TMobileInfoLocation* aLocation);
	TInt GetCCBSStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyCCBSStatusChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneCCBSStatus* aCcbsStatus);
	TInt NotifyCCBSStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt DeactivateCCBS(const TTsyReqHandle aTsyReqHandle, TInt* aIndex);
	TInt DeactivateCCBSCancel(const TTsyReqHandle aTsyReqHandle);
	TInt AcceptCCBSRecall(const TTsyReqHandle aTsyReqHandle, TInt* aIndex, TDes* aCallName);
	TInt AcceptCCBSRecallCancel(const TTsyReqHandle aTsyReqHandle);
	TInt RefuseCCBSRecall(const TTsyReqHandle aTsyReqHandle, TInt* aIndex);
	TInt NotifyCCBSRecall(const TTsyReqHandle aTsyReqHandle, TDes8* aCCBSEntry);
	TInt NotifyCCBSRecallCancel(const TTsyReqHandle aTsyReqHandle);


	TInt GetCallCompletionPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aReqData, TInt* aBufSize);
	TInt ProcessGetCallCompletionPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aReqData, TInt* aBufSize);
	TInt GetCallCompletionPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetCallCompletionCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetAlternatingCallCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifyAlternatingCallCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifyAlternatingCallCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetAlternatingCallMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneAlternatingCallMode* aMode, RMobilePhone::TMobileService* aFirstService);
	TInt SetAlternatingCallMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneAlternatingCallMode* aMode, RMobilePhone::TMobileService* aFirstService);
	TInt SetAlternatingCallModeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyAlternatingCallModeChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneAlternatingCallMode* aMode, RMobilePhone::TMobileService* aFirstService);
	TInt NotifyAlternatingCallModeChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetALSLine(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneALSLine* aALSLine);
	TInt SetALSLine(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneALSLine* aALSLine);
	TInt SetALSLineCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyALSLineChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneALSLine* aALSLine);
	TInt NotifyALSLineChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetCostCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifyCostCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifyCostCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt ClearCostMeter(const TTsyReqHandle aTsyReqHandle,RMobilePhone::TMobilePhoneCostMeters* aMeters);
	TInt ClearCostMeterCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetMaxCostMeter(const TTsyReqHandle aTsyReqHandle, TUint *aUnits);
	TInt SetMaxCostMeterCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetPuct(const TTsyReqHandle aTsyReqHandle, TDes8* aPuct);
	TInt SetPuctCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetCostInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aCostInfo);
	TInt GetCostInfoCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyCostInfoChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCostInfo);
	TInt NotifyCostInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetSecurityCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifySecurityCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifySecurityCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetLockInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneLock *aLock, TDes8* aLockInfo) ;
	TInt GetLockInfoCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyLockInfoChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneLock* aLock, TDes8* aLockInfo);
	TInt NotifyLockInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetLockSetting(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneLock *aLock, RMobilePhone::TMobilePhoneLockSetting *aLockSetting);
	TInt SetLockSettingCancel(const TTsyReqHandle aTsyReqHandle);
	TInt ChangeSecurityCode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode *aType, RMobilePhone::TMobilePhonePasswordChangeV1 *aChange);
	TInt ChangeSecurityCodeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetSecurityCodeInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode *aSecurityCode, TDes8* aSecurityCodeInfo);
	TInt GetSecurityCodeInfoCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySecurityCodeInfoChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode* aSecurityCode,TDes8* aSecurityCodeInfo);
	TInt NotifySecurityCodeInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyOnSecurityEvent(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityEvent* aEvent);
	TInt NotifyOnSecurityEventCancel(const TTsyReqHandle aTsyReqHandle);
	TInt VerifySecurityCode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode *aType, RMobilePhone::TCodeAndUnblockCode* aData);
	TInt VerifySecurityCodeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt AbortSecurityCode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode *aType);

	TInt NotifyMessageWaiting(const TTsyReqHandle aTsyReqHandle, TInt* aCount);
	TInt NotifyMessageWaitingCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetIccMessageWaitingIndicators(const TTsyReqHandle aTsyReqHandle, TDes8* aMsgIndicators);
	TInt GetIccMessageWaitingIndicatorsCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetIccMessageWaitingIndicators(const TTsyReqHandle aTsyReqHandle, TDes8* aMsgIndicators);
	TInt SetIccMessageWaitingIndicatorsCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyIccMessageWaitingIndicatorsChange(const TTsyReqHandle aTsyReqHandle, TDes8* aMsgIndicators);
	TInt NotifyIccMessageWaitingIndicatorsChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetFdnStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneFdnStatus* aFdnStatus);
	TInt GetFdnStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetFdnSetting(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneFdnSetting *aFdnSetting);
	TInt SetFdnSettingCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyFdnStatusChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneFdnStatus* aFdnStatus);
	TInt NotifyFdnStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetMulticallParams(const TTsyReqHandle aTsyReqHandle, TDes8* aMulticallParams);
	TInt SetMulticallParams(const TTsyReqHandle aTsyReqHandle, TInt* aUserMaxBearers);
	TInt GetMulticallParamsCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetMulticallParamsCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyMulticallParamsChange(const TTsyReqHandle aTsyReqHandle, TDes8* aMulticallParams);
	TInt NotifyMulticallParamsChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetIncomingCallType(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneIncomingCallType* aCallType, TDes8* aDataParams);
	TInt GetIncomingCallTypeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetIncomingCallType(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneIncomingCallType* aCallType, TDes8* aDataParams);
	TInt SetIncomingCallTypeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyIncomingCallTypeChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneIncomingCallType* aCallType, TDes8* aDataParams);
	TInt NotifyIncomingCallTypeChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetUUSSetting(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneUUSSetting* aSetting);
	TInt GetUUSSettingCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetUUSSetting(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneUUSSetting* aSetting);
	TInt SetUUSSettingCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyUUSSettingChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneUUSSetting* aSetting);
	TInt NotifyUUSSettingChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetMultimediaCallPreference(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneMultimediaSettings* aMmSettings);

	TInt SetMultimediaCallPreference(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneMultimediaSettings* aMmSettings);
	TInt SetMultimediaCallPreferenceCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyMultimediaCallPreferenceChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneMultimediaSettings* aMmSettings);
	TInt NotifyMultimediaCallPreferenceChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetNetworkSecurityLevel(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNetworkSecurity* aSecurity);
	TInt GetNetworkSecurityLevelCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyNetworkSecurityLevelChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNetworkSecurity* aSecurity);
	TInt NotifyNetworkSecurityLevelChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt EnumerateUSimApplications(const TTsyReqHandle aTsyReqHandle,TInt* aCount, RMobilePhone::TAID* aActiveAID);
	TInt EnumerateUSimApplicationsCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt SetUSimApplicationStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID, RMobilePhone::TUSimAppAction* aAction);
	TInt SetUSimApplicationStatusCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetUSimApplicationInfo(const TTsyReqHandle aTsyReqHandle, TInt* aIndex,RMobilePhone::TUSimApplicationInfoV2* aInfo);
	TInt GetUSimApplicationInfoCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt NotifyUSimApplicationsInfoChange(const TTsyReqHandle aTsyReqHandle, TInt* aCount, RMobilePhone::TAID* aActiveAID);
	TInt NotifyUSimApplicationsInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt SetUSimAppsSelectionMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aMode);
	TInt GetUSimAppsSelectionMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aMode);
	TInt NotifyUSimAppsSelectionModeChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aMode);
	TInt NotifyUSimAppsSelectionModeChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetMailboxNumbers(const TTsyReqHandle aTsyReqHandle, TDes8 *aMialBox);
	TInt GetMailboxNumbersCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyMailboxNumbersChange(const TTsyReqHandle aTsyReqHandle, TDes8 *aMialBox);
	TInt NotifyMailboxNumbersChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyMmsUpdate(const TTsyReqHandle aTsyReqHandle, TDes8* aNotification);
	TInt NotifyMmsUpdateCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetMmsUserConnParams(const TTsyReqHandle aTsyReqHandle, TDes8* aConnectivity);
	TInt SetMmsUserConnParamsCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetMmsUserPreferences(const TTsyReqHandle aTsyReqHandle,TDes8* aPreferences);
	TInt SetMmsUserPreferencesCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetMmsConfig(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMmsConnParams aType, TDes8* aConnectivity);
	TInt GetMmsConfigCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyMmsConfig(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMmsConnParams aType, TDes8* aConnectivity);
	TInt NotifyMmsConfigCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetHomeNetworkSearchPeriod(const TTsyReqHandle aTsyReqHandle, TInt* aSearchIntv);
	TInt GetHomeNetworkSearchPeriodCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetLastUsedAccessTechnology(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNetworkAccess* aAccessTech);
	TInt GetLastUsedAccessTechnologyCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetNetworkInvScanSetting(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneInvestigationScan* aSetting);
	TInt NotifyNetworkInvScanChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneInvestigationScan* aSetting);
	TInt NotifyNetworkInvScanChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyNetworkInvScanEvent(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneInvestigationScanEvent* aEvent);
	TInt NotifyNetworkInvScanEventCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetCallForwardingIndicator(const TTsyReqHandle aTsyReqHandle, TDes8* aIndicator);
	TInt GetCallForwardingIndicatorCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetCompMethodName(const TTsyReqHandle aTsyReqHandle, TUint aCmi, TDes* aAlphaTag);
	TInt GetCompMethodNameCancel(const TTsyReqHandle aTsyReqHandle);
	TInt EnumerateAPNEntries(const TTsyReqHandle aTsyReqHandle, TUint32* aIndex);
	TInt EnumerateAPNEntriesCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetAPNname(const TTsyReqHandle aTsyReqHandle, TUint32 aIndex, TDes8* aAPN);
	TInt GetAPNnameCancel(const TTsyReqHandle aTsyReqHandle);
	TInt AppendAPNName(const TTsyReqHandle aTsyReqHandle, TDes8* aAPN);
	TInt AppendAPNNameCancel(const TTsyReqHandle aTsyReqHandle);
	TInt DeleteAPNName(const TTsyReqHandle aTsyReqHandle, TUint32 aIndex);
	TInt DeleteAPNNameCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyAPNListChanged(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyAPNListChangedCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetAPNControlListServiceStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAPNControlListServiceStatus aAPNControlListServiceStatus);
	TInt SetAPNControlListServiceStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetAPNControlListServiceStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAPNControlListServiceStatus* aAPNControlListServiceStatus);
	TInt GetAPNControlListServiceStatusCancel(const TTsyReqHandle aTsyReqHandle);	
	TInt NotifyAPNControlListServiceStatusChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAPNControlListServiceStatus* aAPNControlListServiceStatus);
	TInt NotifyAPNControlListServiceStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetAirTimeDuration(const TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime);
	TInt NotifyAirTimeDurationChange(const TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime);
	TInt NotifyAirTimeDurationChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt TerminateAllCalls(const TTsyReqHandle aTsyReqHandle);
	TInt TerminateAllCallsCancel(const TTsyReqHandle aTsyReqHandle);
    TInt TerminateActiveCalls(const TTsyReqHandle aTsyReqHandle);
    TInt TerminateActiveCallsCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySendNetworkServiceRequest(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNotifySendSSOperation* aOperation, TDes8* aRequestComplete);
	TInt NotifySendNetworkServiceRequestCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyAllSendNetworkServiceRequest(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNotifySendSSOperation* aOperation, TDes8* aRequestComplete);
	TInt NotifyAllSendNetworkServiceRequestCancel(const TTsyReqHandle aTsyReqHandle);
	TInt ClearBlacklist(const TTsyReqHandle aTsyReqHandle);
	TInt ClearBlacklistCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyDTMFEvent(const TTsyReqHandle aTsyReqHandle,RMobilePhone::TMobilePhoneDTMFEvent* aEvent);
	TInt NotifyDTMFEventCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetPersonalisationCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt GetPersonalisationCapsCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetPersonalisationStatus(const TTsyReqHandle aTsyReqHandle, TUint32* aPersSchemes);
	TInt GetPersonalisationStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetPersonalisationStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePersonalisation* aPersSchemes, TDes* aPersCode);
	TInt SetPersonalisationStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetCurrentNetworkName(const TTsyReqHandle aTsyReqHandle, TDes8* aNetworkName, TDes8* aLocInfo);
	TInt GetCurrentNetworkNameCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetPreferredNetworksPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt ProcessGetPreferredNetworksPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt GetPreferredNetworksPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetPreferredNetworksCancel(const TTsyReqHandle aTsyReqHandle);
	TInt ProcessStorePreferredNetworksListL(TTsyReqHandle aTsyReqHandle, TDes8* aBuffer);
	TInt StorePreferredNetworksList(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer);
	TInt StorePreferredNetworksListCancel(TTsyReqHandle aTsyReqHandle);
	TInt NotifyStorePreferredNetworksListChange(TTsyReqHandle aTsyReqHandle);
	TInt NotifyStorePreferredNetworksListChangeCancel(TTsyReqHandle aTsyReqHandle);
	TInt GetCipheringIndicatorStatus(TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobileCallCipheringIndicator& aIndicatorStatus);
	TInt GetCipheringIndicatorStatusCancel(TTsyReqHandle aTsyReqHandle);
	TInt NotifyCipheringIndicatorStatus(TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobileCallCipheringIndicator& aIndicatorStatus);
	TInt NotifyCipheringIndicatorStatusCancel(TTsyReqHandle aTsyReqHandle);
	TInt SetIdentityServiceStatus(TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneIdService aService, RMobilePhone::TMobilePhoneIdServiceSetting aSetting); 
	TInt SetIdentityServiceStatusCancel(TTsyReqHandle aTsyReqHandle);
	TInt NotifyIdentityServiceStatus(TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneIdService aService, RMobilePhone::TMobilePhoneIdServiceStatus* aStatus);
	TInt NotifyIdentityServiceStatusCancel(TTsyReqHandle aTsyReqHandle);
	TInt NotifyIdentitySuppressionRejected(TTsyReqHandle aTsyReqHandle);
	TInt NotifyIdentitySuppressionRejectedCancel(TTsyReqHandle aTsyReqHandle);
	TInt NotifyMulticallIndicatorChange(TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneMulticallIndicator* aMulticallIndicator);
	TInt NotifyMulticallIndicatorChangeCancel(TTsyReqHandle aTsyReqHandle);
	TInt GetAuthorizationInfoPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient,TInt* aBufSize);
	TInt ProcessGetAuthorizationInfoPhase1L(const TTsyReqHandle aTsyReqHandle,
											 RMobilePhone::TClientId* aId,
											 TInt* aBufSize);
	TInt GetAuthorizationInfoPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* ,TDes8* aBuffer);
	TInt GetAuthorizationInfoCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetAuthenticationData(const TTsyReqHandle aTsyReqHandle,TDes8* aAuthenticationData);
	TInt GetAuthenticationDataCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyImsAuthorizationInfoChanged(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyImsAuthorizationInfoChangedCancel(const TTsyReqHandle aTsyReqHandle);

	TInt EnumerateSmartCardApplications(const TTsyReqHandle aTsyReqHandle,TUint* aCount);
	TInt EnumerateSmartCardApplicationsCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetSmartCardApplicationInfo(const TTsyReqHandle aTsyReqHandle,TUint* aAppIndex, TDes8* aInfoBuffer);
	TInt GetSmartCardApplicationInfoCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySmartCardApplicationInfoChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TSmartCardApplicationEvent* aEvent, RMobilePhone::TAID* aAID);
	TInt NotifySmartCardApplicationInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetSmartCardApplicationStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID, RMobilePhone::TSmartCardApplicationAction aAction);
	TInt SetSmartCardApplicationStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetScFileInfo(const TTsyReqHandle aTsyReqHandle, const RMobilePhone::TScFilePath& aFilePathId, TDes8* aInfo);
	TInt GetScFileInfoCancel(const TTsyReqHandle aTsyReqHandle);	
	TInt ReadScFile(const TTsyReqHandle aTsyReqHandle, const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aReadBuffer);
	TInt ReadScFileCancel(const TTsyReqHandle aTsyReqHandle);
	TInt UpdateScFile(const TTsyReqHandle aTsyReqHandle, const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aWriteBuffer);
	TInt UpdateScFileCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetCurrentActiveUSimApplication(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID);
	TInt GetCurrentActiveUSimApplicationCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetSmartCardAuthenticationData(const TTsyReqHandle aTsyReqHandle, TDes8* aSmartCardAuthData, RMobilePhone::TAID* aAID);
	TInt GetSmartCardAuthenticationDataCancel(const TTsyReqHandle aTsyReqHandle);

	TInt CharToSeptNumL(const TChar& aChar) const;
	void ConvertTextToBinary(const TDesC& aSrc, TDes8& aDst);
	TInt SetEmergencyClient(const TTsyReqHandle aTsyReqHandle, TUint32* aEmergencyRequest);
	
	TInt GetAuthenticationListPhase1(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneAuthenticationIds::TAuthRequestData* aRequest,TInt* aBufSize);
	TInt ProcessGetGbaPhase1L(const TTsyReqHandle aTsyReqHandle,RMobilePhone::TClientId* aId,TInt* aBufSize);
	TInt ProcessGetMbmsPhase1L(const TTsyReqHandle aTsyReqHandle,RMobilePhone::TClientId* aId,TInt* aBufSize);
	TInt GetAuthenticationListPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId*,TDes8* aBuffer);
	TInt GetAuthenticationListCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifyAuthenticateDataChange(const TTsyReqHandle aTsyReqHandle, TDes8* aAuthInfo);
	TInt NotifyAuthenticateDataChangeCancel(TTsyReqHandle aTsyReqHandle);
	
	TInt GetAuthenticationParams(const TTsyReqHandle aTsyReqHandle,TDes8* aAuthInfo,TDes8* aParams);
	TInt GetAuthenticationParamsCancel(TTsyReqHandle aTsyReqHandle);
	
	TInt SetGbaBootstrapParams(const TTsyReqHandle aTsyReqHandle,RMobilePhone::TAID* aAID,TDes8* aParams);
	TInt SetGbaBootstrapParamsCancel(TTsyReqHandle aTsyReqHandle);
	
	TInt NotifyWlanDataChange(const TTsyReqHandle aTsyReqHandle, TDes8* aData);
	TInt NotifyWlanDataChangeCancel(TTsyReqHandle aTsyReqHandle);

	TInt NotifyPreferredWlanSIDListChange(TTsyReqHandle aTsyReqHandle);
	TInt NotifyPreferredWlanSIDListChangeCancel(TTsyReqHandle aTsyReqHandle);
	
	TInt GetWlanData(const TTsyReqHandle aTsyReqHandle,TDes8* aData);
	TInt GetWlanDataCancel(TTsyReqHandle aTsyReqHandle);

	TInt SetWlanData(const TTsyReqHandle aTsyReqHandle,TDes8* aData);
	TInt SetWlanDataCancel(TTsyReqHandle aTsyReqHandle);

	TInt GetPreferredWlanSIDsPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt ProcessGetPreferredWlanSIDsPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt GetPreferredWlanSIDsPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetPreferredWlanSIDsCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt StorePreferredWlanSIDList(const TTsyReqHandle aTsyReqHandle,TDes8* aBuffer);
	TInt StorePreferredWlanSIDListCancel(TTsyReqHandle aTsyReqHandle);
	
	TInt ProcessStorePreferredWlanSIDListL(TTsyReqHandle aTsyReqHandle,TDes8* aBuffer);
	
private:

	class CActiveListNode : public CBase
		{
		public:
			CActiveListNode(CActive *aActive, const TTsyReqHandle aTsyReqHandle);
			virtual ~CActiveListNode();
		public:
			CActive *iActive;
			const TTsyReqHandle iTsyReqHandle;
			TSglQueLink iNext;
		};


	TInt FindDelayedReq(const TTsyReqHandle aTsyReqHandle, CActiveListNode *& aNode);

private: // data
	TInt iNotifyModeChange;
	TInt iNotifyIccAccessCaps;
	TInt iNotifyBatteryInfoChange;
	TInt iNotifySignalStrengthChange;
	TInt iNotifyIndicatorChange;
	TInt iNotifyDTMFCapsChange;
	TInt iNotifyStopInDTMFString;
	TInt iNotifyCurrentNetworkChange;
	TInt iNotifyCellInfoChange;	
	TInt iNotifyNetworkRegistrationStatusChange;
	TInt iNotifyCallForwardingStatusChange;
	TInt iNotifyCallForwardingActive;
	TInt iNotifyNetworkSelectionSettingChange;
	TInt iNotifyNetworkTimeInfoChange;
	TInt iNotifyCallServiceCaps;
	TInt iNotifyCallBarringStatusChange;
	TInt iNotifyCallWaitingStatusChange;
	TInt iNotifyCCBSStatusChange;
	TInt iNotifyCCBSRecall;
	TInt iNotifyAlternatingCallCapsChange;
	TInt iNotifyAlternatingCallModeChange;
	TInt iNotifyALSLineChange;
	TInt iNotifyCostCapsChange;
	TInt iNotifyCostInfoChange;
	TInt iNotifySecurityCapsChange;
	TInt iNotifyLockInfoChange;
	TInt iNotifySecurityCodeInfoChange;
	TInt iNotifyOnSecurityEvent;
	TInt iNotifyMessageWaiting;
	TInt iNotifyIccMessageWaitingChange;
	TInt iNotifyFdnStatusChange;
	TInt iNotifyMulticallParamsChange;
	TInt iNotifyIncomingCallTypeChange;
	TInt iNotifyUUSSettingChange;
	TInt iNotifyDefaultPrivacyChange;
	TInt iNotifyMmCallPrefChange;
	TInt iNotifyNetworkSecurityChange;
	TInt iNotifyUSimAppsSelectionModeChange;
	TInt iNotifyUSimApplicationsInfoChange;
	TInt iNotifyMailBoxNumbersChange;
	TInt iNotifyMmsUpdate;
	TInt iNotifyMmsConfig;
	TInt iNotifyNetworkInvScanEvent;
	TInt iNotifyNetworkInvScanChange;
	TInt iNotifyAirTimeChangeChange;
	TInt iTerminateAllCalls;
	TInt iTerminateActiveCalls;
	TInt iNotifySendNetworkServiceRequest;
	TInt iClearBlacklist;
	TInt iNotifyDTMFEvent;
	TInt iNotifyCipheringIndicatorStatus;
	TInt iNotifyIdentityServiceStatus;
	TInt iNotifyIdentitySuppressionRejected;
	TInt iNotifyMulticallIndicatorChange;
	TInt iNotifyImsAuthorizationInfoChange;
	TInt iNotifySmartCardApplicationInfoChange;
	TInt iNotifyAuthenticationDataChange;
	TInt iNotifyWlanDataChange;
	
	TInt iGbaBootstrapAuthDataFail;
	TInt iGbaNafAuthDataFail;
	TInt iMbmsMskUpdateAuthDataFail;
	TInt iMbmsMtkGenerateAuthDataFail;
	TInt iMbmsMukDeleteAuthDataFail;
	TInt iMbmsMskDeleteAuthDataFail;

	TInt iPhoneTestCount;
	CArrayPtrFlat<CListReadAllAttempt>* iGetDetectedNetworks;
	CArrayPtrFlat<CListReadAllAttempt>* iGetCFStatus;
	CArrayPtrFlat<CListReadAllAttempt>* iGetCB;
	CArrayPtrFlat<CListReadAllAttempt>* iGetCWStatus;
	CArrayPtrFlat<CListReadAllAttempt>* iGetCcbsRequests;
	CArrayPtrFlat<CListReadAllAttempt>* iGetPreferredNetworks;
	CArrayPtrFlat<CListReadAllAttempt>* iGetAuthorizationData;
	CArrayPtrFlat<CListReadAllAttempt>* iGetWlanSIDsData;

	TSglQue<CActiveListNode> iActiveRequestObjects;
	CArrayPtrFlat<CListReadAllAttempt>*  iAuthReadAll;
	
public:
	RMobilePhone::CImsAuthorizationInfoV5* iAuthInfo; 
	};


//
//	CDelayedCompleter
//

class CDelayedCompleter : public CTimer
	{
public:
	static CDelayedCompleter *NewL(const TTsyReqHandle aTsyReqHandle, CPhoneDMmTsy *aPhoneObject,
		CTelObject *aTelObject);
	void RunL();

protected:
	CDelayedCompleter(const TTsyReqHandle aTsyReqHandle, CPhoneDMmTsy *aPhoneObject,
		CTelObject *aTelObject);
	void ConstructL();

protected:
	const TTsyReqHandle iTsyReqHandle;
	CPhoneDMmTsy *iPhoneObject;
	CTelObject *iTelObject;
	};


/**************************************************************************/
//
//	CSmsMessagingDMmTsy
//
/**************************************************************************/

class CSmsMessagingDMmTsy : public CSubSessionExtDummyBase
	{
public:
	static CSmsMessagingDMmTsy* NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	CSmsMessagingDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	void ConstructL();
	~CSmsMessagingDMmTsy();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
private:
	TInt GetCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps);

	TInt ReceiveMessage(const TTsyReqHandle aTsyReqHandle, TDes8* aMsgData, TDes8* aMsgAttributes);
	TInt ReceiveMessageCancel(const TTsyReqHandle aTsyReqHandle);

	TInt AckSmsStored(const TTsyReqHandle aTsyReqHandle, TDesC8* aMsgData, TBool *aFull);
	TInt AckSmsStoredCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NackSmsStored(const TTsyReqHandle aTsyReqHandle, TDesC8* aMsgData, TInt* aRpCause);
	TInt NackSmsStoredCancel(const TTsyReqHandle aTsyReqHandle);

	TInt ResumeSmsReception(const TTsyReqHandle aTsyReqHandle);

	TInt SendMessage(const TTsyReqHandle aTsyReqHandle, TDesC8* aMsgData, TDes8* aMsgAttributes);
	TInt SendMessageCancel(const TTsyReqHandle aTsyReqHandle);

	TInt SendMessageNoFdnCheck(const TTsyReqHandle aTsyReqHandle, TDesC8* aMsgData, TDes8* aMsgAttributes);
	TInt SendMessageNoFdnCheckCancel(const TTsyReqHandle aTsyReqHandle);

	TInt EnumerateMessageStores(const TTsyReqHandle aTsyReqHandle, TInt* aCount);
	TInt GetMessageStoreInfo(const TTsyReqHandle aTsyReqHandle, TInt *aIndex, TDes8* aInfo);
	TInt GetMessageStoreInfoCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetReceiveMode(const TTsyReqHandle aTsyReqHandle, RMobileSmsMessaging::TMobileSmsReceiveMode* aReceiveMode);
	TInt SetReceiveMode(const TTsyReqHandle aTsyReqHandle, RMobileSmsMessaging::TMobileSmsReceiveMode* aReceiveMode);
	TInt SetReceiveModeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyReceiveModeChange(const TTsyReqHandle aTsyReqHandle, RMobileSmsMessaging::TMobileSmsReceiveMode* aReceiveMode);
	TInt NotifyReceiveModeChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetMoSmsBearer(const TTsyReqHandle aTsyReqHandle, RMobileSmsMessaging::TMobileSmsBearer* aBearer);
	TInt SetMoSmsBearer(const TTsyReqHandle aTsyReqHandle, RMobileSmsMessaging::TMobileSmsBearer* aBearer);
	TInt SetMoSmsBearerCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyMoSmsBearerChange(const TTsyReqHandle aTsyReqHandle, RMobileSmsMessaging::TMobileSmsBearer* aBearer);
	TInt NotifyMoSmsBearerChangeCancel(const TTsyReqHandle aTsyReqHandle);

	// specific SMSP store functionality
	TInt ReadSmspListPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient,TInt* aBufSize);
	TInt ProcessReadSmspListPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TInt* aBufSize);
	TInt ReadSmspListPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* ,TDes8* aBuffer);
	TInt ReadSmspListCancel(const TTsyReqHandle aTsyReqHandle);
	TInt StoreSmspList(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer);
	TInt ProcessStoreSmspListL(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer);
	TInt StoreSmspListCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySmspListChange(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySmspListChangeCancel(const TTsyReqHandle aTsyReqHandle);

	static TInt ValidateSmspEntry(const RMobileSmsMessaging::TMobileSmspEntryV1& aEntry);
	static void InitSmspEntry(RMobileSmsMessaging::TMobileSmspEntryV1& aEntry);

private:
	TInt iReceiveMessage;
	TInt iNotifySmspListChange;
	TInt iNotifyReceiveModeChange;
	TInt iNotifyMoSmsBearerChange;

	TName iMessagingName;
	CPhoneDMmTsy *iPhone;
	CArrayPtrFlat<CListReadAllAttempt>*  iSmspReadAll;
	};

/**************************************************************************/
//
//	CBroadcastMessagingDMmTsy
//
/**************************************************************************/

class CBroadcastMessagingDMmTsy : public CSubSessionExtDummyBase
	{
public:
	static CBroadcastMessagingDMmTsy* NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	CBroadcastMessagingDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	void ConstructL();
	~CBroadcastMessagingDMmTsy();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
private:
	TInt GetCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps);
	TInt ReceiveMessage(const TTsyReqHandle aTsyReqHandle, TDes8* aMsg, TDes8* aMsgAtts);
	TInt ReceiveMessageCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetFilterSetting(const TTsyReqHandle aTsyReqHandle, RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter* aSetting);
	TInt SetFilterSetting(const TTsyReqHandle aTsyReqHandle, RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter* aSetting);
	TInt SetFilterSettingCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyFilterSettingChange(const TTsyReqHandle aTsyReqHandle, RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter* aSetting);
	TInt NotifyFilterSettingChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetLanguageFilter(const TTsyReqHandle aTsyReqHandle, TDes16* aLangFilter);
	TInt GetLanguageFilterCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetLanguageFilter(const TTsyReqHandle aTsyReqHandle, TDesC16* aLangFilter);
	TInt SetLanguageFilterCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyLanguageFilterChange(const TTsyReqHandle aTsyReqHandle, TDes16* aLangFilter);
	TInt NotifyLanguageFilterChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetBroadcastIdListPhase1(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneBroadcastIdList::TGetBroadcastIdRequest* aReqData, TInt* aBufSize);
	TInt ProcessGetBroadcastIdListPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt GetBroadcastIdListPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetBroadcastIdListCancel(const TTsyReqHandle aTsyReqHandle);

	void StoreBroadcastIdListL(const TTsyReqHandle aTsyReqHandle, TDes8* aList);
	TInt StoreBroadcastIdListCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyBroadcastIdListChange(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyBroadcastIdListChangeCancel(const TTsyReqHandle aTsyReqHandle);

private:
	TInt iNotifyFilterSettingChange;
	TInt iNotifyLanguageFilterChange;
	TInt iReceiveMessage;
	TInt iNotifyBroadcastIdListChange;

	TName iMessagingName;
	CPhoneDMmTsy *iPhone;
	CArrayPtrFlat<CListReadAllAttempt>*  iCbmiListRequests;
	};

/**************************************************************************/
//
//	CUssdMessagingDMmTsy
//
/**************************************************************************/

class CUssdMessagingDMmTsy : public CSubSessionExtDummyBase
	{
public:
	static CUssdMessagingDMmTsy* NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	CUssdMessagingDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	void ConstructL();
	~CUssdMessagingDMmTsy();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
private:
	TInt GetCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps);
	TInt ReceiveMessage(const TTsyReqHandle aTsyReqHandle, TDes8* aMsgData, TDes8* aMsgAttributes);
	TInt ReceiveMessageCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SendMessage(const TTsyReqHandle aTsyReqHandle, TDesC8* aMsgData, TDesC8* aMsgAttributes);
	TInt SendMessageCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SendMessageNoFdnCheck(const TTsyReqHandle aTsyReqHandle, TDesC8* aMsgData, TDesC8* aMsgAttributes);
	TInt SendMessageNoFdnCheckCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SendRelease(const TTsyReqHandle aTsyReqHandle, TDes8* aReturnResult);
	TInt SendReleaseCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyNetworkRelease(const TTsyReqHandle aTsyReqHandle, TDes8* aReturnResult);
	TInt NotifyNetworkReleaseCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyNetworkRelease(const TTsyReqHandle aTsyReqHandle, TDes8* aReturnResult, TDes8* aStatus);
private:
	TInt iReceiveMessage;
	TName iMessagingName;
	CPhoneDMmTsy *iPhone;
	TInt iSendRelease;
	TInt iNetworkRelease;
	};

/**************************************************************************/
//
//	CSmsStoreDMmTsy
//
/**************************************************************************/

class CSmsStoreDMmTsy : public CSubSessionExtDummyBase
	{
public:
	static CSmsStoreDMmTsy* NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	CSmsStoreDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	void ConstructL();
	~CSmsStoreDMmTsy();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
private:
	// general phone storage functionality
	TInt GetInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhoneStore::TMobilePhoneStoreInfoV1* aInfo);
	TInt GetInfoCancel(const TTsyReqHandle aTsyReqHandle);

	TInt Read(const TTsyReqHandle aTsyReqHandle, TDes8* aEntry);
	TInt ReadCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Write(const TTsyReqHandle aTsyReqHandle, TDesC8* aEntry);
	TInt WriteCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Delete(const TTsyReqHandle aTsyReqHandle, TInt* aIndex);
	TInt DeleteCancel(const TTsyReqHandle aTsyReqHandle);
	TInt DeleteAll(const TTsyReqHandle aTsyReqHandle);
	TInt DeleteAllCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyStoreEvent(const TTsyReqHandle aTsyReqHandle, RMobilePhoneStore::TMobileStoreEvent *aEvent, TInt* aIndex);
	TInt NotifyStoreEventCancel(const TTsyReqHandle aTsyReqHandle);

	// specific SMS store functionality
	TInt ReadAllPhase1(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneSmsList::TBatchRequestData* aRequest,TInt* aBufSize);
	TInt ProcessReadAllGsmPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TInt* aBufSize);
	TInt ProcessReadAllCdmaPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TInt* aBufSize);
	TInt ReadAllPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* ,TDes8* aBuffer);
	TInt ReadAllCancel(const TTsyReqHandle aTsyReqHandle);

private:
	TInt iNotifyStoreEvent;

	TName iSmsStoreName;
	CArrayPtrFlat<CListReadAllAttempt>*  iSmsReadAll;
	CPhoneDMmTsy *iPhone;
	};

/**************************************************************************/
//
//	CNamStoreDMmTsy
//
/**************************************************************************/

class CNamStoreDMmTsy : public CSubSessionExtDummyBase
	{
public:
	static CNamStoreDMmTsy* NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	CNamStoreDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	void ConstructL();
	~CNamStoreDMmTsy();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
private:
	// general phone storage functionality

	TInt GetInfo(const TTsyReqHandle aTsyReqHandle, RMobileNamStore::TMobileNamStoreInfoV1* aInfo);
	TInt GetInfoCancel(const TTsyReqHandle aTsyReqHandle);

	static TInt ValidateNamEntry(RMobileNamStore::TMobileNamEntryV1& aEntry);
	static void InitNamEntry(RMobileNamStore::TMobileNamEntryV1& aEntry);
	static TInt ValidateNamEntry(RMobileNamStore::TMobileNamEntryV4& aEntry);
	static void InitNamEntry(RMobileNamStore::TMobileNamEntryV4& aEntry);

	TInt Read(const TTsyReqHandle aTsyReqHandle, TDes8* aEntry);
	TInt ReadCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Write(const TTsyReqHandle aTsyReqHandle, TDesC8* aEntry);
	TInt WriteCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Delete(const TTsyReqHandle aTsyReqHandle, TInt* aIndex);
	TInt DeleteCancel(const TTsyReqHandle aTsyReqHandle);
	TInt DeleteAll(const TTsyReqHandle aTsyReqHandle);
	TInt DeleteAllCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyStoreEvent(const TTsyReqHandle aTsyReqHandle, RMobilePhoneStore::TMobileStoreEvent *aEvent, TInt* aIndex);
	TInt NotifyStoreEventCancel(const TTsyReqHandle aTsyReqHandle);

	TInt ReadAllPhase1(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneNamList::TNamRequestData* aRequestData,TInt* aBufSize);
	TInt ReadAllPhase1V4(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneNamList::TNamRequestData* aRequestData,TInt* aBufSize);
	TInt ProcessReadAllPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TInt* aBufSize);
	TInt ProcessReadAllPhase1V4L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TInt* aBufSize);
	TInt ReadAllPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* ,TDes8* aBuffer);
	TInt ReadAllPhase2V4(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* ,TDes8* aBuffer);
	TInt ReadAllCancel(const TTsyReqHandle aTsyReqHandle);

	TInt SetActiveNam(const TTsyReqHandle aTsyReqHandle, TInt *aNamId);
	TInt SetActiveNamCancel(const TTsyReqHandle aTsyReqHandle);

	TInt StoreAll(const TTsyReqHandle aTsyReqHandle, TInt *aNamId, TDes8* aBuffer);
	TInt StoreAllV4(const TTsyReqHandle aTsyReqHandle, TInt *aNamId, TDes8* aBuffer);
	TInt ProcessStoreAllL(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer);
	TInt ProcessStoreAllV4L(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer);
	TInt StoreAllCancel(const TTsyReqHandle aTsyReqHandle);


private:
	TInt iNotifyStoreEvent;

	TName iNamStoreName;
	CArrayPtrFlat<CListReadAllAttempt>*  iNamReadAll;
	CPhoneDMmTsy *iPhone;
	};

/**************************************************************************/
//
//	CONStoreDMmTsy
//
/**************************************************************************/

class CONStoreDMmTsy : public CSubSessionExtDummyBase
	{
public:
	static CONStoreDMmTsy* NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	CONStoreDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	void ConstructL();
	~CONStoreDMmTsy();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
private:
	// general phone storage functionality

	TInt GetInfo(const TTsyReqHandle aTsyReqHandle, RMobileONStore::TMobileONStoreInfoV1* aInfo);
	TInt GetInfoCancel(const TTsyReqHandle aTsyReqHandle);

	static TInt ValidateONEntry(RMobileONStore::TMobileONEntryV1& aEntry);
	static void InitONEntry(RMobileONStore::TMobileONEntryV1& aEntry);

	TInt Read(const TTsyReqHandle aTsyReqHandle, TDes8* aEntry);
	TInt ReadCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Write(const TTsyReqHandle aTsyReqHandle, TDesC8* aEntry);
	TInt WriteCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Delete(const TTsyReqHandle aTsyReqHandle, TInt* aIndex);
	TInt DeleteCancel(const TTsyReqHandle aTsyReqHandle);
	TInt DeleteAll(const TTsyReqHandle aTsyReqHandle);
	TInt DeleteAllCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyStoreEvent(const TTsyReqHandle aTsyReqHandle, RMobilePhoneStore::TMobileStoreEvent *aEvent, TInt* aIndex);
	TInt NotifyStoreEventCancel(const TTsyReqHandle aTsyReqHandle);

	TInt ReadAllPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId,TInt* aBufSize);
	TInt ProcessReadAllPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TInt* aBufSize);
	TInt ReadAllPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId,TDes8* aBuffer);
	TInt ReadAllCancel(const TTsyReqHandle aTsyReqHandle);

	TInt StoreAll(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer);
	TInt ProcessStoreAllL(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer);
	TInt StoreAllCancel(const TTsyReqHandle aTsyReqHandle);


private:
	TInt iNotifyStoreEvent;

	TName iONStoreName;
	CArrayPtrFlat<CListReadAllAttempt>*  iONReadAll;
	CPhoneDMmTsy *iPhone;
	};


/**************************************************************************/
//
//	CENStoreDMmTsy
//
/**************************************************************************/

class CENStoreDMmTsy : public CSubSessionExtDummyBase
	{
public:
	static CENStoreDMmTsy* NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	CENStoreDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	void ConstructL();
	~CENStoreDMmTsy();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
private:
	// general phone storage functionality

	TInt GetInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhoneStore::TMobilePhoneStoreInfoV1* aInfo);
	TInt GetInfoCancel(const TTsyReqHandle aTsyReqHandle);

	static TInt ValidateENEntry(RMobileENStore::TMobileENEntryV1& aEntry);
	static void InitENEntry(RMobileENStore::TMobileENEntryV1& aEntry);

	TInt Read(const TTsyReqHandle aTsyReqHandle, TDes8* aEntry);
	TInt ReadCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Write(const TTsyReqHandle aTsyReqHandle, TDesC8* aEntry);
	TInt WriteCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Delete(const TTsyReqHandle aTsyReqHandle, TInt* aIndex);
	TInt DeleteCancel(const TTsyReqHandle aTsyReqHandle);
	TInt DeleteAll(const TTsyReqHandle aTsyReqHandle);
	TInt DeleteAllCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyStoreEvent(const TTsyReqHandle aTsyReqHandle, RMobilePhoneStore::TMobileStoreEvent *aEvent, TInt* aIndex);
	TInt NotifyStoreEventCancel(const TTsyReqHandle aTsyReqHandle);
																			
	TInt ReadAllPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId,TInt* aBufSize);
	TInt ProcessReadAllPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TInt* aBufSize);
	TInt ReadAllPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId,TDes8* aBuffer);
	TInt ReadAllCancel(const TTsyReqHandle aTsyReqHandle);

private:
	TInt iNotifyStoreEvent;

	TName iENStoreName;
	CArrayPtrFlat<CListReadAllAttempt>*  iENReadAll;
	CPhoneDMmTsy *iPhone;
	};

/**************************************************************************/
//
//	CMobilePhoneBookStoreDMmTsy
//
/**************************************************************************/

class CMobilePhoneBookStoreDMmTsy : public CSubSessionExtDummyBase
	{
public:
	static CMobilePhoneBookStoreDMmTsy* NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName, const TDesC& aType =KEtelUSimPhoneBookType);
	CMobilePhoneBookStoreDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName, const TDesC& aType =KEtelUSimPhoneBookType);
	void ConstructL();
	~CMobilePhoneBookStoreDMmTsy();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
private:
	// general phone storage functionality

	TInt GetInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aInfo);
	TInt GetInfoCancel(const TTsyReqHandle aTsyReqHandle);

	TInt Read(const TTsyReqHandle aTsyReqHandle, RMobilePhoneBookStore::TPBIndexAndNumEntries* aIndexAndEntries, TDes8* aPBData);
	TInt ReadCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Write(const TTsyReqHandle aTsyReqHandle, TDesC8* aPBData, TInt* aIndex);
	TInt WriteCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Delete(const TTsyReqHandle aTsyReqHandle, TInt* aIndex);
	TInt DeleteCancel(const TTsyReqHandle aTsyReqHandle);
																				
private:
	TName iPhoneBookStoreName;
	TName iPhoneBookData;
	TName iIccType;
	CPhoneDMmTsy *iPhone;
	};

/**************************************************************************/
//
//	CMobileConferenceCallDMmTsy
//
/**************************************************************************/

class CMobileConferenceCallDMmTsy : public CSubSessionExtDummyBase
	{
public:
	static CMobileConferenceCallDMmTsy* NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	CMobileConferenceCallDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	void ConstructL();
	~CMobileConferenceCallDMmTsy();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
private:
	// general phone storage functionality

	TInt GetCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt CreateConference(const TTsyReqHandle aTsyReqHandle);
	TInt CreateConferenceCancel(const TTsyReqHandle aTsyReqHandle);
	TInt AddCall(const TTsyReqHandle aTsyReqHandle, TDes* aCallName);
	TInt AddCallCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Swap(const TTsyReqHandle aTsyReqHandle);
	TInt SwapCancel(const TTsyReqHandle aTsyReqHandle);
	TInt HangUp(const TTsyReqHandle aTsyReqHandle);
	TInt HangUpCancel(const TTsyReqHandle aTsyReqHandle);
	TInt EnumerateCalls(const TTsyReqHandle aTsyReqHandle, TUint* aCount);
	TInt GetMobileCallInfo(const TTsyReqHandle aTsyReqHandle, TInt *aIndex, TDes8* aCallInfo);
	TInt GetConferenceStatus(const TTsyReqHandle aTsyReqHandle, RMobileConferenceCall::TMobileConferenceStatus* aStatus);
	TInt NotifyConferenceStatusChange(const TTsyReqHandle aTsyReqHandle, RMobileConferenceCall::TMobileConferenceStatus* aStatus);
	TInt NotifyConferenceStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyConferenceEvent(const TTsyReqHandle aTsyReqHandle, RMobileConferenceCall::TMobileConferenceEvent* aEvent, TDes* aCallName);
	TInt NotifyConferenceEventCancel(const TTsyReqHandle aTsyReqHandle);


private:
	CPhoneDMmTsy *iPhone;
	TName iConferenceName;
	TInt iNotifyCapsChange;
	TInt iNotifyConferenceStatusChange;
	TInt iNotifyConferenceEvent;
	};

/**************************************************************************/
//
//	CPhoneFactoryDMmTsy
//
/**************************************************************************/

class CPhoneFactoryDMmTsy: public CPhoneFactoryDummyBase
	{
public:
	static CPhoneFactoryDMmTsy* NewL();
	~CPhoneFactoryDMmTsy();
	virtual CPhoneBase* NewPhoneL(const TDesC& aName);
	virtual TBool IsSupported(const TInt aMixin);
private:
	CPhoneFactoryDMmTsy();
	};


/**************************************************************************/
//
//	CSmartCardEapDMmTsy
//
/**************************************************************************/

class CSmartCardEapDMmTsy : public CSubSessionExtDummyBase
	{
class CThreadTerminationNotifier;
friend class CThreadTerminationNotifier;

public:
	static CSmartCardEapDMmTsy* NewL(CPhoneDMmTsy *aPhone, RMobilePhone::TAID& aAID, RMobileSmartCardEap::TEapType& aEapType, CPhoneFactoryDummyBase* aFac);
	CSmartCardEapDMmTsy(CPhoneDMmTsy *aPhone, RMobilePhone::TAID& aAID, RMobileSmartCardEap::TEapType& aEapType, CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CSmartCardEapDMmTsy();

public:
	virtual void Init();
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);

private:
	static CSmartCardEapDMmTsy* This(TAny* aPtr);

	TInt DMmInitialiseEapMethod(const TTsyReqHandle aTsyReqHandle, TThreadId* aThreadId);
	TInt DMmInitialiseEapMethodCancel(const TTsyReqHandle aTsyReqHandle);
	TInt DMmGetUserIdentity(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapUserIdType* aEapIdType, TDes8* aUserId);
	TInt DMmGetUserIdentityCancel(const TTsyReqHandle aTsyReqHandle);
	TInt DMmGetAuthenticationStatus(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapAuthStatus* aAuthStatus);
	TInt DMmGetAuthenticationStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt DMmGetEapKey(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapKeyTag* aEapKeyTag, TDes8* aKey);
	TInt DMmGetEapKeyCancel(const TTsyReqHandle aTsyReqHandle);
	TInt DMmSetAuthenticateDataForPhase1(const TTsyReqHandle aTsyReqHandle, TDes8* aEapAuthData, TInt* aPhase1Size);
	TInt DMmGetAuthenticateDataForPhase2(const TTsyReqHandle aTsyReqHandle, TDes8* aEapAuthData, TDes8* aPhase2Resp);
	TInt DMmSmartCardEapAuthenticationCancel(const TTsyReqHandle aTsyReqHandle);
	TInt DMmReleaseEapMethod(const TTsyReqHandle aTsyReqHandle);
	TInt DMmGetEapMethodAccessStatus(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapMethodAccessStatus* aEapState);
	TInt DMmNotifyEapMethodAccessStatusChange(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapMethodAccessStatus* aEapState);
	TInt DMmNotifyEapMethodAccessStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	void DMmCompleteNotifyEapMethodAccessStatusChange();
	virtual RHandleBase* GlobalKernelObjectHandle();
	void ClientHasTerminated(TInt aExitReason);

private:
	class CActiveListNode : public CBase
		{
	public:
		CActiveListNode(CActive *aActive, const TTsyReqHandle aTsyReqHandle);
		virtual ~CActiveListNode();
	public:
		CActive *iActive;
		const TTsyReqHandle iTsyReqHandle;
		TSglQueLink iNext;
		};

	class TNotifyData
		{
	public:
		TBool iNotifyPending;
		TTsyReqHandle iNotifyHandle;
		TAny* iNotifyData;
		};

	class CThreadTerminationNotifier : public CActive
		{
	public:
		static CThreadTerminationNotifier* NewL(CSmartCardEapDMmTsy* aSubSess, const TThreadId& aId);
		void Start();
		~CThreadTerminationNotifier();

	private:
		CThreadTerminationNotifier(CSmartCardEapDMmTsy* aPhone);
		void ConstructL(const TThreadId& aId);
		void DoCancel();
		void RunL();

	private:
		CSmartCardEapDMmTsy* iSubSess;
		RThread iCliThread;
		};

private: // data
	TSglQue<CActiveListNode> iActiveRequestObjects;
	CPhoneDMmTsy *iPhone;
	TPtrC8 iRespPtrC;
	TBool iSSInitialised;
	RMobilePhone::TAID iAID;
	RMobileSmartCardEap::TEapType iEapType;
	RSemaphore iSemaphr;
	RMobileSmartCardEap::TEapMethodAccessStatus iAccessStatus;
	TNotifyData iEapAccessNotifyData;
	CThreadTerminationNotifier* iCliTerminationNotifier;
	};


/**************************************************************************/
//
//	CMobileLocationServicesDMmTsy
//
/**************************************************************************/

class CMobileLocationServicesDMmTsy : public CSubSessionExtDummyBase
	{
public:
	static CMobileLocationServicesDMmTsy* NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	CMobileLocationServicesDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName);
	void ConstructL();
	~CMobileLocationServicesDMmTsy();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
    virtual void Init(); 
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	TInt iCount;
private:
    TInt SendMtlrResponse(const TTsyReqHandle aTsyReqHandle,RMobileLocationServices::TMtlrResponse* aMtlrResponse);
    TInt SendMtlrResponse(const TTsyReqHandle aTsyReqHandle,RMobileLocationServices::TMtlrError* aMtlrError);
    TInt SendMtlrResponse(const TTsyReqHandle aTsyReqHandle,RMobileLocationServices::TMtlrReject* aMtlrReject);
	TInt NotifyMtlr(const TTsyReqHandle aTsyReqHandle,TDes8* aNotifyMtlr);
	TInt SendMolr(const TTsyReqHandle aTsyReqHandle,TDes8* aStartMolr,TDes8* aMolrReturnResult);
	TInt NotifyMtlrCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SendMolrCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SendMtlrResponseCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyMeasurementControl(const TTsyReqHandle aTsyReqHandle,TDes8* aMeasurementControl);
	TInt SendMeasurementReport(const TTsyReqHandle aTsyReqHandle,TDes8* aMeasurementReport);
	TInt SendMeasurementReport(const TTsyReqHandle aTsyReqHandle,RMobileLocationServices::TMeasurementControlFailure* aMeasurementControlFailure);
	TInt NotifyMeasurementControlCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SendMeasurementReportCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetLcsDomain(const TTsyReqHandle aTsyReqHandle,RMobileLocationServices::TDomain* aDomain);
	TInt NotifyResetUePositioningInformation(const TTsyReqHandle aTsyReqHandle, RMobileLocationServices::TUePosTechnology* aUePosTechnology);
	TInt NotifyResetUePositioningInformationCancel(const TTsyReqHandle aTsyReqHandle);
private:
	CPhoneDMmTsy *iPhone;
	TName        iLocationServices;
	};

#endif // __MMTSY_H
