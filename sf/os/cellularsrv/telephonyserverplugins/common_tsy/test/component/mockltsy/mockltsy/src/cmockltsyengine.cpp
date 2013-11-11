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
//

#include "cmockltsyengine.h"
#include "mmockmesshandlerbase.h"
#include "cmmmessagerouter.h"
#include "tmockltsydata.h"
#include "ipcnames.h"
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>

/**
Factory function
*/
CMockLtsyEngine* CMockLtsyEngine::NewL(CMmMessageRouter* aMsgRouter)
	{
	CMockLtsyEngine* self =  new (ELeave)CMockLtsyEngine(aMsgRouter);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructor
*/	
CMockLtsyEngine::CMockLtsyEngine(CMmMessageRouter* aMsgRouter)
	: iMsgRouter(aMsgRouter)
	{
	
	}
	
/**
Generate a completion
*/
void CMockLtsyEngine::DoCompleteEventL(const TMockSYEvent& aEvent)
	{
	MMockMessHandlerBase* handler;
	handler = iMsgRouter->RouteRequest(aEvent.iCmdId);
	ASSERT(handler!=NULL);
	handler->CompleteL(aEvent.iCmdId, aEvent.iData, aEvent.iResultCode);
	}


/**
*/
void CMockLtsyEngine::LogData(const TDesC& aDesc)
	{
	Log(aDesc);
	}


/**
*/
void CMockLtsyEngine::LogRequest(TInt aCmdId, const MMockSyEngineData& /*aData*/,TInt aResultCode)
	{
	TBuf<KMaxLogLineSize> buffer;
	const TDesC &ipcName = IpcName(aCmdId);
	if(ipcName != KNullDesC)
		buffer.Format(_L(">>> ipc=%S Err=%d"),&ipcName, aResultCode);
	else
		buffer.Format(_L(">>> ipc=%d (unmapped ipc) Err=%d"),aCmdId, aResultCode);		
	Log(buffer);
	}

/**
*/
void CMockLtsyEngine::LogCompletion(TInt aCmdId, const TDesC8& /*aData*/,TInt aResultCode)
	{
	TBuf<KMaxLogLineSize> buffer;
	const TDesC& ipcName = IpcName(aCmdId);
	if(ipcName != KNullDesC)
		buffer.Format(_L("<<< ipc=%S Err=%d"), &ipcName, aResultCode);
	else
		buffer.Format(_L("<<< ipc=%d (unmapped ipc) Err=%d"), aCmdId, aResultCode);
	Log(buffer);	
	}

/**
*/
void CMockLtsyEngine::LogExpectError(TInt aCmdId, const MMockSyEngineData& aData,
	TInt aExpectedCmd,const TDesC8& aExpectedData, TBool aIsErrorIgnored)
	{
	TBuf<KMaxLogLineSize> buffer;
	const TDesC& ipcName = IpcName(aCmdId);
	
	if(!aIsErrorIgnored)
		buffer.Copy(_L("ERROR: "));
	else
		buffer.Copy(_L("IGNORED: "));
	
	if (aExpectedCmd != 0)
		{
		const TDesC& expectedIpcName = IpcName(aExpectedCmd);
		if(ipcName != KNullDesC)
			{
			if (expectedIpcName != KNullDesC)
				buffer.AppendFormat(_L("Expected ipc=%S Received ipc=%S"), &expectedIpcName, &ipcName);
			else
				buffer.AppendFormat(_L("Expected ipc=%d (unmapped ipc) Received ipc=%S"), aExpectedCmd, &ipcName);
			}
		else
			{
			if (expectedIpcName != KNullDesC)
				buffer.AppendFormat(_L("Expected ipc=%S Received ipc=%d (unmapped ipc)"),&expectedIpcName,aCmdId);
			else
				buffer.AppendFormat(_L("Expected ipc=%d (unmapped ipc) Received ipc=%d (unmapped ipc)"),aExpectedCmd,aCmdId);
			}
		Log(buffer);	
		if (aExpectedCmd == aCmdId)
			{
			MMockMessHandlerBase* handler = iMsgRouter->RouteRequest(aCmdId);
			handler->LogErrorData(aExpectedCmd, aData, aExpectedData);
			}
		}
	else
		{
		if( ipcName != KNullDesC)
			buffer.AppendFormat(_L("Expected ipc=NONE Received ipc=%S"), &ipcName);
		else
			buffer.AppendFormat(_L("Expected ipc=NONE Received ipc=%d (unmapped ipc)"), aCmdId);
		Log(buffer);	
		}
	}


TBool CMockLtsyEngine::CanIgnoreUnexpectedIpc(TInt aCmdId)
	{
	// CTSY sends EMmTsyUpdateLifeTimeIPC request to LTSY every 10sec. If a test case making call lasts
	// more than 10 sec, Mockltsy should be informed that EMmTsyUpdateLifeTimeIPC will be received at some point.
	// However, some test cases take more than 10 sec on some hardwares and less than 10sec on the other hardwares.
	// In this case, it is hardly possible to keep Mockltsy posted. Therefore, it is assumed that EMmTsyUpdateLifeTimeIPC
	// may received by Mockltsy at unexpected times and this doesn't indicate any error.
	TBool canIgnore = EFalse;

    if(EMmTsyUpdateLifeTimeIPC == aCmdId)
		{
		canIgnore = ETrue;
		}
	return canIgnore;
	}
