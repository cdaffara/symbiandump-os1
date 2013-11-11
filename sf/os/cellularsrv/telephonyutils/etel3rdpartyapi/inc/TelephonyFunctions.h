/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
*
*/


#ifndef TELEPHONYFUNCTIONS_H__
#define TELEPHONYFUNCTIONS_H__

/** @file
 *  Definition of CTelephonyFunctions class. Used by CTelephony class.
 */

#include <e32base.h>
#include <etel.h>
#include <etelmm.h>
#include <e32property.h>

#include <sacls.h>

#include "Etel3rdParty.h"

#include <commsdattypesv1_1.h>

const TInt KTelephonyDllUidValue = 0x1000AA4F;	///< Unique to this Dll
const TUid KTelephonyDllUid={KTelephonyDllUidValue};	///< Unique to this Dll
_LIT(KTelephonyCategory, "Telephony");

class CGetPhoneIdAct;
class CGetSubscriberIdAct;
class CGetIndicatorAct;
class CGetBatteryInfoAct;
class CGetSignalStrengthAct;
class CGetIccLockInfoAct;
class CSendDTMFTonesAct;
class CFlightModeChangeAct;
class CNotifyIndicatorAct;
class CNotifyBatteryInfoAct;
class CNotifySignalStrengthAct;
class CNotifyIccLockInfoAct;
class CGetSupplementaryServiceStatusAct;
class CGetIdentityServiceStatusAct;
class CNotifyLineStatusAct;
class CDialNewCallAct;
class CGetCallDynamicCapsAct;
class CHoldAct;
class CResumeAct;
class CSwapAct;
class CHangupAct;
class CAnswerIncomingCallAct;
class CNotifyCallStatusAct;
class CNotifyRemotePartyInfoAct;
class CGetNetworkRegistrationStatusAct;
class CGetCurrentNetworkInfoAct;
class CGetCurrentNetworkNameAct;
class CGetOperatorNameAct;
class CNotifyNetworkRegistrationStatusAct;
class CNotifyCurrentNetworkInfoAct;	 
class CNotifyIncomingCallAct;

class CTelephonyFunctions : public CBase
/**
Class which uses Etel.DLL on behalf of CTelephony objects.
*/
	{

public:	
	enum TState	///< State of the CTelephonyFunctions object
		{
		EIdle,
		EDialling,	
		EAnswering,	
		EEstablishingDataChannel, 
		EDataChannelInUse,	
		ECleaningUp	
		};
	
   
	enum TOperations	///< Operations of the CTelephonyFunctions object
		{
		EGetPhoneId,                      // 0
		EGetSubscriberId,
		EGetFlightMode,
		EGetIndicator,
		EGetBatteryInfo,
		EGetSignalStrength,               // 5
		EGetIccLockInfo,
		ESendDTMFTones,
		EDialNewCall,
		EGetCallDynamicCaps,
		EGetCallStatus,                   // 10
		EHold,
		EResume,
		ESwap,
		EHangup,
		EAnswerIncomingCall,              // 15
		ENotifyIncomingCall,
		EGetNetworkRegistrationStatus,
		EGetCurrentNetworkInfo,
		EGetCurrentNetworkName,
		EGetOperatorName,                 // 20
		EGetCFSupplServicesStatus,
		EGetCBSupplServicesStatus,
		EGetCWSupplServicesStatus,
		EGetIdentityServiceStatus,

		EFlightModeChange,                // 25
		ENotifyIndicator,
		ENotifyBatteryInfo,
		ENotifySignalStrength,
		ENotifyPin1LockInfo,
		ENotifyPin2LockInfo,			  // 30
		ENotifyNetworkRegistrationStatus,
		ENotifyCurrentNetworkInfo,
		ENotifyOwnedCall1Status,
		ENotifyOwnedCall2Status,
		ENotifyOwnedCall1RemoteInfo,	  // 35
		ENotifyOwnedCall2RemoteInfo,
		ENotifyVoiceLineStatus,

		EMaxNumberOperations
		};
		
	enum TCallPoolOperation
		{
		EUnset,
		EAnswer,
		EDial		
		};
		
	#define KErrTelephonyOutOfSequence (-1)	
	
	#define KNoMultimodeTsy (-1)	
	
	// Constructors
	static CTelephonyFunctions* NewLC();
	static CTelephonyFunctions* NewL();
		
	// Destructor - virtual and class not intended
	// for derivation, so not exported
	~CTelephonyFunctions();
	
	// Phone Functionality
	TInt GetPhoneIdL(TRequestStatus& aReqStatus, TDes8& aId);
	TInt GetSubscriberIdL(TRequestStatus& aReqStatus, TDes8& aId);
	TInt GetFlightMode(TRequestStatus& aReqStatus, TDes8& aMode);
	TInt GetIndicatorL(TRequestStatus& aReqStatus, TDes8& aIndicator);
	TInt GetBatteryInfoL(TRequestStatus& aReqStatus, TDes8& aBatteryInfo);
	TInt GetSignalStrengthL(TRequestStatus& aReqStatus, TDes8& aSignalStrength);
	TInt GetLockInfoL(TRequestStatus& aReqStatus, const CTelephony::TIccLock& aLock, TDes8& aLockInfo);
	TInt SendDTMFTonesL(TRequestStatus& aReqStatus, const TDesC& aTones);
	TInt GetLineStatus(const CTelephony::TPhoneLine& aLine, TDes8& aStatus);
	TInt GetCallInfoL(TDes8& aCallSelect, TDes8& aCallInfo, TDes8& aRemoteInfo);
	TInt DialNewCallL(TRequestStatus& aStatus, TDes8& aCallParams, 
		const CTelephony::TTelNumber& aTelNumber, CTelephony::TCallId& aCallId, const CTelephony::TPhoneLine aLine = CTelephony::EVoiceLine); 
	TInt GetCallDynamicCaps(const CTelephony::TCallId& aCallId, TDes8& aCallCaps); 
	TInt GetCallStatusL(const CTelephony::TCallId& aCallId, TDes8& aCallStatus); 
	TInt HoldL(TRequestStatus& aReqStatus, const CTelephony::TCallId& aCallId);
	TInt ResumeL(TRequestStatus& aReqStatus, const CTelephony::TCallId& aCallId);
	TInt SwapL(TRequestStatus& aReqStatus, const CTelephony::TCallId& aCallId1, 
		const CTelephony::TCallId& aCallId2);
	TInt HangupL(TRequestStatus& aReqStatus, const CTelephony::TCallId& aCallId);
	TInt AnswerIncomingCallL(TRequestStatus& aReqStatus, CTelephony::TCallId& aCallId, 
		const CTelephony::TPhoneLine aLine=CTelephony::EVoiceLine);
	// Network Functionality
	TInt GetNetworkRegistrationStatusL(TRequestStatus& aReqStatus, TDes8& aStatus);
	TInt GetCurrentNetworkInfoL(TRequestStatus& aReqStatus, TDes8& aNetworkInfo);
	TInt GetCurrentNetworkNameL(TRequestStatus& aReqStatus, TDes8& aNetworkName);
	TInt GetOperatorNameL(TRequestStatus& aReqStatus, TDes8& aOperator);
 	// (Basic) Supplementary Services Functionality
	TInt GetCFSupplementaryServiceStatusL(TRequestStatus& aRequestStatus, const CTelephony::TCallForwardingCondition aCondition, TDes8& aDes, const CTelephony::TServiceGroup aServiceGroup);
	TInt GetCBSupplementaryServiceStatusL(TRequestStatus& aRequestStatus, const CTelephony::TCallBarringCondition aCondition, TDes8& aDes, const CTelephony::TServiceGroup aServiceGroup);
	TInt GetCWSupplementaryServiceStatusL(TRequestStatus& aRequestStatus, TDes8& aDes, const CTelephony::TServiceGroup aServiceGroup);
 	TInt GetIdentityServiceStatusL(TRequestStatus& aReqStatus,
	 	const CTelephony::TIdentityService& aService, TDes8& aStatus);
  	// Cancel Request Functionality
	TInt CancelAsync(CTelephony::TCancellationRequest aCancel);
 	// Notification Functionality
	TInt NotifyChangeL(TRequestStatus& aReqStatus, const CTelephony::TNotificationEvent& aEvent, TDes8& aDes);
	void ResetPendingNotifyRequest(CTelephony::TNotificationEvent aEvent);
	void CompleteRequest(TOperations aOPeration, TInt aCompleteCode);
 	
	// Deprecated functions

	void CleanUp();		// Close/Free outstanding "Open" objects etc.
	inline TRequestStatus** RequestStatus();
	inline RMobileCall* Call();
	inline RMobileCall* Call(const CTelephony::TCallId aCallId);
	inline RMobileLine* Line(const CTelephony::TPhoneLine aLine);
	inline RMobilePhone* Phone();
	inline RProperty* PhonePowerProperty();
	TInt GetISVCall(CTelephony::TCallId& aCallId);
	void CloseAndReset(const CTelephony::TCallId aCallId);

	static void CopyTelAddress(const RMobilePhone::TMobileAddress& aFromAddress, CTelephony::TTelAddress& aToAddress);
	static void GetCallStatus(const RMobileCall::TMobileCallStatus aMMCallStatus, CTelephony::TCallStatus& aTelCallStatus);

	inline void	SetAsyncRequestOutstandingFalse();
	inline TRequestStatus** GetRequestStatus(CTelephonyFunctions::TOperations aOp);
	inline TBool IsRequestPending(CTelephonyFunctions::TOperations aOp);
	inline void	SetRequestPending(CTelephonyFunctions::TOperations aOp, TBool aBool);
	
private:	
	CTelephonyFunctions();	
	void ConstructL();	
	void GetCommDbTSYnameL(TDes& aTableName);	// get TSY name from CommDb
	void InitialisePhoneL();
	TBool IsFlightModeOn();
	TInt NotifyFlightModeL(TRequestStatus& aRequestStatus, TDes8& aDes);
	TInt NotifyOwnedCall1RemotePartyInfoL(TRequestStatus& aRequestStatus, TDes8& aDes);
	TInt NotifyOwnedCall2RemotePartyInfoL(TRequestStatus& aRequestStatus, TDes8& aDes); 
	TInt NotifyVoiceLineStatusL(TRequestStatus& aRequestStatus, TDes8& aDes); 
	TInt NotifyOwnedCall1StatusL(TRequestStatus& aRequestStatus,  TDes8& aDes); 
	TInt NotifyOwnedCall2StatusL(TRequestStatus& aRequestStatus, TDes8& aDes); 
	TInt NotifyIndicatorL(TRequestStatus& aRequestStatus, TDes8& aDes); 
	TInt NotifyPinLockInfoL(TRequestStatus& aRequestStatus, const CTelephony::TNotificationEvent& aEvent, TDes8& aDes); 
	TInt NotifyBatteryInfoL(TRequestStatus& aRequestStatus, TDes8& aDes); 
	TInt NotifySignalStrengthL(TRequestStatus& aRequestStatus, TDes8& aDes); 
	TInt NotifyNetworkRegistrationStatusL(TRequestStatus& aRequestStatus, TDes8& aDes);
	TInt NotifyCurrentNetworkInfoL(TRequestStatus& aRequestStatus, TDes8& aDes);
	
private:
	RTelServer				iTelServer;
	RMobilePhone			iPhone;	
	RMobileLine				iLineData;	
	RMobileLine				iLineFax;	
	RMobileLine				iLineVoice;	
	TName					iLineVoiceName;
	TName					iCallName;		 
	RProperty				iPhonePowerProperty;
	//TSAPhoneStatus			iFlightMode;
	TInt					iTsyVersion;
	
	// Active Objects
	CGetPhoneIdAct*			iGetPhoneId;
	CGetSubscriberIdAct*	iGetSubscriberId;
	CGetIndicatorAct*		iGetIndicator;
	CGetBatteryInfoAct*		iGetBatteryInfo;
	CGetSignalStrengthAct*	iGetSignalStrength;
	CGetIccLockInfoAct*		iGetIccLockInfo;
	CSendDTMFTonesAct*		iSendDTMFTones;	
	CDialNewCallAct*		iDialNewCall;
	CHoldAct*				iHold;
	CResumeAct*				iResume;
	CSwapAct*				iSwap;
	CHangupAct*				iHangup;
	CAnswerIncomingCallAct*	iAnswerIncomingCall;
	
	CFlightModeChangeAct*			iFlightModeChange;
	CNotifyIndicatorAct* 			iNotifyIndicator;
	CNotifyBatteryInfoAct*			iNotifyBatteryInfo;
	CNotifySignalStrengthAct*		iNotifySignalStrength;
	CNotifyIccLockInfoAct*			iNotifyIccLockInfo;
	CNotifyNetworkRegistrationStatusAct* 	iNotifyNetworkRegStatus;
	CNotifyCurrentNetworkInfoAct*	iNotifyCurrentNetworkInfo;
	CNotifyCallStatusAct*			iNotifyOwnedCall1Status;
	CNotifyCallStatusAct*			iNotifyOwnedCall2Status;
	CNotifyRemotePartyInfoAct* 		iNotifyOwnedCall1RemoteInfo;
	CNotifyRemotePartyInfoAct* 		iNotifyOwnedCall2RemoteInfo;
	
	// Internal Active Objects
	CNotifyLineStatusAct* 	iInternalNotifyVoiceLineStatus;
	CNotifyIncomingCallAct*	iInternalNotifyIncomingCall;

	// Network Functionality
	CGetNetworkRegistrationStatusAct*	iGetNetworkRegistrationStatus;
	CGetCurrentNetworkInfoAct*			iGetCurrentNetworkInfo;
	CGetCurrentNetworkNameAct*			iGetCurrentNetworkName;
	CGetOperatorNameAct*				iGetOperatorName;

 	// (Basic) Supplementary Services Functionality
	CGetSupplementaryServiceStatusAct*	iGetCFSupplementaryServiceStatus;
	CGetSupplementaryServiceStatusAct*	iGetCWSupplementaryServiceStatus;
	CGetSupplementaryServiceStatusAct*	iGetCBSupplementaryServiceStatus;
 	CGetIdentityServiceStatusAct*		iGetIdentityServiceStatus;
 
	TRequestStatus*			iRequestStatus;
	RMobileCall				iCall;	
	RMobileCall				iIncomingCall;	

	TState			iState;	
	TBool			iTelServerIsOpen;	///< so destructor can Close() if necc.
	TBool			iPhoneIsOpen;	///< so destructor/CleanUp() can Close() if necc.
	TBool			iLineIsVoiceOpen;	///< so destructor/CleanUp() can Close() if necc.
	TBool			iLineIsDataOpen;	///< so destructor/CleanUp() can Close() if necc.
	TBool			iLineIsFaxOpen;	///< so destructor/CleanUp() can Close() if necc.
	TBool			iAsyncRequestOutstanding;	///< ETrue if async call in progress.
	TBool			iCallIsOpen;	///< so destructor/CleanUp() can Close() if necc.
	
	RPointerArray<TRequestStatus> iReqStatusTable;
	RArray<TBool> iReqPendingTable;		
	RArray<RMobileCall>	iCallPool;	
	RArray<TCallPoolOperation>	iCallPoolStatus;	
	}; // class CTelephonyFunctions
	
class CAsyncRequestBaseAct : public CActive
	{
	
public:		
	~CAsyncRequestBaseAct();
	CAsyncRequestBaseAct();
protected:	
	void RunL();	// from CActive
	virtual void Complete() = 0;
	};

/**
Interface implemented by classes which want to be notified when an
event completes. These objects should register themselves with the 
relevant active object that handles the event in question using 
the RegisterObserver() function. 
The AO will call EventCompleted() when it completes. 
DeregisterObserver() can be called when the object no longer 
wishes to be notified.

@see MIncomingCallNameSubject
*/ 
class MEventObserver
	{
public:
	virtual void EventCompleted() = 0;
	};
	
/**
Interface implemented by the CNotifyIncomingCallAct AO
to allow other objects register themselves to be notified of 
when an incoming call arrives, obtain the call name and reset
it when there is no longer an incoming call.

Two AOs currently make use of the methods in this interface:
CNotifyLineStatusAct resets the call name when the line
is no longer ringing and CAnswerIncomingCallAct retrieves the 
call name when there is an incoming call so that the call can be 
opened.
*/
class MIncomingCallNameSubject
	{
public:
 	virtual void RegisterObserver(MEventObserver* aObserver) = 0;
	virtual void DeregisterObserver(MEventObserver* aObserver) = 0;
	virtual void ResetCallName() = 0;
	virtual TInt CallName(TName&) const = 0;
	};

#include "TelephonyFunctions.inl"	

#endif	// TELEPHONYFUNCTIONS_H__
