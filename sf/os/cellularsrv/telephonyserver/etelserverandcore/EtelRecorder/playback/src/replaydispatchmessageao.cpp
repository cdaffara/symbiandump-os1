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
// Function definitions for message dispatcher active objects.
// 
//

/**
 @file
*/

#include "replaydispatchmessageao.h"
#include "replaymain.h"


CDispatchMessageBaseAO::~CDispatchMessageBaseAO()
	{
	Cancel();
	}
		

CDispatchMessageBaseAO::CDispatchMessageBaseAO(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
: CActive(EPriorityStandard), iMessage(aMessage), iMessageId(aMessageId), iConsole(aConsole), iObserver(aObserver), iSessionManager(aSessionManager)
	{
	CActiveScheduler::Add(this);
	}
	
void CDispatchMessageBaseAO::DoCancel()
	{
	
	}
	
TInt CDispatchMessageBaseAO::StartL()
	{
	return DoStartL();
	}
void CDispatchMessageBaseAO::RunL()
 	{
 	LOGLINE3(_L("<<< Dispatched message ID=%d completed with %d"), this->MessageId(), iStatus.Int());
	iObserver.DispatchFinished(this);
	}
	
void CDispatchMessageBaseAO::AsyncComplete(TInt aCompletionCode)
	{
	TRequestStatus *pS=(&iStatus);
	iStatus = KRequestPending;
	SetActive();
	User::RequestComplete(pS,aCompletionCode);
	}
	
void CDispatchMessageBaseAO::CreateRBufCopyL(RBuf16& aCopyTo, const TDesC8& aCopyFrom)
	{
	//copying a unicode string from a 8-bit des back into a 16-bit des so length will be half what it is in 8-bit des
	TInt len = aCopyFrom.Length();
	aCopyTo.CreateL(len/2);
	aCopyTo.SetLength(len/2);
	for(TInt i=0;i<len;i+=2)
		{
		//first byte is low, second byte is high bits
		aCopyTo[i/2] = aCopyFrom[i] | (aCopyFrom[i+1] << 8);
		}
	}
	
void CDispatchMessageBaseAO::CreateRBufCopyL(RBuf8& aCopyTo, const TDesC8& aCopyFrom)
	{
	aCopyTo.CreateL(aCopyFrom.Length());
	aCopyTo.Copy(aCopyFrom);
	}
	
TInt CDispatchMessageBaseAO::MessageId()
	{
	return iMessageId;
	}

//
CDispatchMessageSessionConnectAO* CDispatchMessageSessionConnectAO::NewL(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
	{
	CDispatchMessageSessionConnectAO* p = new(ELeave) CDispatchMessageSessionConnectAO(aConsole, aMessageId, aMessage, aObserver, aSessionManager);
	return p;
	}
	
TInt CDispatchMessageSessionConnectAO::DoStartL()
	{
	LOGLINE1(_L("Dispatching a session connect message"));
	//get id of session handle to use when creating new session in replay.
	CRecordMessage::TSessionInfo* sess = iMessage->GetSessionInfo();
	if(!sess)
		{
		ReplaySession::Panic(ReplaySession::ECouldNotFindSession);
		}
	
	//get a new session handle	
	RReplaySession& session = iSessionManager.GetNewSessionL(sess->iSession);
		
	//do the connect operation. 
	TInt ret = session.Connect();
	
	//frig the async complete
	AsyncComplete(ret);	

	return KErrNone;
	}
	
CDispatchMessageSessionConnectAO::CDispatchMessageSessionConnectAO(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
:	CDispatchMessageBaseAO(aConsole, aMessageId, aMessage, aObserver, aSessionManager)
{	
}

//

CDispatchMessageSessionCloseAO* CDispatchMessageSessionCloseAO::NewL(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
	{
	CDispatchMessageSessionCloseAO* p = new(ELeave) CDispatchMessageSessionCloseAO(aConsole, aMessageId, aMessage, aObserver, aSessionManager);
	return p;
	}
	
TInt CDispatchMessageSessionCloseAO::DoStartL()
	{
	LOGLINE1(_L("Dispatching a session close message"));
	//get id of session handle to use when creating new session in replay.
	CRecordMessage::TSessionInfo* sess = iMessage->GetSessionInfo();
	if(!sess)
		{
		ReplaySession::Panic(ReplaySession::ECouldNotFindSession);
		}
	
	//get the session handle
	TInt ret = iSessionManager.CloseAndRemoveSession(sess->iSession);	
	if(ret != KErrNone)
		{
		//session handle not found??
		ReplaySession::Panic(ReplaySession::ECouldNotFindSession);
		}
	else
		{
		//frig the async complete
		AsyncComplete(KErrNone);				
		}
	return KErrNone;
	}
	
CDispatchMessageSessionCloseAO::CDispatchMessageSessionCloseAO(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
:	CDispatchMessageBaseAO(aConsole, aMessageId, aMessage, aObserver, aSessionManager)
{	
}

//
CDispatchMessageSubSessionOpenAO* CDispatchMessageSubSessionOpenAO::NewL(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
	{
	CDispatchMessageSubSessionOpenAO* p = new(ELeave) CDispatchMessageSubSessionOpenAO(aConsole, aMessageId, aMessage, aObserver, aSessionManager);
	return p;
	}
	
TInt CDispatchMessageSubSessionOpenAO::DoStartL()
	{
	LOGLINE1(_L("Dispatching a subsession open message"));
	//get id of session handle to use when creating new session in replay.
	CRecordMessage::TSessionInfo* sess = iMessage->GetSessionInfo();
	if(!sess)
		{
		ReplaySession::Panic(ReplaySession::ECouldNotFindSession);
		}
		
	//get the session from which this will be opened
	RReplaySession* session;
	TInt ret = iSessionManager.GetSessionFromSessionId(sess->iSession, session);
	if(ret != KErrNone)
		{
		//session handle not found??
		ReplaySession::Panic(ReplaySession::ECouldNotFindSession);
		}
	else
		{
		//get a new session handle	
		RReplaySubSession& subsession = iSessionManager.GetNewSubSessionL(sess->iSubSession, sess->iSubSessionHandle, *session);
		
		//get arg data to be used in the open
		TInt ar0LookupOffset = ENull;
		TInt ar1LookupOffset = ENull;
		TInt ar2LookupOffset = ENull;
		SetupArgDataL(ar0LookupOffset, ar1LookupOffset, ar2LookupOffset);
		//perform the subsession request
		ret = DispatchRequest(subsession, iMessage->GetMessageHeader()->iIpc, KDispatchTypeLookup[ar0LookupOffset][ar1LookupOffset][ar2LookupOffset]);		
		}	
	
	//frig the async complete
	AsyncComplete(ret);	

	return KErrNone;
	}
	
CDispatchMessageSubSessionOpenAO::CDispatchMessageSubSessionOpenAO(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
:	CDispatchMessageFormatArgsAOBase(aConsole, aMessageId, aMessage, aObserver, aSessionManager)
{	
}

TInt CDispatchMessageSubSessionOpenAO::DispatchRequest(RReplaySubSession& aSubSession, CRecordMessage::TIpc aIpc, TDispatchType aType)
	{
	TBool bUnsupported  = ETrue;
	TInt ret = KErrNone;
	
	//these should all really be filled in  - but I will do them when I come across the need for a particular combination of types.
	switch(aType)
		{
		case EIsaNullNullNull:
		break;
		case EIsaNullNullDes8:
		break;
		case EIsaNullNullDes16:
		break;
		case EIsaNullNullInt:
		break;
		case EIsaNullDes8Null:
		break;
		case EIsaNullDes8Des8:
		break;
		case EIsaNullDes8Des16:
		break;
		case EIsaNullDes8Int:
		break;
		case EIsaNullDes16Null:
		break;
		case EIsaNullDes16Des8:
		break;
		case EIsaNullDes16Des16:
		break;
		case EIsaNullDes16Int:
		break;
		case EIsaNullIntNull:
		break;
		case EIsaNullIntDes8:
		break;
		case EIsaNullIntDes16:
		break;
		case EIsaNullIntInt:
		break;	
		case EIsaDes8NullNull:
		break;
		case EIsaDes8NullDes8:
		break;
		case EIsaDes8NullDes16:
		break;
		case EIsaDes8NullInt:
		break;
		case EIsaDes8Des8Null:
		break;
		case EIsaDes8Des8Des8:
		break;
		case EIsaDes8Des8Des16:
		break;
		case EIsaDes8Des8Int:
		break;
		case EIsaDes8Des16Null:
		break;
		case EIsaDes8Des16Des8:
		break;
		case EIsaDes8Des16Des16:
		break;
		case EIsaDes8Des16Int:
		break;
		case EIsaDes8IntNull:
		break;
		case EIsaDes8IntDes8:
		break;
		case EIsaDes8IntDes16:
		break;
		case EIsaDes8IntInt:
		break;		
		case EIsaDes16NullNull:
		break;
		case EIsaDes16NullDes8:
		break;
		case EIsaDes16NullDes16:
		break;
		case EIsaDes16NullInt:
			ret = aSubSession.Open(aIpc, iArg0_16bit, TIpcArgs::ENothing, *reinterpret_cast<TInt*>(const_cast<TUint8*>(iArg2_8bit.Ptr())));
			bUnsupported = EFalse;
		break;
		case EIsaDes16Des8Null:
		break;
		case EIsaDes16Des8Des8:
		break;
		case EIsaDes16Des8Des16:
		break;
		case EIsaDes16Des8Int:
		break;
		case EIsaDes16Des16Null:
			ret = aSubSession.Open(aIpc, iArg0_16bit, iArg1_16bit, TIpcArgs::ENothing);
			bUnsupported = EFalse;
		break;
		case EIsaDes16Des16Des8:
		break;
		case EIsaDes16Des16Des16:
		break;
		case EIsaDes16Des16Int:
			ret = aSubSession.Open(aIpc, iArg0_16bit, iArg1_16bit, *reinterpret_cast<TInt*>(const_cast<TUint8*>(iArg2_8bit.Ptr())));
			bUnsupported = EFalse;
		break;
		case EIsaDes16IntNull:
		break;
		case EIsaDes16IntDes8:
		break;
		case EIsaDes16IntDes16:
		break;
		case EIsaDes16IntInt:
		break;		
		case EIsaIntNullNull:
		break;
		case EIsaIntNullDes8:
		break;
		case EIsaIntNullDes16:
		break;
		case EIsaIntNullInt:
		break;
		case EIsaIntDes8Null:
		break;
		case EIsaIntDes8Des8:
		break;
		case EIsaIntDes8Des16:
		break;
		case EIsaIntDes8Int:
		break;
		case EIsaIntDes16Null:
		break;
		case EIsaIntDes16Des8:
		break;
		case EIsaIntDes16Des16:
		break;
		case EIsaIntDes16Int:
		break;
		case EIsaIntIntNull:
		break;
		case EIsaIntIntDes8:
		break;
		case EIsaIntIntDes16:
		break;
		case EIsaIntIntInt:
		break;
		default:
		break;
		};
		
	if(bUnsupported)
		{
		//not supported yet!!
		LOGLINE2(_L("WARNING: open subsession dispatch request of type %d not yet supported."), aType);
		Panic(ReplaySession::EUnsupportedRequestType);
		}
		
	return ret;
	}

//
CDispatchMessageSubSessionCloseAO* CDispatchMessageSubSessionCloseAO::NewL(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
	{
	CDispatchMessageSubSessionCloseAO* p = new(ELeave) CDispatchMessageSubSessionCloseAO(aConsole, aMessageId, aMessage, aObserver, aSessionManager);
	return p;
	}
	
TInt CDispatchMessageSubSessionCloseAO::DoStartL()
	{
	LOGLINE1(_L("Dispatching a sub session close message"));
	//get id of session handle to use when creating new session in replay.
	CRecordMessage::TSessionInfo* sess = iMessage->GetSessionInfo();
	if(!sess)
		{
		ReplaySession::Panic(ReplaySession::ECouldNotFindSession);
		}
	
	//get the sub session handle
	TInt ret = iSessionManager.CloseAndRemoveSubSession(sess->iSession, sess->iSubSession, sess->iSubSessionHandle);
	if(ret != KErrNone)
		{
		//session handle not found??
		ReplaySession::Panic(ReplaySession::ECouldNotFindSubSession);
		}
	else
		{
		//frig the async complete
		AsyncComplete(KErrNone);				
		}
	return KErrNone;
	}

CDispatchMessageSubSessionCloseAO::CDispatchMessageSubSessionCloseAO(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
:	CDispatchMessageBaseAO(aConsole, aMessageId, aMessage, aObserver, aSessionManager)
{	
}

//
	
CDispatchMessageFormatArgsAOBase::CDispatchMessageFormatArgsAOBase(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
:	CDispatchMessageBaseAO(aConsole, aMessageId, aMessage, aObserver, aSessionManager)
{	
}

CDispatchMessageFormatArgsAOBase::~CDispatchMessageFormatArgsAOBase()
	{
	iArg0_8bit.Close();
	iArg0_16bit.Close();
	iArg1_8bit.Close();
	iArg1_16bit.Close();
	iArg2_8bit.Close();
	iArg2_16bit.Close();
	}
	
void CDispatchMessageFormatArgsAOBase::SetupArgDataL(TInt& ar0LookupOffset,TInt& ar1LookupOffset,TInt& ar2LookupOffset)
	{
	//find out what the message args are and put them into the correct format.
	CRecordMessage::TMessageHeader* header = iMessage->GetMessageHeader();
	if(!header)
		{
		ReplaySession::Panic(ReplaySession::ECouldNotFindHeader);
		}
	
	TPtrC8 argData(KNullDesC8);
	//arg0
	if(header->iArg0Info.iArgFlags & CRecordMessage::KArgValid)
		{
		iMessage->GetArg(0, argData);
		if (argData == KNullDesC8)
			{
			LOGLINE1(_L("WARNING: arg0 is NULL"));
			}
		ar0LookupOffset = CreateArgCopyL(iArg0_8bit, iArg0_16bit, argData, (CRecordMessage::TArgFlags)(header->iArg0Info.iArgFlags & ~CRecordMessage::KArgValid));				
		}
	//arg1
	argData.Set(KNullDesC8);
	if(header->iArg1Info.iArgFlags & CRecordMessage::KArgValid)
		{
		iMessage->GetArg(1, argData);
		if (argData == KNullDesC8)
			{
			LOGLINE1(_L("WARNING: arg1 is NULL"));
			}		
		ar1LookupOffset = CreateArgCopyL(iArg1_8bit, iArg1_16bit, argData, (CRecordMessage::TArgFlags)(header->iArg1Info.iArgFlags & ~CRecordMessage::KArgValid));					
		}
	//arg2
	argData.Set(KNullDesC8);
	if(header->iArg2Info.iArgFlags & CRecordMessage::KArgValid)
		{
		iMessage->GetArg(2, argData);
		if (argData == KNullDesC8)
			{
			LOGLINE1(_L("WARNING: arg2 is NULL"));
			}			
		ar2LookupOffset = CreateArgCopyL(iArg2_8bit, iArg2_16bit, argData, (CRecordMessage::TArgFlags)(header->iArg2Info.iArgFlags & ~CRecordMessage::KArgValid));				
		}
		//I don't think Arg3 is ever used.	
	}

TArgTypes CDispatchMessageFormatArgsAOBase::CreateArgCopyL(RBuf8& aCopToBuf8, RBuf16& aCopyToBuf16, const TDesC8& aCopyFrom, CRecordMessage::TArgFlags aCopiedType)
	{
	if(aCopiedType == CRecordMessage::KArgTypeIsDes16)
		{
		//copy to 16-bit buffer
		CreateRBufCopyL(aCopyToBuf16, aCopyFrom);
		}
	else
		{
		//copy to 8-bit buffer
		CreateRBufCopyL(aCopToBuf8, aCopyFrom);
		}
	
	//returns  the type of the arg as an index into the lookup table
	TArgTypes ret;
	switch(aCopiedType)
		{
		case CRecordMessage::KArgTypeIsDes8:
		ret = EDes8;
		break;
		case CRecordMessage::KArgTypeIsDes16:
		ret = EDes16;
		break;
		case CRecordMessage::KArgTypeIsInt: 
		ret = EInt;		
		break;
		default:
		LOGLINE2(_L("WARNING: unrecognised type, cannot match to lookup table type = %d"), aCopiedType);
		ret = ENull;
		
		}
		
	return ret;
	}
	
//

CDispatchMessageFormatArgsForIpcAOBase::CDispatchMessageFormatArgsForIpcAOBase(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
: CDispatchMessageFormatArgsAOBase(aConsole, aMessageId, aMessage, aObserver, aSessionManager)
	{
	
	
	}
	
template<class T0>
void CDispatchMessageFormatArgsForIpcAOBase::DispatchRequest(T0& aSession, CRecordMessage::TIpc aIpc, TDispatchType aType)
	{
	//these should all really be filled in  - but I will do them when I come across the need for a particular combination of types.
	TBool bUnsupported  = ETrue;
	switch(aType)
		{
		case EIsaNullNullNull:
			aSession.DispatchAsync(aIpc, iStatus);
			bUnsupported = EFalse;
		break;
		case EIsaNullNullDes8:
		break;
		case EIsaNullNullDes16:
		break;
		case EIsaNullNullInt:
		break;
		case EIsaNullDes8Null:
		break;
		case EIsaNullDes8Des8:
		break;
		case EIsaNullDes8Des16:
		break;
		case EIsaNullDes8Int:
		break;
		case EIsaNullDes16Null:
		break;
		case EIsaNullDes16Des8:
		break;
		case EIsaNullDes16Des16:
		break;
		case EIsaNullDes16Int:
		break;
		case EIsaNullIntNull:	
			aSession.DispatchAsync(aIpc, iStatus, TIpcArgs::ENothing, *reinterpret_cast<TInt*>(const_cast<TUint8*>(iArg1_8bit.Ptr())), TIpcArgs::ENothing);
			bUnsupported = EFalse;
		break;
		case EIsaNullIntDes8:
		break;
		case EIsaNullIntDes16:
		break;
		case EIsaNullIntInt:
		break;
	
		case EIsaDes8NullNull:
			aSession.DispatchAsync(aIpc, iStatus, &iArg0_8bit, TIpcArgs::ENothing, TIpcArgs::ENothing);
			bUnsupported = EFalse;
		break;
		case EIsaDes8NullDes8:
		break;
		case EIsaDes8NullDes16:
		break;
		case EIsaDes8NullInt:
		break;
		case EIsaDes8Des8Null:
		break;
		case EIsaDes8Des8Des8:
		break;
		case EIsaDes8Des8Des16:
		break;
		case EIsaDes8Des8Int:
		break;
		case EIsaDes8Des16Null:
		break;
		case EIsaDes8Des16Des8:
		break;
		case EIsaDes8Des16Des16:
		break;
		case EIsaDes8Des16Int:
		break;
		case EIsaDes8IntNull:
			aSession.DispatchAsync(aIpc, iStatus, &iArg0_8bit, *reinterpret_cast<TInt*>(const_cast<TUint8*>(iArg1_8bit.Ptr())), TIpcArgs::ENothing);
			bUnsupported = EFalse;
		break;
		case EIsaDes8IntDes8:
			aSession.DispatchAsync(aIpc, iStatus, &iArg0_8bit, *reinterpret_cast<TInt*>(const_cast<TUint8*>(iArg1_8bit.Ptr())), &iArg2_8bit);
			bUnsupported = EFalse;
		break;
		case EIsaDes8IntDes16:
			aSession.DispatchAsync(aIpc, iStatus, &iArg0_8bit, *reinterpret_cast<TInt*>(const_cast<TUint8*>(iArg1_8bit.Ptr())), &iArg2_16bit);
			bUnsupported = EFalse;
		break;
		case EIsaDes8IntInt:
		break;
		
		case EIsaDes16NullNull:
			aSession.DispatchAsync(aIpc, iStatus, &iArg0_16bit, TIpcArgs::ENothing, TIpcArgs::ENothing);
			bUnsupported = EFalse;
			break;
		case EIsaDes16NullDes8:
		break;
		case EIsaDes16NullDes16:
		break;
		case EIsaDes16NullInt:
		break;
		case EIsaDes16Des8Null:
			aSession.DispatchAsync(aIpc, iStatus, &iArg0_16bit, &iArg1_8bit, TIpcArgs::ENothing);
			bUnsupported = EFalse;
		break;
		case EIsaDes16Des8Des8:
			aSession.DispatchAsync(aIpc, iStatus, &iArg0_16bit, &iArg1_8bit, &iArg2_8bit);
			bUnsupported = EFalse;
		break;
		case EIsaDes16Des8Des16:
		break;
		case EIsaDes16Des8Int:
		break;
		case EIsaDes16Des16Null:
		break;
		case EIsaDes16Des16Des8:
		break;
		case EIsaDes16Des16Des16:
		break;
		case EIsaDes16Des16Int:
		break;
		case EIsaDes16IntNull:
			aSession.DispatchAsync(aIpc, iStatus, &iArg0_16bit, *reinterpret_cast<TInt*>(const_cast<TUint8*>(iArg1_8bit.Ptr())), TIpcArgs::ENothing);
			bUnsupported = EFalse;
		break;
		case EIsaDes16IntDes8:
		break;
		case EIsaDes16IntDes16:
			aSession.DispatchAsync(aIpc, iStatus, &iArg0_16bit, *reinterpret_cast<TInt*>(const_cast<TUint8*>(iArg1_8bit.Ptr())), &iArg2_16bit);
			bUnsupported = EFalse;
		break;
		case EIsaDes16IntInt:
		break;
		
		case EIsaIntNullNull:
			aSession.DispatchAsync(aIpc, iStatus, *reinterpret_cast<TInt*>(const_cast<TUint8*>(iArg0_8bit.Ptr())), TIpcArgs::ENothing, TIpcArgs::ENothing);
			bUnsupported = EFalse;
		break;
		case EIsaIntNullDes8:
		break;
		case EIsaIntNullDes16:
		break;
		case EIsaIntNullInt:
		break;
		case EIsaIntDes8Null:
		break;
		case EIsaIntDes8Des8:
		break;
		case EIsaIntDes8Des16:
		break;
		case EIsaIntDes8Int:
		break;
		case EIsaIntDes16Null:
			aSession.DispatchAsync(aIpc, iStatus, *reinterpret_cast<TInt*>(const_cast<TUint8*>(iArg0_8bit.Ptr())), &iArg1_16bit, TIpcArgs::ENothing);
			bUnsupported = EFalse;
		break;
		case EIsaIntDes16Des8:
		break;
		case EIsaIntDes16Des16:
		break;
		case EIsaIntDes16Int:
		break;
		case EIsaIntIntNull:
			aSession.DispatchAsync(aIpc, iStatus, *reinterpret_cast<TInt*>(const_cast<TUint8*>(iArg0_8bit.Ptr())), *reinterpret_cast<TInt*>(const_cast<TUint8*>(iArg1_8bit.Ptr())), TIpcArgs::ENothing);
			bUnsupported = EFalse;
		break;
		case EIsaIntIntDes8:
		break;
		case EIsaIntIntDes16:
		break;
		case EIsaIntIntInt:
		break;
		default:
		break;
		};
		
	if(bUnsupported)
		{
		//not supported yet!!
		LOGLINE2(_L("WARNING: dispatch request of type %d not yet supported."), aType);
		Panic(ReplaySession::EUnsupportedRequestType);
		}
	}

	
//
CDispatchMessageSessionIpcAO* CDispatchMessageSessionIpcAO::NewL(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
	{
	CDispatchMessageSessionIpcAO* p = new(ELeave) CDispatchMessageSessionIpcAO(aConsole, aMessageId, aMessage, aObserver, aSessionManager);
	return p;
	}
	
TInt CDispatchMessageSessionIpcAO::DoStartL()
	{
	LOGLINE1(_L("Dispatching a session IPC message"));
	
	//get id of session handle to use when creating new session in replay.
	CRecordMessage::TSessionInfo* sess = iMessage->GetSessionInfo();
	if(!sess)
		{
		ReplaySession::Panic(ReplaySession::ECouldNotFindSession);
		}
	
	//get a new session handle	
	RReplaySession* session;
	TInt ret = iSessionManager.GetSessionFromSessionId(sess->iSession, session);
	if(ret != KErrNone)
		{
		//session handle not found??
		ReplaySession::Panic(ReplaySession::ECouldNotFindSession);
		}
	else
		{
		TInt ar0LookupOffset = ENull;
		TInt ar1LookupOffset = ENull;
		TInt ar2LookupOffset = ENull;
		SetupArgDataL(ar0LookupOffset, ar1LookupOffset, ar2LookupOffset);		
		//perform the subsession request
		DispatchRequest(*session, iMessage->GetMessageHeader()->iIpc, KDispatchTypeLookup[ar0LookupOffset][ar1LookupOffset][ar2LookupOffset]);		
		SetActive();
		}
	return KErrNone;
	}
	
CDispatchMessageSessionIpcAO::CDispatchMessageSessionIpcAO(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
:	CDispatchMessageFormatArgsForIpcAOBase(aConsole, aMessageId, aMessage, aObserver, aSessionManager)
{	
}

//
CDispatchMessageSubSessionIpcAO* CDispatchMessageSubSessionIpcAO::NewL(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
	{
	CDispatchMessageSubSessionIpcAO* p = new(ELeave) CDispatchMessageSubSessionIpcAO(aConsole, aMessageId, aMessage, aObserver, aSessionManager);
	return p;
	}
	
TInt CDispatchMessageSubSessionIpcAO::DoStartL()
	{
	LOGLINE1(_L("Dispatching a subsession IPC message"));

	
	//get id of session handle to use when creating new session in replay.
	CRecordMessage::TSessionInfo* sess = iMessage->GetSessionInfo();
	if(!sess)
		{
		ReplaySession::Panic(ReplaySession::ECouldNotFindSubSession);
		}
	
	//get a new session handle	
	RReplaySubSession* session;
	TInt ret = iSessionManager.GetSubSessionFromSubSessionHandle(sess->iSession, sess->iSubSession, sess->iSubSessionHandle,session);
	if(ret != KErrNone)
		{
		//session handle not found??
		ReplaySession::Panic(ReplaySession::ECouldNotFindSubSession);
		}
	else
		{
		TInt ar0LookupOffset = ENull;
		TInt ar1LookupOffset = ENull;
		TInt ar2LookupOffset = ENull;
		SetupArgDataL(ar0LookupOffset, ar1LookupOffset, ar2LookupOffset);		
		//perform the subsession request
		DispatchRequest(*session, iMessage->GetMessageHeader()->iIpc, KDispatchTypeLookup[ar0LookupOffset][ar1LookupOffset][ar2LookupOffset]);		
		SetActive();
		}
	return KErrNone;
	}
	
CDispatchMessageSubSessionIpcAO::CDispatchMessageSubSessionIpcAO(CConsoleBase* aConsole, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
:	CDispatchMessageFormatArgsForIpcAOBase(aConsole, aMessageId, aMessage, aObserver, aSessionManager)
{	
}
