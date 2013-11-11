// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTSTATE_H__
#define BTSTATE_H__

#include "btcommutil.h"

class TBTPortState;
NONSHARABLE_CLASS(CBTPortStateFactory) : public CBase
	{
friend class CBTPortFactory;	
// only a friend can instantiate a CBTPortStateFactory...
public:
	enum TCSYState
		{
		EIdle,
		ELoadingProtocol,
		EDiscovering,
		ESDPConnected,
		ESDPServiceQuery,
		ESDPServiceIDListRetrieved,
		ESDPAttribListRetrieved,
		ESecuritySetup,
		EConnecting,
		EOpen,
		EClosing,
		EError,
	// *** keep next one last ***
		EMaxCSYStates,
		};

	~CBTPortStateFactory();
	TBTPortState& GetState(const TCSYState aState);
	TInt StateIndex(const TBTPortState* aState) const;
private:
	static CBTPortStateFactory *NewL();
	CBTPortStateFactory();
	void InitL();
private:
	TFixedArray<TBTPortState*, EMaxCSYStates> iStates;
public:
	TInt	iSDPServRecordHandleCount;
	TInt	iExtractedHandleCount;
	};

NONSHARABLE_CLASS(TBTPortState)
// abstract base class for CSY state objects.
	{
public:
	TBTPortState(CBTPortStateFactory* aFactory);
	virtual ~TBTPortState()=0;
	virtual void Open(CBTPortProxy* aContext)=0;
	virtual void Read(CBTPortProxy* aContext,TAny* aPtr,TInt aLength)=0;
	virtual void Write(CBTPortProxy* aContext,TAny* aPtr,TInt aLength)=0;
	virtual void Close(CBTPortProxy* aContext)=0;
	virtual void DoRunL(CBTPortProxy* aContext)=0;
	virtual void DoCancel(CBTPortProxy* aContext)=0;
	virtual void WriteCancel(CBTPortProxy* aContext)=0;
	virtual void ReadCancel(CBTPortProxy* aContext)=0;
	virtual void DoWriteCompleted(CBTPortProxy* aContext,TInt aError)=0;
	virtual void DoReadCompleted(CBTPortProxy* aContext,TInt aError)=0;
	virtual void DoLockedAction(CBTPortProxy* aContext)=0;
	virtual void Error(CBTPortProxy* aContext,TInt aError)=0;
protected:
	void PanicInState(TBTCommPanic aPanic) const;
protected:
	CBTPortStateFactory* iFactory;
private:
    // Forbid copying
	TBTPortState(const TBTPortState&);
	const TBTPortState& operator=(const TBTPortState&);
	};

NONSHARABLE_CLASS(TBTPortDefaultState) : public TBTPortState
// contains default overrides for TBTPortState functions
	{
public:
	TBTPortDefaultState(CBTPortStateFactory* aParent);
	virtual ~TBTPortDefaultState();
	virtual void Open(CBTPortProxy* aContext);
	virtual void Read(CBTPortProxy* aContext,TAny* aPtr, TInt aLength);
	virtual void Write(CBTPortProxy* aContext,TAny* aPtr, TInt aLength);
	virtual void Close(CBTPortProxy* aContext);
	virtual void DoRunL(CBTPortProxy* aContext);
	virtual void DoCancel(CBTPortProxy* aContext);
	virtual void WriteCancel(CBTPortProxy* aContext);
	virtual void ReadCancel(CBTPortProxy* aContext);
	virtual void DoLockedAction(CBTPortProxy* aContext);
	virtual void DoWriteCompleted(CBTPortProxy* aContext, TInt aError);
	virtual void DoReadCompleted(CBTPortProxy* aContext, TInt aError);
	virtual void Error(CBTPortProxy* aContext,TInt aError);
	virtual void LogStateError(CBTPortProxy* aContext, TInt aError)=0;
	};


NONSHARABLE_CLASS(TBTPortCommonBaseState) : public TBTPortDefaultState
	{
public:
	TBTPortCommonBaseState(CBTPortStateFactory* aParent);
	virtual void Read(CBTPortProxy* aContext,TAny* aPtr, TInt aLength);
	virtual void Write(CBTPortProxy* aContext,TAny* aPtr, TInt aLength);
	virtual void WriteCancel(CBTPortProxy* aContext);
	virtual void ReadCancel(CBTPortProxy* aContext);
	virtual void Close(CBTPortProxy* aContext);
	};

NONSHARABLE_CLASS(TBTPortStateIdle) : public TBTPortCommonBaseState
	{
public:
	TBTPortStateIdle(CBTPortStateFactory* aParent);
	~TBTPortStateIdle();
	virtual void DoRunL(CBTPortProxy* aContext);
	virtual void Open(CBTPortProxy* aContext);
	virtual void Close(CBTPortProxy* aContext);
	virtual void Read(CBTPortProxy* aContext,TAny* aPtr, TInt aLength);
	virtual void Write(CBTPortProxy* aContext,TAny* aPtr, TInt aLength);
	virtual void LogStateError(CBTPortProxy* aContext, TInt aError);
private:
	void SockServConnect(CBTPortProxy* aContext); // to be called by friendly class 
												//upon re-opening while closing
	friend class CBTPortProxy; 
	};

NONSHARABLE_CLASS(TBTPortStateLoadingProtocol) : public TBTPortCommonBaseState
	{
public:
	TBTPortStateLoadingProtocol(CBTPortStateFactory* aParent);
	~TBTPortStateLoadingProtocol();
	virtual void DoRunL(CBTPortProxy* aContext);
	virtual void Close(CBTPortProxy* aContext);
	virtual void Read(CBTPortProxy* aContext,TAny* aPtr, TInt aLength);
	virtual void Write(CBTPortProxy* aContext,TAny* aPtr, TInt aLength);
	virtual void LogStateError(CBTPortProxy* aContext, TInt aError);
private:
	void StartProtocol(CBTPortProxy* aContext); 
private:
	TBool iClosePending;

	friend class CBTPortProxy; 
	};

NONSHARABLE_CLASS(TBTPortStateDiscovering) : public TBTPortCommonBaseState
	{
public:
	TBTPortStateDiscovering(CBTPortStateFactory* aParent);
	~TBTPortStateDiscovering();
	virtual void DoRunL(CBTPortProxy* aContext);
	virtual void DoLockedAction(CBTPortProxy* aContext);
	virtual void LogStateError(CBTPortProxy* aContext, TInt aError);
	};


NONSHARABLE_CLASS(TBTPortStateSDPConnected) : public TBTPortCommonBaseState
	{
public:
	TBTPortStateSDPConnected(CBTPortStateFactory* aParent);
	~TBTPortStateSDPConnected();
	virtual void DoRunL(CBTPortProxy* aContext);
	virtual void LogStateError(CBTPortProxy* aContext, TInt aError);
	};

NONSHARABLE_CLASS(TBTPortStateSDPServiceQuery) : public TBTPortCommonBaseState
	{
public:
	TBTPortStateSDPServiceQuery(CBTPortStateFactory* aParent);
	~TBTPortStateSDPServiceQuery();
	virtual void DoRunL(CBTPortProxy* aContext);
	virtual void LogStateError(CBTPortProxy* aContext, TInt aError);
	};

NONSHARABLE_CLASS(TBTPortStateServiceIDListRetrieved) : public TBTPortCommonBaseState
	{
public:
	TBTPortStateServiceIDListRetrieved(CBTPortStateFactory* aParent);
	~TBTPortStateServiceIDListRetrieved();
	virtual void DoRunL(CBTPortProxy* aContext);
	virtual void LogStateError(CBTPortProxy* aContext, TInt aError);
	};


NONSHARABLE_CLASS(TBTPortStateSDPAttributeListRetrieved) : public TBTPortCommonBaseState
	{
public:
	TBTPortStateSDPAttributeListRetrieved(CBTPortStateFactory* aParent);
	~TBTPortStateSDPAttributeListRetrieved();
	virtual void DoRunL(CBTPortProxy* aContext);
	virtual void DoLockedAction(CBTPortProxy* aContext);
	virtual void LogStateError(CBTPortProxy* aContext, TInt aError);
	};

NONSHARABLE_CLASS(TBTPortStateConnecting) : public TBTPortCommonBaseState
	{
public:
	TBTPortStateConnecting(CBTPortStateFactory* aParent);
	~TBTPortStateConnecting();
	virtual void DoRunL(CBTPortProxy* aContext);
	virtual void LogStateError(CBTPortProxy* aContext,TInt aError);
	};

NONSHARABLE_CLASS(TBTPortStateOpen) : public TBTPortCommonBaseState
	{
public:
	TBTPortStateOpen(CBTPortStateFactory* aParent);
	~TBTPortStateOpen();
	virtual void Read(CBTPortProxy* aContext,TAny* aPtr,TInt aLength);
	virtual void Write(CBTPortProxy* aContext,TAny* aPtr,TInt aLength);
	virtual void WriteCancel(CBTPortProxy* aContext);
	virtual void ReadCancel(CBTPortProxy* aContext);
	virtual void Close(CBTPortProxy* aContext);
	virtual void DoLockedAction(CBTPortProxy* aContext);
	virtual void DoWriteCompleted(CBTPortProxy* aContext,TInt aError);
	virtual void DoReadCompleted(CBTPortProxy* aContext,TInt aError);
	virtual void LogStateError(CBTPortProxy* aContext,TInt aError);
private:
	void HandleIPCWriteToClient(CBTPortProxy* aContext);
	};

NONSHARABLE_CLASS(TBTPortStateClosing) : public TBTPortCommonBaseState
	{
public:
	TBTPortStateClosing(CBTPortStateFactory* aParent);
	~TBTPortStateClosing();
	virtual void Read(CBTPortProxy* aContext,TAny* aPtr,TInt aLength);
	virtual void Write(CBTPortProxy* aContext,TAny* aPtr,TInt aLength);
	virtual void Close(CBTPortProxy* aContext);
	virtual void DoRunL(CBTPortProxy* aContext);
	virtual void DoLockedAction(CBTPortProxy* aContext);
	virtual void DoWriteCompleted(CBTPortProxy* aContext,TInt aError);
	virtual void DoReadCompleted(CBTPortProxy* aContext,TInt aError);
	virtual void LogStateError(CBTPortProxy* aContext,TInt aError);
	};

NONSHARABLE_CLASS(TBTPortErrorState) : public TBTPortState
/**
	Error State.
	The CSY will move in this state after an error has been returned.
	From this state on Read and Write (from the client) we will return
	the error code, hence notify the client. Unfortunately this is the only
	way that we can notify C32 (and hence the client).
*/
	{
public:
	TBTPortErrorState(CBTPortStateFactory* aParent);
	virtual ~TBTPortErrorState();
	virtual void Open(CBTPortProxy* aContext);
	virtual void Read(CBTPortProxy* aContext,TAny* aPtr,TInt aLength);
	virtual void Write(CBTPortProxy* aContext,TAny* aPtr,TInt aLength);
	virtual void Close(CBTPortProxy* aContext);
	virtual void DoRunL(CBTPortProxy* aContext);
	virtual void DoCancel(CBTPortProxy* aContext);
	virtual void WriteCancel(CBTPortProxy* aContext);
	virtual void ReadCancel(CBTPortProxy* aContext);
	virtual void DoLockedAction(CBTPortProxy* aContext);
	virtual void DoWriteCompleted(CBTPortProxy* aContext,TInt aError);
	virtual void DoReadCompleted(CBTPortProxy* aContext,TInt aError);
	virtual void Error(CBTPortProxy* aContext,TInt aError);
	};




#endif
