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

#ifndef __A2DPBTHEADSETAUDIOIF_H__
#define __A2DPBTHEADSETAUDIOIF_H__

#include <e32base.h>
#include <bttypes.h> //TBTDevAddr
#include <es_sock.h> //RSocketServer, RSocket
#include <bt_sock.h> //RBTPhysicalLinkAdaptor
#include "GavdpStateMachine.h"
#include "RTPStreamer.h"

class CA2dpCodecUtilities; //fwd ref
class CMMFCodec; //fwd ref
class TRange;	// fwd ref


/**
API to access and send audio to an A2DP compliant audio headset
This API is used internally within the Symbian reference 
CMMFDevSound/CMMFHwDevices/CRoutingSoundPlayDevice.

@internalTechnology
*/
class CA2dpBTHeadsetAudioInterface : public CBase, public MGAVDPStateChangeObserver, public MRTPStreamerObserver
	{
public:
	IMPORT_C static CA2dpBTHeadsetAudioInterface* NewL();
	IMPORT_C ~CA2dpBTHeadsetAudioInterface();	
	IMPORT_C void Initialize(const TBTDevAddr& aRemoteAddress, TRequestStatus& aStatus);
	IMPORT_C void CancelInitialize();
	IMPORT_C TInt GetSupportedDataTypes(RArray<TFourCC>& aSupportedDataTypes) const;
	IMPORT_C TInt GetSupportedSampleRates(RArray<TUint>& aSupportedDiscreteRates, RArray<TRange>& aSupportedRateRanges) const;
	IMPORT_C TInt GetSupportedChannels(RArray<TUint>& aSupportedChannels, TMMFStereoSupport& aStereoSupport) const;
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
	
	//APIs only used for unit testing purposes - not needed for BT server or routing sound device
	IMPORT_C MGavdpUser* TEST_MGavdpUser();
	IMPORT_C void TEST_ForceRemoteSBCCodecConfiguration(const TSBCCodecCapabilities& aRemoteCodecConfiguration);
private:
	CA2dpBTHeadsetAudioInterface();
	void ConstructL();
	
	void ProcessGAVDPStateChangeIdleToOpen(TInt aError);
	void ProcessGAVDPStateChangeOpenToStreaming(TInt aError);
	void ProcessGAVDPStateChangeStreamingToOpen(TInt aError);

	//MGAVDPStateChangeObserver
	void GAVDPStateChangeComplete(const TGAVDPState& aInitialState, TInt aError);
	void GAVDPStateMachineEvent(TInt aError);
	void GAVDPStateMachineStreamSuspendedByRemoteHeadset();
	TInt GAVDPStateMachineReconfigureByRemoteHeadset();
	void GAVDPStateMachineStreamResumedByRemoteHeadset();
	
	//MRTPStreamerObserver
	void RTPStreamerEvent(TInt aError);

	TInt SetSniffMode(TBool aSniffMode);
private:
	//owned classes
	CGAVDPStateMachine* iGAVDPStateMachine;
	CActiveRTPStreamer* iRTPStreamer;
	CMMFCodec* iCodec;
	CA2dpAudioCodecConfiguration* iA2dpCodecConfiguration;
	
	RSocketServ iSocketServer;
	RBTPhysicalLinkAdapter iBTPhysicalLinkAdapter;
	TRequestStatus* iInitializeRequestStatus;
	TRequestStatus* iOpenDeviceRequestStatus;
	TRequestStatus* iCloseDeviceRequestStatus;
	TRequestStatus* iNotifyRequestStatus;
	TFourCC	iDataType;
	TBool	iAudioSettingsHaveChanged;
	TBool	iReconfiguring;
	TBool	iPaused;
	TInt iOpenDeviceError;
	};

#endif

