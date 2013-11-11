// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "A2dpBTHeadsetAudioIfServer.h"
#include "A2dpBTHeadsetAudioIfServerStart.h"
#include "A2dpBTheadsetAudioIf.h"
#include "MMFBtRoutingSoundDevice.h"	// for TRange


CA2dpBTHeadsetAudioIfServer* CA2dpBTHeadsetAudioIfServer::NewL()
	{
	CA2dpBTHeadsetAudioIfServer* self = new(ELeave) CA2dpBTHeadsetAudioIfServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CA2dpBTHeadsetAudioIfServer::~CA2dpBTHeadsetAudioIfServer()
	{
	delete iBTAudioInterface;
	}
	
CMmfIpcSession* CA2dpBTHeadsetAudioIfServer::NewSessionL(const TVersion &aVersion) const
	{
	TVersion version(	KBTAudioServerMajorVersionNumber,
						KBTAudioServerMinorVersionNumber,
						KBTAudioServerBuildVersionNumber);

	if(!User::QueryVersionSupported(version, aVersion))
		{
		User::Leave(KErrNotSupported);
		}
	
	CA2dpBTHeadsetAudioIfSession* session = CA2dpBTHeadsetAudioIfSession::NewL(iBTAudioInterface);
	return session;
	}

CA2dpBTHeadsetAudioIfServer::CA2dpBTHeadsetAudioIfServer() :
							 CMmfIpcServer(EPriorityStandard)
	{
	}
	
void CA2dpBTHeadsetAudioIfServer::ConstructL()
	{
	// Create the interface to the BT headset
	iBTAudioInterface = CA2dpBTHeadsetAudioInterface::NewL();
	
	// Call base class to Start server
	TName name(RThread().Name());
	StartL(name);
	}


/*
 Session implementation.
 */
CA2dpBTHeadsetAudioIfSession* CA2dpBTHeadsetAudioIfSession::NewL( CA2dpBTHeadsetAudioInterface* 
																	aBTAudioInterface)
	{
	CA2dpBTHeadsetAudioIfSession* self = new(ELeave) CA2dpBTHeadsetAudioIfSession(aBTAudioInterface);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CA2dpBTHeadsetAudioIfSession::CA2dpBTHeadsetAudioIfSession(CA2dpBTHeadsetAudioInterface* aBTAudioInterface) :
							iBTAudioInterface(aBTAudioInterface)
	{
	}

void CA2dpBTHeadsetAudioIfSession::ConstructL()
	{
	// Create the AO event handlers
	iInitHandler = CA2dpBTHeadsetIfEventHandler::NewL();
	iOpenDeviceHandler = CA2dpBTHeadsetIfEventHandler::NewL();
	iCloseDeviceHandler = CA2dpBTHeadsetIfEventHandler::NewL();
	iPlayDataHandler = CA2dpBTHeadsetIfEventHandler::NewL();
	iNotifyErrorHandler = CA2dpBTHeadsetIfEventHandler::NewL();
	}

CA2dpBTHeadsetAudioIfSession::~CA2dpBTHeadsetAudioIfSession()
	{
	delete iInitHandler;
	delete iOpenDeviceHandler;
	delete iCloseDeviceHandler;
	delete iPlayDataHandler;
	delete iNotifyErrorHandler;
	}
	
void CA2dpBTHeadsetAudioIfSession::ServiceL(const RMessage2& aMessage)
	{
	// Service the message
	TBool complete = EFalse;
	TInt err = KErrNone;
	switch(aMessage.Function())
		{
	case EBTAudioServerInitialize:
		complete = DoInitializeL(aMessage);
		break;
	case EBTAudioServerCancelInitialize:
		complete = DoCancelInitializeL(aMessage);
		break;
	case EBTAudioServerCopyFourCCArrayData:
		complete = DoCopyFourCCArrayDataL(aMessage);
		break;
	case EBTAudioServerCopyChannelsArrayData:
		complete = DoCopyChannelsArrayDataL(aMessage);
		break;
	case EBTAudioServerGetSupportedDataTypes:
		err = DoGetSupportedDataTypesL(aMessage);
		aMessage.Complete(err);
		break;
	case EBTAudioServerGetSupportedSampleRates:
		err = DoGetSupportedSampleRatesL(aMessage);
		aMessage.Complete(err);
		break;
	case EBTAudioServerGetSupportedSampleRatesDiscrete:
		complete = DoGetSupportedSampleRatesDiscreteL(aMessage);
		break;
	case EBTAudioServerGetSupportedSampleRatesRange:
		complete = DoGetSupportedSampleRatesRangeL(aMessage);
		break;
	case EBTAudioServerGetSupportedChannels:
		err = DoGetSupportedChannelsL(aMessage);
		aMessage.Complete(err);
		break;
	case EBTAudioServerSetDataType:
		err = DoSetDataTypeL(aMessage);
		aMessage.Complete(err);
		break;
	case EBTAudioServerSetSampleRate:
		err = DoSetSampleRateL(aMessage);
		aMessage.Complete(err);
		break;
	case EBTAudioServerSetChannels:
		err = DoSetChannelsL(aMessage);
		aMessage.Complete(err);
		break;
	case EBTAudioServerOpenDevice:
		complete = DoOpenDeviceL(aMessage);
		break;
	case EBTAudioServerCancelOpenDevice:
		complete = DoCancelOpenDevice(aMessage);
		break;
	case EBTAudioServerCloseDevice:
		complete = DoCloseDeviceL(aMessage);
		break;
	case EBTAudioServerVolume:
		complete = DoVolumeL(aMessage);
		break;
	case EBTAudioServerSetVolume:
		err = DoSetVolumeL(aMessage);
		aMessage.Complete(err);
		break;
	case EBTAudioServerPlayData:
		complete = DoPlayDataL(aMessage);
		break;
	case EBTAudioServerCancelPlayData:
		complete = DoCancelPlayDataL(aMessage);
		break;
	case EBTAudioServerFlushBuffer:
		complete = DoFlushBufferL(aMessage);
		break;
	case EBTAudioServerBytesPlayed:
		complete = DoBytesPlayedL(aMessage);
		break;
	case EBTAudioServerResetBytesPlayed:
		complete = DoResetBytesPlayedL(aMessage);
		break;
	case EBTAudioServerPauseBuffer:
		complete = DoPauseBufferL(aMessage);
		break;
	case EBTAudioServerResumePlaying:
		complete = DoResumePlayingL(aMessage);
		break;
	case EBTAudioServerNotifyError:
		complete = DoNotifyErrorL(aMessage);
		break;
	case EBTAudioServerCancelNotifyError:
		complete = DoCancelNotifyErrorL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}

	if (complete)
		{
		aMessage.Complete(KErrNone);
		}
	}

TBool CA2dpBTHeadsetAudioIfSession::DoInitializeL(const RMmfIpcMessage& aMessage)
	{
	// Get the address from the message
	TPckgBuf<TBTDevAddr> btAddrPckg;
	aMessage.ReadL(0, btAddrPckg);
	TBTDevAddr& devAddr = btAddrPckg();
	iInitHandler->StartL(aMessage);
	iBTAudioInterface->Initialize(devAddr, iInitHandler->iStatus);
		
	return EFalse;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoCancelInitializeL(const RMmfIpcMessage& /*aMessage*/)
	{
	iBTAudioInterface->CancelInitialize();
	iInitHandler->Stop();
	return ETrue;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoCopyFourCCArrayDataL(const RMmfIpcMessage& aMessage)
	{
	const TInt KBufExpandSize8 = 8;//two TInts
	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i = 0; i < iFourCCArray.Count(); i++)
		{
		stream.WriteInt32L(iFourCCArray[i].FourCC());
		}
	aMessage.WriteL(0, dataCopyBuffer->Ptr(0));
	stream.Close();
	CleanupStack::PopAndDestroy(2);//dataCopyBuffer, stream
	return ETrue;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoCopyUintArrayDataL(const RMmfIpcMessage& aMessage, RArray<TUint> aArray)
	{
	const TInt KBufExpandSize8 = 8;//two TInts
	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i = 0; i < aArray.Count(); i++)
		{
		stream.WriteInt32L(aArray[i]);
		}
	aMessage.WriteL(0, dataCopyBuffer->Ptr(0));
	stream.Close();
	CleanupStack::PopAndDestroy(2);//dataCopyBuffer, stream	
	return ETrue;
	}

TInt CA2dpBTHeadsetAudioIfSession::DoGetSupportedDataTypesL(const RMmfIpcMessage& aMessage)
	{
	iFourCCArray.Reset();

	TInt err = iBTAudioInterface->GetSupportedDataTypes(iFourCCArray);

	TPckgBuf<TInt> pckg;
	pckg() = iFourCCArray.Count();
	aMessage.WriteL(TInt(0),pckg);	
	
	return err;
	}

TInt CA2dpBTHeadsetAudioIfSession::DoGetSupportedSampleRatesL(const RMmfIpcMessage& aMessage)
	{
	iDiscreteArray.Reset();
	iRangeArray.Reset();
	
	TInt err = iBTAudioInterface->GetSupportedSampleRates(iDiscreteArray, iRangeArray);
	
	TPckgBuf<TRatesArrayElements> pckg;
	pckg().iDiscrete = iDiscreteArray.Count();
	pckg().iRange = iRangeArray.Count();
	
	aMessage.WriteL(0, pckg);
	
	return err;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoGetSupportedSampleRatesDiscreteL(const RMmfIpcMessage& aMessage)
	{
	return DoCopyUintArrayDataL(aMessage, iDiscreteArray);
	}

TBool CA2dpBTHeadsetAudioIfSession::DoGetSupportedSampleRatesRangeL(const RMmfIpcMessage& aMessage)
	{
	const TInt KBufExpandSize8 = 8;//two TInts
	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i = 0; i < iRangeArray.Count(); i++)
		{
		stream.WriteInt32L(iRangeArray[i].iLow);
		stream.WriteInt32L(iRangeArray[i].iHigh);
		}
	aMessage.WriteL(0, dataCopyBuffer->Ptr(0));
	stream.Close();
	CleanupStack::PopAndDestroy(2);//dataCopyBuffer, stream
	return ETrue;
	}

TInt CA2dpBTHeadsetAudioIfSession::DoGetSupportedChannelsL(const RMmfIpcMessage& aMessage)
	{
	// Array of uints
	iStereoSupportArray.Reset();
	TMMFStereoSupport stereoSupport;

	TInt err = iBTAudioInterface->GetSupportedChannels(iStereoSupportArray, stereoSupport);
	
	TPckgBuf<TChannelsSupport> pckg;
	pckg().iElementCount = iStereoSupportArray.Count();
	pckg().iSupport = stereoSupport;
	
	aMessage.WriteL(0, pckg);
	
	return err;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoCopyChannelsArrayDataL(const RMmfIpcMessage& aMessage)
	{
	return DoCopyUintArrayDataL(aMessage, iStereoSupportArray);
	}

TInt CA2dpBTHeadsetAudioIfSession::DoSetDataTypeL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TFourCC> dataTypePckg;
	aMessage.ReadL(0, dataTypePckg);
	TFourCC dataType = dataTypePckg();
	return iBTAudioInterface->SetDataType(dataType);
	}

TInt CA2dpBTHeadsetAudioIfSession::DoSetSampleRateL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TUint> ratePckg;
	aMessage.ReadL(0, ratePckg);
	TUint rate = ratePckg();
	return iBTAudioInterface->SetSampleRate(rate);
	}

TInt CA2dpBTHeadsetAudioIfSession::DoSetChannelsL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TChannelsSupport> pckgBuf;
	aMessage.ReadL(0, pckgBuf);
	TChannelsSupport support = pckgBuf();
	return iBTAudioInterface->SetChannels(support.iElementCount, support.iSupport);
	}

TBool CA2dpBTHeadsetAudioIfSession::DoOpenDeviceL(const RMmfIpcMessage& aMessage)
	{
	iOpenDeviceHandler->StartL(aMessage);
	iBTAudioInterface->OpenDevice(iOpenDeviceHandler->iStatus);
	return EFalse;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoCancelOpenDevice(const RMmfIpcMessage& /*aMessage*/)
	{
	iBTAudioInterface->CancelOpenDevice();
	return ETrue;
	}
	
TBool CA2dpBTHeadsetAudioIfSession::DoCloseDeviceL(const RMmfIpcMessage& aMessage)
	{
	iCloseDeviceHandler->StartL(aMessage);
	iBTAudioInterface->CloseDevice(iCloseDeviceHandler->iStatus);
	return EFalse;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoVolumeL(const RMmfIpcMessage& aMessage)
	{	
	TUint volume = iBTAudioInterface->Volume();
	TPckgBuf<TUint> pckg(volume);
	aMessage.WriteL(0, pckg);
	return ETrue;
	}

TInt CA2dpBTHeadsetAudioIfSession::DoSetVolumeL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TUint> pckg;
	aMessage.ReadL(0, pckg);
	TUint volume = pckg();
	return iBTAudioInterface->SetVolume(volume);
	}

TBool CA2dpBTHeadsetAudioIfSession::DoPlayDataL(const RMmfIpcMessage& aMessage)
	{
	// Client's in the same process so access the buffer directly
	const TPtr8* ptr = static_cast<const TPtr8*>(aMessage.Ptr0());
	iPlayDataHandler->StartL(aMessage);
	iBTAudioInterface->PlayData(*ptr, iPlayDataHandler->iStatus);
	return EFalse;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoCancelPlayDataL(const RMmfIpcMessage& /*aMessage*/)
	{
	iBTAudioInterface->CancelPlayData();
	iPlayDataHandler->Stop();
	return ETrue;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoFlushBufferL(const RMmfIpcMessage& /*aMessage*/)
	{
	iBTAudioInterface->FlushBuffer();
	return ETrue;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoBytesPlayedL(const RMmfIpcMessage& aMessage)
	{
	TUint bytesPlayed = iBTAudioInterface->BytesPlayed();
	TPckgBuf<TUint> pckg(bytesPlayed);
	aMessage.WriteL(0, pckg);
	return ETrue;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoResetBytesPlayedL(const RMmfIpcMessage& /*aMessage*/)
	{
	iBTAudioInterface->ResetBytesPlayed();
	return ETrue;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoPauseBufferL(const RMmfIpcMessage& /*aMessage*/)
	{
	iBTAudioInterface->PauseBuffer();
	return ETrue;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoResumePlayingL(const RMmfIpcMessage& /*aMessage*/)
	{
	iBTAudioInterface->ResumePlaying();
	return ETrue;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoNotifyErrorL(const RMmfIpcMessage& aMessage)
	{
	iNotifyErrorHandler->StartL(aMessage);
	iBTAudioInterface->NotifyError(iNotifyErrorHandler->iStatus);
	return EFalse;
	}

TBool CA2dpBTHeadsetAudioIfSession::DoCancelNotifyErrorL(const RMmfIpcMessage& /*aMessage*/)
	{
	iBTAudioInterface->CancelNotifyError();
	iNotifyErrorHandler->Stop();
	return ETrue;
	}

/**
 * Implementation of Active Object to handle asynch requests to the Bluetooth interface.
 */
CA2dpBTHeadsetIfEventHandler* CA2dpBTHeadsetIfEventHandler::NewL()
	{
	CA2dpBTHeadsetIfEventHandler* self = new(ELeave) CA2dpBTHeadsetIfEventHandler();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CA2dpBTHeadsetIfEventHandler::~CA2dpBTHeadsetIfEventHandler()
	{
	Cancel();
	delete iMessage;
	}

void CA2dpBTHeadsetIfEventHandler::RunL()
	{
	TInt err = iStatus.Int();
	// Complete the message
	iMessage->Complete(err);
	delete iMessage;
	iMessage = NULL;
	}

void CA2dpBTHeadsetIfEventHandler::DoCancel()
	{
	if (iMessage)
		{
		iMessage->Complete(KErrCancel);		
		}
	}
	
CA2dpBTHeadsetIfEventHandler::CA2dpBTHeadsetIfEventHandler() : CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

void CA2dpBTHeadsetIfEventHandler::ConstructL()
	{
	}

void CA2dpBTHeadsetIfEventHandler::StartL(const RMmfIpcMessage& aMessage)
	{
	// take a copy of the message to complete later
	delete iMessage;	// ensure we only have one message!
	iMessage = NULL;
	iMessage = new(ELeave) RMmfIpcMessage(aMessage);

	if (!IsActive())
		{
		SetActive();
		}
	}
	
void CA2dpBTHeadsetIfEventHandler::Stop()
	{
	Cancel();
	}
