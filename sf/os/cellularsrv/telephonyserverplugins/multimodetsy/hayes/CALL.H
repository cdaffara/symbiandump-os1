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


#ifndef __CALL_H__
#define __CALL_H__

#include <et_phone.h>
#include "TSYCONFG.H"
#include "ATBASE.H"
#include <f32file.h>
//
// CAcquireEntry
//
class CAcquireOwnerList;
class CFaxSharedFileHandles;
class CAcquireEntry : public CBase
/**
@internalComponent
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
@internalComponent
*/
	{
public:
	static CAcquireOwnerList* NewL();
	CAcquireOwnerList();
	~CAcquireOwnerList();
	void ConstructL();
	CAcquireEntry* FindByTsyReqHandle(const TTsyReqHandle aTsyReqHandle);
	void Remove(CAcquireEntry* aEntry);
public:
	TDblQue<CAcquireEntry> iAcquireList;
	};

class CCallHayes;
class CHeartbeatRunner : public CBase, public MBeating
/**
@internalComponent
*/
	{
public:
	static CHeartbeatRunner* NewL(CCallHayes* aCallHayes,CNotifications* aNotificationStore);
	~CHeartbeatRunner(); 
	TTimeIntervalSeconds CallDuration() const;
	void Start();
	void Stop();
protected:
	CHeartbeatRunner(CCallHayes* aCallHayes,CNotifications* aNotificationStore);
private:
	void ConstructL();
	// functions for MBeating protocol
	void Beat(); // called when beat works ok
	void Synchronize(); // called when we need to synchronize
private:
	TTime iStartTime; 
	CHeartbeat* iHeartbeat; 
	TInt iTicks; 
	CCallHayes* iCallHayes;
	CNotifications* iNotificationStore;
	};

//
// CCallHayes - General Call Functionality
//

class CATHangUp;
class CATAnswerData;
class CATConnectData;
class CATQuickInit;
class CATWaitForNoCarrier;
class CCallHayes : public CCallBase
/**
@internalComponent
*/
	{
public:
	CCallHayes(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	void ConstructL(const TName& aName);
	~CCallHayes();
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	static void CloseCall(TAny* aObj);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);
// MCallBaseTSY pure virtuals
	virtual TInt GetInfo(const TTsyReqHandle aTsyReqHandle,RCall::TCallInfo* aCallInfo);
	virtual TInt NotifyCapsChange(const TTsyReqHandle aTsyReqHandle,RCall::TCaps* aCaps);
	virtual TInt NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* aHookStatus);
    virtual TInt NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus);
	virtual TInt NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NotifyDurationChange(const TTsyReqHandle aTsyReqHandle,TTimeIntervalSeconds* aTime);
	virtual TInt NotifyDurationChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt LoanDataPort(const TTsyReqHandle aTsyReqHandle,RCall::TCommPort* aCommPort);
	virtual TInt LoanDataPortCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt RecoverDataPort(const TTsyReqHandle aTsyReqHandle);
	virtual TInt RecoverDataPortAndRelinquishOwnership();
    virtual TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aCallStatus);
    virtual TInt TransferOwnership(const TTsyReqHandle aTsyReqHandle);
    virtual TInt AcquireOwnership(const TTsyReqHandle aTsyReqHandle);
	virtual TInt AcquireOwnershipCancel(const TTsyReqHandle aTsyReqHandle);
    virtual TInt GetBearerServiceInfo(const TTsyReqHandle aTsyReqHandle,RCall::TBearerService* aService);
	virtual TInt GetCallParams(const TTsyReqHandle aTsyReqHandle, TDes8* aParams);
	virtual TInt GetCallDuration(const TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime);
	virtual TInt GetFaxSettings(const TTsyReqHandle aTsyReqHandle,RCall::TFaxSessionSettings* aSettings);
	virtual TInt SetFaxSettings(const TTsyReqHandle aTsyReqHandle,const RCall::TFaxSessionSettings* aSettings);

// MTelObjectTSY pure virtuals 
	virtual CTelObject* OpenNewObjectByNameL(const TDesC&);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CheckAndSetRegistrationParams(const TInt aIpc,const TDes8* aDes1,const TDes8* aDes2);


// MSubSessionExtBaseTSY pure virtual
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	
// own pure virtual
	virtual void AnswerImmediately()=0;

// other funtions
	RCall::TStatus GetCoreCallStatus();
	TInt ChangeCallStatus(RMobileCall::TMobileCallStatus aCallStatus);
	void ChangeLineStatus(RCall::TStatus aLineStatus);
	void SetToIdle();
	void SetToIdleAndCompleteReq(TTsyReqHandle aTsyReqHandle,TInt aStatus);
	TCallInfoTSY* CallInfo();
	void GetCallInfo(TCallInfoIndex* aCallInfoIndex);
	TBool CheckName(const TDesC& aName) const;
	void GetCallDuration(TTimeIntervalSeconds& aTime) const;
	void StartCallTicker() const;
	void StopCallTicker() const;
	void ResetIsForIncomingCall();
	TBool IsForIncomingCall() const;
	void SetOwnedByTSY();
	void SetUnownedByTSY();
	TBool IsOwnedByTSY() const;
	virtual TBool CollateCurrentMobileCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCallCaps)=0;
	virtual TBool CollateCurrentCoreCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCallCaps)=0;
	
protected:
	void SetCallParams(const TDesC8* aParams);
	void GetDefaultCallParams();
	TInt ValidateRequest(const TTsyReqHandle aTsyReqHandle, RCall::TStatus aLineStatus);

public:
	CPhoneGlobals* iPhoneGlobals;
	CATWaitForNoCarrier* iWaitForNoCarrier;
protected:
	TCallInfoTSY iCallInfo;
	CATIO* iIo;
	CATInit* iInit;
	CATQuickInit* iQuickInit;
	CAcquireOwnerList* iList;
	CHeartbeatRunner* iCallTimer;
	TBool iIsForIncomingCall;
	TBool iIsOwnedByTSY;
	TTsyReqHandle iAnswerTsyReqHandle;
	RMobileCall::TMobileCallCapsV1 iMobileCaps;
	RCall::TCaps iCaps;
	};


//
// CCallMobile - Call functionality for mobile phones
//
class CCallMobile : public CCallHayes
/**
@internalComponent
*/
	{
public:
	CCallMobile(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aGlobals);
	~CCallMobile();

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TInt CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);

private:
	virtual TInt GetCaps(const TTsyReqHandle aTsyReqHandle,RCall::TCaps* aCaps);

	// ETELMM supported requests
	TInt GetMobileCallCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps);
	TInt NotifyMobileCallCapsChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps);
	TInt NotifyMobileCallCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetMobileCallStatus(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus);
	TInt NotifyMobileCallStatusChange(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus);
	TInt NotifyMobileCallStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);

	virtual void CollateCoreCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCallCaps)=0;
	virtual TBool CollateCurrentMobileCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCallCaps);
	virtual TBool CollateCurrentCoreCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCallCaps);
	};



//Added for Java Demo 4.4.99
//
// CCallMobileVoice - Specific Voice Call Functionality
//
class CATDialVoice;
class CATHangUpVoice;
class CATAnswerVoice;
class CCallMobileVoice : public CCallMobile
/**
@internalComponent
*/
	{
public:
	static CCallMobileVoice* NewL(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aPhoneGlobals,const TName& aName);
	CCallMobileVoice(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aGlobals);
	~CCallMobileVoice();
	
// MCallBaseTSY pure virtuals
	virtual TInt RecoverDataPortAndRelinquishOwnership();
	//
	virtual TInt LoanDataPort(const TTsyReqHandle aTsyReqHandle,RCall::TCommPort* aCommPort);
	virtual TInt LoanDataPortCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt RecoverDataPort(const TTsyReqHandle aTsyReqHandle);
	virtual TInt Dial(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams,TDesC* aTelNumber);
	virtual TInt DialCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt Connect(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams);
	virtual TInt ConnectCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt AnswerIncomingCall(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams);
	virtual TInt AnswerIncomingCallCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt HangUp(const TTsyReqHandle aTsyReqHandle);
	virtual TInt HangUpCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt RelinquishOwnership();

	virtual void AnswerImmediately();
	virtual void CollateCoreCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCallCaps);

protected:
	void ConstructL(const TName& aName);

private:
	CATDialVoice* iDialVoice;
	CATAnswerVoice* iAnswerVoice;
	CATHangUpVoice* iHangUpVoice;
	};

//
// CCallMobileData - Specific Data Call Functionality
//
class CATDialData;
class CATHangUpData;
class CATSetToOnlineDataMode;
class CATSetCBST;
class CCallMobileData : public CCallMobile
/**
@internalComponent
*/
	{
public:
	static CCallMobileData* NewL(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aPhoneGlobals,const TName& aName);
	CCallMobileData(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aGlobals);
	~CCallMobileData();
	
// MCallBaseTSY pure virtuals
	virtual TInt RecoverDataPortAndRelinquishOwnership();
	//
	virtual TInt LoanDataPort(const TTsyReqHandle aTsyReqHandle,RCall::TCommPort* aCommPort);
	virtual TInt LoanDataPortCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt RecoverDataPort(const TTsyReqHandle aTsyReqHandle);
	virtual TInt Dial(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams,TDesC* aTelNumber);
	virtual TInt DialCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt Connect(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams);
	virtual TInt ConnectCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt AnswerIncomingCall(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams);
	virtual TInt AnswerIncomingCallCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt HangUp(const TTsyReqHandle aTsyReqHandle);
	virtual TInt HangUpCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt RelinquishOwnership();

	// CCallHayes pure virtual
	virtual void AnswerImmediately();

	virtual void CollateCoreCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCallCaps);

	// Utility function used by objects created by this class
	// (this has been done to reduce code size).
	TInt AssembleCBSTSetString(TBuf8<KGenericBufferSize>& aTxBuffer);

protected:
	void ConstructL(const TName& aName);
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);

private:
	// Utility functions
	void SetDataCallParams(const TDesC8* aCallParams);
	TInt GetMobileDataCallCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps);

private:
	CATSetCBST* iSetCBST;
	CATDialData* iDialData;
	CATAnswerData* iAnswerData;
	CATConnectData* iConnectData;
	CATHangUpData* iHangUpData;
	CATSetToOnlineDataMode* iATSetToOnlineDataMode;
	};

#endif
