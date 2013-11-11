// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// replaydispatchmessage.h
// Class definitions for message dispatcher active objects.
// 
//

/**
 @file
*/

#ifndef __REPLAYDISPATCHMESSAGE_H__
#define __REPLAYDISPATCHMESSAGE_H__

#include "replaymodel.h"
#include "replaysession.h"

class CRecordMessage;

class CDispatchMessageBaseAO : public CActive
	{
public:
	virtual ~CDispatchMessageBaseAO();
	
public: //from CActive
	void DoCancel();
	
public:
	TInt StartL();
	TInt MessageId();
	
protected://to be implemented
	virtual TInt DoStartL() = 0;
	
protected:
	CDispatchMessageBaseAO(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
	void CreateRBufCopyL(RBuf8& aCopyTo, const TDesC8& aCopyFrom);
	void CreateRBufCopyL(RBuf16& aCopyTo, const TDesC8& aCopyFrom);
protected: //from CActive
	void RunL();
protected:
	void AsyncComplete(TInt aCompletionCode);
protected:
	CRecordMessage* iMessage;
	TInt iMessageId;
	CConsoleBase* iConsole;
	MModelCallback& iObserver;
	MSessionManager& iSessionManager;
	};
	
class CDispatchMessageSessionConnectAO: public CDispatchMessageBaseAO
	{
public:
	static CDispatchMessageSessionConnectAO* NewL(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
protected: //from CDispatchMessageBaseAO
	TInt DoStartL();
protected:
	CDispatchMessageSessionConnectAO(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
	};
	
class CDispatchMessageSessionCloseAO: public CDispatchMessageBaseAO
	{
public:
	static CDispatchMessageSessionCloseAO* NewL(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
protected: //from CDispatchMessageBaseAO
	TInt DoStartL();
protected:
	CDispatchMessageSessionCloseAO(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
	};
	


class CDispatchMessageFormatArgsAOBase: public CDispatchMessageBaseAO
	{
protected:
	~CDispatchMessageFormatArgsAOBase();
protected:
	CDispatchMessageFormatArgsAOBase(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
	TArgTypes CreateArgCopyL(RBuf8& aCopToBuf8, RBuf16& aCopyToBuf16, const TDesC8& aCopyFrom, CRecordMessage::TArgFlags aCopiedType);
	void SetupArgDataL(TInt& ar0LookupOffset,TInt& ar1LookupOffset,TInt& ar2LookupOffset);
protected:
	RBuf8 iArg0_8bit;
	RBuf16 iArg0_16bit;
	RBuf8 iArg1_8bit;
	RBuf16 iArg1_16bit;
	RBuf8 iArg2_8bit;
	RBuf16 iArg2_16bit;
	};
	
class CDispatchMessageSubSessionOpenAO: public CDispatchMessageFormatArgsAOBase
	{
public:
	static CDispatchMessageSubSessionOpenAO* NewL(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
protected: //from CDispatchMessageBaseAO
	TInt DoStartL();
	TInt DispatchRequest(RReplaySubSession& aSubSession, CRecordMessage::TIpc, TDispatchType aType);
protected:
	CDispatchMessageSubSessionOpenAO(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
	};
	
class CDispatchMessageFormatArgsForIpcAOBase : public CDispatchMessageFormatArgsAOBase
	{
public:
	CDispatchMessageFormatArgsForIpcAOBase(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
protected:
	template<class T0>
	void DispatchRequest(T0& aSession, CRecordMessage::TIpc, TDispatchType aType);	
	};
	
class CDispatchMessageSubSessionCloseAO: public CDispatchMessageBaseAO
	{
public:
	static CDispatchMessageSubSessionCloseAO* NewL(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
protected: //from CDispatchMessageBaseAO
	TInt DoStartL();
protected:
	CDispatchMessageSubSessionCloseAO(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
	};
		 
class CDispatchMessageSessionIpcAO: public CDispatchMessageFormatArgsForIpcAOBase
	{
public:
	static CDispatchMessageSessionIpcAO* NewL(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
protected: //from CDispatchMessageBaseAO
	TInt DoStartL();
protected:
	
protected:
	CDispatchMessageSessionIpcAO(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
	};
	
class CDispatchMessageSubSessionIpcAO: public CDispatchMessageFormatArgsForIpcAOBase
	{
public:
	static CDispatchMessageSubSessionIpcAO* NewL(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
protected: //from CDispatchMessageBaseAO
	TInt DoStartL();
protected:
	CDispatchMessageSubSessionIpcAO(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
	};
	

#endif
