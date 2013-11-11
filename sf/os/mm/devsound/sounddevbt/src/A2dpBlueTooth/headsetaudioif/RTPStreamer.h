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

#ifndef __RTPSTREAMER_H__
#define __RTPSTREAMER_H__

#include <e32base.h>
#include <es_sock.h> //RSocketServer, RSocket
#include <rtp.h>

const TUint KRTPHeaderSize = 12;
const TUint KSendBucketSize = 4;//sets number of internally buffered audio frames	

class CMMFCodec; //fwd ref
class CAudioBufferArray;
class TSBCFrameParameters;
class CA2dpCodecUtilities;

/**
Callback class implemented by CA2dpBTHeadsetAudioInterface to get error events
back from CActiveRTPStreamer
*/
class MRTPStreamerObserver
	{
public:
/**
Callback when the CRtpSendPackets detects an error condition
event

@internalComponent
@param aError standard SymbianOS error
*/
	void virtual RTPStreamerEvent(TInt aError) = 0;
	};

		
/**
RTP Streamer used by the CA2dpBTHeadsetAudioInterface.
This class is used by the CA2dpBTHeadsetAudioInterface in order to stream
buffers of audio to the headset.  The CActiveRTPStreamer can buffer a number
of buffers internally set by the KSendBucketSize.

@internalComponent
*/
NONSHARABLE_CLASS(CActiveRTPStreamer) : public CTimer
	{
public:
	static CActiveRTPStreamer*	NewL(RSocket& aSock, MRTPStreamerObserver& aRTPStreamerObserver);
	~CActiveRTPStreamer();
	
	//callbacks from RTP	
	static void RTPCallback(CActiveRTPStreamer* aStreamer, const TRtpEvent& aEvent);
	static void RTPSendSourceCallback(CActiveRTPStreamer* aStreamer, const TRtpEvent& aEvent);
	
	void SetCodec(CMMFCodec& aCodec);
	void SetAudioConfiguration(const CA2dpAudioCodecConfiguration& aAudioCodecConfiguration);
	void Send(const TDesC8& aData, TRequestStatus& aStatus);
	void Pause();
	void Resume();
	void CancelLastSendBuffer();
	void FlushPendingSendBuffers();
	TUint BytesSent() const;
	void ResetBytesSent();
private:
	CActiveRTPStreamer(MRTPStreamerObserver& aRTPStreamerObserver);
	void ConstructL(RSocket& aSock);	
	void InitializeForSendL(const TDesC8& aData);
	TUint CodecProcessPayloadL(const TDesC8& aData, TDes8& aPayload);
	void PacketSent(TRtpEventType aEvent);
	void RTPSessionEvent(const TRtpEvent& aEvent);
	void CompleteSendRequestStatus(TInt aError);
	
	//CActive
	void RunL();
	void DoCancel();
private:	
	MRTPStreamerObserver& iRTPStreamerObserver;
	CAudioBufferArray*	iAudioBufferArray; //this class is owned
	TTimeIntervalMicroSeconds32	iRTPPacketDuration;
	RRtpSession			iRTPSession;
	RRtpSendSource		iRTPSendSource;
	TRtpEventType		iRTPEventType;
	TBool				iRtpCanSend;
	TRequestStatus*		iSendStatus;
	TUint iMaxMTULength;
	TUint iFrameLength;
	TUint iBufferLength;
	TUint iNumberOfInputBytesToMakeRTPPacket;
	TUint8 iPayloadType;
	TUint iTimeStampIncrement;
	TUint iTimeStamp;
	TBuf8<4> iMediaPayloadHeader;
	TUint iBitRate;
	TUint iBytesSent;
	TBool iBufferParamsInitialized;
	TBool iPaused;
	TInt iUnrecoverableError;
	TUint iSendBufferSize;
	
	//these classes aren't owned
	CMMFCodec* iCodec;
	CA2dpAudioCodecConfiguration* iA2dpAudioCodecConfiguration;	
	};

#endif

