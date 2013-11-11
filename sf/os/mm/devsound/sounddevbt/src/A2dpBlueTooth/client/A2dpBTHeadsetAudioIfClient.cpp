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

#include <bttypes.h>
#include <s32mem.h>	// RDesReadStream

#include "A2dpBTHeadsetAudioIfClient.h"
#include "A2dpBTHeadsetAudioIfClientServer.h"
#include "A2dpBTHeadsetAudioIfServerStart.h"
#include "MMFBtRoutingSoundDevice.h"

const TInt KBluetoothAddressBufferLength = 32;

EXPORT_C RA2dpBTHeadsetAudioInterface::RA2dpBTHeadsetAudioInterface()
	{
	}

EXPORT_C TInt RA2dpBTHeadsetAudioInterface::Connect()
	{
	TRAPD(err, iBufAddr = HBufC::NewL(KBluetoothAddressBufferLength));
	if (err)
		{
		delete iBufAddr;
		iBufAddr = NULL;
		return err;
		}

	TRAP(err, iPckgBuf = new(ELeave)TPckgBuf<TBTDevAddr>);
	if (err)
		{
		delete iPckgBuf;
		iPckgBuf = NULL;
		return err;
		}

	TVersion version(KBTAudioServerMajorVersionNumber,
					KBTAudioServerMinorVersionNumber,
					KBTAudioServerBuildVersionNumber);
	// Assume the server is already running and attempt to create a session	
	return CreateSession(KA2DPAudioServerName, version);
	}

EXPORT_C void RA2dpBTHeadsetAudioInterface::Close()
	{
	// Call the base class
	RMmfSessionBase::Close();
	
	delete iPckgBuf;
	iPckgBuf = NULL;
	
	delete iBufAddr;
	iBufAddr = NULL;
	}

EXPORT_C void RA2dpBTHeadsetAudioInterface::Initialize(const TBTDevAddr& aRemoteAddress,
														TRequestStatus& aStatus)
	{
	if (iPckgBuf)
		{
		(*iPckgBuf)() = aRemoteAddress;
		SendReceiveResult(EBTAudioServerInitialize, *iPckgBuf, aStatus);
		}
	else
		{
		// iPckgBuf not created => Connect() wasn't called or the returned error code was ignored.
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrDisconnected);
		}
	}

EXPORT_C void RA2dpBTHeadsetAudioInterface::CancelInitialize()
	{
	SendReceive(EBTAudioServerCancelInitialize);
	}

EXPORT_C void RA2dpBTHeadsetAudioInterface::GetSupportedDataTypesL(RArray<TFourCC>& aSupportedDataTypes) const
	{
	aSupportedDataTypes.Reset();
	TPckgBuf<TInt> numberOfElementsPckg;
	TInt err = SendReceiveResult(EBTAudioServerGetSupportedDataTypes, numberOfElementsPckg);
	User::LeaveIfError(err);

	HBufC8* buf = HBufC8::NewLC(numberOfElementsPckg() * sizeof(TFourCC));
	TPtr8 ptr = buf->Des();
	err = SendReceiveResult(EBTAudioServerCopyFourCCArrayData, ptr);
	User::LeaveIfError(err);

	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);
	
	for (TInt i = 0; i < numberOfElementsPckg(); i++)
		{
		err = aSupportedDataTypes.Append(stream.ReadInt32L());
		if (err)
			{//note we don't destroy array because we don't own it
			//but we do reset it as it is incomplete
			aSupportedDataTypes.Reset();
			User::Leave(err);
			}
		}
	CleanupStack::PopAndDestroy(2, buf);
	}
	
EXPORT_C void RA2dpBTHeadsetAudioInterface::GetSupportedSampleRatesL(RArray<TUint>& aSupportedDiscreteRates,
																	RArray<TRange>& aSupportedRateRanges) const
	{
	aSupportedDiscreteRates.Reset();
	TPckgBuf<TRatesArrayElements> numberOfElementsPckg;
	
	TInt err = SendReceiveResult(EBTAudioServerGetSupportedSampleRates, numberOfElementsPckg);
	User::LeaveIfError(err);
	
	HBufC8* buf = HBufC8::NewLC(numberOfElementsPckg().iDiscrete * sizeof(TUint));
	TPtr8 ptr = buf->Des();
	err = SendReceiveResult(EBTAudioServerGetSupportedSampleRatesDiscrete, ptr);
	User::LeaveIfError(err);
	
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	// Populate the discrete rates array
	for (TInt i = 0; i < numberOfElementsPckg().iDiscrete; i++)
		{
		err = aSupportedDiscreteRates.Append(stream.ReadInt32L());
		if (err)
			{//note we don't destroy array because we don't own it
			//but we do reset it as it is incomplete
			aSupportedDiscreteRates.Reset();
			User::Leave(err);
			}
		}
		
	ptr.SetLength(0); //clear out exiting data	
	// Get the rates range array (# of elements and the elements themselves)
	buf = buf->ReAllocL(numberOfElementsPckg().iRange * sizeof(TRange));
	ptr = buf->Des();
	stream.Close();
	stream.Open(ptr);
	err = SendReceiveResult(EBTAudioServerGetSupportedSampleRatesRange, ptr);
	User::LeaveIfError(err);
	TRange range;
	for (TInt i = 0; i < numberOfElementsPckg().iRange; i++)
		{
		range.iLow = stream.ReadInt32L();
		range.iHigh = stream.ReadInt32L();					
		err = aSupportedRateRanges.Append(range);
		if (err)
			{
			aSupportedRateRanges.Reset();
			User::Leave(err);
			}
		}
		
	CleanupStack::PopAndDestroy(2, buf);//stream, buf		
	}
	
EXPORT_C void RA2dpBTHeadsetAudioInterface::GetSupportedChannelsL(RArray<TUint>& aSupportedChannels,
																TMMFStereoSupport& aStereoSupport) const

	{
	aSupportedChannels.Reset();
	TChannelsSupport channelsSupport;
	channelsSupport.iElementCount = 0;
	channelsSupport.iSupport = EMMFNone;
	TPckgBuf<TChannelsSupport> channelsSupportPckg(channelsSupport);
	
	TInt err = SendReceiveResult(EBTAudioServerGetSupportedChannels, channelsSupportPckg);
	User::LeaveIfError(err);

	aStereoSupport = channelsSupportPckg().iSupport;
	HBufC8* buf = HBufC8::NewLC(channelsSupportPckg().iElementCount * sizeof(TUint));
	TPtr8 ptr = buf->Des();
	err = SendReceiveResult(EBTAudioServerCopyChannelsArrayData, ptr);
	User::LeaveIfError(err);
	
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	// Populate the stereo support array
	for (TInt i = 0; i < channelsSupportPckg().iElementCount; i++)
		{
		err = aSupportedChannels.Append(stream.ReadInt32L());
		if (err)
			{//note we don't destroy array because we don't own it
			//but we do reset it as it is incomplete
			aSupportedChannels.Reset();
			User::Leave(err);
			}
		}		
	CleanupStack::PopAndDestroy(2, buf); //stream, buf
	}

EXPORT_C TInt RA2dpBTHeadsetAudioInterface::SetDataType(const TFourCC& aDataType)
	{
	TPckgBuf<TFourCC> dataTypePckg(aDataType);
	return SendReceiveResult(EBTAudioServerSetDataType, dataTypePckg);
	}
	
EXPORT_C TInt RA2dpBTHeadsetAudioInterface::SetSampleRate(TUint aSampleRate)
	{
	TPckgBuf<TUint> sampleRatePckg(aSampleRate);
	return SendReceiveResult(EBTAudioServerSetSampleRate, sampleRatePckg);
	}

EXPORT_C TInt RA2dpBTHeadsetAudioInterface::SetChannels(TUint aChannels,
														TMMFStereoSupport aStereoSupport)
	{
	TChannelsSupport channelsSupport;
	channelsSupport.iElementCount = aChannels;
	channelsSupport.iSupport = aStereoSupport;
	TPckgBuf<TChannelsSupport> pckgBuf(channelsSupport);
	return SendReceiveResult(EBTAudioServerSetChannels, pckgBuf);
	}
	
EXPORT_C void RA2dpBTHeadsetAudioInterface::OpenDevice(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	SendReceive(EBTAudioServerOpenDevice, aStatus);
	}

EXPORT_C void RA2dpBTHeadsetAudioInterface::CancelOpenDevice()
	{
	SendReceive(EBTAudioServerCancelOpenDevice);
	}
	
EXPORT_C void RA2dpBTHeadsetAudioInterface::CloseDevice(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	SendReceive(EBTAudioServerCloseDevice, aStatus);
	}
	
EXPORT_C TUint RA2dpBTHeadsetAudioInterface::Volume() const
	{
	TUint volume = 0;
	TPckgBuf<TUint> volumePckg(volume);
	SendReceiveResult(EBTAudioServerVolume, volumePckg);
	return volumePckg();
	}

EXPORT_C TInt RA2dpBTHeadsetAudioInterface::SetVolume(TUint aVolume)
	{
	TPckgBuf<TUint> volumePckg(aVolume);
	return SendReceiveResult(EBTAudioServerSetVolume, volumePckg);
	}

EXPORT_C void RA2dpBTHeadsetAudioInterface::PlayData(const TDesC8& aData, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	SendReceive(EBTAudioServerPlayData, aData, aStatus);
	}

EXPORT_C void RA2dpBTHeadsetAudioInterface::CancelPlayData()
	{
	SendReceive(EBTAudioServerCancelPlayData);
	}

EXPORT_C void RA2dpBTHeadsetAudioInterface::FlushBuffer()
	{
	SendReceive(EBTAudioServerFlushBuffer);
	}

EXPORT_C TUint RA2dpBTHeadsetAudioInterface::BytesPlayed() const
	{
	TUint bytes = 0;
	TPckgBuf<TUint> bytesPlayedPckg(bytes);
	SendReceive(EBTAudioServerBytesPlayed, bytesPlayedPckg);
	return bytesPlayedPckg();
	}

EXPORT_C void RA2dpBTHeadsetAudioInterface::ResetBytesPlayed()
	{
	SendReceive(EBTAudioServerResetBytesPlayed);
	}

EXPORT_C void RA2dpBTHeadsetAudioInterface::PauseBuffer()
	{
	SendReceive(EBTAudioServerPauseBuffer);
	}

EXPORT_C void RA2dpBTHeadsetAudioInterface::ResumePlaying()
	{
	SendReceive(EBTAudioServerResumePlaying);
	}

EXPORT_C void RA2dpBTHeadsetAudioInterface::NotifyError(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	SendReceive(EBTAudioServerNotifyError, aStatus);
	}

EXPORT_C void RA2dpBTHeadsetAudioInterface::CancelNotifyError()
	{
	SendReceive(EBTAudioServerCancelNotifyError);
	}


