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

#ifndef __A2DPBTHEADSETAUDIOIFCLIENT_H__
#define __A2DPBTHEADSETAUDIOIFCLIENT_H__

#include <e32base.h>
#include <e32std.h>
#include <mmf/server/sounddevice.h>
#include <bttypes.h>

// Forward declarations.
class TFourCC;
class TRange;

/**
*@internalTechnology
*/
class RA2dpBTHeadsetAudioInterface : public RMmfSessionBase
	{
public:
	IMPORT_C RA2dpBTHeadsetAudioInterface();
	IMPORT_C TInt Connect();	// Connect to the server
	IMPORT_C void Close();

	IMPORT_C void Initialize(const TBTDevAddr& aRemoteAddress, TRequestStatus& aStatus);
	IMPORT_C void CancelInitialize();
	IMPORT_C void GetSupportedDataTypesL(RArray<TFourCC>& aSupportedDataTypes) const;
	IMPORT_C void GetSupportedSampleRatesL(RArray<TUint>& aSupportedDiscreteRates, RArray<TRange>& aSupportedRateRanges) const;
	IMPORT_C void GetSupportedChannelsL(RArray<TUint>& aSupportedChannels, TMMFStereoSupport& aStereoSupport) const;
	IMPORT_C TInt SetDataType(const TFourCC& aDataType);
	IMPORT_C TInt SetSampleRate(TUint aSampleRate);
	IMPORT_C TInt SetChannels(TUint aChannels, TMMFStereoSupport aStereoSupport);
	IMPORT_C void OpenDevice(TRequestStatus& aStatus);
	IMPORT_C void CancelOpenDevice();
	IMPORT_C void CloseDevice(TRequestStatus& aStatus);
	IMPORT_C TUint Volume() const;
	IMPORT_C TInt SetVolume(TUint aVolume);
	IMPORT_C void PlayData(const TDesC8& aData, TRequestStatus& aStatus);
	IMPORT_C void CancelPlayData();
	IMPORT_C void FlushBuffer();
	IMPORT_C TUint BytesPlayed() const;
	IMPORT_C void ResetBytesPlayed();
	IMPORT_C void PauseBuffer();
	IMPORT_C void ResumePlaying();
	IMPORT_C void NotifyError(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyError();
private:
	HBufC*	iBufAddr;
	TPckgBuf<TBTDevAddr>* iPckgBuf;
	};


#endif
