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

#include "AudioBufferArray.h"
#include "BtSBCFrameParameters.h"
#include "A2dpCodecUtilities.h"

/**
Audio Buffer Array Panics
**/
enum TAudioBufferArrayPanic
	{
	EAudioBufferArrayIncompleteFrame, //0
	EAudioBufferArrayMiscount, //1
	EAudioBufferArrayNonA2dpDataType, //2
	EAudioBufferArrayNoRTPPacketsPerAudioBuffer //3
	};


static void Panic(TAudioBufferArrayPanic aPanic)
// Panic client
	{
	_LIT(KAudioBufferArrayPanicName, "A2DP Audio Buf Panic");
	User::Panic(KAudioBufferArrayPanicName, aPanic);
	}
	

/**
Creates a CRtpSendPackets array of RRtpSendPackets

@param aRtpSendSource
@param aNumberOfPackets this is the number of RRtpSendPackets stored in
the array
*/	
CRtpSendPackets* CRtpSendPackets::NewL(RRtpSendSource& aRtpSendSource, TUint aNumberOfPackets)
	{
	CRtpSendPackets* self = new (ELeave) CRtpSendPackets ();
	CleanupStack::PushL(self);
	self->ConstructL(aRtpSendSource, aNumberOfPackets);
	CleanupStack::Pop(self);
	return self;
	}

	
void CRtpSendPackets::ConstructL(RRtpSendSource& aRtpSendSource, TUint aNumberOfPackets)
	{
	// create all the RTP send packets now	
	TInt err = KErrNone;
	for (TInt i=0; i<aNumberOfPackets; i++)
		{
		RRtpSendPacket sendPacket = aRtpSendSource.NewSendPacketL();
		err = iRtpSendPackets.Append(sendPacket);
		if (err)
			{//probably run out of memory so need to close the packets
			CloseAndResetSendPackets();
			User::Leave(err);
			}
		}
	}

	
CRtpSendPackets::~CRtpSendPackets()
	{
	CloseAndResetSendPackets();
	}


void CRtpSendPackets::CloseAndResetSendPackets()
	{
	// destroy all the RTP send packets now
	TUint numberOfSendPackets = iRtpSendPackets.Count();
	for (TInt i=0; i<numberOfSendPackets; i++)
		{
		RRtpSendPacket& p = iRtpSendPackets[i];
		p.Close();
		}
	iRtpSendPackets.Reset();
	}

/**
Creates  a FIFO array of audio buffers stored as CRtpSendPackets
The paramers passed in are used to determine the size and number
of RTP packets required to store an audio buffer
The payload size on aRtpSendSource is set.

@param aRtpSendSource used to create the send packets that contain
the audio buffers.  The payload size is set on the aRtpSendSource to
a value calculated from the other parameters
@param aNumberOfAudioBuffers this is the number of audio buffers
stored in the FIFO
@param aAudioBufferLength this is the length of the audio buffer that needs
to be sent to the headset ie buffer size after SBC processing in the case of SBC
@param aMTULength this is the max data length allowing for the restriction imposed
by the underlying MTU bearer ie bluetooth and the headset.
@param aTotalRTPHeaderLength the RTP header length including the 
RTP media payload header ie RTP header length + media payload header length
@param aFrameLength 
*/	
CAudioBufferArray* CAudioBufferArray::NewL(RRtpSendSource& aRtpSendSource, 
									TUint aNumberOfAudioBuffers, 
									TUint aAudioBufferLength, 
									TUint aMTULength, 
								    TUint aTotalRTPHeaderLength,
									TUint aFrameLength)
	{
	CAudioBufferArray* self = new (ELeave) CAudioBufferArray();
	CleanupStack::PushL(self);
	self->ConstructL(aRtpSendSource, aNumberOfAudioBuffers, aAudioBufferLength, aMTULength, aTotalRTPHeaderLength, aFrameLength);
	CleanupStack::Pop(self);
	return self;
	}


void CAudioBufferArray::ConstructL(RRtpSendSource& aRtpSendSource,
						TUint aNumberOfAudioBuffers,
						TUint aAudioBufferLength,
						TUint aMTULength,
						TUint aTotalRTPHeaderLength,
						TUint aFrameLength)
	{
	//the buffer should always contain an intiger number of audio frames
	//the following ASSERT_DEBUG should be present but is commented out as the RVCT 
	//compiler generates a warning
	//__ASSERT_DEBUG(!(aAudioBufferLength%aFrameLength),EAudioBufferArrayIncompleteFrame);
	
	//calculate the number of frames in the audio buffer size and if 
	//more than 15 frames then calculate the highest common factor and use this 
	//calculate the frames length
	iFrameLength = aFrameLength;

	//calculate the total number of frames in the buffer
	TUint numberOfFramesPerAudioBuffer = aAudioBufferLength/iFrameLength;
	//for now set the number of audio frames in an RTP packet to the total
	iNumberOfFramesPerRtpPacket = numberOfFramesPerAudioBuffer;
	
	TUint lengthOfAudioDataInRtpPacket = 0;
	TInt usableRTPPayloadLength = aMTULength-aTotalRTPHeaderLength;
	
	//check whether all the audio frames will actually fit into one RTP packet
	if ((numberOfFramesPerAudioBuffer > KMaxNumberOfSBCFramesPerRTPPacket)||((aAudioBufferLength+iFrameLength)>usableRTPPayloadLength))//+iFrameLength in case of cached frame
		{
		//we cannot get all the audio frames into a single RTP packet
		//for SBC only a max of 15 SBC frames allowed per packet
		//if the buffer size exceeds 15 frames and/or is too large
		//for the underlying MTU size then we need to break
		//the buffer into buffers of less then 16 frames.
		//for non SBC the frames are generaly larger so we'll keep
		//the 15 for now even for non SBC
		//we need to calculate how may frames should go into
		//each RTP packet and how many RTP packets we need to send a
		//complete audio buffer
		//we are going to calculate such that every RTP packet
		//has the same number of frames
		//note that if we are using non SBC then the frame size tends to be larger
		//so the MTU limit is likely to be the dominant factor
		//although in principle this code should only use the 15 frame limit for
		//for SBC
		iNumberOfFramesPerRtpPacket = 0; 
		
		
		//only a max of 15 SBC frames allowed per packet
		//if the buffer size exceeds 15 frames then we need to break
		//the buffer into buffers of less then 16 frames.
		//so find HCF 
		//-1 for cached frames ie if iNumberOfFramesPerRtpPacket
		//was the same as KMaxNumberOfSBCFramesPerRTPPacket and we got a cached frame
		//then we would blow the limit
		for (TUint i=KMaxNumberOfSBCFramesPerRTPPacket-1; i; i--)
			{
			if (!(numberOfFramesPerAudioBuffer%i))
				{
				//check we don't blow the MTU size
				if ((i*iFrameLength) <= usableRTPPayloadLength)
					{
					iNumberOfFramesPerRtpPacket = i;
					break;
					}
				}
			}
		if (!iNumberOfFramesPerRtpPacket)
			{//the frame length was too big for the MTU length
			//one frame of audio + one frame of cached audio
			//would exceed the length supported by the underlying bearer
			//note that the A2DP specification section 4.3.4 does allow
			//SBC frames to be fragmented across multiple packets if
			//less than one frame, but this ref implementation
			//does not support this, since this should be rare.
			//this may happen more often for non SBC eg mp3 frames
			//but we don't support fragmented frames
			User::Leave(KErrTooBig);
			}
		iNumberOfRtpPacketsPerAudioBuffer = numberOfFramesPerAudioBuffer/iNumberOfFramesPerRtpPacket;
		
		//this could probably be optimized somewhat such that the
		//iInputBytesPerRtpPacket value was such that no caching was required 
		//in the codec
		
		if (!iNumberOfRtpPacketsPerAudioBuffer)//this isn't really necessary or could be ASSERT but needed supress armv5 compiler warning
			{
			Panic(EAudioBufferArrayNoRTPPacketsPerAudioBuffer);
			}
			
		iInputBytesPerRtpPacket = aAudioBufferLength/iNumberOfRtpPacketsPerAudioBuffer;
		if (iInputBytesPerRtpPacket%2)
			{//we have an odd number of bytes
			iInputBytesPerRtpPacket++;//round up to next byte
			}
		lengthOfAudioDataInRtpPacket = iNumberOfFramesPerRtpPacket*iFrameLength;
		}//if ((numberOfFramesPerAudioBuffer > KMaxNumberOfSBCFramesPerRTPPacket)||((encodedAudioBufferLength+iFrameLength)>aMaxMTULength))
	else
		{//we can fit the entire buffer in one RTP packet
		iNumberOfRtpPacketsPerAudioBuffer = 1;
		iInputBytesPerRtpPacket = aAudioBufferLength;
		lengthOfAudioDataInRtpPacket = aAudioBufferLength;
		}

	TUint payloadSize = aTotalRTPHeaderLength+lengthOfAudioDataInRtpPacket+iFrameLength;//+ extra framelength for cached frames
	aRtpSendSource.SetDefaultPayloadSize(payloadSize);
	
	//now we have set the payload size we can create the audio buffers
	//stored as CRtpSendPackets* 
	for (TInt i=0; i<aNumberOfAudioBuffers; i++)
		{
		CRtpSendPackets* sendPacketArray = CRtpSendPackets::NewL(aRtpSendSource, iNumberOfRtpPacketsPerAudioBuffer);
		User::LeaveIfError(iAudioBufferArray.Append(sendPacketArray));
		}
	}

	
CAudioBufferArray::~CAudioBufferArray()
	{
	// destroy all the audio buffers now
	TUint numberOfAudioBuffers = iAudioBufferArray.Count();
	for (TInt i=0; i<numberOfAudioBuffers; i++)
		{
		CRtpSendPackets* sendPacketArray = iAudioBufferArray[i];
		delete sendPacketArray;
		}
	iAudioBufferArray.Close();
	}


/**
This function is called when the current audio buffer has been filled
with audio and is in a state to be sent to the headset
The function updates the next audio buffer to fill to the next 
available free audio buffer
*/
void CAudioBufferArray::CurrentAudioBufferReadyToSend()
	{
	iNextAudioBufferToFill++;
	if (iNextAudioBufferToFill >= iAudioBufferArray.Count())
		{
		iNextAudioBufferToFill = 0;
		}
	iNumberOfReadyAudioBuffers++;
	__ASSERT_DEBUG((iNumberOfReadyAudioBuffers<=iAudioBufferArray.Count()),Panic(EAudioBufferArrayMiscount));
	}


/**
This function cancels the most recently filled audio buffer that is ready to send
The audio buffer corresponds to the audio buffer sent in the last CActiveRTPStreamer::Send()
This is used in order to cancel a Send request.
It effectively undoes the operation performed in CurrentAudioBufferReadyToSend()
so CurrentAudioBufferReadyToSend() must have been called at least once prior.

@param aSendInProgress set to ETrue if an audio buffer is currently being
sent
*/
void CAudioBufferArray::CancelMostRecentAudioBuffer(TBool aSendInProgress)
	{
	__ASSERT_DEBUG((iNumberOfReadyAudioBuffers),Panic(EAudioBufferArrayMiscount));
	if ((iNumberOfReadyAudioBuffers == 1) && (aSendInProgress))
		{//then we only have one ready buffer , which is being sent
		//so we want to stop any further sending of the current buffer
		__ASSERT_DEBUG((iAudioBufferBeingSent == iNextAudioBufferToFill),Panic(EAudioBufferArrayMiscount));
		//now we need prevent any further send packets in the current audio buffer being sent
		//the following line of code will force us to move onto the next audio 
		//buffer discarding any RTP packets in the current audio buffer
		//see CurrentSendPacketSent()
		iNextRtpPacketToSend = iNumberOfRtpPacketsPerAudioBuffer-1;
		}
	else if (iNumberOfReadyAudioBuffers)
		{
		if (!iNextAudioBufferToFill)
			{
			iNextAudioBufferToFill = iAudioBufferArray.Count();
			}
		else
			{
			iNextAudioBufferToFill--;
			}
		iNumberOfReadyAudioBuffers--;
		}	
	}
	

/**
This function flushes the pending send packets that are ready to send.
Only the current send packet is valid
*/
void CAudioBufferArray::FlushPendingPackets()	
	{
	//check that we actually have some audio buffers to flush
	if (iNumberOfReadyAudioBuffers > 1)
		{
		if (iAudioBufferBeingSent >= iAudioBufferArray.Count())
			{
			iNextAudioBufferToFill = 0;
			}
		else
			{
			iNextAudioBufferToFill = iAudioBufferBeingSent+1;
			}
		iNumberOfReadyAudioBuffers = 1; //the current send packet
		}
	else if (iNumberOfReadyAudioBuffers == 1)
		{
		//now we need to flush out the send packets in the current audio buffer being sent
		//the following line of code will force us to move onto the next audio 
		//buffer discarding any RTP packets in the current audio buffer
		//see CurrentSendPacketSent()
		iNextRtpPacketToSend = iNumberOfRtpPacketsPerAudioBuffer-1;
		}
	}

	
/*
This function returns the current RTP packet to be sent to the headet
if there are no packets that are ready to be sent
ie iNumberOfReadyPackets = 0 then the RRtpSendPacket
will be invalid.
CurrentSendPacketSent() needs to be called when a send packet has been
acknowledged as being sent by the RTP stack
*/	
RRtpSendPacket& CAudioBufferArray::CurrentSendPacket()
	{
	CRtpSendPackets* currentSendAudioBuffer = iAudioBufferArray[iAudioBufferBeingSent];
	return currentSendAudioBuffer->Packet(iNextRtpPacketToSend);
	}	


/*
This function is called when the RTP module has made the callback indicating that the 
current send packet has been sent.
The function updates the current send packet to the next packet to be sent

@param aEntireAudioBufferSent this is set to true if the 
current entire audio buffer has been sent.  The RTPStreamer uses
this information to determine whether to complete the send request status
*/	
void CAudioBufferArray::CurrentSendPacketSent(TBool& aEntireAudioBufferSent)
	{
	aEntireAudioBufferSent = EFalse;
	if (iNumberOfReadyAudioBuffers)//this could be 0 if the current packet sent was sent and subsequently cancelled
		{
		iNextRtpPacketToSend++;
		if (iNextRtpPacketToSend >= iNumberOfRtpPacketsPerAudioBuffer)
			{//then we have sent all the RTP packets in the current audio buffer
			iNextRtpPacketToSend = 0;
			iAudioBufferBeingSent++; //we've finished with this audio buffer so move onto the next one
			//do something to show we are finished with audio buffer and complete request status
			if (iAudioBufferBeingSent >= iAudioBufferArray.Count())
				{
				iAudioBufferBeingSent = 0;
				}
			iNumberOfReadyAudioBuffers--;
			aEntireAudioBufferSent = ETrue;	
			}		
		}
	//else if iNumberOfReadyAudioBuffers = 0 then the packet must have been canceled so do nothing
	__ASSERT_DEBUG((iNumberOfReadyAudioBuffers<=iAudioBufferArray.Count()),Panic(EAudioBufferArrayMiscount));//check underflow
	}
	






	






	

