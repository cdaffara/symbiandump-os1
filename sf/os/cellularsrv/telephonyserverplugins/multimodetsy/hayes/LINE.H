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

#ifndef __LINE_H__
#define __LINE_H__

#include <et_phone.h>
#include "ATWAIT.H"

//
// CLineHayes - General Line Functionality
//
class CCallEntry;
class CLineHayes : public CLineBase
/**
@internalComponent
*/
	{
public:
	CLineHayes(CATIO* aIo,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CLineHayes();
	void AppendNewCallL(CCallHayes* aNewCall);
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	static void CloseLine(TAny* aObj);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);
// MTelObjectTSY pure virtuals
	virtual void Init();
	virtual const CArrayFixFlat<TInt>* ArrayOfMemberDataSizes(const TInt aIpc) const;
	
// MLineBaseTSY pure virtuals
	virtual TInt GetInfo(const TTsyReqHandle aTsyReqHandle,RLine::TLineInfo* aLineInfo);
	virtual TInt NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RLine::TCaps* aCaps);
	virtual TInt NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NotifyIncomingCall(const TTsyReqHandle aTsyReqHandle, TName* aName);
	virtual TInt NotifyIncomingCallCancel(const TTsyReqHandle aTsyReqHandle);
  	virtual TInt NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* aHookStatus);
	virtual TInt NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aLineStatus);
	virtual TInt NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NotifyCallAdded(const TTsyReqHandle aTsyReqHandle,TName* aName);
	virtual TInt NotifyCallAddedCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus);
	virtual TInt EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aCount);
	virtual TInt GetHookStatus(const TTsyReqHandle aTsyReqHandle,RCall::THookStatus* aHookStatus);

// MSubSessionExtBaseTSY pure virtual
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);

// other functions
	TBool StopMyCallRinging();
	void GetLineStatus(RCall::TStatus& aLineStatus);
	void RemoveCall(CCallHayes* aCallHayes);
	void GetLastCallName(TDes& aName) const;
	void GetNameOfCallForAnswering(TDes& aName);
	void SetCallsHookStatus(RCall::THookStatus aHookStatus);
	TBool AnswerIfPossible();
	TBool CheckForOutstandingAnswer();
	void SetPreAllocCall();
	void ResetPreAllocCall();
	void ResetNotifyIncomingCall();
	TBool IsNotifyIncomingCallOutstanding();
	void FreePreAllocCallIfNecessary();

protected:
	void ConstructL(const TName& aName);
	CCallEntry* CheckNewObject(const TDesC& aName);

private:
	void Dummy();
	void Dummy1();

public:
	TName iLineName;
protected:
	CATIO* iIo;
	CATInit* iInit;
	CPhoneGlobals* iPhoneGlobals;
	TDblQue<CCallEntry> iCalls;
	TInt iPosOfLastAddedCall;
	TInt iNameIndex;
private:
	CArrayFixFlat<TInt>* iSizeOfMemberData;
	TBool iNotifyIncomingCallOutstanding;
protected:
	CCallEntry* iPreAlloc;
	};


//
// CLineMobile - Line functionality for mobile phones
//

class CLineMobile : public CLineHayes
/**
@internalComponent
*/
	{
public:
	CLineMobile(CATIO* aIo,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CLineMobile();

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TInt CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);

private:
	// ETELMM supported requests
	TInt GetMobileLineStatus(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus);
	TInt NotifyMobileLineStatusChange(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus);
	TInt NotifyMobileLineStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	};


//Added for Java Demo 4.4.99
//
// CLineMobileVoice - Voice Specific Line Functionality
//
class CCallMobileVoice;
class CLineMobileVoice : public CLineMobile
/**
@internalComponent
*/
	{
public:
	static CLineMobileVoice* NewL(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aPhoneGlobals,const TName& aName);
	CLineMobileVoice(CATIO* aATIO,CATInit* aInit,CPhoneGlobals*);
	~CLineMobileVoice();

// MTelObjectTSY pure virtuals
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

// MLineBaseTSY pure virtuals
	virtual TInt GetCaps(const TTsyReqHandle aTsyReqHandle,RLine::TCaps* aCaps);
    virtual TInt GetCallInfo(const TTsyReqHandle,TCallInfoIndex*);

//
protected:
	void GenerateName(TDes& aName);
	void ConstructL(const TName& aName);
	};

//
// CLineMobileData - Data Specific Line Functionality
//
class CCallMobileData;
class CLineMobileData : public CLineMobile
/**
@internalComponent
*/
	{
public:
	static CLineMobileData* NewL(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aPhoneGlobals,const TName& aName);
	CLineMobileData(CATIO* aATIO,CATInit* aInit,CPhoneGlobals*);
	~CLineMobileData();

// MTelObjectTSY pure virtuals
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

// MLineBaseTSY pure virtuals
	virtual TInt GetCaps(const TTsyReqHandle aTsyReqHandle,RLine::TCaps* aCaps);
    virtual TInt GetCallInfo(const TTsyReqHandle,TCallInfoIndex*);

//
protected:
	void GenerateName(TDes& aName);
	void ConstructL(const TName& aName);
	};

class CCallEntry : public CBase
/**
@internalComponent
*/
	{
	friend class CLineHayes;
public:
	static CCallEntry* NewL(CCallHayes* aCall);
	CCallEntry(CCallHayes* aCall);
	~CCallEntry();
	void Deque();
	void CompleteAndDeque(TInt aError);
public:
	CCallHayes* iCallHayes;
private:
	TDblQueLink iLink;
	};

#endif
