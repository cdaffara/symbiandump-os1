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
// MockSY engine for Hwrm plugin (Light and Vibra)
// 
//

#include "cmockhwrmpluginengine.h"
#include "cmockhwrmpluginsy.h"
#include "tmockhwrmplugindata.h"

#include <hwrm/HWRMLightCommands.h>

/**
Factory function
*/

/**
Constructor
*/	
CMockHwrmPluginEngine::CMockHwrmPluginEngine(CMockHwrmPluginSY& aMockHwrmPluginSY)
	:iMockHwrmPluginSY(aMockHwrmPluginSY)
	{
	}

/**
Queue an event
*/
void CMockHwrmPluginEngine::DoQueueEventL(TMockSYEvent::TEventType aType, TInt aCmdId, HBufC8* aData, 
		TInt aResultCode, TBool aLeave, TInt aDelay)
	{
	if(aType == TMockSYEvent::EMessage)
        {   
	    iNumStarted++; // got a  event queued by the client
        }
	CMockSYEngine::DoQueueEventL(aType,aCmdId, aData,aResultCode,aLeave,aDelay);
	}

/**
Handler for the SY: handle ProcessCommand. Return KErrCorrupt if it's not the expected command
*/
TInt CMockHwrmPluginEngine::ExecuteProcessCommandL(const TInt aCommandId, const TUint8 aTransId, TDesC8& aData)
	{
	if (iNumStarted == 0)
		{
		QueueProcessedEventL(aCommandId,aTransId,aData);
		}
    else
        {
        iNumStarted--;
        CMockSYEngine::CheckAndUpdateTransId(aTransId, aCommandId);
        }
    
	iLatestTransId = aTransId;
	TMockHwrmPluginData commandData(aTransId,aData);
	TInt err = ExecuteCommandL(aCommandId, commandData);
	return err;
	}
	
/**
Handler for the SY: handle CancelCommand. Return KErrCorrupt if it's not the expected command
*/
TInt CMockHwrmPluginEngine::ExecuteCancelCommandL(const TUint8 aTransId, const TInt aCommandId)
	{
	iLatestTransId = aTransId;
	TMockHwrmPluginData commandData(aTransId);
	TInt err = ExecuteCommandL(-aCommandId, commandData);
	return err;
	}

/**
Generate a completion event
*/
void CMockHwrmPluginEngine::DoCompleteEventL(const TMockSYEvent& aEvent)
	{
	TMockHwrmPluginData completeData;
	completeData.DeserialiseL(aEvent.iData);
	if (completeData.HasTransId())
		{
		// client provide a trans id, use it
 		iMockHwrmPluginSY.DoProcessResponse(aEvent.iCmdId, completeData.TransId(), completeData.CommandData());
		}
	else
		{
		// client didn't provide a trans id, the trans id of the latest command
 		iMockHwrmPluginSY.DoProcessResponse(aEvent.iCmdId, iLatestTransId, completeData.CommandData());
		}
	}

/**
*/
void CMockHwrmPluginEngine::Reset()
	{
	CMockSYEngine::Reset();
	iNumStarted = 0;
	}

/**
*/
void CMockHwrmPluginEngine::QueueProcessedEventL(const TInt aCommandId, const TUint8 aTransId, TDesC8& aData)
	{
	// Request
	RBuf8 buffer;
	
	// Request
	
	FurtherProcessEventL(aCommandId, aData);

	TMockHwrmPluginData reqData(aTransId,aData);
	reqData.SerialiseL(buffer);
	// copy serialised data to a HBufC8 pointer
	HBufC8* req = HBufC8::New(buffer.Length());
	TPtr8 reqPtr = req->Des();
	reqPtr = buffer;
	// queue that as expected request (take ownership of req)
	CMockSYEngine::DoQueueEventL(TMockSYEvent::EMessage,aCommandId,req,KErrNone,EFalse,0);
	buffer.Close();
	
	QueueProcessedCompletionL(aCommandId, aTransId, buffer, aData);
	// copy serialised data to a HBufC8 pointer
	HBufC8* resp = HBufC8::New(buffer.Length());
	TPtr8 respPtr = resp->Des();
	respPtr = buffer;
	// queue that as response (take ownership of req)
	CMockSYEngine::DoQueueEventL(TMockSYEvent::ECompletion,aCommandId,resp,KErrNone,EFalse,1);
	buffer.Close();
	}

/**
*/
void CMockHwrmPluginEngine::LogRequest(TInt aCmdId, const MMockSyEngineData& aData,TInt aResultCode)
	{
	const TMockHwrmPluginData& data = static_cast<const TMockHwrmPluginData&>(aData);
	TBuf<KMaxLogLineSize> buffer;
	buffer.Format(_L(">>> Cmd=%d, TransId=%d, Err=%d"),aCmdId, data.TransId(), aResultCode);
	Log(buffer);	
	}

/**
*/
void CMockHwrmPluginEngine::LogCompletion(TInt aCmdId, const TDesC8& aData,TInt aResultCode)
	{
	TMockHwrmPluginData data;
	data.DeserialiseL(aData);
	TBuf<KMaxLogLineSize> buffer;
	buffer.Format(_L("<<< Cmd=%d, TransId=%d, Err=%d"),aCmdId, data.TransId(),aResultCode);
	Log(buffer);	
	}

/**
*/
void CMockHwrmPluginEngine::LogExpectError(TInt aCmdId, const MMockSyEngineData& aData,
	TInt aExpectedCmd,const TDesC8& aExpectedData)
	{
	const TMockHwrmPluginData& data = static_cast<const TMockHwrmPluginData&>(aData);
	TMockHwrmPluginData expectedData;
	expectedData.DeserialiseL(aExpectedData);
	TBuf<KMaxLogLineSize> buffer;
	buffer.Format(_L("ERROR: Expected Cmd=%d, TransId=%d Received Cmd=%d,TransId=%d, "),
		aExpectedCmd, expectedData.TransId(), aCmdId, data.TransId());
	Log(buffer);	
	}

void CMockHwrmPluginEngine::QueueProcessedCompletionL(const TInt /*aCommandId*/, const TUint8 aTransId, RBuf8& aBuffer, TDesC8& /*aData*/)
	{
	//default response is an error-code message which is the same for light,fmtx,vibra
	HWRMLightCommand::TErrorCodeResponsePackage resultPckg(KErrNone);
	TMockHwrmPluginData respData(aTransId,resultPckg);
	respData.SerialiseL(aBuffer);
	}

void CMockHwrmPluginEngine::FurtherProcessEventL(const TInt /*aCommandId*/, TDesC8& /*aData*/)
	{
	}
