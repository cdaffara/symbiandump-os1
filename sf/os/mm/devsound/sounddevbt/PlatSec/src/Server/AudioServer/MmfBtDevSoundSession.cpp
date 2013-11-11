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
#include "MmfBtDevSoundSession.h"
#include "MmfBtDevSoundSessionXtnd.h"
#include "MmfBtDevSoundSessionBody.h"

#include "MmfBtAudioClientServer.h"
#include "MmfBtAudioServer.h"
#include "MmfBtDevSoundServer.h"


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
	case EMMFDevSoundProxyReceiveICEvent:
		complete = DoInitializeCompleteEventL(aMessage);
		break;
	case EMMFDevSoundProxyCancelReceiveICEvent:
		complete = DoCancelInitializeCompleteEventL(aMessage);
		break;
	case EMMFDevSoundProxyReceiveBTBFEvent:
		complete = DoBufferToBeFilledEventL(aMessage);
		break;
	case EMMFDevSoundProxyCancelReceiveBTBFEvent:
		complete = DoCancelBufferToBeFilledEventL(aMessage);
		break;	
	case EMMFDevSoundProxyReceiveBTBEEvent:
		complete = DoBufferToBeEmptiedEventL(aMessage);
		break;
	case EMMFDevSoundProxyCancelReceiveBTBEEvent:
		complete = DoCancelBufferToBeEmptiedEventL(aMessage);
		break;
	case EMMFDevSoundProxyReceivePEEvent:
		complete = DoPlayErrorEventL(aMessage);
		break;
	case EMMFDevSoundProxyCancelReceivePEEvent:
		complete = DoCancelPlayErrorEventL(aMessage);
		break;
	case EMMFDevSoundProxyReceiveREEvent:
		complete = DoRecordErrorEventL(aMessage);
		break;
	case EMMFDevSoundProxyCancelReceiveREEvent:
		complete = DoCancelRecordErrorEventL(aMessage);
		break;
	case EMMFDevSoundProxyReceiveSETCEvent:
		complete = DoSendEventToClientEventL(aMessage);
		break;
	case EMMFDevSoundProxyCancelReceiveSETCEvent:
		complete = DoCancelSendEventToClientEventL(aMessage);
		break;
	case EMMFDevSoundProxyReceiveTFEvent:
		complete = DoToneFinishedEventL(aMessage);
		break;
	case EMMFDevSoundProxyCancelReceiveTFEvent:
		complete = DoCancelToneFinishedEventL(aMessage);
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
	case EMMFDevSoundProxyConvertInit:
		complete = DoConvertInitL(aMessage);
		break;
	case EMMFDevSoundProxyConvertData:
		complete = DoConvertDataL(aMessage);
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
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		aMessage.Complete(KErrNone);
	}

TBool CMMFDevSoundSession::DoInitialize1L(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	MmfMessageUtil::ReadL(aMessage,0,buf);
	TMMFState mode = buf().iMode;
	iBody->InitializeL(*this, mode);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoInitialize2L(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	MmfMessageUtil::ReadL(aMessage,0,buf);
	TUid HWDev = buf().iHWDev;
	TMMFState mode = buf().iMode;
	iBody->InitializeL(*this, HWDev, mode);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoInitialize3L(const RMmfIpcMessage& /*aMessage*/)
	{
	User::Leave(KErrNotSupported);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoInitialize4L(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	aMessage.ReadL(TInt(0),buf);
	TFourCC desiredFourCC = buf().iDesiredFourCC;
	TMMFState mode = buf().iMode;
	iBody->InitializeL(*this, desiredFourCC, mode);
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
	iBody->PlayData();
	return ETrue;
	}

TBool CMMFDevSoundSession::DoRecordDataL(const RMmfIpcMessage& aMessage)
	{
	iBody->RecordData(aMessage);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoStopL(const RMmfIpcMessage& /*aMessage*/)
	{
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
	TInt DTMFLength = aMessage.GetDesLength(0);

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
	TInt toneLength = aMessage.GetDesLength(0);

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

TBool CMMFDevSoundSession::DoConvertInitL(const RMmfIpcMessage& /*aMessage*/)
	{
	iBody->ConvertInitL();
	return ETrue;
	}

TBool CMMFDevSoundSession::DoConvertDataL(const RMmfIpcMessage& /*aMessage*/)
	{
	iBody->ConvertData();
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
	stream.Close();
	CleanupStack::PopAndDestroy(2);//iDataCopyBuffer, stream
	return ETrue;
	}


TBool CMMFDevSoundSession::DoGetRecordedBufferL(const RMmfIpcMessage& aMessage)
	{
	MmfMessageUtil::Write(aMessage, TInt(0), iBufferRecord->Data());
	return ETrue;
	}


TBool CMMFDevSoundSession::DoInitializeCompleteEventL(const RMmfIpcMessage& aMessage)
	{
	if (iICMessage)
		{
		delete iICMessage;
		iICMessage = NULL;
		}
	iICMessage = new (ELeave) RMmfIpcMessage(aMessage);
	return EFalse;
	}

TBool CMMFDevSoundSession::DoCancelInitializeCompleteEventL(const RMmfIpcMessage& /*aMessage*/)
	{
	if(iICMessage)
		iICMessage->Complete(KErrCancel);
	return ETrue;
	}


TBool CMMFDevSoundSession::DoBufferToBeFilledEventL(const RMmfIpcMessage& aMessage)
	{
	if(iBTBFMessage)
		{
		delete iBTBFMessage;
		iBTBFMessage = NULL;
		}
	iBTBFMessage = new (ELeave) RMmfIpcMessage(aMessage);
	return EFalse;
	}

TBool CMMFDevSoundSession::DoCancelBufferToBeFilledEventL(const RMmfIpcMessage& /*aMessage*/)
	{
	if(iBTBFMessage)
		iBTBFMessage->Complete(KErrCancel);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoBufferToBeEmptiedEventL(const RMmfIpcMessage& aMessage)
	{
	if(iBTBEMessage)
		{
		delete iBTBEMessage;
		iBTBEMessage = NULL;
		}
	iBTBEMessage = new (ELeave) RMmfIpcMessage(aMessage);
	return EFalse;
	}

TBool CMMFDevSoundSession::DoCancelBufferToBeEmptiedEventL(const RMmfIpcMessage& /*aMessage*/)
	{
	if(iBTBEMessage)
		iBTBEMessage->Complete(KErrCancel);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoPlayErrorEventL(const RMmfIpcMessage& aMessage)
	{
	if(iPEMessage)
		{
		delete iPEMessage;
		iPEMessage = NULL;
		}
	iPEMessage = new (ELeave) RMmfIpcMessage(aMessage);
	return EFalse;
	}

TBool CMMFDevSoundSession::DoCancelPlayErrorEventL(const RMmfIpcMessage& /*aMessage*/)
	{
	if(iPEMessage)
		iPEMessage->Complete(KErrCancel);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoRecordErrorEventL(const RMmfIpcMessage& aMessage)
	{
	if(iREMessage)
		{
		delete iREMessage;
		iREMessage = NULL;
		}
	iREMessage = new (ELeave) RMmfIpcMessage(aMessage);
	return EFalse;
	}

TBool CMMFDevSoundSession::DoCancelRecordErrorEventL(const RMmfIpcMessage& /*aMessage*/)
	{
	if(iREMessage)
		iREMessage->Complete(KErrCancel);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoToneFinishedEventL(const RMmfIpcMessage& aMessage)
	{
	if(iTFMessage)
		{
		delete iTFMessage;
		iTFMessage = NULL;
		}
	iTFMessage = new (ELeave) RMmfIpcMessage(aMessage);
	return EFalse;
	}

TBool CMMFDevSoundSession::DoCancelToneFinishedEventL(const RMmfIpcMessage& /*aMessage*/)
	{
	if(iTFMessage)
		iTFMessage->Complete(KErrCancel);
	return ETrue;
	}

TBool CMMFDevSoundSession::DoSendEventToClientEventL(const RMmfIpcMessage& aMessage)
	{
	if(iSETCMessage)
		{
		delete iSETCMessage;
		iSETCMessage = NULL;
		}
	iSETCMessage = new (ELeave) RMmfIpcMessage(aMessage);
	return EFalse;
	}

TBool CMMFDevSoundSession::DoCancelSendEventToClientEventL(const RMmfIpcMessage& /*aMessage*/)
	{
	if(iSETCMessage)
		iSETCMessage->Complete(KErrCancel);
	return ETrue;
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
	delete iBTBFMessage;
	delete iBTBEMessage;
	delete iPEMessage;
	delete iREMessage;
	delete iTFMessage;
	delete iICMessage;
	delete iSETCMessage;
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
	}

//callbacks
void CMMFDevSoundSession::InitializeComplete(TInt aError)
	{
	if (iICMessage)
		{
		iICMessage->Complete(aError);
		delete iICMessage;
		iICMessage = NULL;
		}
	}

void CMMFDevSoundSession::ToneFinished(TInt aError)
	{
	iTFMessage->Complete(aError);
	delete iTFMessage;
	iTFMessage = NULL;
	}

void CMMFDevSoundSession::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	/* store here "aBuffer" to be used later with PlayData*/
	iBufferPlay = reinterpret_cast<CMMFDataBuffer*>(aBuffer);

	TMMFDevSoundProxyHwBuf set;
	set.iBufferType = iBufferPlay->Type();
	set.iRequestSize = iBufferPlay->RequestSize();
	set.iBufferSize = iBufferPlay->Data().MaxLength();
	set.iLastBuffer = iBufferPlay->LastBuffer();
	TMMFDevSoundProxyHwBufPckg pckg(set);
	TInt err = MmfMessageUtil::Write(*iBTBFMessage, 0, pckg);
	iBTBFMessage->Complete(err);
	delete iBTBFMessage;
	iBTBFMessage = NULL;
	}

void CMMFDevSoundSession::PlayError(TInt aError)
	{
	if (iPEMessage)
		{
		iPEMessage->Complete(aError);
		delete iPEMessage;
		iPEMessage = NULL;
		}
	else
		{
		// need to send message but for some reason nobody to send to - just swallow
		__DEBUGGER() 
		}
	}

void CMMFDevSoundSession::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	iBufferRecord = reinterpret_cast<CMMFDataBuffer*>(aBuffer);

	TMMFDevSoundProxyHwBuf set;
	set.iBufferType = iBufferRecord->Type();
	set.iRequestSize = iBufferRecord->RequestSize();
	set.iBufferSize = iBufferRecord->Data().MaxLength();
	set.iLastBuffer = iBufferRecord->LastBuffer();
	TMMFDevSoundProxyHwBufPckg pckg(set);
	TInt err = MmfMessageUtil::Write(*iBTBEMessage, 0, pckg);
	iBTBEMessage->Complete(err);
	delete iBTBEMessage;
	iBTBEMessage = NULL;
	}

void CMMFDevSoundSession::RecordError(TInt aError)
	{
	iREMessage->Complete(aError);
	delete iREMessage;
	iREMessage = NULL;
	}

void CMMFDevSoundSession::ConvertError(TInt /*aError*/)
	{
	}
void CMMFDevSoundSession::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	}

void CMMFDevSoundSession::SendEventToClient(const TMMFEvent& aEvent)
	{
	TMMFEventPckg iEventPckg(aEvent);
	TInt err = MmfMessageUtil::Write(*iSETCMessage, 0, iEventPckg);
	iSETCMessage->Complete(err);
	delete iSETCMessage;
	iSETCMessage = NULL;
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




