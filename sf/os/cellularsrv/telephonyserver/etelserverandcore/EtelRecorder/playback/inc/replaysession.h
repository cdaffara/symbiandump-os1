// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Class definitions for client side replayer sessions.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef _REPLAYSESSION_H_
#define _REPLAYSESSION_H_

#include <f32file.h>
#include "crecordmessage.h"
#include <s32file.h> 

#include "replaytypes.h"

class RReplaySession;
class RReplaySubSession;
class MSessionManager
	{
public:
	virtual TInt GetSessionFromSessionId(CRecordMessage::TSession aId, RReplaySession*& aSessionHandle) = 0;
	virtual RReplaySession& GetNewSessionL(CRecordMessage::TSession aId) = 0;
	virtual RReplaySubSession& GetNewSubSessionL(CRecordMessage::TSubSession aId, CRecordMessage::TSubSessionHandle aHandle, const RReplaySession& aSessionHandle) = 0;
	virtual TInt GetSubSessionFromSubSessionHandle(CRecordMessage::TSession aSessionId, CRecordMessage::TSubSession aSubsessionId, CRecordMessage::TSubSessionHandle aHandle, RReplaySubSession*& aSessionHandle)=0;
	virtual TInt CloseAndRemoveSession(CRecordMessage::TSession aId) = 0;
	virtual TInt CloseAndRemoveSubSession(CRecordMessage::TSession aSessionId, CRecordMessage::TSubSession aSubsessionId, CRecordMessage::TSubSessionHandle aHandle) = 0;
	};
	

class RReplaySession : public RSessionBase
	{
public:
	RReplaySession(CRecordMessage::TSession aId);
	inline CRecordMessage::TSession Id() const;
	TInt Connect();
	
	//0 args
	void DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus);

	//val, val, val
	template<class T0, class T1, class T2>
	void DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0 aArg0, T1 aArg1, T2 aArg2);
	//ptr, val, val
	template<class T0, class T1, class T2>
	void DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1 aArg1, T2 aArg2);
	//val, ptr, val
	template<class T0, class T1, class T2>
	void DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0 aArg0, T1* aArg1, T2 aArg2);
	//ptr, ptr, val
	template<class T0, class T1, class T2>
	void DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1* aArg1, T2 aArg2);
	//ptr, val, ptr
	template<class T0, class T1, class T2>
	void DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1 aArg1, T2* aArg2);
	//ptr, ptr, ptr
	template<class T0, class T1, class T2>
	void DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1* aArg1, T2* aArg2);
	
private:
	CRecordMessage::TSession iId;
	};
	
class RReplaySubSession : public RSubSessionBase
	{
public:
	RReplaySubSession(CRecordMessage::TSubSession aId, CRecordMessage::TSubSessionHandle aHandleId, const RReplaySession& aSession);
	inline CRecordMessage::TSubSession Id() const;
	inline CRecordMessage::TSubSessionHandle HandleId() const;
	inline CRecordMessage::TSession SessionId() const;
	void Close(); 
	
	TInt Open(CRecordMessage::TIpc aIpc, TDes16& aArg0, TDes16& aArg1, TIpcArgs::TNothing aArg2);
	TInt Open(CRecordMessage::TIpc aIpc, TDes16& aArg0, TIpcArgs::TNothing aArg1, TInt aArg2);
	TInt Open(CRecordMessage::TIpc aIpc, TDes16& aArg0, TDes16& aArg1, TInt aArg2);
	
	//0 args
	void DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus);

	//val, val, val
	template<class T0, class T1, class T2>
	void DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0 aArg0, T1 aArg1, T2 aArg2);
	//ptr, val, val
	template<class T0, class T1, class T2>
	void DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1 aArg1, T2 aArg2);
	//val, ptr, val
	template<class T0, class T1, class T2>
	void DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0 aArg0, T1* aArg1, T2 aArg2);
	//ptr, ptr, val
	template<class T0, class T1, class T2>
	void DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1* aArg1, T2 aArg2);
	//ptr, val, ptr
	template<class T0, class T1, class T2>
	void DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1 aArg1, T2* aArg2);
	//ptr, ptr, ptr
	template<class T0, class T1, class T2>
	void DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1* aArg1, T2* aArg2);
	
private:
	CRecordMessage::TSubSession iId;
	CRecordMessage::TSubSessionHandle iHandleId;
	const RReplaySession& iSession;
	};
	
inline CRecordMessage::TSubSession RReplaySubSession::Id() const
	{
	return iId;
	}
	
inline CRecordMessage::TSubSessionHandle RReplaySubSession::HandleId() const
	{
	return iHandleId;
	}
	
inline CRecordMessage::TSession RReplaySubSession::SessionId() const
	{
	return iSession.Id();
	}
	
inline CRecordMessage::TSession RReplaySession::Id() const
	{
	return iId;
	}
	

	
//
// NOTE: Bodies of templated methods are provided here because CodeWarrior requires the bodies of templated
// functions to be in the same compilation unit than the caller functions.
//
	

//following functions repeated for both RReplaySession and RReplaySubSession classes

//val, val, val
template<class T0, class T1, class T2>
void RReplaySession::DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0 aArg0, T1 aArg1, T2 aArg2)
	{
	TIpcArgs args(aArg0, aArg1, aArg2);
	return SendReceive(aIpc,args,aStatus);
	}
	
template<class T0, class T1, class T2>
void RReplaySubSession::DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0 aArg0, T1 aArg1, T2 aArg2)
	{
	TIpcArgs args(aArg0, aArg1, aArg2);
	return SendReceive(aIpc,args,aStatus);
	}
	
//ptr, val, val
template<class T0, class T1, class T2>
void RReplaySession::DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1 aArg1, T2 aArg2)
	{
	TIpcArgs args(aArg0, aArg1, aArg2);
	return SendReceive(aIpc,args,aStatus);	
	}
	
template<class T0, class T1, class T2>
void RReplaySubSession::DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1 aArg1, T2 aArg2)
	{
	TIpcArgs args(aArg0, aArg1, aArg2);
	return SendReceive(aIpc,args,aStatus);	
	}
	
//val, ptr, val
template<class T0, class T1, class T2>
void RReplaySession::DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0 aArg0, T1* aArg1, T2 aArg2)
	{
	TIpcArgs args(aArg0, aArg1, aArg2);
	return SendReceive(aIpc,args,aStatus);	
	}
	
template<class T0, class T1, class T2>
void RReplaySubSession::DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0 aArg0, T1* aArg1, T2 aArg2)
	{
	TIpcArgs args(aArg0, aArg1, aArg2);
	return SendReceive(aIpc,args,aStatus);	
	}
	
//ptr, ptr, val
template<class T0, class T1, class T2>
void RReplaySession::DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1* aArg1, T2 aArg2)
	{
	TIpcArgs args(aArg0, aArg1, aArg2);
	return SendReceive(aIpc,args,aStatus);	
	}
	
template<class T0, class T1, class T2>
void RReplaySubSession::DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1* aArg1, T2 aArg2)
	{
	TIpcArgs args(aArg0, aArg1, aArg2);
	return SendReceive(aIpc,args,aStatus);	
	}
	
//ptr, ptr, ptr
template<class T0, class T1, class T2>
void RReplaySession::DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1* aArg1, T2* aArg2)
	{
	TIpcArgs args(aArg0, aArg1, aArg2);
	return SendReceive(aIpc,args,aStatus);	
	}
	
template<class T0, class T1, class T2>
void RReplaySubSession::DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1* aArg1, T2* aArg2)
	{
	TIpcArgs args(aArg0, aArg1, aArg2);
	return SendReceive(aIpc,args,aStatus);	
	}
	
//ptr, val, ptr
template<class T0, class T1, class T2>
void RReplaySession::DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1 aArg1, T2* aArg2)
	{
	TIpcArgs args(aArg0, aArg1, aArg2);
	return SendReceive(aIpc,args,aStatus);	
	}
	
template<class T0, class T1, class T2>
void RReplaySubSession::DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus, T0* aArg0, T1 aArg1, T2* aArg2)
	{
	TIpcArgs args(aArg0, aArg1, aArg2);
	return SendReceive(aIpc,args,aStatus);	
	}

#endif  // _REPLAYSESSION_H_
