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
#include "MmfDevSoundProxy.h"
#include "MmfDevSoundServerStart.h"
#include "MmfBase.hrh"
#include "MmfAudioClientServer.h"
#include <mmf/plugin/mmfdevsoundcustominterface.hrh>
#include <ecom/ecom.h>


/**
@internalTechnology

This function raises a panic

@param	aError
		one of the several panic codes that may be raised by this dll

@panic	EMMFDevSoundProxyPlayDataWithoutInitialize is raised when playdata is called without initialization
@panic	EMMFDevSoundProxyRecordDataWithoutInitialize is raised when recorddata is called without initialization
@panic	EMMFDevSoundProxyConvertDataWithoutInitialize is raised when convertdata is called without initialization
*/
GLDEF_C void Panic(TMMFDevSoundProxyPanicCodes aPanicCode)
	{
	User::Panic(KMMFDevSoundProxyPanicCategory, aPanicCode);
	}
	
EXPORT_C RMMFDevSoundProxy::RMMFDevSoundProxy()
 : iBuffer(NULL), iSeqName(NULL), iMsgQueueHandle(NULL), iAudioServerProxy (NULL)
 	{ 		
 	}
	
EXPORT_C void RMMFDevSoundProxy::Close()
	{
	RMmfSessionBase::Close();
	iState = EIdle;
	if(iAudioServerProxy)
		{
		iAudioServerProxy->Close();
		delete iAudioServerProxy;	
		}
	delete iBuffer;
	delete iSeqName;
	}

EXPORT_C TInt RMMFDevSoundProxy::Open(RHandleBase& aMsgQueueHandle)
	{
	TInt err = KErrNone;
	iMsgQueueHandle = &aMsgQueueHandle;
	TRAP(err, iSeqName = HBufC::NewL(KMaxFixedSequenceNameLength));
	if(err == KErrNone)
		{
		TRAP(err, iAudioServerProxy = new (ELeave) RMMFAudioServerProxy());	
		if(err == KErrNone)
			{
			err = iAudioServerProxy->Open();
			if(err == KErrNone)
				{
				err = SetReturnedHandle(iAudioServerProxy->GetDevSoundSessionHandle());	
				}
			}
		}
	if(err)
		{
		Close();
		}
			
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
	TIpcArgs args(&pckg, *iMsgQueueHandle);
	return RSessionBase::SendReceive(EMMFDevSoundProxyInitialize1, args);
	}

EXPORT_C TInt RMMFDevSoundProxy::InitializeL(TUid aHWDev, TMMFState aMode)
	{
	TMMFDevSoundProxySettings set;
	set.iHWDev = aHWDev;
	set.iMode = aMode;
	TMMFDevSoundProxySettingsPckg pckg(set);
	TIpcArgs args(&pckg, *iMsgQueueHandle);
	return RSessionBase::SendReceive(EMMFDevSoundProxyInitialize2, args);
	}

EXPORT_C TInt RMMFDevSoundProxy::InitializeL(TFourCC aDesiredFourCC, TMMFState aMode)
	{
	TMMFDevSoundProxySettings set;
	set.iDesiredFourCC = aDesiredFourCC;
	set.iMode = aMode;
	TMMFDevSoundProxySettingsPckg pckg(set);
	TIpcArgs args(&pckg, *iMsgQueueHandle);
	return RSessionBase::SendReceive(EMMFDevSoundProxyInitialize4, args);
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
	TInt err = SendReceive(EMMFDevSoundProxySetConfig, pckg);
	User::LeaveIfError(err);	
	return err;
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
	iState = EPlaying;
	}

EXPORT_C void RMMFDevSoundProxy::RecordInitL()
	{
	User::LeaveIfError(SendReceive(EMMFDevSoundProxyRecordInit));
	iState = ERecording;
	}

EXPORT_C void RMMFDevSoundProxy::PlayData()
	{
	__ASSERT_ALWAYS(iState == EPlaying, Panic(EMMFDevSoundProxyPlayDataWithoutInitialize));

	TMMFDevSoundProxyHwBuf set;
	set.iLastBuffer = iBuffer->LastBuffer();	
	TMMFDevSoundProxyHwBufPckg pckg(set);
	SendReceive(EMMFDevSoundProxyPlayData, pckg, iBuffer->Data());
	}

EXPORT_C void RMMFDevSoundProxy::RecordData()
	{
	__ASSERT_ALWAYS(iState == ERecording, Panic(EMMFDevSoundProxyRecordDataWithoutInitialize));
	SendReceive(EMMFDevSoundProxyRecordData);
	}

EXPORT_C void RMMFDevSoundProxy::Stop()
	{
	SendReceive(EMMFDevSoundProxyStop);
	iState = EIdle;
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
	User::LeaveIfError(SendReceive(EMMFDevSoundProxyPlayDualTone, pckg));
	}

EXPORT_C void RMMFDevSoundProxy::PlayDTMFStringL(const TDesC& aDTMFString)
	{
	User::LeaveIfError(SendReceive(EMMFDevSoundProxyPlayDTMFString, aDTMFString));
	}

EXPORT_C void RMMFDevSoundProxy::PlayToneSequenceL(const TDesC8& aData)
	{
	User::LeaveIfError(SendReceive(EMMFDevSoundProxyPlayToneSequence, aData));
	}

EXPORT_C void RMMFDevSoundProxy::PlayFixedSequenceL(TInt aSequenceNumber)
	{
	TPckgBuf<TInt> seqNum(aSequenceNumber);	
	User::LeaveIfError(SendReceive(EMMFDevSoundProxyPlayFixedSequence, seqNum));
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

EXPORT_C TInt RMMFDevSoundProxy::BufferToBeFilledData(TMMFDevSoundProxyHwBufPckg& aSetPckg)
	{
	// Note that there will only ever be one of these requests outstanding per session
	return SendReceiveResult(EMMFDevSoundProxyBTBFData, aSetPckg);
	}
	
EXPORT_C TInt RMMFDevSoundProxy::BufferToBeEmptiedData(TMMFDevSoundProxyHwBufPckg& aSetPckg)
	{
	// Note that there will only ever be one of these requests outstanding per session
	return SendReceiveResult(EMMFDevSoundProxyBTBEData, aSetPckg);	
	}

EXPORT_C void RMMFDevSoundProxy::SetBuffer(CMMFDataBuffer* aBuffer)
	{
	if(iBuffer)
		{
		delete iBuffer;
		}
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

EXPORT_C TInt RMMFDevSoundProxy::EmptyBuffers()
	{
	return SendReceive(EMMFDevSoundProxyEmptyBuffers);
	}

EXPORT_C TInt RMMFDevSoundProxy::GetTimePlayed(TTimeIntervalMicroSeconds& aTime)
	{
	TTimeIntervalMicroSeconds time(0);
	TPckgBuf<TTimeIntervalMicroSeconds> timePckg(time);
	TInt err = SendReceiveResult(EMMFDevSoundProxyGetTimePlayed, KNullDesC8, KNullDesC8, timePckg);
	if(err==KErrNone)
		{
		aTime = timePckg();
		}
	return err;
	}
	
// implementation of a simple CustomCommand() scheme
EXPORT_C TInt RMMFDevSoundProxy::SyncCustomCommand(TUid aUid, const TDesC8& aParam1, const TDesC8& aParam2, TDes8* aOutParam)
	{
	// the UID of the custom command is passed as an integer so as maintain consistency with the async methods below
	TInt command = aUid.iUid;
	
	if (aOutParam==NULL)
		{
		return SendReceive(EMMFDevSoundProxySyncCustomCommand, command, aParam1, aParam2);
		}
	else
		{
		return SendReceiveResult(EMMFDevSoundProxySyncCustomCommandResult, command, aParam1, aParam2, *aOutParam);		
		}
	}

EXPORT_C void RMMFDevSoundProxy::AsyncCustomCommand(TUid aUid, TRequestStatus& aStatus, const TDesC8& aParam1, const TDesC8& aParam2, TDes8* aOutParam)
	{
	// this seemingly only allows 1 simultaneous async call. Need an array of TRequestStatus for more than one call.
	// the UID of the custom command is passed as an integer so as to prevent the need of a consistent UID array.
	TInt command = aUid.iUid;

	if (aOutParam==NULL)
		{
		SendReceive(EMMFDevSoundProxyAsyncCustomCommand, command, aParam1, aParam2, aStatus);
		}
	else
		{
		SendReceiveResult(EMMFDevSoundProxyAsyncCustomCommandResult, command, aParam1, aParam2, *aOutParam, aStatus);		
		}
	}

EXPORT_C TInt RMMFDevSoundProxy::SetClientThreadInfo(TThreadId& aTid)
	{
	TPckgBuf<TThreadId> threadId(aTid);
	return SendReceive(EMMFDevSoundProxySetClientThreadInfo, threadId);
	}
