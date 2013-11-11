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

#ifndef __AUDIOBUFFERARRAY_H__
#define __AUDIOBUFFERARRAY_H__

#include <e32base.h>
#include <rtp.h>

const TUint KMaxNumberOfSBCFramesPerRTPPacket = 15;

class TSBCFrameParameters;
class CA2dpAudioCodecConfiguration;
/**
This class contains an array of RRtpSendPackets that contain
the audio to be sent to the headset over RTP
Note that the RRtpSendPackets are only created once upfront
it is assumed that these packets can be recycled
Each audio buffer consists of one CRtpSendPackets class instance,
however due to the underlying restrictions imposed by A2DP and 
the BT L2CAP bearer MTU size limit- each CRtpSendPackets may
consist of one or more RRtpSendPacket

@internalComponent
*/
NONSHARABLE_CLASS(CRtpSendPackets) : public CBase
	{
public:
	static CRtpSendPackets* NewL(RRtpSendSource& aRtpSendSource, TUint aNumberOfPackets);
	~CRtpSendPackets();
	void ConstructL(RRtpSendSource& aRtpSendSource, TUint aNumberOfPackets);
	inline RRtpSendPacket& Packet(TUint aPacketNumber);
private:
	void CloseAndResetSendPackets();
private:
	RArray<RRtpSendPacket> iRtpSendPackets;
	};


/**
This function obtains the next free RTPsendPacket
that is available to be filled with audio data

@return RRtpSendPacket that is free and hence can be filled with audio
*/	
inline RRtpSendPacket& CRtpSendPackets::Packet(TUint aPacketNumber)
	{
	return iRtpSendPackets[aPacketNumber];
	}



/**
This class is used by CActiveRTPStreamer
It is a FIFO used to provide a buffer array of audio buffers.
Each audio buffer is contained in one CRtpSendPackets which 
may itself be broken up into multiple RTP packets.
The size and number of packets each audio buffer is broken down is
calculated from the various input parameters provided to the NewL.
This class makes the assumption than provided the audio parameters
are unchanged, then the buffer length stays the same for each buffer sent,
(except the last buffer)
This assumption has been made, as by not assuming this, then the frame duration
and number of frames calculations would need to be recalculated for every audio buffer
and the RTPSendPackets would need to be created for every buffer.

@internalComponent
*/	
NONSHARABLE_CLASS(CAudioBufferArray) : public CBase
	{
public:
	static CAudioBufferArray* NewL(RRtpSendSource& aRtpSendSource,
							TUint aNumberOfAudioBuffers,
							TUint aAudioBufferSize,
							TUint aMTULength,
							TUint aTotalRTPHeaderLength,
							TUint aFrameLength);
	~CAudioBufferArray();
	void CurrentAudioBufferReadyToSend();
	void CancelMostRecentAudioBuffer(TBool aSendInProgress);
	void FlushPendingPackets();
	RRtpSendPacket& CurrentSendPacket();
	void CurrentSendPacketSent(TBool& aEntireAudioBufferSent);
	inline CRtpSendPackets* CurrentAudioBufferRtpSendPackets() const;
	inline TUint NumberOfAudioBuffersReadyToSend() const;
	inline TUint MaxNumberOfAudioBuffers() const;
	inline TUint InputBytesPerRTPPacket() const;
	inline TUint NumberOfRtpPacketsPerAudioBuffer() const;
	inline TUint NumberOfFramesPerRtpPacket() const;
private:
	void ConstructL(RRtpSendSource& aRtpSendSource, 
				TUint aNumberOfAudioBuffers,
				TUint aAudioBufferSize,
				TUint aMTULength,
				TUint aTotalRTPHeaderLength,
				TUint aFrameLength);
private:
	RPointerArray<CRtpSendPackets> iAudioBufferArray;
	TUint	iAudioBufferBeingSent;
	TUint	iNextRtpPacketToSend;
	TUint	iNextAudioBufferToFill;
	TUint	iNumberOfReadyAudioBuffers; 
	TUint	iNumberOfRtpPacketsPerAudioBuffer;
	TUint	iInputBytesPerRtpPacket;
	TUint	iFrameLength;
	TUint	iNumberOfFramesPerRtpPacket;
	};


/**
This function obtains the current free audio buffer
in the form of a CRTPsendPackets
The  CRTPsendPackets returned are available to be filled with audio data

@return CRtpSendPackets array that is free and hence can be filled with audio
*/	
inline CRtpSendPackets* CAudioBufferArray::CurrentAudioBufferRtpSendPackets() const
	{
	return iAudioBufferArray[iNextAudioBufferToFill];
	}
	
	
/**
This function returns the total number of audio buffers in the
audio buffer array that have been filled with audio and hence are
ready to be sent to the headset.

@return number of audio buffers ready to send
*/	
inline TUint CAudioBufferArray::NumberOfAudioBuffersReadyToSend() const
	{
	return iNumberOfReadyAudioBuffers;
	}
	

/**
This function returns the maximum number of audio buffers that 
can be stored in the audio buffer array.
It is set by the aNumberOfAudioBuffers parameter in the constructor

@return max number of audio buffers that can be stored in the audio 
buffer array
*/	
inline TUint CAudioBufferArray::MaxNumberOfAudioBuffers() const
	{
	return iAudioBufferArray.Count();
	}
	

/**
This function returns the number of input bytes 
ie bytes prior to any codec processing that will
(after possible codec processing) constitute an RTP packet

@return input bytes per RTP packet
*/
inline TUint CAudioBufferArray::InputBytesPerRTPPacket() const
	{
	return iInputBytesPerRtpPacket;
	}
	

/**
This function returns the number of RTP packets that make up an audio buffer

@return number of RTP packets
*/	
inline TUint CAudioBufferArray::NumberOfRtpPacketsPerAudioBuffer() const
	{
	return iNumberOfRtpPacketsPerAudioBuffer;
	}


/**
This function returns the number of audio frames in a single RTP packet
ie the total number of audio frames in a single audio buffer is given by:
NumberOfFramesPerRtpPacket()*NumberOfRtpPacketsPerAudioBuffer()

@return number of audio frames in a single RTP packet
*/	
inline TUint CAudioBufferArray::NumberOfFramesPerRtpPacket() const 
	{
	return iNumberOfFramesPerRtpPacket;
	};
	
#endif

