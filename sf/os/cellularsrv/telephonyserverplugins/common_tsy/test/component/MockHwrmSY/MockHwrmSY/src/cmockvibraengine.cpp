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
// MockSY engine for the Light plugin
// 
//

#include "cmockvibraengine.h"
#include "cmockhwrmpluginsy.h"
#include <hwrm/HWRMVibraCommands.h>

_LIT(KStartVibraCmdId,"EStartVibraWithDefaultSettingsCmdId");
_LIT(KStartVibraWithDefaultSettingsCmdId,"EStartVibraWithDefaultSettingsCmdId");
_LIT(KStopVibraCmdId,"EStopVibraCmdId");
_LIT(KPulseVibraCmdId,"EPulseVibraCmdId");

/**
Factory function
*/
CMockVibraEngine* CMockVibraEngine::NewL(CMockHwrmPluginSY& aMockHwrmPluginSY)
	{
	CMockVibraEngine* self =  new (ELeave)CMockVibraEngine(aMockHwrmPluginSY);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructor
*/	
CMockVibraEngine::CMockVibraEngine(CMockHwrmPluginSY& aMockHwrmPluginSY)
	:CMockHwrmPluginEngine(aMockHwrmPluginSY)
	{
	}


/**
*/
void CMockVibraEngine::LogRequest(TInt aCmdId, const MMockSyEngineData& aData,TInt aResultCode)
	{
	const TMockHwrmPluginData& data = static_cast<const TMockHwrmPluginData&>(aData);
	TBuf<KMaxLogLineSize> buffer;
	const TDesC& cmdName = CommandName(aCmdId);
	buffer.Format(_L(">>> Cmd=%S, TransId=%d, Err=%d"),&cmdName, data.TransId(), aResultCode);
	Log(buffer);	
	}

/**
*/
void CMockVibraEngine::LogCompletion(TInt aCmdId, const TDesC8& aData,TInt aResultCode)
	{
	TMockHwrmPluginData data;
	data.DeserialiseL(aData);
	TBuf<KMaxLogLineSize> buffer;
	const TDesC& cmdName = CommandName(aCmdId);
	buffer.Format(_L("<<< Cmd=%S, TransId=%d, Err=%d"),&cmdName, data.TransId(),aResultCode);
	Log(buffer);	
	}

/**
*/
void CMockVibraEngine::LogExpectError(TInt aCmdId, const MMockSyEngineData& aData,
	TInt aExpectedCmd,const TDesC8& aExpectedData)
	{
	const TMockHwrmPluginData& data = static_cast<const TMockHwrmPluginData&>(aData);
	TMockHwrmPluginData expectedData;
	expectedData.DeserialiseL(aExpectedData);

	DoLogError(aCmdId, data, aExpectedCmd, expectedData);
	DoLogCommandData(aExpectedCmd, expectedData, _L("Expected"));
	DoLogCommandData(aCmdId, data, _L("Received"));	
	}

/**
Log an error
*/
void CMockVibraEngine::DoLogError(TInt aCmdId, const TMockHwrmPluginData& aData,
	TInt aExpectedCmd,const TMockHwrmPluginData& aExpectedData)
	{
	TBuf<KMaxLogLineSize> buffer;
	const TDesC& expectedCmdName = CommandName(aExpectedCmd);
	const TDesC& cmdName = CommandName(aCmdId);
	buffer.Format(_L("ERROR: Expected %S, TransId=%d; Received %S,TransId=%d, "),
		&expectedCmdName, aExpectedData.TransId(), &cmdName, aData.TransId());
	Log(buffer);
	}

void CMockVibraEngine::QueueProcessedCompletionL(const TInt aCommandId, const TUint8 aTransId, RBuf8& aBuffer, TDesC8& aData)
	{
	CMockHwrmPluginEngine::QueueProcessedCompletionL(aCommandId, aTransId, aBuffer, aData);
	}

void CMockVibraEngine::FurtherProcessEventL(const TInt /*aCommandId*/, TDesC8& /*aData*/)
	{
	}

/**
Log command data
*/
void CMockVibraEngine::DoLogCommandData(TInt aCmdId, const TMockHwrmPluginData& aCmdData, const TDesC& aComment)
	{
	if (aCmdId == HWRMVibraCommand::EStartVibraCmdId)
		{
		TBuf<KMaxLogLineSize> buffer;
		HWRMVibraCommand::TStartVibraCmdDataPackage vibraCmd;
		vibraCmd.Copy(aCmdData.CommandData());
		buffer.Format(_L("%S: Intensity=%d"),&aComment, vibraCmd());
		Log(buffer);
		}
	}

const TDesC& CMockVibraEngine::CommandName(TInt aCmdId)
	{
	switch (aCmdId)
		{
		case HWRMVibraCommand::EStartVibraCmdId:			
			return KStartVibraCmdId;
		case HWRMVibraCommand::EStartVibraWithDefaultSettingsCmdId:
			return KStartVibraWithDefaultSettingsCmdId;
		case HWRMVibraCommand::EStopVibraCmdId:
			return KStopVibraCmdId;
		case HWRMVibraCommand::EPulseVibraCmdId:
			return KPulseVibraCmdId;
		}		
	return KNullDesC;
	}
