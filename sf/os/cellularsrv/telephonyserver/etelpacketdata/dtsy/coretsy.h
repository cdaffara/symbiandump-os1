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
// This header file contains TSY classes that represent the core ETel API support
// 
//

/**
 @file
 @internalComponent 
*/

#ifndef __CORETSY_H
#define __CORETSY_H

#include <e32base.h>
#include <et_phone.h>
#include "etelpckt.h"
/**
@internalTechnology
*/
#ifdef __MARM__
const TInt KEtelTimerGranularity = 31000;
#else
const TInt KEtelTimerGranularity = 80000;
#endif
/**
Test version numbers
@internalTechnology
*/
const TUint KTsyEtelMajorVersionNumber=0;
const TUint KTsyEtelMinorVersionNumber=0;
const TUint KTsyEtelBuildVersionNumber=1;

/**
@internalTechnology
*/
enum TTsyPanic
	{
	KErrEtelDataCorrupted,
	KErrEtelInvalidRequestMode,
	KErrEtelNotRecognisedCancelHandle
	};
/**
@internalTechnology
*/
void TsyPanic(TTsyPanic aPanic);

class TTsyTimer
/**
@internalTechnology
*/
	{
public:
	TTsyTimer();
	TBool iPending;
	TDeltaTimerEntry iEntry;
	TTsyReqHandle iTsyReqHandle;
	};



class CPhoneFactoryDummyBase : public CPhoneFactoryBase
/**
(Dummy) Phone factory base class

@internalTechnology
*/
	{
public:
	CPhoneFactoryDummyBase();
	~CPhoneFactoryDummyBase();

	void QueueTimer(TTsyTimer& aTsyTimer,const TTsyReqHandle aHandle,TTimeIntervalMicroSeconds32 aTimeInMicroSeconds,
						TInt (*aFunction)(TAny *aPtr), TAny* aPtr);
	TBool RemoveTimer(TTsyTimer& aHandle);
	void ResetPending(TTsyTimer& aHandle);

	virtual TInt GetPhoneInfo(const TInt aIndex, RTelServer::TPhoneInfo& aPhoneFactoryBaseInfo);
	virtual TInt EnumeratePhones();

private:
	CDeltaTimer* iTimer;
	};



class CSubSessionExtDummyBase : public CSubSessionExtBase
/**
(Dummy) CSubSession Extension base class	
@internalTechnology
*/
	{
public:
	CSubSessionExtDummyBase(CPhoneFactoryDummyBase* aFac);
	~CSubSessionExtDummyBase();
	CPhoneFactoryDummyBase* FacPtr() const;
private:
	CPhoneFactoryDummyBase* iFac;
	};


class CFaxDummyBase : public CFaxBase
/**
(Dummy) CFaxBase class
@internalTechnology
*/
	{
public:
	CFaxDummyBase(CPhoneFactoryDummyBase* aFac);
	~CFaxDummyBase();

	virtual TInt Read(const TTsyReqHandle aTsyReqHandle,TDes8* aDes);
	virtual TInt Write(const TTsyReqHandle aTsyReqHandle,TDesC8* aDes);
	virtual TInt WaitForEndOfPage(const TTsyReqHandle aTsyReqHandle);
	virtual TInt TerminateFaxSession(const TTsyReqHandle aTsyReqHandle);
	virtual TInt GetProgress(const TTsyReqHandle aTsyReqHandle,RFax::TProgress* aProgress);
	virtual TInt ProgressNotification(const TTsyReqHandle aTsyReqHandle, RFax::TProgress* aProgress);
	virtual TInt ProgressNotificationCancel(const TTsyReqHandle aTsyReqHandle);

	// Handler functions
	static TInt WaitForEndOfPageHandler(TAny* aPtr);
	static TInt ReadHandler(TAny* aPtr);
	static TInt WriteHandler(TAny* aPtr);
	static TInt ProgressNotificationHandler(TAny* aPtr);
private:
	static CFaxDummyBase* This(TAny* aPtr);
	CPhoneFactoryDummyBase* iFac;
	TTsyTimer iRead;
	TTsyTimer iWrite;
	TTsyTimer iWaitForEndOfPage;
	TTsyTimer iProgressNotification;

	TDes8* iReadParams;
	RFax::TProgress* iProgressNotificationParams;
	};


class CFaxDGprsTsy : public CFaxDummyBase
/**
(Dummy) CFaxDGprsTsy class
@internalComponent
*/
	{
public:
	static CFaxDGprsTsy* NewL(CPhoneFactoryDummyBase* aFac);
	CFaxDGprsTsy(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CFaxDGprsTsy();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	RHandleBase* GlobalKernelObjectHandle();
private:
	RChunk iChunk;
	};


class CCallDummyBase : public CCallBase
/**
(Dummy) CCallBase class
@internalTechnology
*/
	{
public:
	CCallDummyBase(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CCallDummyBase();
	
	virtual TInt Dial(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams,TDesC* aTelNumber);
	virtual TInt DialCancel(const TTsyReqHandle aTsyReqHandle);

	virtual TInt Connect(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams);
	virtual TInt ConnectCancel(const TTsyReqHandle aTsyReqHandle);
	
	virtual TInt AnswerIncomingCall(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams);
	virtual TInt AnswerIncomingCallCancel(const TTsyReqHandle aTsyReqHandle);
	
	virtual TInt HangUp(const TTsyReqHandle aTsyReqHandle);
	virtual TInt HangUpCancel(const TTsyReqHandle aTsyReqHandle);
	
	virtual TInt NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RCall::TCaps* aCaps);
	virtual TInt NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	virtual TInt NotifyDurationChange(const TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime);
	virtual TInt NotifyDurationChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	virtual TInt NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* aHookStatus);
	virtual TInt NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	virtual TInt NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus);
	virtual TInt NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	virtual TInt LoanDataPort(const TTsyReqHandle aTsyReqHandle,RCall::TCommPort*);
	virtual TInt LoanDataPortCancel(const TTsyReqHandle aTsyReqHandle);
	
	virtual TInt RecoverDataPort(const TTsyReqHandle aTsyReqHandle);
	virtual TInt RecoverDataPortAndRelinquishOwnership();
	
	virtual TInt TransferOwnership(const TTsyReqHandle aTsyReqHandle);
	virtual TInt AcquireOwnership(const TTsyReqHandle aTsyReqHandle);
	virtual TInt AcquireOwnershipCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt RelinquishOwnership();
	
	virtual TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus*);
	virtual TInt GetCaps(const TTsyReqHandle aTsyReqHandle,RCall::TCaps* aCaps);
	virtual TInt GetInfo(const TTsyReqHandle aTsyReqHandle, RCall::TCallInfo* aCallInfo);
	virtual TInt GetBearerServiceInfo(const TTsyReqHandle aTsyReqHandle,RCall::TBearerService*);
	virtual TInt GetCallParams(const TTsyReqHandle aTsyReqHandle, TDes8* aParams);
	virtual TInt GetCallDuration(const TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime);
	
	virtual TInt GetFaxSettings(const TTsyReqHandle aTsyReqHandle,RCall::TFaxSessionSettings* aSettings);
	virtual TInt SetFaxSettings(const TTsyReqHandle aTsyReqHandle,const RCall::TFaxSessionSettings* aSettings);

	// Handler functions
	static TInt NotifyCapsChangeHandler(TAny* aPtr);
	static TInt NotifyDurationChangeHandler(TAny* aPtr);
	static TInt NotifyStatusChangeHandler(TAny* aPtr);
	static TInt LoanDataPortHandler(TAny* aPtr);
	static TInt DialHandler(TAny* aPtr);
	static TInt AnswerHandler(TAny* aPtr);
	static TInt HangUpHandler(TAny* aPtr);

	CPhoneFactoryDummyBase* FacPtr() const;
private:
	static CCallDummyBase* This(TAny* aPtr);
	CPhoneFactoryDummyBase* iFac;

	TTsyTimer iNotifyCapsChange;
	TTsyTimer iNotifyDurationChange;
	TTsyTimer iNotifyStatusChange;
	TTsyTimer iDial;
	TTsyTimer iAnswer;
	TTsyTimer iHangUp;
	TTsyTimer iLoanDataPortTimer;
	
	RCall::TStatus* iNotifyStatusChangeParams;
	};


class CLineDummyBase : public CLineBase
/**
(Dummy) CLineBase class
@internalTechnology
*/
	{
public:
	CLineDummyBase(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CLineDummyBase();

	virtual TInt NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus);
	virtual TInt NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);

	virtual TInt NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* aHookStatus);
	virtual TInt NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	virtual TInt NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RLine::TCaps* aCaps);
	virtual TInt NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	virtual TInt NotifyIncomingCall(const TTsyReqHandle aTsyReqHandle, TName* aName);
	virtual TInt NotifyIncomingCallCancel(const TTsyReqHandle aTsyReqHandle);
	
	virtual TInt NotifyCallAdded(const TTsyReqHandle aTsyReqHandle,TName* aName);
	virtual TInt NotifyCallAddedCancel(const TTsyReqHandle aTsyReqHandle);
	
	virtual TInt GetCaps(const TTsyReqHandle aTsyReqHandle,RLine::TCaps* aCaps);
	virtual TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus*);
	virtual TInt GetHookStatus(const TTsyReqHandle aTsyReqHandle,RCall::THookStatus* aHookStatus);
	virtual TInt GetInfo(const TTsyReqHandle aTsyReqHandle, RLine::TLineInfo* aLineInfo);

	virtual TInt EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aCount);
	virtual TInt GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex*);

	// Handler functions
	static TInt NotifyCapsChangeHandler(TAny* aPtr);
	static TInt NotifyStatusChangeHandler(TAny* aPtr);
	static TInt NotifyIncomingCallHandler(TAny* aPtr);
	static TInt NotifyCallAddedHandler(TAny* aPtr);

	CPhoneFactoryDummyBase* FacPtr() const;
private:
	static CLineDummyBase* This(TAny* aPtr);
	CPhoneFactoryDummyBase* iFac;
	
	TTsyTimer iNotifyCapsChange;
	TTsyTimer iNotifyIncomingCall;
	TTsyTimer iNotifyStatusChange;
	TTsyTimer iNotifyCallAdded;

	RCall::TStatus* iStatusParams;
	RLine::TCaps* iCapsParams;
	TName* iNameParams;
	
protected:
	TInt iNameIndex;
	};


class CPhoneDummyBase : public CPhoneBase
/**
(Dummy) CPhoneBase class.
@internalTechnology
*/
	{
public:
	CPhoneDummyBase(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CPhoneDummyBase();

	virtual TInt ControlledInitialisation(const TTsyReqHandle aTsyReqHandle);
	virtual TInt ControlledInitialisationCancel(const TTsyReqHandle aTsyReqHandle);
	
	virtual TInt NotifyModemDetected(const TTsyReqHandle aTsyReqHandle, RPhone::TModemDetection* aDetection);
	virtual TInt NotifyModemDetectedCancel(const TTsyReqHandle aTsyReqHandle);
	
	virtual TInt NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RPhone::TCaps* aCaps);
	virtual TInt NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);

	virtual TInt GetInfo(const TTsyReqHandle aTsyReqHandle, RPhone::TPhoneInfo* aPhoneInfo);
	virtual TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RPhone::TStatus*);
	virtual TInt GetCaps(const TTsyReqHandle aTsyReqHandle,RPhone::TCaps*);

	virtual TInt EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt*);
	virtual TInt GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex*);

	// Handler functions
	static TInt NotifyPhoneDetectedHandler(TAny* aPtr);
	static TInt NotifyCapsChangeHandler(TAny* aPtr);

	CPhoneFactoryDummyBase* FacPtr() const;

private:
	static CPhoneDummyBase* This(TAny* aPtr);
	CPhoneFactoryDummyBase* iFac;

	TTsyTimer iNotifyPhoneDetected;
	TTsyTimer iNotifyCapsChange;
	
	RPhone::TModemDetection* iDetectionParams;

protected:
	};


#endif
