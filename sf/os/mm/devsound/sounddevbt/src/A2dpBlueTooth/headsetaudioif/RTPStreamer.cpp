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

#include <bluetoothav.h>
#include <mmf/server/mmfcodec.h>
#include "mmfSBCCodecImplementationUIDs.hrh"
#include <hal.h>
#include "A2dpCodecUtilities.h"
#include "AudioBufferArray.h"
#include "RTPStreamer.h"


/**
RTP Streamer Panics
**/
enum TRTPStreamerPanic
	{
	ERTPStreamerSendPacketMiscount, //0
	ERTPStreamerSendNotCompleted, //1
	ERTPStreamerEmptyBuffer, //2
	ERTPStreamerRTPEventError, //3
	ERTPStreamerIncompleteSBCFrame, //4
	ERTPStreamerUnexpectedEvent, //5
	ERTPStreamerBufferProcessingLengthMismatch, //6
	ERTPStreamerInvalidDataType, //7
	ERTPStreamerNoConfiguration //8
	};


static void Panic(TRTPStreamerPanic aPanic)
// Panic client
	{
	_LIT(KRTPStreamerPanicName, "RTP Streamer");
	User::Panic(KRTPStreamerPanicName, aPanic);
	}
	
	

/**
Creates CActiveRTPStreamer

@param aSock RSocket that can be used to stream audio to the headset
@param aRTPStreamObserver mixin used to inform the CA2dpBTHeadsetAudioInterface
of asynchronous error events
@return CActiveRTPStreamer*
*/
CActiveRTPStreamer* CActiveRTPStreamer::NewL(RSocket& aSock, MRTPStreamerObserver& aRTPStreamerObserver)
	{
	CActiveRTPStreamer* self = new (ELeave) CActiveRTPStreamer (aRTPStreamerObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aSock);
	CleanupStack::Pop(self);
	return self;
	}


/**
Make priortiy high so other RunLs don't impact CTimer accuracy too much
*/
CActiveRTPStreamer::CActiveRTPStreamer(MRTPStreamerObserver& aRTPStreamerObserver) : CTimer(EPriorityHigh), iRTPStreamerObserver(aRTPStreamerObserver), iRtpCanSend(ETrue)
	{
	CActiveScheduler::Add(this);
	}


CActiveRTPStreamer::~CActiveRTPStreamer()
	{
	Cancel();
	delete iAudioBufferArray;
	
	//RTP defect fix DEF57144- RRtpSendSource Cancel()
	//If the line of code below does not compile then your build is too old
	iRTPSendSource.Cancel();
	
	iRTPSendSource.Close();	
	iRTPSession.Close();	
	}


void CActiveRTPStreamer::ConstructL(RSocket& aSock)
	{
	CTimer::ConstructL();
	// get the MTU length limit
	TInt mtu = 0;
	User::LeaveIfError(aSock.GetOpt(EAvdtpMediaGetMaximumPacketSize, KSolBtAVDTPMedia, mtu));
	iMaxMTULength = mtu;//the line above wont accept iMaxMTULength directly because it is unsigned
	iRTPSession.OpenL(aSock, iMaxMTULength);
	iRTPSendSource = iRTPSession.NewSendSourceL();
	
	//register callbacks - all terminal callbacks are one shot
	iRTPSession.RegisterEventCallbackL(ERtpAnyEvent,RTPCallback,this);									
	iRTPSession.RegisterEventCallbackL(ERtpSessionFail,RTPCallback,this, ERtpOneShot);
	iRTPSession.RegisterEventCallbackL(ERtpBufferOverflow,RTPCallback,this);
	iRTPSession.RegisterEventCallbackL(ERtpUndersizedPacket,RTPCallback,this);
	iRTPSendSource.RegisterEventCallbackL(ERtpAnyEvent,RTPSendSourceCallback,this);
	iRTPSendSource.RegisterEventCallbackL(ERtpSendFail,RTPSendSourceCallback,this, ERtpOneShot);
	}


/**
Function called by the CA2dpBTHeadsetAudioInterface to set the codec and the codec
settings used by the CActiveRTPStreamer.
Calling this function forces a recalculation of all the timings the next
time Send() is called

@param aCodec the codec to be used.  If this is set to NULL then no codec is used
only the CSBCCodec can be used here
@param aConfigType a Uid to identify the aConfigData used to configure the settings
only KMmfUidSBCConfigure is currently defined.  In future other types may be defined
for mp3, AAC and ATRAC3
@param aConfigData The configuration data
@return standard SymbianOS error code
*/
void CActiveRTPStreamer::SetCodec(CMMFCodec& aCodec)
	{
	//if there is a codec then it must be SBC else codec must be on the headset
	iCodec = &aCodec;
	}


void CActiveRTPStreamer::SetAudioConfiguration(const CA2dpAudioCodecConfiguration& aAudioCodecConfiguration)
	{
	iA2dpAudioCodecConfiguration = const_cast<CA2dpAudioCodecConfiguration*>(&aAudioCodecConfiguration);
	//if there is a new codec configuration then cannot assume the buffer 
	//length will be the same so reset
	iBufferLength = 0;
	iBufferParamsInitialized = EFalse;//will result in a call to InitializeForSendL
	iTimeStampIncrement = 0;
	iNumberOfInputBytesToMakeRTPPacket = 0;
	}

/**
Internal function to perform frame size related initialization
ie creation and setting of the RTPSendPacket audio buffer array.
Assumes all buffers are the same size (except for the last buffer)

@param The length of the audio buffer sent in Send()
*/
void CActiveRTPStreamer::InitializeForSendL(const TDesC8& aData)
	{
	iSendBufferSize = aData.Size(); //store buffer length - this shouldn't change till the last buffer
	__ASSERT_DEBUG(iSendBufferSize,Panic(ERTPStreamerEmptyBuffer));
	__ASSERT_DEBUG(iA2dpAudioCodecConfiguration,Panic(ERTPStreamerNoConfiguration));
	TUint encodedBufferSize = iSendBufferSize;
	
	if (iCodec)
		{
		iCodec->ResetL(); //clear out any cached data from previous settings
		//if we are using a local codec - ie SBC then we get the frame length 
		//and bit rate from the local codec settings
		//since aData will contain pcm16
		iFrameLength = iA2dpAudioCodecConfiguration->LocalSBCCodecConfiguration().CalcFrameLength();
		iBitRate = iA2dpAudioCodecConfiguration->LocalSBCCodecConfiguration().CalcBitRate(iFrameLength)*1000;//*1000 as bitrate is in KHz
		//if we are putting data through the local SBC codec then 
		//the encoded buffer size sent to the headset in not the same as the aData buffer in Send()
		encodedBufferSize = iA2dpAudioCodecConfiguration->CalculateSBCBufferLength(iSendBufferSize);
		}
	else
		{
		//if we don't use a local codec then we get the frame legth and bit rate
		//direct from the header
		CA2dpCodecFrameHeaderParser* headerParser = CA2dpCodecFrameHeaderParser::NewL(iA2dpAudioCodecConfiguration->HeadsetCodecDataType(), aData);
		iFrameLength = headerParser->FrameLength();
		iBitRate = headerParser->BitRate();
		delete headerParser;
		}
	iPayloadType = TRTPa2dpCodecSpecificUtils::PayloadType(iA2dpAudioCodecConfiguration->HeadsetCodecDataType());	
	
	//if the settings have changed then any existing buffered audio buffers
	// will have the old settings so we need to delete the buffer array
	//and recreate from new with the new settings.
	//we also need to cancel in case we are waiting on a RTPSendSourceCallback
	//from a previous send packet
	//RTP defect fix DEF57144- RRtpSendSource Cancel() not in MCL
	//decomment this out when the Cancel method is on the MCL
	//iRTPSendSource.Cancel();

	delete iAudioBufferArray;
	iAudioBufferArray = NULL;
	//calculate the size of the RTP header
	TUint mediaPayloadHeaderLength = TRTPa2dpCodecSpecificUtils::MediaPayloadHeaderLength(iA2dpAudioCodecConfiguration->HeadsetCodecDataType());
	TUint rtpHeaderLength = KRTPHeaderSize + mediaPayloadHeaderLength;
	iAudioBufferArray = CAudioBufferArray::NewL(iRTPSendSource, KSendBucketSize, encodedBufferSize, iMaxMTULength, rtpHeaderLength, iFrameLength);
	
	//determine the payload header
	switch(const_cast<TFourCC&>(iA2dpAudioCodecConfiguration->HeadsetCodecDataType()).FourCC())
		{
		case KMMFFourCCCodeSBC:
			iMediaPayloadHeader.Append(iAudioBufferArray->NumberOfFramesPerRtpPacket());
			break;
		case KMMFFourCCCodeMP3:
			//RFC2250-section 3.5 MBZ+Frag_Offset
			//= 4 bytes all set to 0
			iMediaPayloadHeader.FillZ(4); //0000
			break;
		case KMMFFourCCCodeAAC:
			break;
		case KMMFFourCCCodeATRAC3:
			break;
		default:
			//the datatype is a non A2DP datatype
			//which is not supported so panic
			Panic(ERTPStreamerInvalidDataType);
			break;
		}
	
	//get the number of bytes of data sent that was sent to the RTP streamer
	//that make up one RTP packet
	//in the case of a codec this is the value pre codec processing
	if (iCodec)	
		{
		iNumberOfInputBytesToMakeRTPPacket = iSendBufferSize/iAudioBufferArray->NumberOfRtpPacketsPerAudioBuffer();
		if (iNumberOfInputBytesToMakeRTPPacket%2)
			{//we have an odd number of bytes
			iNumberOfInputBytesToMakeRTPPacket++;//round up to next byte
			}
		}
	else
		{
		iNumberOfInputBytesToMakeRTPPacket = iAudioBufferArray->InputBytesPerRTPPacket();
		}
	//else other codecs not supported
	
	//we set the iFrameDuration which is used to trigger the timer
	//this means that RunL should be called every iFrameDuration and
	//if an RTP packet is ready to sent then it shall be sent
	//since there is no control channel back from the headset, the best
	//we can hope for is to send the data at approx the rate the headset 
	//would expect it and hope that the headset provides it's own approriate
	//internal buffering
	//Note that dues to other AOs running the timing is not accurate and
	//usually slower than the specified time - what is really needed
	//here is a feedback loop where the initial time interval is somewhat
	//faster than the calculated time interval and is adjusted against the
	//system clock and bit rate throughput acordingly so the throughput always
	//matches the bit rate.
	iRTPPacketDuration = TTimeIntervalMicroSeconds32(TFrameTimingUtils::FrameDuration(iFrameLength,iBitRate).Int() * iAudioBufferArray->NumberOfFramesPerRtpPacket());
	RDebug::Printf("RTP Packet Duration = %d mS", iRTPPacketDuration.Int());
	TInt fastCounterFrequency;
	HAL::Get(HALData::EFastCounterFrequency,fastCounterFrequency);
	RDebug::Printf("sys clock timing frequency = %d Hz", fastCounterFrequency);
	iTimeStampIncrement = TFrameTimingUtils::TimeStampIncrementPerFrame(iA2dpAudioCodecConfiguration->HeadsetCodecDataType(), iFrameLength, iBitRate, iA2dpAudioCodecConfiguration->SampleRate())
						 * iAudioBufferArray->NumberOfFramesPerRtpPacket();	
	RDebug::Printf("Calculated RTP packet time stamp increment = %d",iTimeStampIncrement);
	RDebug::Printf("FrameLength = %d", iFrameLength);
	RDebug::Printf("Calculated bitRate = %d", iBitRate);
	RDebug::Printf("Number of frames per RTP packet = %d", iAudioBufferArray->NumberOfFramesPerRtpPacket());
	RDebug::Printf("Number of RTP packets per audio buffer = %d", iAudioBufferArray->NumberOfRtpPacketsPerAudioBuffer());
	RDebug::Printf("Sample rate = %d", iA2dpAudioCodecConfiguration->SampleRate());
	}
	

/**
Internal function to pass the pcm16 audio data in aData and use the codec
to process the data back in aPayload

@return the number of source bytes processed
*/
TUint CActiveRTPStreamer::CodecProcessPayloadL(const TDesC8& aData,TDes8& aPayload)
	{
	TPtr8 srcBufferPtr(const_cast<TUint8*>(aData.Ptr()),aData.Length());
	srcBufferPtr.SetLength(aData.Length());
	//+1 -1 to skip SBC media payload header
	TPtr8 dstBufferPtr(const_cast<TUint8*>(aPayload.Ptr()+1),aPayload.MaxLength()-1);
	CMMFPtrBuffer* srcBuffer = CMMFPtrBuffer::NewL(srcBufferPtr);
	CleanupStack::PushL(srcBuffer);
	CMMFPtrBuffer* dstBuffer = CMMFPtrBuffer::NewL(dstBufferPtr);
	CleanupStack::PushL(dstBuffer);
	TCodecProcessResult result = iCodec->ProcessL(*srcBuffer,*dstBuffer);
	if (result == 	TCodecProcessResult::EProcessIncomplete)
		{
		User::Leave(KErrArgument);
		}
	aPayload.Append(dstBuffer->Data());
	CleanupStack::PopAndDestroy(dstBuffer);
	CleanupStack::PopAndDestroy(srcBuffer);
	return result.iSrcBytesProcessed;
	}


/**
This is the main function for CActiveRTPStreamer in that it is the 
function used to send data to the headset over RTP.
The function is asynchronous to the RunL which does the actual sending.
The data is stored in the CRtpSendPackets FIFO and will be sent at the 
next RunL provided the RTP can accept the data.  If not it just stays
buffered in the CRtpSendPackets FIFO until it can be sent.
The request status is completed when the buffer is stored, not when it is sent
this is to more closely mimic the behaviour of the sound driver.
If adding the buffer to the CRtpSendPackets FIFO causes it to be full
then the request status won't be completed until there is space in the FIFO
which won't be until a callback from the RTP stack has been received 
indicating that the CRtpSendPackets FIFO can now discard that entry.
Only one Send at a time is accepted ie the request status
of the previous send must be completed before Send can be called again.
To simplify the software and improve performance, fixed sized buffers are assumed.ie
the buffer length is only calculated once on the first frame and when the settings
change.

@param aData  The data to be sent to the headset. This may go via 
a codec eg if the data is pcm16 or sent directly to the headset if the data is SBC,mp3,AAC,ATRAC3
It is the responsibility of the client ie CA2dpBTHeadsetAudioInterface to
call SetCodecConfiguration first.

@param aStatus
*/
void CActiveRTPStreamer::Send(const TDesC8& aData, TRequestStatus& aStatus)
	{
	if (iSendStatus)
		{
		__ASSERT_DEBUG((*iSendStatus != KRequestPending),Panic(ERTPStreamerSendNotCompleted));
		}
	iSendStatus = &aStatus;
	*iSendStatus = KRequestPending;
	
	if (iUnrecoverableError)
		{
		User::RequestComplete(iSendStatus,iUnrecoverableError);
		return;
		}
		
	if (!iBufferParamsInitialized)
		{
		TRAPD(err,InitializeForSendL(aData));
		if (err)
			{
			User::RequestComplete(iSendStatus,err);
			return;
			}
		iBufferParamsInitialized = ETrue;
		}
		
	TUint numberOfRtpPacketsPerAudioBuffer = iAudioBufferArray->NumberOfRtpPacketsPerAudioBuffer();	

	if (aData.Size() != iSendBufferSize)
		{
		//then we are on the last buffer
		//in which case we need to recalculate the number of Rtp packets
		//required to make up the audio frame since the last buffer
		//is smaller.
		TUint lastBufferLength = aData.Size();
		if (iCodec)
			{
			lastBufferLength = iA2dpAudioCodecConfiguration->CalculateSBCBufferLength(aData.Size());
			}
		//we keep the same number of RTP packets per audio buffer as before
		TUint numberOfSBCFramesInLastBuffer = lastBufferLength/iFrameLength;
		TUint numberOfFramesPerRtpPacket = iAudioBufferArray->NumberOfFramesPerRtpPacket();
		
		//the devisor below may not always devide without a remainder
		//which means the last Rtp packet sent will not be full
		//if we have a remainder then we need another Rtp packet
		numberOfRtpPacketsPerAudioBuffer = numberOfSBCFramesInLastBuffer/numberOfFramesPerRtpPacket;
		if (numberOfSBCFramesInLastBuffer%numberOfFramesPerRtpPacket)
			{
			numberOfRtpPacketsPerAudioBuffer++;
			}
		}

		
	CRtpSendPackets* sendPackets = iAudioBufferArray->CurrentAudioBufferRtpSendPackets();
	TUint srcBytesProcessed = 0;
	for (TUint i=0; i<numberOfRtpPacketsPerAudioBuffer;i++)
		{
		RDebug::Printf("NewRTPPacketReceived %d",User::FastCounter());
		RRtpSendPacket& sendPacket = sendPackets->Packet(i);
		sendPacket.SetPayloadType(iPayloadType);
		sendPacket.SetTimestamp(iTimeStamp);
		iTimeStamp += iTimeStampIncrement;
		TDes8& payload = sendPacket.WritePayload();
		payload.Zero();
		payload.Append(iMediaPayloadHeader);
			
		//aData may have to be sent as multiple packets
		TUint8* sourceDataOffset = const_cast<TUint8*>(aData.Ptr())+srcBytesProcessed;
		TPtr8 srcBufferPtr(sourceDataOffset,iNumberOfInputBytesToMakeRTPPacket);
		TUint srcBytesStillRemaining = aData.Size() - srcBytesProcessed;
		TUint lengthOfsrcBuffer = iNumberOfInputBytesToMakeRTPPacket;
		if (srcBytesStillRemaining < lengthOfsrcBuffer )
			{//probably a last buffer condition or modulo 2 pcm16 rounding condition
			lengthOfsrcBuffer = srcBytesStillRemaining;
			}
		srcBufferPtr.SetLength(lengthOfsrcBuffer);
		
		//sanity check - the following should always be true
		__ASSERT_DEBUG((srcBytesProcessed == iNumberOfInputBytesToMakeRTPPacket*i),Panic(ERTPStreamerBufferProcessingLengthMismatch));
				
		if (iCodec)
			{
			TRAPD(err,srcBytesProcessed += CodecProcessPayloadL(srcBufferPtr,payload));
			if (err)
				{
				//something has gone wrong so abort streaming
				User::RequestComplete(iSendStatus,err);
				return;
				}
			}
		else //no need to process via codec - aData can go straight to the headset
			{
			srcBytesProcessed +=lengthOfsrcBuffer;
			payload.Append(srcBufferPtr);
			}
		}//	for (TUint i=0; i<iAudioBufferArray->NumberOfRtpPacketsPerAudioBuffer();i++)
	iAudioBufferArray->CurrentAudioBufferReadyToSend();
	//else we have no more send packets so cannot complete request status
	//until one of the send packets has been sent and acknowledged.


	//we'll send an event to ourselves and either send the packet if we can
	//we could complete the iSendStatus TRequestStatus here before returing 
	//from the Send but we won't in order to more closely mimic the existing
	// sound driver PlayData behaviour
	//if there is already a request active then it will be the timer
	//ie this will effectively kick things off if the timer is not running
	TRequestStatus* stat = &iStatus;
	if (!IsActive())
		{
		User::RequestComplete(stat, KErrNone);
		SetActive();
		}			
	}


/**
Function to stop further internally buffered packets being sent to the headset
*/	
void CActiveRTPStreamer::Pause()
	{
	iPaused = ETrue;
	Cancel();
	}


/**
Function called after pause to resume sending buffers to the headset
*/	
void CActiveRTPStreamer::Resume()
	{
	if (iPaused)
		{
		iPaused = EFalse;
		TRequestStatus* stat = &iStatus;
		if (!IsActive())
			{
			User::RequestComplete(stat, KErrNone);
			SetActive();
			}
		}
	}


/**
Function called from  CA2dpBTHeadsetAudioInterface::CancelPlayData()
Used to cancel an outstanding status request for a Send()
*/
void CActiveRTPStreamer::CancelLastSendBuffer()
	{
	if (iSendStatus)
		{
		if (*iSendStatus == KRequestPending)//make sure there is a pending request to cancel
			{
			iAudioBufferArray->CancelMostRecentAudioBuffer(!iRtpCanSend);
			User::RequestComplete(iSendStatus, KErrCancel);
			}
		}
	}
	

/**
Function to flush out the bufferes stored in CRtpSenPackets
*/
void CActiveRTPStreamer::FlushPendingSendBuffers()
	{
	iAudioBufferArray->FlushPendingPackets();
	
	if(iCodec)
		{//flush out codec cache
		TRAP_IGNORE(iCodec->ResetL());
		}
	}

	
/**
Function to return total number of bytes sent prior to codec processing
ie bytes of pcm16 not SBC
Note this the number of bytes sent is only updated when the packet
has been acknowledged as being sent correctly by the RTP stack
ie this value will always be slightly less than the bytes sent in Send()
*/
TUint CActiveRTPStreamer::BytesSent() const
	{
	return iBytesSent;
	}


/**
Function to reset the number of bytes sent
*/	
void CActiveRTPStreamer::ResetBytesSent()
	{
	iBytesSent = 0;
	}


/**
The RunL is called every frame duration interval.
It checks to see if there are any packets to be sent to the headset
and if so send it.
One issue to be resolved at integration testing is if there are no packets
to send then this is analogous to a KErrUnderflow condition on the 
sound driver. Do we need to mimic this behaviour for the a2dp interface?

The Send request status is completed if there is room in the CRtpSendPackets
for another buffer.
*/	
void CActiveRTPStreamer::RunL()
	{
	if ((iPaused)||(!iAudioBufferArray))
		{
		return;
		}
		
	if(iRtpCanSend && iAudioBufferArray->NumberOfAudioBuffersReadyToSend())
		{
		RRtpSendPacket& sendPacket = iAudioBufferArray->CurrentSendPacket();
		sendPacket.Send();
		iRtpCanSend = EFalse; //have to wait for callback before we can send again
		}

	if (iSendStatus)
		{
		if ((iAudioBufferArray->NumberOfAudioBuffersReadyToSend() < KSendBucketSize)
	    	&&(*iSendStatus == KRequestPending))
			{//still some free packets to fill so complete request status
			User::RequestComplete(iSendStatus, KErrNone);
			iSendStatus = NULL;
			}
		//else if the iRtpSendPackets FIFO is full then we can't complete
		//the request status until we've had an ERtpSendSucceeded event
		}
		//are there any more buffers that are ready to send?
		//if so then send the next packet after a time delay
		//keep calling this RunL every frame duration till as long as we have packets to send
		//if there are no packets ready to send then we need to wait 
		//for another call to Send();
	if (iAudioBufferArray->NumberOfAudioBuffersReadyToSend()) 
		{//there are packets ready to send so fire off next RunL after one RTP packet duration
		RDebug::Printf("RTPPacket Sent %d",User::FastCounter());
		After(iRTPPacketDuration);
		}
	}


/**
Cancel
*/	
void CActiveRTPStreamer::DoCancel()
	{
	CTimer::DoCancel();
	CompleteSendRequestStatus(KErrCancel);
	}


/**
Utility function to complete Send TRequestStatus with aError
*/
void CActiveRTPStreamer::CompleteSendRequestStatus(TInt aError)
	{
	if (iSendStatus)
		{
		if (*iSendStatus == KRequestPending)
			{
			User::RequestComplete(iSendStatus, aError);
			}
		}
	}


/**
Called by RTP stack when a packet has been sent
If the packet was sent ok then complete the iSendStatus if it is pending
and update the number of bytes sent
If the packet was not sent ok then the error is regarded as unrecoverable
since this should not happen.  If it does happen then the CA2dpBTHeadsetAudioInterface
is informed.  If there is an outstanding Send TRequestStatus then this is
completed with KErrCommsFrame.  Not sure if this is the most appropriate error code?
*/	
void CActiveRTPStreamer::PacketSent(TRtpEventType aEvent)
	{
	if (aEvent == ERtpSendSucceeded)
		{
		RDebug::Printf("Sent RTPPacket Acknowledged %d",User::FastCounter());
		TBool entireAudioBufferSent = EFalse;	
		iAudioBufferArray->CurrentSendPacketSent(entireAudioBufferSent);		
		//check if there is an outstanding send request status
		//we can only complete the send request status if all the
		//RTP packets in the audio buffer have been sent.
		if (entireAudioBufferSent)
			{
			CompleteSendRequestStatus(KErrNone);
			}
		iRtpCanSend = ETrue;
		iBytesSent += iNumberOfInputBytesToMakeRTPPacket;
		}
	else if (aEvent == ERtpSendFail)
		{
		//if we fail to send the packet then chances are something
		//has gone wrong that may not be recoverable
		//so we will complete the request status and halt further streaming
		//some testing may be required to see if it is possible to
		//recover in which case the packet could be sent again
		iUnrecoverableError = KErrCommsFrame;//probably the nearest error code
		CompleteSendRequestStatus(iUnrecoverableError);
		//inform iRTPStreamerObserver ie the CA2dpBTHeadsetAudioInterface
		//this will initiate a GAVDP state machine reset which will destroy the CActiveRTPStreamer 
		iRTPStreamerObserver.RTPStreamerEvent(iUnrecoverableError);
		}
	else 
		{//we've not registered for any other events so this shouldn't happen
		Panic(ERTPStreamerUnexpectedEvent);
		}
	}
	

/**
Called by RTP stack when some sort of general error has occured
eg switching off the headset, or the headset going out of range 
The CA2dpBTHeadsetAudioInterface is informed. 
*/
void CActiveRTPStreamer::RTPSessionEvent(const TRtpEvent& aEvent)
	{
	switch(aEvent.Type())
		{
		case ERtpSessionFail:
			iUnrecoverableError = KErrDisconnected;
			break;
		case ERtpBufferOverflow:
			iUnrecoverableError = KErrOverflow;
			break;
		case ERtpUndersizedPacket:
			iUnrecoverableError = KErrCommsFrame;
			break;
		default:
			Panic(ERTPStreamerRTPEventError); //we haven't registered for anything else
			break;
		}
		
	//complete outstanding Send (CA2dpBTHeadsetAudioInterface::PlayData) request status
	CompleteSendRequestStatus(iUnrecoverableError);		
	//inform CA2dpBTHeadsetAudioInterface
	iRTPStreamerObserver.RTPStreamerEvent(iUnrecoverableError);
	}


/**
Static callback from RTP stack
*/	
void CActiveRTPStreamer::RTPSendSourceCallback(CActiveRTPStreamer* aStreamer, const TRtpEvent& aEvent)
	{
	__ASSERT_DEBUG((aEvent.IsSendSourceEvent()),Panic(ERTPStreamerRTPEventError));
	__ASSERT_DEBUG((aEvent.SendSource() == aStreamer->iRTPSendSource),Panic(ERTPStreamerRTPEventError));
	// for now assume it was sending complete
	// do next bit
	aStreamer->PacketSent(aEvent.Type());
	}
	

/**
Static callback from RTP stack
*/
void CActiveRTPStreamer::RTPCallback(CActiveRTPStreamer* aStreamer, const TRtpEvent& aEvent)
	{
	__ASSERT_DEBUG((aEvent.IsSessionEvent()),Panic(ERTPStreamerRTPEventError)); 
	__ASSERT_DEBUG((aEvent.Session() == aStreamer->iRTPSession),Panic(ERTPStreamerRTPEventError));
	
	aStreamer->RTPSessionEvent(aEvent);
	}
