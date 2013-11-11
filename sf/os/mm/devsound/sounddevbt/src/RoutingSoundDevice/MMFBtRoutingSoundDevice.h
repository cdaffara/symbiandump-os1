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

#ifndef __MMFBTROUTINGSOUNDDEVICE_H__
#define __MMFBTROUTINGSOUNDDEVICE_H__

#include <e32base.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/server/sounddevice.h>

#include <mdasound.h>

#include "A2dpBTHeadsetAudioIfClient.h"
#include "MmfBtRoutingPrioritySettings.h"

/**
 * TRange - primarily used to store bounds for a range of sample rates.
 * @internalComponent
 */
class TRange
	{
public:
	TUint iLow;
	TUint iHigh;
	};

/**
 *  Routing sound play device
 *  @internalComponent
 */
class CRoutingSoundPlayDevice : public CBase
	{ 
public:	
	IMPORT_C static CRoutingSoundPlayDevice* NewL();
	~CRoutingSoundPlayDevice();
	
	IMPORT_C void Initialize(TUid aDeviceUid,
							const TDesC8& aDeviceConfig,
							TRequestStatus& aStatus);
	IMPORT_C void CancelInitialize(TUid aDeviceUid);
	IMPORT_C void OpenDevice(TUid aDeviceUid, TRequestStatus& aStatus);
	IMPORT_C void CancelOpenDevice(TUid aDeviceUid);
	IMPORT_C void CloseDevice(TUid aDeviceUid, TRequestStatus& aStatus);
	IMPORT_C TInt GetSupportedSampleRates(RArray<TUint>& aSupportedDiscreteRates, RArray<TRange>& aSupportedRateRanges);
	IMPORT_C TInt GetSupportedChannels(RArray<TUint>& aSupportedChannels, TMMFStereoSupport& aStereoSupport);
	IMPORT_C TInt GetSupportedDataTypes(RArray<TFourCC>& aSupportedDataTypes);
	IMPORT_C TInt SetDataType(const TFourCC& aDataType);
	IMPORT_C TInt SetSampleRate(TUint aSampleRate);
	IMPORT_C TInt SetChannels(TUint aChannels, TMMFStereoSupport aStereoSupport);
	IMPORT_C TInt SetBufferSize(TUint aBufferSize);
	IMPORT_C void FlushBuffer();
	IMPORT_C void NotifyError(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyError();	
	IMPORT_C TUint Volume() const;
	IMPORT_C TInt SetVolume(TUint aVolume);
	IMPORT_C void PlayData(const TDesC8& aData, TRequestStatus& aStatus);
	IMPORT_C void CancelPlayData();
	IMPORT_C TUint BytesPlayed();
	IMPORT_C TInt ResetBytesPlayed();
	IMPORT_C TInt PauseBuffer();
	IMPORT_C TInt ResumePlaying();
	IMPORT_C TInt Handle() const;
protected:	
	CRoutingSoundPlayDevice();
	void ConstructL();
private:
	RMdaDevSound iSpeakerDevice;
	RA2dpBTHeadsetAudioInterface iBTDevice;
	TUid iDeviceUid;
	TBTDevAddr iBTAddress;
	TUint iSampleRate;
	TUint iChannels;
	TMMFStereoSupport iStereoSupport;
	TUint iBufferSize;

	TRequestStatus* iInitializeStatus;
	TRequestStatus* iOpenDeviceStatus;
	TRequestStatus* iCloseDeviceStatus;
	};



/**
 *  Routing sound record device
 *  @internalComponent
 */
class CRoutingSoundRecordDevice : public CBase
	{
public:
	IMPORT_C static CRoutingSoundRecordDevice* NewL();
	IMPORT_C ~CRoutingSoundRecordDevice();
	
	IMPORT_C void Initialize(TUid aDeviceUid,
							const TDesC8& aDeviceConfig,
							TRequestStatus& aStatus);
	IMPORT_C void CancelInitialize(TUid aDeviceUid);
	IMPORT_C void OpenDevice(TUid aDeviceUid, TRequestStatus& aStatus);
	IMPORT_C void CancelOpenDevice(TUid aDeviceUid);
	IMPORT_C void CloseDevice(TUid aDeviceUid, TRequestStatus& aStatus);
	IMPORT_C TInt GetSupportedSampleRates(RArray<TUint>& aSupportedDiscreteRates, RArray<TRange>& aSupportedRateRanges);
	IMPORT_C TInt GetSupportedChannels(RArray<TUint>& aSupportedChannels, TMMFStereoSupport& aStereoSupport);
	IMPORT_C TInt SetSampleRate(TUint aSampleRate);
	IMPORT_C TInt SetChannels(TUint aChannels, TMMFStereoSupport aStereoSupport);
	IMPORT_C TInt SetBufferSize(TUint aBufferSize);
	IMPORT_C void FlushBuffer();
	IMPORT_C void NotifyError(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyError();
	IMPORT_C TUint Gain() const;
	IMPORT_C TInt SetGain(TUint aGain);
	IMPORT_C void RecordData(TDes8& aData, TRequestStatus& aStatus);
	IMPORT_C void CancelRecordData();
	IMPORT_C TUint BytesRecorded();
	IMPORT_C TInt Handle() const;
	
protected:
	void ConstructL();
	CRoutingSoundRecordDevice();
private:
	// although the microphone is the only device for now, there may be
	// a BT microphone in the future, so keep the device uid
	RMdaDevSound iInputDevice;
	TUid iDeviceUid;
	TUint iSampleRate;
	TUint iChannels;
	TMMFStereoSupport iStereoSupport;
	TUint iBufferSize;

	TRequestStatus* iOpenDeviceStatus;	
	};


#endif // __MMFBTROUTINGSOUNDDEVICE_H__

