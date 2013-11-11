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
// Function definitions for client side replayer sessions.
// 
//

/**
 @file
*/

#include "replaysession.h"
#include <etelmm.h>

//for RSession::Connect stuff
#include <e32property.h>
#include <rsshared.h>

// copied from Etel	
#define ETEL_SERVER_NAME	  _L("!EtelServer")  // EKA2 Server names need a "!" prepended


RReplaySession::RReplaySession(CRecordMessage::TSubSession aId)
: iId(aId)
	{
	}

TInt RReplaySession::Connect()
	{	
	TInt ret(KErrNone);
	ret=StartC32();
	// KUidC32StartPropertyKey (rootserver load level) will always be 10 at the completion of StartC32
	if (ret==KErrNone || ret==KErrAlreadyExists)
		{
		RProperty property;
		TInt propertyValue = 0;
		ret = property.Attach(KUidSystemCategory, RootServer::KUidC32StartPropertyKey.iUid);
		if(ret != KErrNone)
		    {
		    // most likely to have run out of memory
			return ret;
            }
		ret = property.Get(propertyValue);
		if(ret != KErrNone)
	    	{
			return ret;
        	}
		}
	
	if (ret!= KErrNone) return ret;

	TVersion version(1, 0, 606);			
	
	TInt result=CreateSession(ETEL_SERVER_NAME,version,32);
	// Because ETel is now loaded by the Comms Root Server which is generally started during
	// the boot this should commonly succeed; however for test code this is still a possibility
	// Hence here we try starting it; this is an atomic operation (and cheap if it's already started)
	if (result == KErrNotFound  ||  result == KErrServerTerminated)
		{	
		TInt ret(KErrNone);
    	ret = StartC32();
		if (ret!= KErrNone)
			{
			return ret;
			}
		// at this point, etel server shall be started
		result=CreateSession(ETEL_SERVER_NAME,version,32);
		}
		
	return KErrNone;
	}
	
//
void RReplaySubSession::Close()
	{
	CloseSubSession(EEtelClose); 
	}
	
RReplaySubSession::RReplaySubSession(CRecordMessage::TSubSession aId, CRecordMessage::TSubSessionHandle aHandleId, const RReplaySession& aSession)
: iId(aId), iHandleId(aHandleId), iSession(aSession)
	{
	
	}

//EIsaDes16Des16Null	
TInt RReplaySubSession::Open(CRecordMessage::TIpc aIpc, TDes16& aArg0, TDes16& aArg1, TIpcArgs::TNothing /*aArg2*/)
	{
	TIpcArgs args(&aArg0,&aArg1);
	return CreateSubSession(iSession,aIpc,args);
	}

//EIsaDes16NullInt	
TInt RReplaySubSession::Open(CRecordMessage::TIpc aIpc, TDes16& aArg0, TIpcArgs::TNothing aArg1, TInt aArg2)
	{
	TIpcArgs args(&aArg0,aArg1, aArg2);
	return CreateSubSession(iSession,aIpc,args);
	}
	
//EIsaDes16Des16Int	
TInt RReplaySubSession::Open(CRecordMessage::TIpc aIpc, TDes16& aArg0, TDes16& aArg1, TInt aArg2)
	{
	TIpcArgs args(&aArg0,&aArg1, aArg2);
	return CreateSubSession(iSession,aIpc,args);
	}

//following functions repeated for both RReplaySession and RReplaySubSession classes	

//null, null, null	
void RReplaySession::DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus)
	{
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(aIpc,args,aStatus);	
	}
	
void RReplaySubSession::DispatchAsync(CRecordMessage::TIpc aIpc, TRequestStatus& aStatus)
	{
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(aIpc,args,aStatus);	
	}
