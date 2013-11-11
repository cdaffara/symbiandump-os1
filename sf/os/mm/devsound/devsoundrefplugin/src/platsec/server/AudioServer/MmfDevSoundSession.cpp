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

#include <s32mem.h> 
#include "MmfDevSoundSession.h"
#include "MmfDevSoundSessionXtnd.h"
#include "MmfDevSoundSessionBody.h"

#include "MmfAudioClientServer.h"
#include "MmfAudioServer.h"
#include "MmfDevSoundServer.h"

void CMMFDevSoundSession::CreateL(const CMmfIpcServer& aServer)
	{
	CMmfIpcSession::CreateL(aServer);
	CMMFDevSoundServer& server = 
		const_cast<CMMFDevSoundServer&>(static_cast<const CMMFDevSoundServer&>(aServer));
	server.IncrementSessionId();
	iDevSoundSessionId = server.DevSoundSessionId();
	iClientHasCaps = server.CheckClientCapabilities();
	}

TBool CMMFDevSoundSession::CheckClientCapabilities()
	{
	return iClientHasCaps;
	}

void CMMFDevSoundSession::ServiceL(const RMmfIpcMessage& aMessage)
	{
	TBool complete = EFalse;
	switch(aMessage.Function())
		{
	case EMMFDevSoundProxyInitialize1:
		complete = DoInitialize1L(aMessage);
		break;
	case EMMFDevSoundProxyInitialize2:
		complete = DoInitialize2L(aMessage);
		break;
	case EMMFDevSoundProxyInitialize3:
		complete = DoInitialize3L(aMessage);
		break;
	case EMMFDevSoundProxyInitialize4:
		complete = DoInitialize4L(aMessage);
		break;
	case EMMFDevSoundProxyCapabilities:
		complete = DoCapabilitiesL(aMessage);
		break;
	case EMMFDevSoundProxyConfig:
		complete = DoConfigL(aMessage);
		break;
	case EMMFDevSoundProxySetConfig:
		complete = DoSetConfigL(aMessage);
		break;
	case EMMFDevSoundProxyMaxVolume:
		complete = DoMaxVolumeL(aMessage);
		break;
	case EMMFDevSoundProxyVolume:
		complete = DoVolumeL(aMessage);
		break;
	case EMMFDevSoundProxySetVolume:
		complete = DoSetVolumeL(aMessage);
		break;
	case EMMFDevSoundProxyMaxGain:
		complete = DoMaxGainL(aMessage);
		break;
	case EMMFDevSoundProxyGain:
		complete = DoGainL(aMessage);
		break;
	case EMMFDevSoundProxySetGain:
		complete = DoSetGainL(aMessage);
		break;
	case EMMFDevSoundProxyPlayBalance:
		complete = DoGetPlayBalanceL(aMessage);
		break;
	case EMMFDevSoundProxySetPlayBalance:
		complete = DoSetPlayBalanceL(aMessage);
		break;
	case EMMFDevSoundProxyRecordBalance:
		complete = DoGetRecordBalanceL(aMessage);
		break;
	case EMMFDevSoundProxySetRecordBalance:
		complete = DoSetRecordBalanceL(aMessage);
		break;
	case EMMFDevSoundProxyBTBFData:
		complete = DoBufferToBeFilledDataL(aMessage);
		break;
	case EMMFDevSoundProxyBTBEData:
		complete = DoBufferToBeEmptiedDataL(aMessage);
		break;
	case EMMFDevSoundProxyPlayInit:
		complete = DoPlayInitL(aMessage);
		break;
	case EMMFDevSoundProxyRecordInit:
		complete = DoRecordInitL(aMessage);
		break;
	case EMMFDevSoundProxyPlayData:
		complete = DoPlayDataL(aMessage);
		break;
	case EMMFDevSoundProxyRecordData:
		complete = DoRecordDataL(aMessage);
		break;
	case EMMFDevSoundProxyStop:
		complete = DoStopL(aMessage);
		break;
	case EMMFDevSoundProxyPause:
		complete = DoPauseL(aMessage);
		break;
	case EMMFDevSoundProxyPlayTone:
		complete = DoPlayToneL(aMessage);
		break;
	case EMMFDevSoundProxyPlayDualTone:
		complete = DoPlayDualToneL(aMessage);
		break;
	case EMMFDevSoundProxyPlayDTMFString:
		complete = DoPlayDTMFStringL(aMessage);
		break;
	case EMMFDevSoundProxyPlayToneSequence:
		complete = DoPlayToneSequenceL(aMessage);
		break;
	case EMMFDevSoundProxyPlayFixedSequence:
		complete = DoPlayFixedSequenceL(aMessage);
		break;
	case EMMFDevSoundProxySetDTMFLengths:
		complete = DoSetDTMFLengthsL(aMessage);
		break;
	case EMMFDevSoundProxySetVolumeRamp:
		complete = DoSetVolumeRampL(aMessage);
		break;
	case EMMFDevSoundProxyGetSupportedInputDataTypes:
		complete = DoGetSupportedInputDataTypesL(aMessage);
		break;
	case EMMFDevSoundProxyGetSupportedOutputDataTypes:
		complete = DoGetSupportedOutputDataTypesL(aMessage);
		break;
	case EMMFDevSoundProxyCopyFourCCArrayData:
		complete = DoCopyFourCCArrayDataL(aMessage);
		break;
	case EMMFDevSoundProxyGetRecordedBuffer:
		complete = DoGetRecordedBufferL(aMessage);
		break;
	case EMMFDevSoundProxySamplesRecorded:
		complete = DoSamplesRecordedL(aMessage);
		break;
	case EMMFDevSoundProxySamplesPlayed:
		complete = DoSamplesPlayedL(aMessage);
		break;
	case EMMFDevSoundProxySetToneRepeats:
		complete = DoSetToneRepeatsL(aMessage);
		break;
	case EMMFDevSoundProxySetPrioritySettings:
		complete = DoSetPrioritySettingsL(aMessage);
		break;
	case EMMFDevSoundProxyFixedSequenceName:
		complete = DoFixedSequenceNameL(aMessage);
		break;
	case EMMFDevSoundProxyFixedSequenceCount:
		complete = DoFixedSequenceCountL(aMessage);
		break;
	case EMMFDevSoundProxyRequestResourceNotification:
		complete = DoRegisterAsClientL(aMessage);
		break;
	case EMMFDevSoundProxyCancelRequestResourceNotification:
		complete = DoCancelRegisterAsClientL(aMessage);
		break;
	case EMMFDevSoundProxyGetResourceNotificationData:
		complete = DoGetResourceNotificationDataL(aMessage);
		break;
	case EMMFDevSoundProxyWillResumePlay:
		complete = DoWillResumePlayL(aMessage);
		break;
	case EMMFDevSoundProxySetClientThreadInfo:
		complete = DoSetClientThreadInfoL(aMessage);
		break;
	case EMMFDevSoundProxyGetTimePlayed:
		complete = DoGetTimePlayedL(aMessage);
		break;

	// custom command support
	case EMMFDevSoundProxySyncCustomCommand:
		complete = DoSyncCustomCommandL(aMessage);
		break;
	case EMMFDevSoundProxySyncCustomCommandResult:
		complete = DoSyncCustomCommandResultL(aMessage);
		break;
	case EMMFDevSoundProxyAsyncCustomCommand:
		complete = DoAsyncCustomCommandL(aMessage);
		break;
	case EMMFDevSoundProxyAsyncCustomCommandResult:
		complete = DoAsyncCustomCommandResultL(aMessage);
		break;
	case EMMFDevSoundProxyEmptyBuffers:
		complete = DoEmptyBuffersL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		aMessage.Complete(KErrNone);
	}

TBool CMMFDevSoundSession::DoInitialize1L(const RMmfIpcMessage& aMessage)
	{
	if(iMsgQueue.Handle() == 0)
		{
		TInt err = iMsgQueue.Open(aMessage, 1);	// a global queue.
		User::LeaveIfError(err);
		}
	TMMFDevSoundProxySettingsPckg buf;
	MmfMessageUtil::ReadL(aMessage,0,buf);
	TMMFState mode = buf().iMode;
	iBody->InitializeL(*this, mode);
	iBufferPlay = NULL;
	return ETrue;
	}

TBool CMMFDevSoundSession::DoInitialize2L(const RMmfIpcMessage& aMessage)
	{
	if(iMsgQueue.Handle() == 0)
		{
		TInt err = iMsgQueue.Open(aMessage, 1);	// a global queue.
		User::LeaveIfError(err);
		}
	TMMFDevSoundProxySettingsPckg buf;
	MmfMessageUtil::ReadL(aMessage,0,buf);
	TUid HWDev = buf().iHWDev;
	TMMFState mode = buf().iMode;
	iBody->InitializeL(*this, HWDev, mode);
	iBufferPlay = NULL;
	return ETrue;
	}

TBool CMMFDevSoundSession::DoInitialize3L(const RMmfIpcMessage& /*aMessage*/)
	{
	User::Leave(KErrNotSupported);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoInitialize4L(const RMmfIpcMessage& aMessage)
	{
	if(iMsgQueue.Handle() == 0)
		{
		TInt err = iMsgQueue.Open(aMessage, 1);	// a global queue.
		User::LeaveIfError(err);
		}
	TMMFDevSoundProxySettingsPckg buf;
	aMessage.ReadL(TInt(0),buf);
	TFourCC desiredFourCC = buf().iDesiredFourCC;
	TMMFState mode = buf().iMode;
	iBody->InitializeL(*this, desiredFourCC, mode);
	iBufferPlay = NULL;
	return ETrue;
	}

TBool CMMFDevSoundSession::DoCapabilitiesL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings set;
	set.iCaps = iBody->Capabilities();
	TMMFDevSoundProxySettingsPckg pckg(set);
	aMessage.WriteL(TInt(2),pckg);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoConfigL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings set;
	set.iConfig = iBody->Config();
	TMMFDevSoundProxySettingsPckg pckg(set);
	aMessage.WriteL(TInt(2),pckg);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoSetConfigL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	aMessage.ReadL(TInt(0),buf);
	TMMFCapabilities config = buf().iConfig;
	iBody->SetConfigL(config);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoMaxVolumeL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings set;
	set.iMaxVolume = iBody->MaxVolume();
	TMMFDevSoundProxySettingsPckg pckg(set);
	aMessage.WriteL(TInt(2),pckg);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoVolumeL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings set;
	set.iVolume = iBody->Volume();
	TMMFDevSoundProxySettingsPckg pckg(set);
	aMessage.WriteL(TInt(2),pckg);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoSetVolumeL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	aMessage.ReadL(TInt(0),buf);
	TInt volume = buf().iVolume;
	iBody->SetVolume(volume);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoMaxGainL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings set;
	set.iMaxGain = iBody->MaxGain();
	TMMFDevSoundProxySettingsPckg pckg(set);
	aMessage.WriteL(TInt(2),pckg);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoGainL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings set;
	set.iGain = iBody->Gain();
	TMMFDevSoundProxySettingsPckg pckg(set);
	aMessage.WriteL(TInt(2),pckg);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoSetGainL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	aMessage.ReadL(TInt(0),buf);
	TInt gain = buf().iGain;
	iBody->SetGain(gain);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoGetPlayBalanceL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings set;
	iBody->GetPlayBalanceL(set.iLeftPercentage, set.iRightPercentage);
	TMMFDevSoundProxySettingsPckg pckg(set);
	aMessage.WriteL(TInt(2),pckg);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoSetPlayBalanceL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	aMessage.ReadL(TInt(0),buf);
	TInt leftPercentage = buf().iLeftPercentage;
	TInt rightPercentage = buf().iRightPercentage;
	iBody->SetPlayBalanceL(leftPercentage, rightPercentage);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoGetRecordBalanceL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings set;
	iBody->GetRecordBalanceL(set.iLeftPercentage, set.iRightPercentage);
	TMMFDevSoundProxySettingsPckg pckg(set);
	aMessage.WriteL(TInt(2),pckg);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoSetRecordBalanceL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	aMessage.ReadL(TInt(0),buf);
	TInt leftPercentage = buf().iLeftPercentage;
	TInt rightPercentage = buf().iRightPercentage;
	iBody->SetRecordBalanceL(leftPercentage, rightPercentage);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoPlayInitL(const RMmfIpcMessage& /*aMessage*/)
	{
	iBody->PlayInitL();
	return ETrue;
	}

TBool CMMFDevSoundSession::DoRecordInitL(const RMmfIpcMessage& aMessage)
	{
	iBody->RecordInitL(aMessage);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoPlayDataL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxyHwBufPckg buf;
	aMessage.ReadL(TInt(0),buf);
	iBufferPlay->SetLastBuffer(buf().iLastBuffer);

	aMessage.ReadL(TInt(1),iBufferPlay->Data());
	return iBody->PlayData(aMessage);
	}

TBool CMMFDevSoundSession::DoRecordDataL(const RMmfIpcMessage& aMessage)
	{
	return iBody->RecordData(aMessage);
	}

TBool CMMFDevSoundSession::DoStopL(const RMmfIpcMessage& /*aMessage*/)
	{
	if(iMsgQueue.Handle() != 0)
		{
		TMMFDevSoundQueueItem queueItem;
		TInt err = KErrNone;
		while(err != KErrUnderflow)
			{
			err = iMsgQueue.Receive(queueItem);
			}		
		}
	iBody->Stop();
	return ETrue;
	}

TBool CMMFDevSoundSession::DoPauseL(const RMmfIpcMessage& /*aMessage*/)
	{
	iBody->Pause();
	return ETrue;
	}

TBool CMMFDevSoundSession::DoPlayToneL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	aMessage.ReadL(TInt(0),buf);
	TInt frequency = buf().iFrequencyOne;
	TTimeIntervalMicroSeconds duration(buf().iDuration);
	iBody->PlayToneL(frequency, duration);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoPlayDualToneL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	aMessage.ReadL(TInt(0),buf);
	TInt frequencyOne = buf().iFrequencyOne;
	TInt frequencyTwo = buf().iFrequencyTwo;
	TTimeIntervalMicroSeconds duration(buf().iDuration);
	iBody->PlayDualToneL(frequencyOne, frequencyTwo, duration);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoPlayDTMFStringL(const RMmfIpcMessage& aMessage)
	{
	TInt DTMFLength = User::LeaveIfError(aMessage.GetDesLength(0));

	if(iDTMFString)
		{
		delete iDTMFString;
		iDTMFString = NULL;
		}

	iDTMFString = HBufC::NewL(DTMFLength);
	TPtr DTMFPtr = iDTMFString->Des();
	aMessage.ReadL(TInt(0), DTMFPtr);

	iBody->PlayDTMFStringL(*iDTMFString);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoPlayToneSequenceL(const RMmfIpcMessage& aMessage)
	{
	TInt toneLength = User::LeaveIfError(aMessage.GetDesLength(0));

	if(iToneSeqBuf)
		{
		delete iToneSeqBuf;
		iToneSeqBuf = NULL;
		}

	iToneSeqBuf = HBufC8::NewL(toneLength);
	TPtr8 toneSeqPtr = iToneSeqBuf->Des();
	aMessage.ReadL(TInt(0), toneSeqPtr);

	iBody->PlayToneSequenceL(*iToneSeqBuf);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoPlayFixedSequenceL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TInt> buf;
	aMessage.ReadL(TInt(0),buf);
	TInt seqNum = buf();

	iBody->PlayFixedSequenceL(seqNum);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoSetDTMFLengthsL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	aMessage.ReadL(TInt(0),buf);
	TTimeIntervalMicroSeconds32 toneOnLength = buf().iToneOnLength;
	TTimeIntervalMicroSeconds32 toneOffLength = buf().iToneOffLength;
	TTimeIntervalMicroSeconds32 pauseLength = buf().iPauseLength;
	iBody->SetDTMFLengths(toneOnLength, toneOffLength, pauseLength);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoSetVolumeRampL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	aMessage.ReadL(TInt(0),buf);
	TTimeIntervalMicroSeconds duration = buf().iDuration;
	iBody->SetVolumeRamp(duration);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoGetSupportedInputDataTypesL(const RMmfIpcMessage& aMessage)
	{
	iArray.Reset();

	TMMFPrioritySettingsPckg buf;
	aMessage.ReadL(TInt(0),buf);
	TMMFPrioritySettings prioritySet = buf();

	iBody->GetSupportedInputDataTypesL(iArray, prioritySet);

	TPckgBuf<TInt> pckg;
	pckg() = iArray.Count();
	aMessage.WriteL(TInt(2),pckg);

	return ETrue;
	}

TBool CMMFDevSoundSession::DoGetSupportedOutputDataTypesL(const RMmfIpcMessage& aMessage)
	{
	iArray.Reset();

	TMMFPrioritySettingsPckg buf;
	aMessage.ReadL(TInt(0),buf);
	TMMFPrioritySettings prioritySet = buf();

	iBody->GetSupportedOutputDataTypesL(iArray, prioritySet);

	TPckgBuf<TInt> pckg;
	pckg() = iArray.Count();
	aMessage.WriteL(TInt(2),pckg);

	return ETrue;
	}

TBool CMMFDevSoundSession::DoSamplesRecordedL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	pckg() = iBody->SamplesRecorded();
	aMessage.WriteL(TInt(2),pckg);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoSamplesPlayedL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	pckg() = iBody->SamplesPlayed();
	aMessage.WriteL(TInt(2),pckg);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoSetToneRepeatsL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TInt> countRepeat;
	aMessage.ReadL(TInt(0),countRepeat);

	TPckgBuf<TTimeIntervalMicroSeconds> repeatTS;
	aMessage.ReadL(TInt(1),repeatTS);

	iBody->SetToneRepeats(countRepeat(), repeatTS());
	return ETrue;
	}

TBool CMMFDevSoundSession::DoSetPrioritySettingsL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TMMFPrioritySettings> prioritySet;
	aMessage.ReadL(TInt(0),prioritySet);

	iBody->SetPrioritySettings(prioritySet());
	return ETrue;
	}

TBool CMMFDevSoundSession::DoFixedSequenceNameL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TInt> seqNum;
	aMessage.ReadL(0, seqNum);
	aMessage.WriteL(2, iBody->FixedSequenceName(seqNum()).Left(KMaxFixedSequenceNameLength));
	return ETrue;
	}

TBool CMMFDevSoundSession::DoFixedSequenceCountL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TInt> fixSeqCountPckg;
	TInt fixSeqCount = iBody->FixedSequenceCount();
	fixSeqCountPckg = fixSeqCount;

	aMessage.WriteL(TInt(0),fixSeqCountPckg);
	return ETrue;
	}


TBool CMMFDevSoundSession::DoCopyFourCCArrayDataL(const RMmfIpcMessage& aMessage)
	{
	const TInt KBufExpandSize8 = 8;//two TInts
	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<iArray.Count();i++)
		{
		stream.WriteInt32L(iArray[i].FourCC());
		}
	aMessage.WriteL(TInt(2), dataCopyBuffer->Ptr(0));
	CleanupStack::PopAndDestroy(2);//iDataCopyBuffer, stream
	return ETrue;
	}


TBool CMMFDevSoundSession::DoGetRecordedBufferL(const RMmfIpcMessage& aMessage)
	{
	MmfMessageUtil::Write(aMessage, TInt(0), iBufferRecord->Data());
	return ETrue;
	}

TBool CMMFDevSoundSession::DoBufferToBeFilledDataL(const RMmfIpcMessage& aMessage)
	{
	TInt err = MmfMessageUtil::Write(aMessage, 0, iHwBufPckgFill);
	aMessage.Complete(err);
	return EFalse;
	}

TBool CMMFDevSoundSession::DoBufferToBeEmptiedDataL(const RMmfIpcMessage& aMessage)
	{
	TInt err = MmfMessageUtil::Write(aMessage, 0, iHwBufPckgEmpty);
	aMessage.Complete(err);
	return EFalse;
	}

TBool CMMFDevSoundSession::DoRegisterAsClientL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	aMessage.ReadL(0,buf);
	HBufC8* notificationRegistrationData = NULL;
	notificationRegistrationData = HBufC8::NewLC(User::LeaveIfError(aMessage.GetDesLengthL(1)));
	TPtr8 dataPtr(notificationRegistrationData->Des());  	
	aMessage.ReadL(1,dataPtr);
	TInt err = KErrNone;
	err = iBody->RegisterAsClient(buf().iNotificationEventUid,dataPtr);
	CleanupStack::PopAndDestroy(1); // Notification Registeration data
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return EFalse;		
		}
	return ETrue;
	}
	
TBool CMMFDevSoundSession::DoCancelRegisterAsClientL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	aMessage.ReadL(0,buf);
	TInt err = KErrNone;
	err = iBody->CancelRegisterAsClient(buf().iNotificationEventUid);
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return EFalse;		
		}
	return ETrue;
	}

TBool CMMFDevSoundSession::DoGetResourceNotificationDataL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	aMessage.ReadL(0,buf);
	HBufC8* notificationData = NULL;
	notificationData = HBufC8::NewLC(User::LeaveIfError(aMessage.GetDesMaxLengthL(2)));
	TPtr8 dataPtr(notificationData->Des());  	
	aMessage.ReadL(2,dataPtr);
	TInt err = KErrNone;
	err = iBody->GetResourceNotificationData(buf().iNotificationEventUid,dataPtr);
	aMessage.WriteL(2,*notificationData);
	CleanupStack::PopAndDestroy(1); // Notification data
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return EFalse;		
		}
	return ETrue;
	}

TBool CMMFDevSoundSession::DoWillResumePlayL(const RMmfIpcMessage& aMessage)
	{
	TInt err = KErrNone;
	if(CheckClientCapabilities())
		{
		err = iBody->WillResumePlay();
		}
	else
		{
		err = KErrPermissionDenied;
		}
		
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return EFalse;		
		}
	return ETrue;
	}
	
TBool CMMFDevSoundSession::DoEmptyBuffersL(const RMmfIpcMessage& aMessage)
	{
	TInt err = KErrNone;
	err = iBody->EmptyBuffers();
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return EFalse;		
		}
	return ETrue;
	}

TBool CMMFDevSoundSession::DoSetClientThreadInfoL(const RMmfIpcMessage& aMessage)
	{
	if (aMessage.HasCapability(ECapabilityMultimediaDD))
		{
		TPckgBuf<TThreadId> threadId;
		aMessage.ReadL(0, threadId);
		
		CMMFDevSoundServer* server = 
			const_cast<CMMFDevSoundServer*>(static_cast<const CMMFDevSoundServer*>(Server()));
		server->SetClientCapabilitiesL(threadId());
		iClientHasCaps = server->CheckClientCapabilities();	
		}
	else
		{
		User::Leave(KErrPermissionDenied);
		}
	return ETrue;
	}
	
TBool CMMFDevSoundSession::DoGetTimePlayedL(const RMmfIpcMessage& aMessage)
	{
	TInt err = KErrNone;
	TPckgBuf<TTimeIntervalMicroSeconds> timePckg;
	err = iBody->GetTimePlayed(timePckg());
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return EFalse;		
		}
	aMessage.WriteL(TInt(2),timePckg);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoSyncCustomCommandL(const RMmfIpcMessage& aMessage)
	{
	// check whether these are custom interface commands
	// if so then send them into the demux pipeline
	TInt retVal = KErrNone;
	
	// try and process this as a custom interface
	TRAPD(err, retVal = iDeMuxUtility->ProcessCustomInterfaceCommandL(aMessage));
	if (err == KErrNone)
		{
		// we can pass back valid values here since command
		// has been handled by the DeMux framework
		aMessage.Complete(retVal);	
		}
	else if (err != KErrNotFound)
		{
		// the framework left with an error condition
		// so we complete the message with this error
		aMessage.Complete(err);
		}
	else 
		{
		// commmand was not found inside the Custom Interface framework
		// so we can pass onto the DevSound server implementation
		// assume that this will either leave or complete the message
		iBody->DoSyncCustomCommandL(aMessage);
		}
	
	// we complete our own message so don't need the framework to do so
	return EFalse;		
	}
	
TBool CMMFDevSoundSession::DoSyncCustomCommandResultL(const RMmfIpcMessage& aMessage)
	{	
	// check whether these are custom interface commands
	TInt retVal = KErrNone;
	TRAPD(err, retVal = iDeMuxUtility->ProcessCustomInterfaceCommandL(aMessage));
	if (err == KErrNone)
		{
		// we can pass back valid values here since command
		// has been handled by the DeMux framework
		aMessage.Complete(retVal);	
		}
	else if (err != KErrNotFound)
		{
		// the framework left with an error condition
		// so we complete the message with this error
		aMessage.Complete(err);
		}
	else 
		{
		// commmand was not found inside the Custom Interface framework
		// so we can pass onto the DevSound server implementation
		// assume that this will either leave or complete the message
		iBody->DoSyncCustomCommandResultL(aMessage);
		}
	// we complete our own message so don't need the framework to do so
	return EFalse;		
	}
	
TBool CMMFDevSoundSession::DoAsyncCustomCommandL(const RMmfIpcMessage& aMessage)
	{
	// check whether these are custom interface commands
	// async message will complete later
	TRAPD(err, iDeMuxUtility->ProcessCustomInterfaceCommandL(aMessage));
	if ((err != KErrNotFound) && (err != KErrNone))
		{
		// the framework left with an error condition
		// so we complete the message with this error
		aMessage.Complete(err);
		}
	else if (err == KErrNotFound)
		{
		// commmand was not found inside the Custom Interface framework
		// so we can pass onto the DevSound server implementation
		// assume that this will either leave or complete the message
		iBody->DoAsyncCustomCommandL(aMessage);
		}
	return EFalse;
	}
	
TBool CMMFDevSoundSession::DoAsyncCustomCommandResultL(const RMmfIpcMessage& aMessage)
	{
	// check whether these are custom interface commands
	// async message will complete later
	TRAPD(err, iDeMuxUtility->ProcessCustomInterfaceCommandL(aMessage));
	if ((err != KErrNotFound) && (err != KErrNone))
		{
		// the framework left with an error condition
		// so we complete the message with this error
		aMessage.Complete(err);
		}
	else if (err == KErrNotFound)
		{
		// commmand was not found inside the Custom Interface framework
		// so we can pass onto the DevSound server implementation
		// assume that this will either leave or complete the message
		iBody->DoAsyncCustomCommandL(aMessage);
		}
	return EFalse;
	}



void CMMFDevSoundSession::SendEventToClient(/*TMMFAudioPolicyEvent& aEvent*/)
	{
	}


/*
 *
 *	Default Constructor.
 *
 *
 */
CMMFDevSoundSession::CMMFDevSoundSession()
	{
	}

CMMFDevSoundSession::~CMMFDevSoundSession()
	{
	// clear the array of custom interfaces
	for (TInt i = 0; i < iCustomInterfaceArray.Count(); i++)
		{
		// we could have already deleted interfaces without
		// removing them from the array so check for this
		// and only delete release plugin if non-null
		MMMFDevSoundCustomInterfaceDeMuxPlugin* ptr = iCustomInterfaceArray[i].iInterface;
		if (ptr)
			{
			iCustomInterfaceArray[i].iInterface->Release();	
			}
		}
	iCustomInterfaceArray.Reset();
	iCustomInterfaceArray.Close();
		
	delete iDeMuxUtility;

	iMsgQueue.Close();
	iArray.Close();
	delete iDTMFString;
	delete iToneSeqBuf;
	delete iBody;

	CMMFDevSoundServer* server = 
		const_cast<CMMFDevSoundServer*>(static_cast<const CMMFDevSoundServer*>(Server()));
	if (server)
		{
		server->DecrementSessionId();		
		}
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Constructs, and returns a pointer to, a new CMMFDevSoundProxy object.
 *
 *	Leaves on failure.
 *
 */
CMMFDevSoundSession* CMMFDevSoundSession::NewL(RServer2& aPolicyServerHandle)
	{
	CMMFDevSoundSession* self = new (ELeave) CMMFDevSoundSessionXtnd;
	CleanupStack::PushL(self);
	self->ConstructL(aPolicyServerHandle);
	CleanupStack::Pop();
	return self;
	}

/*
 *  -doxygen comments moved to header-
 *
 *	Second phase constructor.
 *
 */
void CMMFDevSoundSession::ConstructL(RServer2& aPolicyServerHandle)
	{
	iBody = CMMFDevSoundSvrImp::NewL(static_cast<CMMFDevSoundSessionXtnd*>(this));
	iBody->Construct3L(aPolicyServerHandle);
	
	iDeMuxUtility = CMMFDevSoundCIDeMuxUtility::NewL(this);
	}

//callbacks
void CMMFDevSoundSession::InitializeComplete(TInt aError)
	{
	// this may be a re-initialization and so we need to
	// re-get our custom interfaces on the DeMux plugins
	for (TInt i = 0; i < iCustomInterfaceArray.Count(); i++)
		{
		// we could have already deleted interfaces without
		// removing them from the array so check for this
		// and only delete release plugin if non-null
		MMMFDevSoundCustomInterfaceDeMuxPlugin* ptr = iCustomInterfaceArray[i].iInterface;
		if (ptr)
			{
			// we can't keep track of..
			// 1. where a custom interface is implemented
			// 2. the uid of the custom interface to be refreshed
			// so assume all have to be refreshed
			TRAPD(err, iCustomInterfaceArray[i].iInterface->RefreshL());	
			
			// if there is an error then this is no longer a 
			// valid interface so could be deleted this from the array
			// but this would involve notifying the client side
			//
			// since we have no way of notifying the client in this
			// implementation and the fact that this is a prototype 
			// implementation and we will leave this up to the licensee 
			// to implement as required
			if (err != KErrNone)
				{
				TMMFEvent event;
				TMMFDevSoundQueueItem item;
				item.iRequest = EMMFDevSoundCustomCommandCloseMuxDemuxPair;
				item.iErrorCode = err;
				event.iEventType.iUid = i+1;
				item.iEventPckg() = event;
				iMsgQueue.Send(item);
				}
			}
		}
	
	// The previous implementation was commented out,
	// so add a new commented out implementation :)
	TMMFDevSoundQueueItem item;
	item.iRequest = EMMFDevSoundProxyICEvent;
	item.iErrorCode = aError;
	iMsgQueue.Send(item); // assumes sufficient space in the queue so ignores the return value
	}

void CMMFDevSoundSession::ToneFinished(TInt aError)
	{
	TMMFDevSoundQueueItem item;
	item.iRequest = EMMFDevSoundProxyTFEvent;
	item.iErrorCode = aError;
	iMsgQueue.Send(item); // assumes sufficient space in the queue so ignores the return value
	}

void CMMFDevSoundSession::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	// Package up the data for retrieval later (using a two stage process
	// as this payload is too large to be sent via the queue in one message)	
	iBufferPlay = reinterpret_cast<CMMFDataBuffer*>(aBuffer);
	iHwBufPckgFill().iBufferType = iBufferPlay->Type();
	iHwBufPckgFill().iRequestSize = iBufferPlay->RequestSize();
	iHwBufPckgFill().iBufferSize = iBufferPlay->Data().MaxLength();
	iHwBufPckgFill().iLastBuffer = iBufferPlay->LastBuffer();	
	
	TMMFDevSoundQueueItem item;
	item.iRequest = EMMFDevSoundProxyBTBFEvent;
	iMsgQueue.Send(item); // assumes sufficient space in the queue so ignores the return value
	}

void CMMFDevSoundSession::PlayError(TInt aError)
	{
	TMMFDevSoundQueueItem item;
	item.iRequest = EMMFDevSoundProxyPEEvent;
	item.iErrorCode = aError;
	iMsgQueue.Send(item); // assumes sufficient space in the queue so ignores the return value
	}

void CMMFDevSoundSession::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	iBufferRecord = reinterpret_cast<CMMFDataBuffer*>(aBuffer);

	iHwBufPckgEmpty().iBufferType = iBufferRecord->Type();
	iHwBufPckgEmpty().iRequestSize = iBufferRecord->RequestSize();
	iHwBufPckgEmpty().iBufferSize = iBufferRecord->Data().MaxLength();
	iHwBufPckgEmpty().iLastBuffer = iBufferRecord->LastBuffer();
	
	TMMFDevSoundQueueItem item;
	item.iRequest = EMMFDevSoundProxyBTBEEvent;
	iMsgQueue.Send(item); // assumes sufficient space in the queue so ignores the return value
	}

void CMMFDevSoundSession::RecordError(TInt aError)
	{
	TMMFDevSoundQueueItem item;
	item.iRequest = EMMFDevSoundProxyREEvent;
	item.iErrorCode = aError;
	iMsgQueue.Send(item); // assumes sufficient space in the queue so ignores the return value
	}

void CMMFDevSoundSession::ConvertError(TInt /*aError*/)
	{
	}

void CMMFDevSoundSession::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	}

void CMMFDevSoundSession::SendEventToClient(const TMMFEvent& aEvent)
	{
	TMMFDevSoundQueueItem item;
	item.iRequest = EMMFDevSoundProxySETCEvent;
	item.iEventPckg() = aEvent;
	iMsgQueue.Send(item); // assumes sufficient space in the queue so ignores the return value
    }

/********************************************************************************
 *				Non Exported public functions begins here						*
 ********************************************************************************/

//
//				Audio Policy specific implementation begins here				//
//

/**
 *
 *	Sets Id for this instance of DevSound
 *
 *	@param	"TInt aDevSoundId"
 *			Integer value assigned by Audio Policy Server
 *
 */
void CMMFDevSoundSessionXtnd::SetDevSoundId(TInt aDevSoundId)
	{
	iBody->SetDevSoundId(aDevSoundId);
	}

/**
 *
 *	Returns information about this DevSound instance.
 *
 *	This method is used by Audio Policy Server to make audio policy decisions.
 *
 *	@return	"TMMFDevSoundinfo"
 *			A reference to TMMFDevSoundinfo object holding the current settings
 *			of this DevSound instance.
 *
 */
TMMFDevSoundInfo CMMFDevSoundSessionXtnd::DevSoundInfo()
	{
 	return iBody->DevSoundInfo();
	}

/**
 *
 *	Called by Audio Policy Server when a request to play is approved by the 
 *	Audio Policy Server.
 *
 *	Leaves on failure.
 *
 */
void CMMFDevSoundSessionXtnd::StartPlayDataL()
	{
	iBody->StartPlayDataL();
	}

/**
 *
 *	Called by Audio Policy Server when a request to record is approved by the 
 *	Audio Policy Server.
 *
 *	Leaves on failure.
 *
 */
void CMMFDevSoundSessionXtnd::StartRecordDataL()
	{
	iBody->StartRecordDataL();
	}

/**
 *
 *	Called by Audio Policy Server when a request to play tone is approved by
 *	the Audio Policy Server.
 *
 *	Leaves on failure.
 *
 */
void CMMFDevSoundSessionXtnd::StartPlayToneL()
	{
	iBody->StartPlayToneL();
	}

/**
 *
 *	Called by Audio Policy Server when a request to play a dual tone is approved by
 *	the Audio Policy Server.
 *
 */
void CMMFDevSoundSessionXtnd::StartPlayDualToneL()
	{
	iBody->StartPlayDualToneL();
	}

/**
 *
 *	Called by Audio Policy Server when a request to play DTMF String is approved
 *	by the Audio Policy Server.
 *
 *	Leaves on failure.
 *
 */
void CMMFDevSoundSessionXtnd::StartPlayDTMFStringL()
	{
	iBody->StartPlayDTMFStringL();
	}

/**
 *
 *	Called by Audio Policy Server when a request to play tone sequence is
 *	approved by the Audio Policy Server.
 *
 *	Leaves on failure.
 *
 */
void CMMFDevSoundSessionXtnd::StartPlayToneSequenceL()
	{
	iBody->StartPlayToneSequenceL();
	}

/**
 *
 *	Called by Audio Policy Server when the current DevSound instance looses the
 *	policy because of another instance with a higher priority wants the device.
 *
 */
void CMMFDevSoundSessionXtnd::SendEvent(const TMMFEvent& aEvent)
	{
	iBody->SendEventToClient(aEvent);
	}


//
//				Audio Policy specific implementation begins here				//
//

/**
 *
 *	Updates the total bytes played.
 *
 */
void CMMFDevSoundSessionXtnd::UpdateBytesPlayed()
	{
	iBody->UpdateBytesPlayed();
	}


// Custom Interface //
TInt CMMFDevSoundSession::DoOpenSlaveL(TUid aInterface, const TDesC8& aPackageBuf)
	{
	// it shouldn't be necessary to check if we have already instantiated this
	// interface since the client would already know - however this is something
	// that a licensee could implement if they required additional functionality
	// e.g. many : 1 mappings between client and DevSound.

	MMMFDevSoundCustomInterfaceDeMuxPlugin* ptr = NULL;
		
	// try and instantiate a plugin tunnelling
	// pair to support this Custom Interface
	ptr = iDeMuxUtility->CreateCustomInterfaceDeMuxL(aInterface);
	
	TInt handle = KNullHandle;
	
	if (ptr)
		{
		TMMFDevSoundCustomInterfaceDeMuxData data;
		data.iInterface = ptr;
		data.iId = aInterface;
			
		CleanupReleasePushL(*ptr);
			
		// setup demux plugin
		ptr->SetInterfaceTarget(iBody);
			
		// try and open interface
		// this will fetch the interface from the svr implementation
		ptr->DoOpenSlaveL(aInterface, aPackageBuf);
		User::LeaveIfError(iCustomInterfaceArray.Append(data));
			
		CleanupStack::Pop();	// ptr
			
		handle = iCustomInterfaceArray.Count();
		return handle;
		}

	// we couldn't set up the interface correctly so return a NULL
	// handle to the client
	return KNullHandle;
	}
	
void CMMFDevSoundSession::DoCloseSlaveL(TInt aHandle)
	{
	if (aHandle==KNullHandle)
		{
		// null-handle -> NOP
		return;
		}
		
	if (aHandle<KNullHandle || aHandle > iCustomInterfaceArray.Count())
		{
		// handle out of range - should not happen, but leave to show error
		User::Leave(KErrBadHandle);
		}
		
	// set the current handle location to NULL
	// can't re-compress array because this will alter handles
	// we could change this to a list type structure but this 
	// seems overkill for the current prototype
	TMMFDevSoundCustomInterfaceDeMuxData& data = iCustomInterfaceArray[aHandle-1];
	
	// close and delete the plugin
	MMMFDevSoundCustomInterfaceDeMuxPlugin* ptr = data.iInterface;
	ptr->DoCloseSlaveL(aHandle);
	ptr->Release();
	
	// clear the entry
	data.iInterface = NULL;
	data.iId.iUid = 0;
	}
	
TInt CMMFDevSoundSession::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	// use the demux utility to get the handle
	TMMFDevSoundCIMessageData data;
	iDeMuxUtility->GetSyncMessageDataL(aMessage, data);
	
	TInt handle = data.iHandle;
	
	if ((handle <= 0) || (handle > (iCustomInterfaceArray.Count())))
		{
		
		User::Leave(KErrBadHandle);
		}
	
	// call on demux plugin
	return iCustomInterfaceArray[handle-1].iInterface->DoSendSlaveSyncCommandL(aMessage);	
	}
	
TInt CMMFDevSoundSession::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	// use the demux utility to get the handle
	TMMFDevSoundCIMessageData data;
	iDeMuxUtility->GetSyncMessageDataL(aMessage, data);
	
	TInt handle = data.iHandle;
	
	if ((handle <= 0) || (handle > (iCustomInterfaceArray.Count())))
		{
		
		User::Leave(KErrBadHandle);
		}
	
	// call on demux plugin
	return iCustomInterfaceArray[handle-1].iInterface->DoSendSlaveSyncCommandResultL(aMessage);	
	}
	
void CMMFDevSoundSession::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& aMessage)
	{
	// use the demux utility to get the handle
	TMMFDevSoundCIMessageData data;
	iDeMuxUtility->GetAsyncMessageDataL(aMessage, data);
	
	TInt handle = data.iHandle;
	
	if ((handle <= 0) || (handle > (iCustomInterfaceArray.Count())))
		{
		User::Leave(KErrBadHandle);
		}
	
	// call on demux plugin
	iCustomInterfaceArray[handle-1].iInterface->DoSendSlaveAsyncCommandL(aMessage);	
	}
	
void CMMFDevSoundSession::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	// use the demux utility to get the handle
	TMMFDevSoundCIMessageData data;
	iDeMuxUtility->GetAsyncMessageDataL(aMessage, data);
	
	TInt handle = data.iHandle;
	
	if ((handle <= 0) || (handle > (iCustomInterfaceArray.Count())))
		{
		User::Leave(KErrBadHandle);
		}
	
	// call on demux plugin
	iCustomInterfaceArray[handle-1].iInterface->DoSendSlaveAsyncCommandResultL(aMessage);	
	}



