// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>
#include <e32math.h>
#include <s32mem.h> 
#include "MmfBtDevSoundProxy.h"
#include "MmfBtDevSoundServerStart.h"
#include "../../../inc/common/mmfBtBase.hrh"
#include "MmfBtAudioClientServer.h"


EXPORT_C void RMMFDevSoundProxy::Close()
	{
	RMmfSessionBase::Close();
	delete iBuffer;
	delete iSeqName;
	}

EXPORT_C TInt RMMFDevSoundProxy::Open()
	{
	TInt err = KErrNone;
	RMMFAudioServerProxy* audioServerProxy = NULL;
	HBufC* devSoundServerName = NULL;
	iBuffer = NULL;
	iSeqName = NULL;
	TRAP(err, iSeqName = HBufC::NewL(KMaxFixedSequenceNameLength));
	if(err)
		{
		iSeqName = NULL;
		return err;
		}

	TRAP(err, audioServerProxy = new (ELeave) RMMFAudioServerProxy());
	if(err != KErrNone)
		{
		delete iSeqName;
		iSeqName = NULL;
		return err;
		}

	err = audioServerProxy->Open();
	if(err != KErrNone)
		{
		delete audioServerProxy;
		delete iSeqName;
		iSeqName = NULL;
		return err;
		}


	TRAP(err, devSoundServerName = audioServerProxy->GetDevSoundServerNameL());
	if(err)
		{
		audioServerProxy->Close();
		delete audioServerProxy;
		delete iSeqName;
		iSeqName = NULL;
		return err;
		}

	TPtr devSoundServerNamePtr = devSoundServerName->Des();
 
	TInt retry=2;
	for (;;)
		{
		err = CreateSession(devSoundServerNamePtr, TVersion(KMMFDevSoundServerVersion,
														KMMFDevSoundServerMinorVersionNumber,
														KMMFDevSoundServerBuildVersionNumber));

		if (err!=KErrNotFound && err!=KErrServerTerminated)
			break;//return r;
		if (--retry==0)
			break;//return r;
		}

	if(err)
		{
		delete iSeqName;
		iSeqName = NULL;
		}
	audioServerProxy->Close();
	delete audioServerProxy;
	delete devSoundServerName;

	return err;
	}

EXPORT_C TInt RMMFDevSoundProxy::SetDevSoundInfo()
	{
	return SendReceive(EMMFAudioLaunchRequests);
	}

EXPORT_C TInt RMMFDevSoundProxy::InitializeL(TMMFState aMode)
	{
	TMMFDevSoundProxySettings set;
	set.iMode = aMode;
	TMMFDevSoundProxySettingsPckg pckg(set);
	return SendReceive(EMMFDevSoundProxyInitialize1, pckg);
	}

EXPORT_C TInt RMMFDevSoundProxy::InitializeL(TUid aHWDev, TMMFState aMode)
	{
	TMMFDevSoundProxySettings set;
	set.iHWDev = aHWDev;
	set.iMode = aMode;
	TMMFDevSoundProxySettingsPckg pckg(set);
	return SendReceive(EMMFDevSoundProxyInitialize2, pckg);
	}

EXPORT_C TInt RMMFDevSoundProxy::InitializeL(CArrayPtr<TUid> /*aHWDevArray*/, TMMFState /*aMode*/)
	{
	return SendReceive(EMMFDevSoundProxyInitialize3);
	}

EXPORT_C TInt RMMFDevSoundProxy::InitializeL(TFourCC aDesiredFourCC, TMMFState aMode)
	{
	TMMFDevSoundProxySettings set;
	set.iDesiredFourCC = aDesiredFourCC;
	set.iMode = aMode;
	TMMFDevSoundProxySettingsPckg pckg(set);
	return SendReceive(EMMFDevSoundProxyInitialize4, pckg);
	}

EXPORT_C TMMFCapabilities RMMFDevSoundProxy::Capabilities()
	{
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceiveResult(EMMFDevSoundProxyCapabilities,KNullDesC8,KNullDesC8,pckg);
	return pckg().iCaps;
	}

EXPORT_C TMMFCapabilities RMMFDevSoundProxy::Config()
	{
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceiveResult(EMMFDevSoundProxyConfig,KNullDesC8,KNullDesC8,pckg);
	return pckg().iConfig;
	}

EXPORT_C TInt RMMFDevSoundProxy::SetConfigL(const TMMFCapabilities& aConfig)
	{
	TMMFDevSoundProxySettings set;
	set.iConfig = aConfig;
	TMMFDevSoundProxySettingsPckg pckg(set);
	return SendReceive(EMMFDevSoundProxySetConfig, pckg);
	}

EXPORT_C TInt RMMFDevSoundProxy::MaxVolume()
	{
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceiveResult(EMMFDevSoundProxyMaxVolume,KNullDesC8,KNullDesC8,pckg);
	return pckg().iMaxVolume;
	}

EXPORT_C TInt RMMFDevSoundProxy::Volume()
	{
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceiveResult(EMMFDevSoundProxyVolume,KNullDesC8,KNullDesC8,pckg);
	return pckg().iVolume;
	}

EXPORT_C TInt RMMFDevSoundProxy::SetVolume(TInt aVolume)
	{
	TMMFDevSoundProxySettings set;
	set.iVolume = aVolume;
	TMMFDevSoundProxySettingsPckg pckg(set);
	return SendReceive(EMMFDevSoundProxySetVolume, pckg);
	}

EXPORT_C TInt RMMFDevSoundProxy::MaxGain()
	{
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceiveResult(EMMFDevSoundProxyMaxGain,KNullDesC8,KNullDesC8,pckg);
	return pckg().iMaxGain;
	}

EXPORT_C TInt RMMFDevSoundProxy::Gain()
	{
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceiveResult(EMMFDevSoundProxyGain,KNullDesC8,KNullDesC8,pckg);
	return pckg().iGain;
	}

EXPORT_C TInt RMMFDevSoundProxy::SetGain(TInt aGain)
	{
	TMMFDevSoundProxySettings set;
	set.iGain = aGain;
	TMMFDevSoundProxySettingsPckg pckg(set);
	return SendReceive(EMMFDevSoundProxySetGain, pckg);
	}

EXPORT_C void RMMFDevSoundProxy::GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	{
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	User::LeaveIfError(SendReceiveResult(EMMFDevSoundProxyPlayBalance,KNullDesC8,KNullDesC8,pckg));
	aLeftPercentage = pckg().iLeftPercentage;
	aRightPercentage = pckg().iRightPercentage;
	}

EXPORT_C void RMMFDevSoundProxy::SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage)
	{
	TMMFDevSoundProxySettings set;
	set.iLeftPercentage = aLeftPercentage;
	set.iRightPercentage = aRightPercentage;
	TMMFDevSoundProxySettingsPckg pckg(set);
	User::LeaveIfError(SendReceive(EMMFDevSoundProxySetPlayBalance, pckg));
	}

EXPORT_C void RMMFDevSoundProxy::GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	{
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	User::LeaveIfError(SendReceiveResult(EMMFDevSoundProxyRecordBalance,KNullDesC8,KNullDesC8,pckg));
	aLeftPercentage = pckg().iLeftPercentage;
	aRightPercentage = pckg().iRightPercentage;
	}

EXPORT_C void RMMFDevSoundProxy::SetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage)
	{
	TMMFDevSoundProxySettings set;
	set.iLeftPercentage = aLeftPercentage;
	set.iRightPercentage = aRightPercentage;
	TMMFDevSoundProxySettingsPckg pckg(set);
	User::LeaveIfError(SendReceive(EMMFDevSoundProxySetRecordBalance, pckg));
	}

EXPORT_C void RMMFDevSoundProxy::PlayInitL()
	{
	User::LeaveIfError(SendReceive(EMMFDevSoundProxyPlayInit));
	}

EXPORT_C void RMMFDevSoundProxy::RecordInitL()
	{
	User::LeaveIfError(SendReceive(EMMFDevSoundProxyRecordInit));
	}

EXPORT_C void RMMFDevSoundProxy::PlayData()
	{
	TMMFDevSoundProxyHwBuf set;
	set.iLastBuffer = iBuffer->LastBuffer();
	TMMFDevSoundProxyHwBufPckg pckg(set);
	
	SendReceive(EMMFDevSoundProxyPlayData, pckg, iBuffer->Data());
	}

EXPORT_C void RMMFDevSoundProxy::RecordData()
	{
	SendReceive(EMMFDevSoundProxyRecordData);
	}

EXPORT_C void RMMFDevSoundProxy::Stop()
	{
	SendReceive(EMMFDevSoundProxyStop);
	}

EXPORT_C void RMMFDevSoundProxy::Pause()
	{
	SendReceive(EMMFDevSoundProxyPause);
	}

EXPORT_C void RMMFDevSoundProxy::PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration)
	{
	TMMFDevSoundProxySettings set;
	set.iFrequencyOne = aFrequency;
	set.iDuration = aDuration;
	TMMFDevSoundProxySettingsPckg pckg(set);
	User::LeaveIfError(SendReceive(EMMFDevSoundProxyPlayTone, pckg));
	}

EXPORT_C void RMMFDevSoundProxy::PlayDualToneL(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration)
	{
	TMMFDevSoundProxySettings set;
	set.iFrequencyOne = aFrequencyOne;
	set.iFrequencyTwo = aFrequencyTwo;
	set.iDuration = aDuration;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceive(EMMFDevSoundProxyPlayDualTone, pckg);
	}

EXPORT_C void RMMFDevSoundProxy::PlayDTMFStringL(const TDesC& aDTMFString)
	{
	SendReceive(EMMFDevSoundProxyPlayDTMFString, aDTMFString);
	}

EXPORT_C void RMMFDevSoundProxy::PlayToneSequenceL(const TDesC8& aData)
	{
	User::LeaveIfError(SendReceive(EMMFDevSoundProxyPlayToneSequence, aData));
	}

EXPORT_C void RMMFDevSoundProxy::PlayFixedSequenceL(TInt aSequenceNumber)
	{
	TPckgBuf<TInt> seqNum(aSequenceNumber);	
	SendReceive(EMMFDevSoundProxyPlayFixedSequence, seqNum);
	}

EXPORT_C void RMMFDevSoundProxy::SetDTMFLengths(TTimeIntervalMicroSeconds32& aToneOnLength,
											    TTimeIntervalMicroSeconds32& aToneOffLength,
											    TTimeIntervalMicroSeconds32& aPauseLength)
	{
	TMMFDevSoundProxySettings set;
	set.iToneOnLength = aToneOnLength;
	set.iToneOffLength = aToneOffLength;
	set.iPauseLength = aPauseLength;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceive(EMMFDevSoundProxySetDTMFLengths, pckg);
	}

EXPORT_C void RMMFDevSoundProxy::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	TMMFDevSoundProxySettings set;
	set.iDuration = aRampDuration;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceive(EMMFDevSoundProxySetVolumeRamp, pckg);
	}

EXPORT_C void RMMFDevSoundProxy::GetSupportedInputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings)
	{
	aSupportedDataTypes.Reset();

	TMMFPrioritySettings prioritySet = aPrioritySettings;
	TMMFPrioritySettingsPckg pckg(prioritySet);

	TPckgBuf<TInt> numberOfElementsPckg;
	User::LeaveIfError(SendReceiveResult(EMMFDevSoundProxyGetSupportedInputDataTypes, pckg, KNullDesC8, numberOfElementsPckg));

	HBufC8* buf = HBufC8::NewLC(numberOfElementsPckg()*sizeof(TFourCC));
	TPtr8 ptr = buf->Des();

	
	User::LeaveIfError(SendReceiveResult(EMMFDevSoundProxyCopyFourCCArrayData,KNullDesC8,KNullDesC8,ptr));
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<numberOfElementsPckg(); i++)
		{
		TInt err = aSupportedDataTypes.Append(stream.ReadInt32L());
		if (err)
			{//note we don't destroy array because we don't own it
			//but we do reset it as it is incomplete
			aSupportedDataTypes.Reset();
			User::Leave(err);
			}
		}
	CleanupStack::PopAndDestroy(2, buf);//stream, buf
	}


EXPORT_C void RMMFDevSoundProxy::GetSupportedOutputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings)
	{
	aSupportedDataTypes.Reset();

	TMMFPrioritySettings prioritySet = aPrioritySettings;
	TMMFPrioritySettingsPckg pckg(prioritySet);

	TPckgBuf<TInt> numberOfElementsPckg;
	User::LeaveIfError(SendReceiveResult(EMMFDevSoundProxyGetSupportedOutputDataTypes, pckg, KNullDesC8, numberOfElementsPckg));

	HBufC8* buf = HBufC8::NewLC(numberOfElementsPckg()*sizeof(TFourCC));
	TPtr8 ptr = buf->Des();

	
	User::LeaveIfError(SendReceiveResult(EMMFDevSoundProxyCopyFourCCArrayData,KNullDesC8,KNullDesC8,ptr));
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<numberOfElementsPckg(); i++)
		{
		TInt err = aSupportedDataTypes.Append(stream.ReadInt32L());
		if (err)
			{//note we don't destroy array because we don't own it
			//but we do reset it as it is incomplete
			aSupportedDataTypes.Reset();
			User::Leave(err);
			}
		}
	CleanupStack::PopAndDestroy(2, buf);//stream, buf
	}

EXPORT_C TInt RMMFDevSoundProxy::SamplesRecorded()
	{
	TPckgBuf<TInt> numSamples;	
	SendReceiveResult(EMMFDevSoundProxySamplesRecorded, KNullDesC8, KNullDesC8, numSamples);
	return numSamples();
	}

EXPORT_C TInt RMMFDevSoundProxy::SamplesPlayed()
	{
	TPckgBuf<TInt> numSamples;	
	SendReceiveResult(EMMFDevSoundProxySamplesPlayed, KNullDesC8, KNullDesC8, numSamples);
	return numSamples();
	}

EXPORT_C void RMMFDevSoundProxy::SetToneRepeats(TInt aRepeatCount, const TTimeIntervalMicroSeconds& aRepeatTrailingSilence)
	{
	TPckgBuf<TInt> countRepeat(aRepeatCount);
	TPckgBuf<TTimeIntervalMicroSeconds> repeatTS(aRepeatTrailingSilence);	
	SendReceive(EMMFDevSoundProxySetToneRepeats, countRepeat, repeatTS);
	}

EXPORT_C void RMMFDevSoundProxy::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	TPckgBuf<TMMFPrioritySettings> prioritySet(aPrioritySettings);
	SendReceive(EMMFDevSoundProxySetPrioritySettings, prioritySet);
	}

EXPORT_C const TDesC& RMMFDevSoundProxy::FixedSequenceName(TInt aSequenceNumber)
	{
	TPckgBuf<TInt> seqNum(aSequenceNumber);
	TPtr SeqNamePtr = iSeqName->Des();
	SeqNamePtr.FillZ();
	SendReceiveResult(EMMFDevSoundProxyFixedSequenceName, seqNum, KNullDesC8, SeqNamePtr);
	return *iSeqName;
	}


EXPORT_C void RMMFDevSoundProxy::ConvertInitL()
	{
	SendReceive(EMMFDevSoundProxyConvertInit);
	}

EXPORT_C void RMMFDevSoundProxy::ConvertData()
	{
	SendReceive(EMMFDevSoundProxyConvertData);
	}

EXPORT_C TAny* RMMFDevSoundProxy::CustomInterface(TUid /*aInterfaceId*/)
	{
	// No custom interfaces are supported at the moment so return NULL.
	//TO DO
	return NULL;
	}

EXPORT_C TInt RMMFDevSoundProxy::FixedSequenceCount()
	{
	TPckgBuf<TInt> fixSeqCountPckg;
	SendReceiveResult(EMMFDevSoundProxyFixedSequenceCount, fixSeqCountPckg);
	return fixSeqCountPckg();
	}

EXPORT_C void RMMFDevSoundProxy::ReceiveInitializeCompleteEvent(TMMFDevSoundProxyHwBufPckg& aSetPckg, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	SendReceive(EMMFDevSoundProxyReceiveICEvent, aSetPckg, aStatus);
	}

EXPORT_C void RMMFDevSoundProxy::CancelReceiveInitializeCompleteEvent()
	{
	SendReceive(EMMFDevSoundProxyCancelReceiveICEvent);
	}

EXPORT_C void RMMFDevSoundProxy::ReceiveBufferToBeFilledEvent(TMMFDevSoundProxyHwBufPckg& aSetPckg, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	SendReceiveResult(EMMFDevSoundProxyReceiveBTBFEvent, aSetPckg, aStatus);
	}

EXPORT_C void RMMFDevSoundProxy::CancelReceiveBufferToBeFilledEvent()
	{
	SendReceive(EMMFDevSoundProxyCancelReceiveBTBFEvent);
	}

EXPORT_C void RMMFDevSoundProxy::ReceiveBufferToBeEmptiedEvent(TMMFDevSoundProxyHwBufPckg& aSetPckg, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	SendReceiveResult(EMMFDevSoundProxyReceiveBTBEEvent, aSetPckg, aStatus);
	}

EXPORT_C void RMMFDevSoundProxy::CancelReceiveBufferToBeEmptiedEvent()
	{
	SendReceive(EMMFDevSoundProxyCancelReceiveBTBEEvent);
	}

EXPORT_C void RMMFDevSoundProxy::ReceivePlayErrorEvent(TMMFDevSoundProxyHwBufPckg& aSetPckg, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	SendReceive(EMMFDevSoundProxyReceivePEEvent, aSetPckg, aStatus);
	}

EXPORT_C void RMMFDevSoundProxy::CancelReceivePlayErrorEvent()
	{
	SendReceive(EMMFDevSoundProxyCancelReceivePEEvent);
	}

EXPORT_C void RMMFDevSoundProxy::ReceiveRecordErrorEvent(TMMFDevSoundProxyHwBufPckg& aSetPckg, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	SendReceive(EMMFDevSoundProxyReceiveREEvent, aSetPckg, aStatus);
	}

EXPORT_C void RMMFDevSoundProxy::CancelReceiveRecordErrorEvent()
	{
	SendReceive(EMMFDevSoundProxyCancelReceiveREEvent);
	}

EXPORT_C void RMMFDevSoundProxy::ReceiveToneFinishedEvent(TMMFDevSoundProxyHwBufPckg& aSetPckg, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	SendReceive(EMMFDevSoundProxyReceiveTFEvent, aSetPckg, aStatus);
	}

EXPORT_C void RMMFDevSoundProxy::CancelReceiveToneFinishedEvent()
	{
	SendReceive(EMMFDevSoundProxyCancelReceiveTFEvent);
	}

EXPORT_C void RMMFDevSoundProxy::ReceiveSendEventToClientEvent(TMMFEventPckg& aEventPckg, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	SendReceiveResult(EMMFDevSoundProxyReceiveSETCEvent, aEventPckg, aStatus);
	}

EXPORT_C void RMMFDevSoundProxy::CancelReceiveSendEventToClientEvent()
	{
	SendReceive(EMMFDevSoundProxyCancelReceiveSETCEvent);
	}

EXPORT_C void RMMFDevSoundProxy::SetBuffer(CMMFDataBuffer* aBuffer)
	{
	if(iBuffer)
		delete iBuffer;
	iBuffer = aBuffer;
	}

EXPORT_C TInt RMMFDevSoundProxy::GetRecordedBufferL(CMMFDataBuffer& aBuffer)
	{
	return SendReceiveResult(EMMFDevSoundProxyGetRecordedBuffer, aBuffer.Data());
	}
	
EXPORT_C TInt RMMFDevSoundProxy::RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData)
	{
	TMMFDevSoundProxySettings set;
	set.iNotificationEventUid = aEventType;
	TMMFDevSoundProxySettingsPckg pckg(set);
	return SendReceive(EMMFDevSoundProxyRequestResourceNotification, pckg, aNotificationRegistrationData);
	}

EXPORT_C TInt RMMFDevSoundProxy::CancelRegisterAsClient(TUid aEventType)
	{
	TMMFDevSoundProxySettings set;
	set.iNotificationEventUid = aEventType;
	TMMFDevSoundProxySettingsPckg pckg(set);
	return SendReceiveResult(EMMFDevSoundProxyCancelRequestResourceNotification, pckg);
	}
	
EXPORT_C TInt RMMFDevSoundProxy::GetResourceNotificationData(TUid aEventType, TDes8& aNotificationData)
	{
	TMMFDevSoundProxySettings set;
	set.iNotificationEventUid = aEventType;
	TMMFDevSoundProxySettingsPckg pckg(set);
	return SendReceiveResult(EMMFDevSoundProxyGetResourceNotificationData, pckg,KNullDesC8,aNotificationData);
	}

EXPORT_C TInt RMMFDevSoundProxy::WillResumePlay()
	{
	return SendReceive(EMMFDevSoundProxyWillResumePlay);
	}

EXPORT_C TInt RMMFDevSoundProxy::SetClientThreadInfo(TThreadId& aTid)
	{
	TPckgBuf<TThreadId> threadId(aTid);
	return SendReceive(EMMFDevSoundProxySetClientThreadInfo, threadId);
	}

