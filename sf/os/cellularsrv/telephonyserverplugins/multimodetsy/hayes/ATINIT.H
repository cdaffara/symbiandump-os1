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
// AT Initialisation
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __ATINIT_H__
#define __ATINIT_H__

#include <cdblen.h>
#include "ATBASE.H"
#include "mSMSMEM.H"

class CATInit : public CATBase
/**
@internalTechnology
*/
	{
public:
	// Object instantiation method
	static CATInit* NewL(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals);
	
	// C++ destructor
	~CATInit();

	// Public API methods
	void StartInit(CATCommands* aPendingCommand, TTsyReqHandle aTsyReqHandle, TAny* aParams);
	void StopInit(TTsyReqHandle aTsyReqHandle);
	void SpecialStart(TTsyReqHandle aTsyReqHandle=NULL);
	TBool JustInitialised(TInt& aError) const;
	void Complete(TInt aError);
	CATSmsMemoryStorage* iEnumerate;
	TBool CheckActiveReqHandle(const TTsyReqHandle aTsyReqHandle);

private:
	// C++ constructor and second stage construct
	CATInit(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals);
	void ConstructL();

	// Methods inherited from CATBase
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	
	// Class Utility Methods
	void InitAttempt();
	void ParseWaitTimeL();
	void ValidateATHExpectStringL();
	void RetryOrFail(TBool aCommsErrorOccurred = EFalse);
	void ParsePhoneBearerServiceTypeL();
	void ParsePhoneCapsL();
	void ParseAnErroredPhoneCaps();
	void ParsePhoneBookStorageL();
	TInt GetIdentityResponse();
	void DoGetIdentityResponseL();
	TBool CheckResponse(TPtrC8 aResult);
	void CheckCMGFResponseL();
	void ParseOperatorL();
	void ParseCurrentMsClassL();
	void ParseMaxMsClassL();
	void ParseCGQREQResponseL();
	void ParseCGATTResponseL();
	void ParseCNMIResponseL();
	void ParseCBSTResponseL();
	void SetSpeedCaps(TInt aVal,RMobileCall::TMobileCallDataCapsV1& aCaps);
	void SetNameCaps(TInt aVal,RMobileCall::TMobileCallDataCapsV1& aCaps);
	void SetCECaps(TInt aVal,RMobileCall::TMobileCallDataCapsV1& aCaps);

private:
	TUint iAttemptCnt;			  // For the entire initialise sequence
	TUint iRetryCounter;		  // For "AT" which is sent after the user-defined modem
								  // init string
	CATCommands* iPendingCommand; // Which command to set going after initialisation
	TAny* iParams;
	CCommChatString* iExpectString;
	CCommChatString* iConnectExpectString;
	CCommChatString* iNoCarrierExpectString;
	TBool iInitJustCompleted;
	TInt iInitError;
	TBool iMoreInfoFlag;
	TBool iCGMIFlag;
	TBool iCGMMFlag;
	TBool iCGMRFlag;
	TBool iCGSNFlag;
			
	enum {
		EShortTimeOutFix,
		EWaitForDSR,
		EWaitForATHangUpWriteComplete,
		EWaitForATHangUpOK,
		EWaitForModemInitWrite,
		EWaitForModemInitOK,
		EWaitForATCheckWrite,
		EATWaitForATCheckOK,
		EWaitForATGetPhoneCapsWrite,
		EWaitForATGetPhoneCapsOK,
		EWaitForATStandardInitWrite,
		EWaitForATStandardInitOK,
		EWaitForATUnsolicitedFixWrite,
		EWaitForATUnsolicitedFixOK,
		EWaitForATCarrierWaitTimeWrite,
		EWaitForATCarrierWaitTimeOK,
		EWaitForATAutoAnswerTimeWrite,
		EWaitForATAutoAnswerTimeOK,
		EATWaitForSendingCMGFRequestWrite,
		EATWaitForSendingCMGFRequestComplete,
		EATWaitForSendingCMGFConfigureWrite,
		EATWaitForCMGFResponseOKComplete,
		EATWaitForSendingCNMIConfigRequest,
		EATWaitForSendingCNMIRequestComplete,
		EATWaitForSendingCNMIConfigureWrite,
		EATWaitForSendingCNMIConfigureComplete,
		EATWaitForTestPhoneBookStorageWriteComplete,
		EATWaitForTestPhoneBookStorageRequestComplete,
		EATWaitForSendingEnumRequest,
		EATWaitForSendingEnumRequestComplete,
		EATWaitForGetPrefMemWriteComplete,
		EATWaitForGetPrefMemReadComplete,
		EATWaitForSendingCGMIRequest,
		EATWaitForSendingCGMIRequestComplete,
		EATWaitForSendingCGMMRequest,
		EATWaitForSendingCGMMRequestComplete,
		EATWaitForSendingCGMRRequest,
		EATWaitForSendingCGMRRequestComplete,
		EATWaitForSendingCGSNRequest,
		EATWaitForSendingCGSNRequestComplete,
		EATGetSubscriberIdWriteComplete,
		EATGetSubscriberIdReadComplete,
		EATWaitForSetExtendedCallIndicationWrite,
		EATWaitForSetExtendedCallIndicationOK,
		EATWaitForClearUnsolicitedNetworkRegistrationWrite,
		EATWaitForClearUnsolicitedNetworkRegistrationOK,
		EATWaitForSetUnsolicitedNetworkRegistrationWrite,
		EATWaitForSetUnsolicitedNetworkRegistrationOK,
		EATWaitForReadUnsolicitedNetworkRegistrationWrite,
		EATWaitForReadUnsolicitedNetworkRegistrationOK,
		EATGetCurrentOperatorWriteComplete,
		EATGetCurrentOperatorReadComplete,
		EATGetBatteryInfoWriteComplete,
		EATGetBatteryInfoReadComplete,
		EATGetSignalInfoWriteComplete,
		EATGetSignalInfoReadComplete,
		EATGetBearerServiceType,    //  DJN  these should be changed ???
		EATSetBearerServiceType,     // DJN
		EATGetCurrentMSCLASSWriteComplete,
		EATGetCurrentMSCLASSReadComplete,
		EATGetMaxMSCLASSWriteComplete,
		EATGetMaxMSCLASSReadComplete,
		EATGetCurrentCGQREQTWriteComplete,
		EATGetCurrentCGQREQTReadComplete,
		EATGetCurrentGprsAttachStateWriteComplete,
		EATGetCurrentGprsAttachStateReadComplete,
		EATWaitForSendingCNMIRequest,
		EATWaitForSendingCNMIComplete,
		EATWaitForSendingCBSTRequest,
		EATWaitForSendingCBSTComplete,
		EATWaitForSendingCSCARequest,
		EATWaitForSendingCSCAComplete,
		EATWaitForSetUnsolicitedGPRSNetworkRegistrationWrite,
		EATWaitForSetUnsolicitedGPRSNetworkRegistrationOK,
		EATWaitForSendingECAMRequest,
		EATWaitForSendingECAMComplete
		} iState;
	};

class CATHangUpData;
class CCallHayes;
class CATQuickInit : public CATBase
/**
@internalTechnology
*/
	{
public:
	static CATQuickInit* NewL(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals);
	~CATQuickInit();
	void StartQuickInit();
	void CancelAndHangUp(TCallInfoTSY* aCallInfoTSY, CATHangUpData* aHangUpData);
protected:
	CATQuickInit(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aEventSource, TInt aStatus);
private:
	void ValidateATHExpectStringL();
	void Complete(TInt aError);
	void RemoveQuickInitExpectStrings();
private:
	TInt iAttempt;
	enum {
		ENotInProgress,
		EWaitForATSilence,
		EWaitForEscapeWriteCompletion,
		EWaitForATEscapeOK,
		EWaitForATWrite,
		EWaitForATOK,
		EATOWaitForWriteComplete,
		EWaitForATOOK,
		EWaitForDTRDropped,
		EWaitForDTRRaised,
		ECancelling
		} iState;
	CCommChatString* iConnectExpectString;
	CCommChatString* iRingExpectString;
	CCommChatString* iNoCarrierExpectString;
	CATHangUpData* iHangUpData;		// temporary storage while quick init cancels, before calling 
	TCallInfoTSY* iCallInfo;	// hang up.
	TBuf8<KCommsDbSvrMaxFieldLength> iConnectString;
	};

#endif
