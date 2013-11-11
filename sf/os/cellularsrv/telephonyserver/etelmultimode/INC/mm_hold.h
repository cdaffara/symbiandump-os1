// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Multimode ETel API v1.0
// Header file for ptr holder classes, used to store client data
// 
//

/**
 @file
*/

#ifndef _MM_HOLD_H_
#define _MM_HOLD_H_

#include <etelmm.h>


class CMmPtrHolder : public CBase
/**
A base class for all ptr holder classes

The ptr holder classes contain the TPtr8's used by asynchronous client-side functions.
The API code also uses them to ensure BC by keeping size of R-classes constant.

@publishedPartner
@released
*/
	{
public:
//	static CMmPtrHolder* NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);
	virtual ~CMmPtrHolder();
public:
	template <typename T> inline TPtr8& Set(TInt aSlot,T& aObject)
		{
		TPtr8& ptr=Ptr(aSlot);
		ptr.Set(REINTERPRET_CAST(TText8*,(&aObject)),sizeof(T),sizeof(T));
		return ptr;
		};

	template <typename T> inline TPtrC8& SetC(TInt aSlot, const T& aObject)
		{
		TPtrC8& ptr=PtrC(aSlot);
		ptr.Set(REINTERPRET_CAST(const TText8*,(&aObject)),sizeof(T));
		return ptr;
		};

protected:
	virtual void ConstructL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);	
	CMmPtrHolder();
private:
	TPtr8& Ptr(TInt aIndex);
	TPtrC8& PtrC(TInt aIndex);
protected:
	RArray<TPtr8> iPtrArray;
	RArray<TPtrC8> iPtrCArray;
	};



class CMobilePhonePtrHolder : public CMmPtrHolder
/**
Holds the TPtr8 arrays for all RMobilePhone requests

This class is internal.

@publishedPartner
@released
*/

	{
public:

	// The ptr holder slot numbers used by RMobilePhone asynchronous requests

	enum TPhonePtrHolderSlots
		{
		ESlot1NotifyIccAccessCapsChange =0,
		ESlot1GetServiceTable,
		ESlot1GetBatteryInfo,
		ESlot1NotifyBatteryInfoChange,
		ESlot1GetSignalStrength,
		ESlot2GetSignalStrength,
		ESlot1NotifySignalStrengthChange,
		ESlot2NotifySignalStrengthChange,
		ESlot1GetIndicator,
		ESlot1NotifyIndicatorChange,
		ESlot1GetPhoneId,
		ESlot1GetSubscriberId,
		ESlot1NotifyDTMFCapsChange,
		ESlot1NotifyModeChange,
		ESlot1GetCurrentNetwork,
		ESlot1NotifyCurrentNetworkChange,
		ESlot1GetNetworkRegistrationStatus,
		ESlot1NotifyNetworkRegistrationStatusChange,
		ESlot1NotifyNetworkSelectionSettingChange,
		ESlot1NotifyNITZInfoChange,
		ESlot1NotifyDefaultPrivacyChange,
		ESlot1NotifyCallServiceCapsChange,
		ESlot1GetFeatureCode,
		ESlot1NotifyCallForwardingStatusChange,
		ESlot1PhoneNotifyCallForwardingActive,
		ESlot2PhoneNotifyCallForwardingActive,
		ESlot2GetIdentityServiceStatus,
		ESlot1NotifyCallBarringStatusChange,
		ESlot1GetCCBSStatus,
		ESlot2GetCCBSStatus,
		ESlot1NotifyCCBSStatusChange,
		ESlot1NotifyAlternatingCallCapsChange,
		ESlot1NotifyAlternatingCallModeChange,
		ESlot2NotifyAlternatingCallModeChange,
		ESlot1NotifyALSLineChange,
		ESlot1NotifyCostCapsChange,
		ESlot1NotifySecurityCapsChange,
		ESlot1NotifyLockInfoChange,
		ESlot1NotifySecurityEvent,
		ESlot1NotifyMessageWaiting,
		ESlot2NotifyMessageWaiting,
		ESlot1NotifyFdnStatusChange,
		ESlot1GetIncomingCallType,
		ESlot1NotifyIncomingCallTypeChange,
		ESlot1GetUUSSetting,
		ESlot1NotifyUUSSettingChange,
		ESlot1NotifyMultimediaCallPreferenceChange,
		ESlot1GetNetworkSecurityLevel,
		ESlot1NotifyNetworkSecurityLevelChange,
		ESlot1NotifyUSimApplicationsInfoChange,
		ESlot2NotifyUSimApplicationsInfoChange,
		ESlot1NotifyUSimAppsSelectionModeChange,
		ESlot1EnumerateUSimApplications,
		ESlot2EnumerateUSimApplications,
		ESlot1NotifyCCBSRecall,
		ESlot1GetFdnStatus,
		ESlot1GetHomeNetworkSearchPeriod,
		ESlot1GetLastUsedAccessTechnology,
		ESlot1NotifyNetworkInvScanChange,
		ESlot1NotifyNetworkInvScanEvent,
		ESlot1EnumerateAPNEntries,
		ESlot1NotifyAirTimeDurationChange,
		ESlot1NotifyDTMFEvent,
		ESlot1GetPersonalisationStatus,
		ESlot1GetCipheringIndicatorStatus,
		ESlot1NotifyCipheringIndicatorStatus,
		ESlot1EnumerateSmartCardApplications,
		ESlot1GetCurrentActiveUSimApplication,
		ESlot1GetIccIdentity,
		ESlot1NotifySecurityCodeInfoChange,
		ESlot1SmartCardAuthenticate,
		ESlot1GetAPNControlListServiceStatus,
		ESlot1NotifyAPNControlListServiceStatusChange,
		EMaxNumberPhonePtrSlots
		};

	enum TPhonePtrCHolderSlots
		{
		ESlot1SetNetworkSelectionSetting = 0,
		ESlot1SetDefaultPrivacy,
		ESlot1ProgramFeatureCode,
		ESlot1SetCallForwardingStatus,
		ESlot2SetCallForwardingStatus,
		ESlot1GetIdentityServiceStatus,
		ESlot1SetCallBarringStatus,
		ESlot2SetCallBarringStatus,
		ESlot1SetCallBarringPassword,
		ESlot1SetCallWaitingStatus,
		ESlot2SetCallWaitingStatus,
		ESlot1SetAlternatingCallMode,
		ESlot2SetAlternatingCallMode,
		ESlot1SetALSLine,
		ESlot1ClearCostMeter,
		ESlot1SetMaxCostMeter,
		ESlot1GetLockInfo,
		ESlot1SetLockSetting,
		ESlot2SetLockSetting,
		ESlot1ChangeSecurityCode,
		ESlot2ChangeSecurityCode,
		ESlot1VerifySecurityCode,
		ESlot2VerifySecurityCode,
		ESlot1SetFdnSetting,
		ESlot1SelectNetwork,
		ESlot2SelectNetwork,
		ESlot1SetMulticallParams,
		ESlot1SetIncomingCallType,
		ESlot1SetUUSSetting,
		ESlot1SetUSimApplicationStatus,
		ESlot2SetUSimApplicationStatus,
		ESlot1GetUSimApplicationInfo,
		ESlot1SetMultimediaCallPreference,
		ESlot1GetPhoneStoreInfo,
		ESlot1AcceptCCBSRecall,		
		ESlot1DeactivateCCBS,	
		ESlot1SetService,
		ESlot1GetMmsConfig,
		ESlot1NotifyMmsConfig,
		ESlot1GetCompMethodName,
		ESlot1GetAPNnameIndex,
		ESlot1DeleteAPNNameIndex,
		ESlot1SetAPNControlListServiceStatus,
		ESlot1SendSSOperation,
		ESlot1SetPersonalisationStatus,
		ESlot1SetIdentityServiceStatus,
		ESlot2SetIdentityServiceStatus,
		ESlot1NotifyIdentityServiceStatus,
		ESlot2NotifyIdentityServiceStatus,
		ESlot1NotifyMulticallIndicatorChange,
		ESlot1GetSmartCardApplicationInfo,
		ESlot1NotifySmartCardApplicationInfoChange,
		ESlot2NotifySmartCardApplicationInfoChange,
		ESlot1SetSmartCardApplicationStatus,
		ESlot2SetSmartCardApplicationStatus,
		ESlot1GetScFileInfo,
		ESlot1ReadScFile,
		ESlot1UpdateScFile,
		ESlot1GetSecurityCodeInfo,
		ESlot1SetGbaBootstrapParams,
		EMaxNumberPhonePtrCSlots
		};

public:
	static CMobilePhonePtrHolder* NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);
	~CMobilePhonePtrHolder();
protected:
	CMobilePhonePtrHolder();
public:
	// Storage for parameters from client which were passed by value.
	RMobilePhone::TMobilePhoneServiceTable iServiceTable;
	RMobilePhone::TMobilePhonePrivacy iPrivacySetting;
	RMobilePhone::TMobilePhoneCFCondition iSetCFCondition;
	RMobilePhone::TNetworkServiceAndAction iProgramFeatureCode;
	RMobilePhone::TNetworkServiceAndAction iGetFeatureCode;
	RMobilePhone::TIdServiceAndLocation iGetIdentityServiceStatus;
	RMobilePhone::TMobilePhoneCBCondition iSetCBStatusCondition;
	RMobilePhone::TMobileInfoLocation iGetCCBSStatusLocation;
	RMobilePhone::TMobileService iSetCallWaitingStatusGroup;
	RMobilePhone::TMobilePhoneServiceAction iSetCallWaitingStatusAction;
	RMobilePhone::TMobilePhoneAlternatingCallMode iSetAlternatingCallModeMode;
	RMobilePhone::TMobileService iSetAlternatingCallModeService;
	RMobilePhone::TMobilePhoneALSLine iSetALSLine;
	TUint iSetMaxCostMeterUnits;
	RMobilePhone::TMobilePhoneCostMeters iClearCostMeter;
	RMobilePhone::TMobilePhoneLock iLock;
	RMobilePhone::TMobilePhoneLock iSetLockSettingLock;
	RMobilePhone::TMobilePhoneLockSetting iSetLockSettingSetting;
	RMobilePhone::TMobilePhoneSecurityCode iVerifySecurityCodeType;
	RMobilePhone::TCodeAndUnblockCode iVerifySecurityCodeData;
	RMobilePhone::TMobilePhoneSecurityCode iChangeSecurityCodeType;
	RMobilePhone::TMobilePhoneFdnSetting iSetFdnSetting;
	TBool iIsManual;
	TInt  iUserMaxBearers;
	RMobilePhone::TMobilePhoneIncomingCallType iCallType;
	RMobilePhone::TMobilePhoneUUSSetting iSetUUSSetting;

	RMobilePhone::TMobilePhoneMultimediaSettings iMmSettings;
	RMobilePhone::TUSimSelectionMode iUSimSelectionMode;
	RMobilePhone::TUSimAppAction iUSimAppAction;
	TInt iUSimAppIndex;
	RMobilePhone::TPhoneStoreNameAndIccType iPhoneStoreNameAndIccType;
	RMobilePhone::TAID iAID;
	TInt iDeactivateCCBSIndex;
	TInt iAcceptCCBSRecallIndex;
	TInt iBarringService;
	RMobilePhone::TMmsConnParams iGetMmsConfig;
	RMobilePhone::TMmsConnParams iNotifyMmsConfig;
	TUint8	iCompMethodIdentifier;
	TUint32 iGetAPNnameIndex;
	TUint32 iDeleteAPNNameIndex;
	RMobilePhone::TAPNControlListServiceStatus iSetAPNControlListServiceStatus;
	RMobilePhone::TMobilePhoneNotifySendSSOperation iSendSSOperation;
	RMobilePhone::TMobilePhonePersonalisation iSetPersonalisationScheme;
	CBufBase* iStoreNetworkBuf;
	TPtr8 iStoreNetworkPtr;
	RMobilePhone::TMobilePhoneIdService iSetIdService;
	RMobilePhone::TMobilePhoneIdServiceSetting iSetIdServiceSetting;
	RMobilePhone::TMobilePhoneIdService iNotifyIdService;
	TInt iScAppIndex;
	RMobilePhone::TSmartCardApplicationAction iScAppAction;
	RMobilePhone::TMobilePhoneSecurityCode iGetSecurityCodeInfoType;
	RMobilePhone::TAID iScAuthAID;
	RMobilePhone::TAID iAuthAID;
	CBufBase* iStoreWlanSIDBuf;
	TPtr8 iStoreWlanSIDPtr;
	};	



class CMobileLinePtrHolder : public CMmPtrHolder
/**
Holds the TPtr8 arrays for all RMobileLine requests.

@publishedPartner
@released
*/
	{
public:
	// The ptr holder slot numbers used by RMobileLine asynchronous requests

	enum TLinePtrHolderSlots
		{
		ESlot1LineNotifyMobileLineStatusChange=0,
		EMaxNumberLinePtrSlots
		};

	enum TLinePtrCHolderSlots
		{
		EMaxNumberLinePtrCSlots = 0
		};

public:
	static CMobileLinePtrHolder* NewL(TInt aSizeOfPtrArray, TInt aSizeOfPtrCArray);
public:
	};	



class CMobileCallPtrHolder : public CMmPtrHolder
/**
Holds the TPtr8 arrays for all RMobileCall requests.

@publishedPartner
@released
*/
	{
public:
		// The ptr holder slot numbers used by RMobileCall asynchronous requests

	enum TCallPtrHolderSlots
		{
		ESlot1NotifyMobileCallStatusChange = 0,
		ESlot1NotifyCallEvent,
		ESlot1NotifyPrivacyConfirmation,
		ESlot1NotifyTrafficChannelConfirmation,
		ESlot1NotifyUUSCapsChange,
		ESlot1ReceiveUUI,
		ESlot1ActivateCcbs,
		ESlot1AnswerMultimediaCallAsVoice,
		EMaxNumberCallPtrSlots
		};

	enum TCallPtrCHolderSlots
		{
		ESlot1Deflect = 0,
		ESlot2Deflect,
		ESlot1SetDynamicHscsdParams,
		ESlot2SetDynamicHscsdParams,
		ESlot1GetMobileDataCallRLPRange,
		ESlot1SendUUI,
		ESlot2SendUUI,
		ESlot1HangupWithUUI,
		ESlot1AnswerWithUUI,
		ESlot1DialNoFdnCheck,
		EMaxNumberCallPtrCSlots
		};

public:
	static CMobileCallPtrHolder* NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);
public:
	RMobileCall::TMobileCallAiur iAiur;
	TInt iRxTimeslots;
	TInt iRLPVersion;
	TBool iMoreUUI;
	RMobileCall::TMobileCallDeflect iDeflectType;
	TName iFallbackCallName;
	};	



class CMobileConferenceCallPtrHolder : public CMmPtrHolder
/**
Holds the TPtr8 arrays for all RMobileConferenceCall requests.

This class is internal.

@publishedPartner
@released
*/
	{
public:

	// The ptr holder slot numbers used by RMobileConferenceCall asynchronous requests

	enum TConferenceCallPtrHolderSlots
		{
		ESlot1NotifyConferenceCallCapsChange = 0,
		ESlot1NotifyConferenceStatusChange,
		ESlot1NotifyConferenceEvent,
		EMaxNumberConferenceCallPtrSlots	
		};

	enum TConferenceCallPtrCHolderSlots
		{
		EMaxNumberConferenceCallPtrCSlots
		};

public:
	static CMobileConferenceCallPtrHolder* NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);	
	};	


class CSmsMessagingPtrHolder : public CMmPtrHolder
/**
Holds the TPtr8 arrays for all RMobileSmsMessaging requests.

This class is internal.

@publishedPartner
@released
*/

	{
public:
	// The ptr holder slot numbers used by RMobileSmsMessaging asynchronous requests

	enum TSmsPtrHolderSlots
		{
		ESlot1NotifyReceiveModeChange = 0,
		ESlot1NotifyMoSmsBearerChange,
		EMaxNumberSmsPtrSlots
		};

	enum TSmsPtrCHolderSlots
		{
		ESlot1AckSmsStored = 0,
		ESlot1NackSmsStored,
		ESlot1GetMessageStoreInfo,
		ESlot1SetReceiveMode,
		ESlot1SetMoSmsBearer,
		EMaxNumberSmsPtrCSlots
		};

public:
	static CSmsMessagingPtrHolder* NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);
	~CSmsMessagingPtrHolder();
protected:
	CSmsMessagingPtrHolder();
public:
	TBool iAckSmsStoredFull;
	TInt iNackSmsStoredCause;
	TInt iGetMessageStoreInfoIndex;
	RMobileSmsMessaging::TMobileSmsReceiveMode iReceiveMode;
	RMobileSmsMessaging::TMobileSmsBearer iSmsBearer;
	CBufBase* iSmspBuf;
	TPtr8 iSmspPtr;
	};	



class CCbsMessagingPtrHolder : public CMmPtrHolder
/**
Holds the TPtr8 arrays for all RMobileBroadcastMessaging requests.

@publishedPartner
@released
*/
	{
public:

	// The ptr holder slot numbers used by RMobileBroadcastMessaging asynchronous requests

	enum TBroadcastPtrHolderSlots
		{
		ESlot1NotifyFilterSettingChange=0,
		ESlot1StoreCbmiList,
		EMaxNumberBroadcastPtrSlots
		};

	enum TBroadcastPtrCHolderSlots
		{
		ESlot1SetFilterSetting=0,
		ESlot1StoreBroadcastIdListL,
		EMaxNumberBroadcastPtrCSlots	
		};

public:
	static CCbsMessagingPtrHolder* NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);
	~CCbsMessagingPtrHolder();
protected:
	CCbsMessagingPtrHolder();
public:
	RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter iSetFilterSetting;
	RMobileBroadcastMessaging::TMobileBroadcastIdType iIdType;
	CBufBase* iBroadcastIdBuf;
	TPtr8 iBroadcastIdPtr;
	};	

class CUssdMessagingPtrHolder : public CMmPtrHolder
/**
@publishedPartner
@released
*/
	{
public:

	// The ptr holder slot numbers used by RMobileUssdMessaging asynchronous requests

	enum TUssdPtrHolderSlots
		{
		EMaxNumberUssdPtrSlots=0,
		};

	enum TUssdPtrCHolderSlots
		{
		EMaxNumberUssdPtrCSlots=0
		};

public:
	~CUssdMessagingPtrHolder();
protected:
	CUssdMessagingPtrHolder();
public:
	};	


class CMobilePhoneStorePtrHolder : public CMmPtrHolder
/** 
A base class PtrHolder used by all classes which inherit from RMobilePhoneStore. 

Holds the TPtr8 arrays for all RMobilePhoneStore requests.

Each class derived from RMobilePhoneStore can create its own
PtrHolder class derived from CMobilePhoneStorePtrHolder.  RMobilePhoneStore derived
classes construct the appropriate PtrHolder class which is passed down to 
RMobilePhoneStore via the RMobilePhoneStore::BaseConstruct method.  Both methods of 
RMobilePhoneStore and a derived class must share the same PtrHolder.  It follows that
they must cooperate to ensure that methods in the base and derived classes do not
use the same slots.  The derived classes should use the slots that are equal to or
greater than EMaxNumberPhoneStorePtrSlots & EMaxNumberPhoneStorePtrCSlots.


@publishedPartner
@released
*/
	{
public:
	
	// The ptr holder slot numbers used by RMobilePhoneStore asynchronous requests

/** This member is internal. */
	enum TPhoneStorePtrHolderSlots
		{
	/** This member is internal. */
		ESlot1NotifyStoreEvent = 0,
	/** This member is internal. */
		ESlot2NotifyStoreEvent,
	/** This member is internal. */
		EMaxNumberPhoneStorePtrSlots
		};

/** This member is internal. */
	enum TPhoneStorePtrCHolderSlots
		{
	/** This member is internal. */
		ESlot1Delete = 0,
	/** This member is internal. */
		EMaxNumberPhoneStorePtrCSlots
		};

public:
	static CMobilePhoneStorePtrHolder* NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);
public:
	TInt iDeleteIndex;
	};	



class CSmsStorePtrHolder : public CMobilePhoneStorePtrHolder
/**
Holds the TPtr8 arrays for all RMobileSmsStore requests.

This class is internal.

@publishedPartner
@released
*/
	{
public:

	// The ptr holder slot numbers used by RMobileSmsStore asynchronous requests

	enum TSmsStorePtrHolderSlots
		{
		EMaxNumberSmsStorePtrSlots = EMaxNumberPhoneStorePtrSlots
		};

	enum TSmsStorePtrCHolderSlots
		{
		ESlot1GetStatusReport = EMaxNumberPhoneStorePtrCSlots,
		EMaxNumberSmsStorePtrCSlots
		};

public:
	static CSmsStorePtrHolder* NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);
public:
	TInt iGetStatusReportIndex;
	};	



class CNamStorePtrHolder : public CMobilePhoneStorePtrHolder
/**
Holds the TPtr8 arrays for all RMobileNamStore requests.

This class is internal.

@publishedPartner
@released
@deprecated 9.5
*/
	{
public:

	// The ptr holder slot numbers used by RMobileNamStore asynchronous requests

	enum TNamStorePtrHolderSlots
		{
		EMaxNumberNamStorePtrSlots = EMaxNumberPhoneStorePtrSlots
		};

	enum TNamStorePtrCHolderSlots
		{
		ESlot1SetActiveNam = EMaxNumberPhoneStorePtrCSlots,
		ESlot1NamListStoreAll,
		ESlot1NamListStoreAllV4,
		EMaxNumberNamStorePtrCSlots
		};

public:
	static CNamStorePtrHolder* NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);
	~CNamStorePtrHolder();
protected:
	CNamStorePtrHolder();
public:
	CBufBase* iNamBuf;
	CBufBase* iNamBufV4;
	TPtr8 iNamPtr;
	TPtr8 iNamPtrV4;
	TInt iSetActiveNamNamId;
	TInt iStoreAllNamId;
	TInt iStoreAllV4NamId;
	};	



class CONStorePtrHolder : public CMobilePhoneStorePtrHolder
/**
Holds the TPtr8 arrays for all RMobileONStore requests.

This class is internal.

@publishedPartner
@released
*/
	{
public:

	// The ptr holder slot numbers used by RMobileOnStore asynchronous requests

	enum TOnStorePtrHolderSlots
		{
		EMaxNumberONStorePtrSlots = EMaxNumberPhoneStorePtrSlots
		};

	enum TOnStorePtrCHolderSlots
		{
		EMaxNumberONStorePtrCSlots = EMaxNumberPhoneStorePtrCSlots
		};

public:
	static CONStorePtrHolder* NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);
	~CONStorePtrHolder();
protected:
	CONStorePtrHolder();
public:
	CBufBase* iONBuf;
	TPtr8 iONPtr;
	};	



class CPhoneBookStorePtrHolder : public CMobilePhoneStorePtrHolder
/**
Holds the TPtr8 arrays for all RMobilePhonebookStore requests.

This class is internal.

@publishedPartner
@released
*/
	{
public:

	// The ptr holder slot numbers used by RMobilePhonebookStore asynchronous requests

	enum TPhoneBookStorePtrHolderSlots
		{
		ESlot1PhoneBookStoreWrite = EMaxNumberPhoneStorePtrSlots,
		EMaxNumberPhoneBookStorePtrSlots
		};

	enum TPhoneBookStorePtrCHolderSlots
		{
		ESlot1PhoneBookStoreRead = EMaxNumberPhoneStorePtrCSlots,
		EMaxNumberPhoneBookStorePtrCSlots 
		};

public:
	static CPhoneBookStorePtrHolder* NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);
public:
	RMobilePhoneBookStore::TPBIndexAndNumEntries iReadPhoneBookEntry;
	};	

/**
Holds the TPtr8 arrays for all RMobileSmartCardEap requests

This class is internal.

@internalComponent
@prototype
*/
class CMobileSmartCardEapPtrHolder : public CMmPtrHolder
	{
public:
	enum TSmartCardEapPtrHolderSlots
		{
		ESlot1GetAuthStatus,            // 0
		ESlot1NotifyEapMethodAccess,    // 1
		EMaxNumberSmartCardEapPtrSlots  // 2
		};

	enum TSmartCardEapPtrCHolderSlots
		{
		ESlot1InitialiseEapMethod,      // 0
		ESlot1GetUserId,                // 1
		ESlot1GetEapKey,                // 2
		EMaxNumberSmartCardEapPtrCSlots // 3
		};

public:
	static CMobileSmartCardEapPtrHolder* NewL(TInt aSizeOfPtrArray, TInt aSizeOfPtrCArray);
	~CMobileSmartCardEapPtrHolder();

protected:
	CMobileSmartCardEapPtrHolder();

public:
	TThreadId iOwnerThreadId;
	RMobileSmartCardEap::TEapUserIdType iEapUserIdType;
	RMobileSmartCardEap::TEapKeyTag iReqEapKeyTag;
	TPtr8 iReqAuthData;
	};



class CMobileLocationServicesPtrHolder : public CMmPtrHolder
/**
Holds the TPtr8 arrays for all RMobileLocationServices requests.

This class is internal.

@internalAll
*/
	{
public:

	// The ptr holder slot numbers used by RMobileLocationServices asynchronous requests

	enum TLocationServicesPtrHolderSlots
		{
		ESlot1LcsNotifyUeResetPositioningInformation,
		EMaxNumberLocationServicesPtrSlots	
		};

	enum TLocationServicesPtrCHolderSlots
		{
		ESlot1LcsSendMtlrResponse,
		ESlot1LcsSendMtlrError,
		ESlot1LcsMtlrReject,
		ESlot1LcsSendMeasurementControlFailure,
		EMaxNumberLocationServicesPtrCSlots
		};

public:
	static CMobileLocationServicesPtrHolder* NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);	
	};	

#endif // _MM_HOLD_H_
