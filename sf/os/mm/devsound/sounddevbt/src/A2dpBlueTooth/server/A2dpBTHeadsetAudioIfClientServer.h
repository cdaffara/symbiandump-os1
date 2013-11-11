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

#ifndef __A2DPBTHEADSETAUDIOIFCLIENTSERVER_H__
#define __A2DPBTHEADSETAUDIOIFCLIENTSERVER_H__

#include <e32base.h>
#include <e32std.h>
#include <mmf/server/sounddevice.h>	// for TMMFStereoSupport

// Message opcodes between client and server
enum TBTAudioServerRequest
	{
	EBTAudioServerInitialize,
	EBTAudioServerCancelInitialize,
	EBTAudioServerCopyFourCCArrayData,
	EBTAudioServerCopyChannelsArrayData,
	EBTAudioServerCopyDiscreteArrayData,
	EBTAudioServerCopyRangeArrayData,
	EBTAudioServerGetSupportedDataTypes,
	EBTAudioServerGetSupportedSampleRates,	
	EBTAudioServerGetSupportedSampleRatesDiscrete,
	EBTAudioServerGetSupportedSampleRatesRange,
	EBTAudioServerGetSupportedChannels,
	EBTAudioServerGetStereoSupport,
	EBTAudioServerSetDataType,
	EBTAudioServerSetSampleRate,
	EBTAudioServerSetChannels,
	EBTAudioServerOpenDevice,
	EBTAudioServerCancelOpenDevice,
	EBTAudioServerCloseDevice,
	EBTAudioServerVolume,
	EBTAudioServerSetVolume,
	EBTAudioServerPlayData,
	EBTAudioServerCancelPlayData,
	EBTAudioServerFlushBuffer,
	EBTAudioServerBytesPlayed,
	EBTAudioServerResetBytesPlayed,
	EBTAudioServerPauseBuffer,
	EBTAudioServerResumePlaying,
	EBTAudioServerNotifyError,
	EBTAudioServerCancelNotifyError,
	EBTAudioServerSelectSEP,
	EBTAudioServerConfigure,
	EBTAudioServerConnect,
	EBTAudioServerSetReadyForStreaming,
	EBTAudioServerSetSniffMode
	};

class TBTDevAddr;

// Helper class to pass the number of elements in both the discrete and range
// arrays across the cli/srvr boundary in one shot.
class TRatesArrayElements
	{
public:
	TInt iDiscrete;
	TInt iRange;
	};

/**
 * TODO - sys doc.
 */
class TChannelsSupport
	{
public:
	TUint iElementCount;
	TMMFStereoSupport iSupport;
	};

#endif
