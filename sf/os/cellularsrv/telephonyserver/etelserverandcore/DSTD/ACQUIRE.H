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

#ifndef __DSTD_ACQUIRE_H
#define __DSTD_ACQUIRE_H

#include <e32base.h>
#include "ET_PHONE.H"

/**
@internalComponent
*/
enum TTsyPanic
	{
	KTsyPanicDataCorrupted,
	KTsyPanicNotRecognisedCancelHandle,
	KTsyPanicUnexpectedReturnValue
	};
/**
@internalComponent
*/
void TsyPanic(TTsyPanic aPanic);

class CPhoneFactoryDummyBase;
class TTsyTimer
/**
@internalComponent
*/
	{
public:
	TTsyTimer();
	TBool iPending;
	TDeltaTimerEntry iEntry;
	TTsyReqHandle iTsyReqHandle;
	};

//
// CAcquireEntry
//
class CAcquireOwnerList;
class CAcquireEntry : public CBase
/**
@internalTechnology
*/
	{
public:
	static CAcquireEntry* NewL(const TTsyReqHandle aTsyReqHandle);
	CAcquireEntry(const TTsyReqHandle aTsyReqHandle);
	~CAcquireEntry();
	void Deque();
public:
	TTsyReqHandle iTsyReqHandle;
private:
	TDblQueLink iLink;
	friend class CAcquireOwnerList;
	};

//
// CAcquireOwnerList
//
class CAcquireOwnerList : public CBase
/**
@internalTechnology
*/
	{
public:
	static CAcquireOwnerList* NewL();
	CAcquireOwnerList();
	~CAcquireOwnerList();
	void ConstructL();
	CAcquireEntry* FindByTsyReqHandle(const TTsyReqHandle aTsyReqHandle);
	void Remove(CAcquireEntry* aEntry);
	TDblQue<CAcquireEntry> iAcquireList;
	};

//
// CFaxDummyBase
//
class CFaxDummyBase : public CFaxBase
/**
@internalTechnology
*/
	{
public:
	CFaxDummyBase(CPhoneFactoryDummyBase* aFac);
	~CFaxDummyBase();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);
	virtual TInt Read(const TTsyReqHandle aTsyReqHandle,TDes8* aDes);
	virtual TInt Write(const TTsyReqHandle aTsyReqHandle,TDesC8* aDes);
	virtual TInt WaitForEndOfPage(const TTsyReqHandle aTsyReqHandle);
	virtual TInt TerminateFaxSession(const TTsyReqHandle aTsyReqHandle);
	virtual TInt GetProgress(const TTsyReqHandle aTsyReqHandle,RFax::TProgress* aProgress);
	virtual TInt ProgressNotification(const TTsyReqHandle aTsyReqHandle, RFax::TProgress* aProgress);
	virtual TInt ProgressNotificationCancel(const TTsyReqHandle aTsyReqHandle);
public:
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

//
// CCallDummyBase
//
class CCallDummyBase : public CCallBase
/**
@internalTechnology
*/
	{
public:
	CCallDummyBase(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CCallDummyBase();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt TransferOwnership(const TTsyReqHandle aTsyReqHandle);
	virtual TInt AcquireOwnership(const TTsyReqHandle aTsyReqHandle);
	virtual TInt AcquireOwnershipCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt RelinquishOwnership();
	virtual TInt CancelSubSession();
//	virtual CTelObject::TReqMode ReqModeL(TInt aIpc);
// Core
	static TInt NotifyCapsChangeHandler(TAny* aPtr);
	static TInt NotifyDurationChangeHandler(TAny* aPtr);
	static TInt NotifyHookChangeHandler(TAny* aPtr);
	static TInt NotifyStatusChangeHandler(TAny* aPtr);
	static TInt LoanDataPortHandler(TAny* aPtr);
	static TInt DialHandler(TAny* aPtr);
	static TInt ConnectHandler(TAny* aPtr);
	static TInt AnswerHandler(TAny* aPtr);
	static TInt HangUpHandler(TAny* aPtr);

	virtual TInt GetInfo(const TTsyReqHandle aTsyReqHandle, RCall::TCallInfo* aCallInfo);
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
	virtual TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus*);
	virtual TInt GetCaps(const TTsyReqHandle aTsyReqHandle,RCall::TCaps* aCaps);
	virtual TInt Dial(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams,TDesC* aTelNumber);
	virtual TInt Connect(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams);
	virtual TInt AnswerIncomingCall(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams);
	virtual TInt HangUp(const TTsyReqHandle aTsyReqHandle);
	virtual TInt GetBearerServiceInfo(const TTsyReqHandle aTsyReqHandle,RCall::TBearerService*);
	virtual TInt DialCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt ConnectCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt AnswerIncomingCallCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt HangUpCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt GetCallParams(const TTsyReqHandle aTsyReqHandle, TDes8* aParams);
	virtual TInt GetCallDuration(const TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime);
	virtual TInt GetFaxSettings(const TTsyReqHandle aTsyReqHandle,RCall::TFaxSessionSettings* aSettings);
	virtual TInt SetFaxSettings(const TTsyReqHandle aTsyReqHandle,const RCall::TFaxSessionSettings* aSettings);
	virtual TInt SetFaxSharedHeaderFile(const TTsyReqHandle aTsyReqHandle, CFaxSharedFileHandles* aFaxSharedFileHandles);
	CPhoneFactoryDummyBase* FacPtr() const;
private:
	static CCallDummyBase* This(TAny* aPtr);
	TInt ProcessingOwnership();
	CPhoneFactoryDummyBase* iFac;
	CAcquireOwnerList* iList;
	TTsyTimer iNotifyCapsChange;
	TTsyTimer iNotifyDurationChange;
	TTsyTimer iNotifyHookChange;
	TTsyTimer iNotifyStatusChange;
	TTsyTimer iDial;
	TTsyTimer iConnect;
	TTsyTimer iAnswer;
	TTsyTimer iHangUp;
	TTsyTimer iLoanDataPortTimer;
	RCall::THookStatus* iNotifyHookChangeParams;
	RCall::TStatus* iNotifyStatusChangeParams;
	};

//
// CLineDummyBase
//
class CLineDummyBase : public CLineBase
/**
@internalTechnology
*/
	{
public:
	CLineDummyBase(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CLineDummyBase();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt CancelSubSession();
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual TInt NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus);
	virtual TInt NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt GetHookStatus(const TTsyReqHandle aTsyReqHandle,RCall::THookStatus* aHookStatus);

	virtual TInt GetInfo(const TTsyReqHandle aTsyReqHandle, RLine::TLineInfo* aLineInfo);
	virtual TInt NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RLine::TCaps* aCaps);
	virtual TInt NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NotifyIncomingCall(const TTsyReqHandle aTsyReqHandle, TName* aName);
	virtual TInt NotifyIncomingCallCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NotifyCallAdded(const TTsyReqHandle aTsyReqHandle,TName* aName);
	virtual TInt NotifyCallAddedCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt GetCaps(const TTsyReqHandle aTsyReqHandle,RLine::TCaps* aCaps);
	virtual TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus*);
	virtual TInt NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* aHookStatus);
	virtual TInt NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle);

	static TInt NotifyCapsChangeHandler(TAny* aPtr);
	static TInt NotifyStatusChangeHandler(TAny* aPtr);
	static TInt NotifyIncomingCallHandler(TAny* aPtr);
	static TInt NotifyHookChangeHandler(TAny* aPtr);
	static TInt NotifyCallAddedHandler(TAny* aPtr);
	CPhoneFactoryDummyBase* FacPtr() const;
private:
	static CLineDummyBase* This(TAny* aPtr);
	CPhoneFactoryDummyBase* iFac;
	TTsyTimer iNotifyCapsChange;
	TTsyTimer iNotifyIncomingCall;
	TTsyTimer iNotifyHookChange;
	TTsyTimer iNotifyStatusChange;
	TTsyTimer iNotifyCallAdded;
	RCall::THookStatus* iHookStatusParams;
	RCall::TStatus* iStatusParams;
	
protected:
	TInt iNameIndex;
	};

//
// CPhoneDummyBase
//
class CPhoneDummyBase : public CPhoneBase
/**
@internalTechnology
*/
	{
public:
	CPhoneDummyBase(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CPhoneDummyBase();

	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt CancelSubSession();
//	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual TInt ControlledInitialisation(const TTsyReqHandle aTsyReqHandle);
	virtual TInt ControlledInitialisationCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NotifyModemDetected(const TTsyReqHandle aTsyReqHandle, RPhone::TModemDetection* aDetection);
	virtual TInt NotifyModemDetectedCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt GetInfo(const TTsyReqHandle aTsyReqHandle, RPhone::TPhoneInfo* aPhoneInfo);
	virtual TInt NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RPhone::TCaps* aCaps);
	virtual TInt NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RPhone::TStatus*);
	virtual TInt GetCaps(const TTsyReqHandle aTsyReqHandle,RPhone::TCaps*);
public:

	static TInt HookChangeHandler(TAny* aPtr);
	static TInt NotifyPhoneDetectedHandler(TAny* aPtr);
	static TInt NotifyCapsChangeHandler(TAny* aPtr);
	CPhoneFactoryDummyBase* FacPtr() const;

private:
	static CPhoneDummyBase* This(TAny* aPtr);
	TTsyTimer iNotifyPhoneDetected;
	TTsyTimer iNotifyCapsChange;
	RPhone::TModemDetection* iDetectionParams;
	CPhoneFactoryDummyBase* iFac;
	void Dummy();
	};

//
//	CPhoneFactoryDummyBase
//
#ifdef __MARM__
const TInt KEtelTimerGranularity = 31000;
#else
const TInt KEtelTimerGranularity = 80000;
#endif

class CPhoneFactoryDummyBase : public CPhoneFactoryBase
/**
@internalTechnology
*/
	{
public:
	CPhoneFactoryDummyBase();
	void ConstructL();
	~CPhoneFactoryDummyBase();

	void QueueTimer(TTsyTimer& aTsyTimer,
						const TTsyReqHandle aHande,
						TTimeIntervalMicroSeconds32 aTimeInMicroSeconds,
						TInt (*aFunction)(TAny *aPtr), TAny* aPtr);
	TBool RemoveTimer(TTsyTimer& aHandle);
	void ResetPending(TTsyTimer& aHandle);
	TBool IsSupported(const TInt aMixin);
private:
	CDeltaTimer* iTimer;
	};

class CSubSessionExtDummyBase : public CSubSessionExtBase
/**
@internalTechnology
*/
	{
public:
	CSubSessionExtDummyBase(CPhoneFactoryDummyBase* aFac);
	~CSubSessionExtDummyBase();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	CPhoneFactoryDummyBase* FacPtr() const;
	virtual TInt CancelSubSession();
private:
	CPhoneFactoryDummyBase* iFac;
	};
/**
@internalComponent
*/
const TUint KTsyEtelMajorVersionNumber=1;
const TUint KTsyEtelMinorVersionNumber=0;
const TUint KTsyEtelBuildVersionNumber=606;


#endif
