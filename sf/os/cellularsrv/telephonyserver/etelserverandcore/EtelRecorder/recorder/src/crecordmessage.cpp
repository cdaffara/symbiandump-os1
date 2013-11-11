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
//

#include "et_record.h"
#ifdef ETEL_RECORDER

#include "crecordmessage.h"

#include <et_clsvr.h>
#include "crecordmessageimpl.h"

CRecordMessage* CRecordMessage::NewLC()
	{
	CRecordMessage* me = new(ELeave)CRecordMessage();
	CleanupStack::PushL(me);
	me->ConstructL();
	return me;
	}
	
CRecordMessage::~CRecordMessage()
	{
	delete iMessage;
	}

CRecordMessage::CRecordMessage()
	{
	}
	
void CRecordMessage::ConstructL()
	{
	iMessage = CRecordMessageImpl::NewL();
	}

 void CRecordMessage::LogRecordHeader()
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogRecordHeaderL());
	}
	
 void CRecordMessage::LogNewSession(const CSession2* aSession)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogNewSessionL(aSession));
	}
	
 void CRecordMessage::LogCloseSession(const CSession2* aSession)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogCloseSessionL(aSession));
	}
	
 void CRecordMessage::LogNewSubSession(const CSession2* aSession, TInt aIpc, const CTelObject* aSubSession, TInt aSubSessionHandle, const TDesC& aArg0Data, TArgAllocBufFromLen aArg1Data, TInt aArg2Data)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogNewSubSessionL( aSession, aIpc, aSubSession, aSubSessionHandle, aArg0Data, aArg1Data, aArg2Data));
	}
	
 void CRecordMessage::LogCloseSubSession(const CSession2* aSession, TInt aIpc, const CTelObject* aSubSession, TInt aSubSessionHandle)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogCloseSubSessionL(aSession, aIpc, aSubSession, aSubSessionHandle));
	}

 void CRecordMessage::LogMessageComplete(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TInt aIpc, TInt aStatus)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogMessageCompleteL(aSession, aSubSession, aSubSessionHandle, aIpc, aStatus));
	}
	
 void CRecordMessage::LogMessageComplete(const CSession2* aSession, TInt aIpc, TInt aStatus)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogMessageCompleteL(aSession, aIpc, aStatus));
	}
		

//0 args only
 void CRecordMessage::LogIpc(const CSession2* aSession, TUint32 aIpc)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aIpc));
	}

//1 args only	
 void CRecordMessage::LogIpc(const CSession2* aSession, TUint32 aIpc, const TDesC& aArg0Data)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aIpc, aArg0Data));
	}
	
 void CRecordMessage::LogIpc(const CSession2* aSession, TUint32 aIpc, TInt aArg0Data)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aIpc, aArg0Data));
	}
	
 void CRecordMessage::LogIpc( const CSession2* aSession, TUint32 aIpc, const TDesC8& aArg0Data)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aIpc, aArg0Data));
	}
	
//2 args only	
 void CRecordMessage::LogIpc( const CSession2* aSession, TUint32 aIpc, const TDesC8& aArg0Data, TInt aArg1Data)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aIpc, aArg0Data, aArg1Data));
	}
	
 void CRecordMessage::LogIpc(const CSession2* aSession, TUint32 aIpc, TInt aArg0Data, TArgAllocBufFromLen aArg1Data)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aIpc, aArg0Data, aArg1Data));
	}
	
 void CRecordMessage::LogIpc(const CSession2* aSession, TUint32 aIpc, const TDes& aArg0Data, const TDes8& aArg1Data)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aIpc, aArg0Data, aArg1Data));
	}
	
//3 args
 void CRecordMessage::LogIpc(const CSession2* aSession, TUint32 aIpc, const TDesC& aArg0Data, const TDesC8& aArg1Data, const TDesC8& aArg2Data)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aIpc, aArg0Data, aArg1Data, aArg2Data));
	}
	
//subsession logs

//1 arg only
 void CRecordMessage::LogIpc(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data)
	{
	//NOTE: This is deliberately aArg1Data first because for all subsession ipc's the 1st arg will ALWAYS be set
	//as it is used to convey the type (isanull, isadesc etc.).  It is Arg0 or Arg2 that may be empty.
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aSubSession, aSubSessionHandle, aIpc, aArg1Data));	
	}
	
//2 args only
 void CRecordMessage::LogIpc(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, const TDesC8& aArg0Data)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aSubSession, aSubSessionHandle, aIpc, aArg1Data, aArg0Data));	
	}
	
 void CRecordMessage::LogIpc(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, const TDesC16& aArg0Data)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aSubSession, aSubSessionHandle, aIpc, aArg1Data, aArg0Data));	
	}
	
 void CRecordMessage::LogIpc(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, TInt aArg0Data)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aSubSession, aSubSessionHandle, aIpc, aArg1Data, aArg0Data));	
	}
	
//3 args only
 void CRecordMessage::LogIpc(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, const TDesC8& aArg0Data, const TDesC8& aArg2Data)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aSubSession, aSubSessionHandle, aIpc, aArg1Data, aArg0Data, aArg2Data));	
	}

 void CRecordMessage::LogIpc(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, const TDesC8& aArg0Data, const TDesC16& aArg2Data)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aSubSession, aSubSessionHandle, aIpc, aArg1Data, aArg0Data, aArg2Data));	
	}
	
 void CRecordMessage::LogIpc(const CSession2* aSession, const CTelObject* aSubSession, TInt aSubSessionHandle, TUint32 aIpc, TInt aArg1Data, const TDesC16& aArg0Data, const TDesC16& aArg2Data)
	{
	TRAP_IGNORE(CRecordMessageImpl::DoLogIpcL(aSession, aSubSession, aSubSessionHandle, aIpc, aArg1Data, aArg0Data, aArg2Data));	
	}
	
 CRecordMessage::TMessageHeader* CRecordMessage::GetMessageHeader()
	{
	if(iMessage)
		return iMessage->DoGetMessageHeader();
	else
		return NULL;
	}
	
 CRecordMessage::TSessionInfo* CRecordMessage::GetSessionInfo()
	{
	if(iMessage)
		return iMessage->DoGetSessionInfo();
	else
		return NULL;
	}
	
	
 void CRecordMessage::GetArg(TInt aArgIdx, TPtrC8& aData) 
	{
	if(iMessage)
		iMessage->DoGetArg(aArgIdx, aData);		
	}
	
 TInt CRecordMessage::InternalizeL(const TDesC8& aExternedMessage, TInt& aMessageSize)
	{
	if(iMessage)
		return iMessage->DoInternalizeL(aExternedMessage, aMessageSize);	
	else
		return KErrNotFound;
	}
	
 CRecordMessage::TArgAllocBufFromLen::TArgAllocBufFromLen(TInt aLen, TArgFlags aBufType)
	: iLenToAlloc(aLen), iBufType(aBufType)
	{
	}

#endif // ETEL_RECORDER
