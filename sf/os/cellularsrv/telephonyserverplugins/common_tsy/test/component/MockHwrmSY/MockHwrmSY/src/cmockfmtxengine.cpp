// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// MockSY engine for the FmTx plugin
// 
//

#include "cmockfmtxengine.h"
#include "cmockhwrmpluginsy.h"
#include <hwrm/HWRMFmTxCommands.h>
#include <hwrm/hwrmfmtx.h>
#include <hwrm/hwrmfmtxdomainpskeys.h>
#include "e32property.h" 

_LIT(KFmTxOnCmdId,"ETxOnCmdId");
_LIT(KFmTxOffCmdId,"ETxOffCmdId");
_LIT(KFmTxSetFrequencyCmdId,"ESetTxFrequencyCmdId");
_LIT(KFmTxScanRequestCmdId,"ETxScanRequestCmdId");
_LIT(KFmTxGetFrequencyRangeCmdId,"EGetTxFrequencyRangeCmdId");
_LIT(KFmTxSetTxRdsPsCmdId,"ESetTxRdsPsCmdId");
_LIT(KFmTxSetTxRdsPtyCmdId,"ESetTxRdsPtyCmdId");
_LIT(KFmTxSetTxRdsPtynCmdId,"ESetTxRdsPtynCmdId");
_LIT(KFmTxSetTxRdsMsCmdId,"ESetTxRdsMsCmdId");
_LIT(KFmTxSetTxRdsLangCmdId,"ESetTxRdsLangIdCmdId");
_LIT(KFmTxSetTxRtCmdId,"ESetTxRtCmdId");
_LIT(KFmTxClearTxRtCmdId,"EClearTxRtCmdId");

/**
Factory function
*/
CMockFmTxEngine* CMockFmTxEngine::NewL(CMockHwrmPluginSY& aMockHwrmPluginSY)
	{
	CMockFmTxEngine* self =  new (ELeave)CMockFmTxEngine(aMockHwrmPluginSY);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructor
*/	
CMockFmTxEngine::CMockFmTxEngine(CMockHwrmPluginSY& aMockHwrmPluginSY)
	:CMockHwrmPluginEngine(aMockHwrmPluginSY)
	{
	}


/**
*/
void CMockFmTxEngine::LogRequest(TInt aCmdId, const MMockSyEngineData& aData,TInt aResultCode)
	{
	const TMockHwrmPluginData& data = static_cast<const TMockHwrmPluginData&>(aData);
	TBuf<KMaxLogLineSize> buffer;
	const TDesC& cmdName = CommandName(aCmdId);
	buffer.Format(_L(">>> Cmd=%S, TransId=%d, Err=%d"),&cmdName, data.TransId(), aResultCode);
	Log(buffer);	
	}

/**
*/
void CMockFmTxEngine::LogCompletion(TInt aCmdId, const TDesC8& aData,TInt aResultCode)
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
void CMockFmTxEngine::LogExpectError(TInt aCmdId, const MMockSyEngineData& aData,
	TInt aExpectedCmd,const TDesC8& aExpectedData)
	{
	const TMockHwrmPluginData& data = static_cast<const TMockHwrmPluginData&>(aData);
	TMockHwrmPluginData expectedData;
	expectedData.DeserialiseL(aExpectedData);

	DoLogError(aCmdId, data, aExpectedCmd, expectedData);
	DoLogCommandData(aExpectedCmd, expectedData, _L("Expected"));
	DoLogCommandData(aCmdId, data, _L("Received"));	
	}

void CMockFmTxEngine::QueueProcessedCompletionL(const TInt aCommandId, const TUint8 aTransId, RBuf8& aBuffer, TDesC8& aData)
	{
	switch (aCommandId)
		{
		case HWRMFmTxCommand::EGetTxFrequencyRangeCmdId:
			{
			HWRMFmTxCommand::TFrequencyRangeData frequencyData;
			frequencyData.iErrorCode = KErrNone;
			frequencyData.iMaxFrequency = 0;	//The values will be set up be the CHWRMFmTxService::ProcessResponseL
			frequencyData.iMinFrequency = 0;	// to KHWRMFmTxBackupMinFreq, KHWRMFmTxBackupMaxFreq, KHWRMFmTxBackupStepSize			
			frequencyData.iStepSize = 0;
			
			
			
			HWRMFmTxCommand::TFrequencyRangePackage resultPckgFrequencyRangePckg(frequencyData);
			TMockHwrmPluginData respFrequencyRangeData(aTransId,resultPckgFrequencyRangePckg);
			respFrequencyRangeData.SerialiseL(aBuffer);
			}
		break;

		case HWRMFmTxCommand::ETxScanRequestCmdId:
			{
			
			HWRMFmTxCommand::TScanRequestPackage requestedNumberOfFreqsPckg;
			requestedNumberOfFreqsPckg.Copy(aData);
			TUint8 requestedNumberOfFreqs = requestedNumberOfFreqsPckg();
			
			TInt completeError = KErrNone;
			
			HWRMFmTxCommand::TScanResponseData scanResponseData;
			scanResponseData.iErrorCode = completeError;
			scanResponseData.iFrequenciesFound = requestedNumberOfFreqs;
			scanResponseData.iChannels[0] = 89000;
			scanResponseData.iChannels[1] = 89100;
			scanResponseData.iChannels[2] = 89200;
			scanResponseData.iChannels[3] = 89300;
			scanResponseData.iChannels[4] = 89400;
			scanResponseData.iChannels[5] = 89500;
			scanResponseData.iChannels[6] = 89600;
			scanResponseData.iChannels[7] = 89700;
			scanResponseData.iChannels[8] = 89800;
			scanResponseData.iChannels[9] = 89900;
			
			HWRMFmTxCommand::TScanResponsePackage resultPckgScanResponsePckg(scanResponseData);
			TMockHwrmPluginData respScanResponseRangeData(aTransId,resultPckgScanResponsePckg);
			respScanResponseRangeData.SerialiseL(aBuffer);
			}
		break;

		default:
			{
			CMockHwrmPluginEngine::QueueProcessedCompletionL(aCommandId,aTransId, aBuffer, aData);
			}
		break;
		}
	}
void CMockFmTxEngine::FurtherProcessEventL(const TInt /*aCommandId*/, TDesC8& /*aData*/)
	{
	}

/**
Log an error
*/
void CMockFmTxEngine::DoLogError(TInt aCmdId, const TMockHwrmPluginData& aData,
	TInt aExpectedCmd,const TMockHwrmPluginData& aExpectedData)
	{
	TBuf<KMaxLogLineSize> buffer;
	const TDesC& expectedCmdName = CommandName(aExpectedCmd);
	const TDesC& cmdName = CommandName(aCmdId);
	buffer.Format(_L("ERROR: Expected %S, TransId=%d; Received %S,TransId=%d, "),
		&expectedCmdName, aExpectedData.TransId(), &cmdName, aData.TransId());
	Log(buffer);
	}

/**
Log command data
*/
void CMockFmTxEngine::DoLogCommandData(TInt aCmdId, const TMockHwrmPluginData& aCmdData, const TDesC& aComment)
	{
	TBuf<KMaxLogLineSize> buffer;
	switch (aCmdId)
		{
		case HWRMFmTxCommand::ESetTxFrequencyCmdId:
			{
			HWRMFmTxCommand::TSetFrequencyPackage FmTxFrequencyData;
			FmTxFrequencyData.Copy(aCmdData.CommandData());
			buffer.Format(_L("%S: frequency=%d"), & aComment,
					FmTxFrequencyData());
			}
		break;

		case HWRMFmTxCommand::ESetTxRdsPsCmdId:
			{			
			HWRMFmTxCommand::TRdsPsPackage FmTxRdsPsData;
			FmTxRdsPsData.Copy(aCmdData.CommandData());
			buffer.Format(_L("%S: FmTxRdsPsData=%s"), & aComment,
					& (FmTxRdsPsData()));//
			}
		break;
		
		case HWRMFmTxCommand::ESetTxRdsPtyCmdId:
			{
			HWRMFmTxCommand::TRdsPtyPackage FmTxRdsPtyData;
			FmTxRdsPtyData.Copy(aCmdData.CommandData());
			buffer.Format(_L("%S: RDSPtyData=%d"), & aComment,
					& (FmTxRdsPtyData()));			
			}
		break;
		
		case HWRMFmTxCommand::ESetTxRdsPtynCmdId:
			{
			HWRMFmTxCommand::TRdsPtynPackage FmTxTRdsPtynData;
			FmTxTRdsPtynData.Copy(aCmdData.CommandData());
			buffer.Format(_L("%S: FmTxTRdsPtynData=%s"), & aComment,
					& (FmTxTRdsPtynData()));
			}
		break;

		case HWRMFmTxCommand::ESetTxRdsMsCmdId:
			{
			HWRMFmTxCommand::TRdsMsPackage FmTxTRdsMsData;
			FmTxTRdsMsData.Copy(aCmdData.CommandData());
			buffer.Format(_L("%S: FmTxTRdsMsData=%d"), & aComment,
					FmTxTRdsMsData());			
			}
		break;
		
		case HWRMFmTxCommand::ESetTxRdsLangIdCmdId:
			{
			HWRMFmTxCommand::TRdsLangIdPackage FmTxTRdsLangIdData;
			FmTxTRdsLangIdData.Copy(aCmdData.CommandData());
			buffer.Format(_L("%S: FmTxTRdsLangIdData=%d"), & aComment,
					FmTxTRdsLangIdData());			
			}
		break;
		
		case HWRMFmTxCommand::ESetTxRtCmdId:
			{
			HWRMFmTxCommand::TRtPackage FmTxTRtData;
			FmTxTRtData.Copy(aCmdData.CommandData());
			buffer.Format(_L("%S: FmTxTRtData.iText=%s, FmTxTRtData.iTag1.iContentType=%d, FmTxTRtData().iTag1.iStartMarker=%d, FmTxTRtData().iTag1.iLengthMarker=%d,FmTxTRtData.iTag2.iContentType=%d, FmTxTRtData().iTag2.iStartMarker=%d, FmTxTRtData().iTag2.iLengthMarker=%d"), & aComment,
			& (FmTxTRtData().iText), FmTxTRtData().iTag1.iContentType, FmTxTRtData().iTag1.iStartMarker, FmTxTRtData().iTag1.iLengthMarker,
			FmTxTRtData().iTag2.iContentType, FmTxTRtData().iTag2.iStartMarker, FmTxTRtData().iTag2.iLengthMarker);			
			}
		break;
		
		}		
		
	Log(buffer);
	}

const TDesC& CMockFmTxEngine::CommandName(TInt aCmdId)
	{
	switch (aCmdId)
		{
		case HWRMFmTxCommand::ETxOnCmdId:			
			return KFmTxOnCmdId;
		case HWRMFmTxCommand::ETxOffCmdId:
			return KFmTxOffCmdId;
		case HWRMFmTxCommand::ESetTxFrequencyCmdId:
			return KFmTxSetFrequencyCmdId;
		case HWRMFmTxCommand::ETxScanRequestCmdId:
			return KFmTxScanRequestCmdId;
		case HWRMFmTxCommand::EGetTxFrequencyRangeCmdId:
			return KFmTxGetFrequencyRangeCmdId;
		case HWRMFmTxCommand::ESetTxRdsPsCmdId:
			return KFmTxSetTxRdsPsCmdId;
		case HWRMFmTxCommand::ESetTxRdsPtyCmdId:
			return KFmTxSetTxRdsPtyCmdId;
		case HWRMFmTxCommand::ESetTxRdsPtynCmdId:
			return KFmTxSetTxRdsPtynCmdId;
		case HWRMFmTxCommand::ESetTxRdsMsCmdId:
			return KFmTxSetTxRdsMsCmdId;
		case HWRMFmTxCommand::ESetTxRdsLangIdCmdId:
			return KFmTxSetTxRdsLangCmdId;
		case HWRMFmTxCommand::ESetTxRtCmdId:
			return KFmTxSetTxRtCmdId;
		case HWRMFmTxCommand::EClearTxRtCmdId:
			return KFmTxClearTxRtCmdId;

		}		
		return KNullDesC;
	}
