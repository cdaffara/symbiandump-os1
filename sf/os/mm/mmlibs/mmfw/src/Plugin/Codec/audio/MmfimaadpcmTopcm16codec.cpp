// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "MMFImaAdPcmToPcm16Codec.h"
#include "MMFImaAudioCodecState.h"

#include <mmf/common/mmfaudio.h> // TMMFAudioConfig
#include <mmf/plugin/mmfcodecimplementationuids.hrh> // KUidMmfCodecAudioSettings

const TUid KUidCodecAudioConfig = {KUidMmfCodecAudioSettings};

// __________________________________________________________________________
// Implementation

CMMFCodec* CMMFImaAdPcmPcm16Codec::NewL(TAny* aInitParams)
	{
	CMMFImaAdPcmPcm16Codec* self=new(ELeave) CMMFImaAdPcmPcm16Codec();
	CleanupStack::PushL(self);
	self->ConstructL(aInitParams);
	CleanupStack::Pop(self);
	return STATIC_CAST( CMMFCodec*, self );
	}

CMMFImaAdPcmPcm16Codec::~CMMFImaAdPcmPcm16Codec()
	{
	}

CMMFImaAdPcmPcm16Codec::CMMFImaAdPcmPcm16Codec() : iImaAdpcmTo16Pcm(1)
	{
	}

void CMMFImaAdPcmPcm16Codec::ConstructL(TAny*  /*aInitParams*/)
	{
	iTempSrcBufferPtr = iTempSrcBuffer;
	iTempSrcBufferCount = 0;
	
	iChannels = 1;
	iSamplesRate = 0;
	iSamplesPerBlock = KImaAdpcmSamplesPerBlock;
	iBlockAlign = KImaAdpcmBlockAlign;
	}

void CMMFImaAdPcmPcm16Codec::ResetL()
	{
	//Reset the actual codec
	TMMFImaAdpcmCodecStateOld state;
	state.iIndex = 0;
	state.iPredicted = 0;
	iImaAdpcmTo16Pcm.SetState(state);
	iTempSrcBufferPtr = iTempSrcBuffer;
	iTempSrcBufferCount = 0;
	}

/**
CMMFImaAdPcmPcm16Codec::ProcessL

This function converts IMA ADPCM samples to PCM 16 samples, 
it is for mono ADPCM only at the moment.
*/
TCodecProcessResult CMMFImaAdPcmPcm16Codec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	TCodecProcessResult result;
	result.iStatus = TCodecProcessResult::EProcessIncomplete;

	//convert from generic CMMFBuffer to CMMFDataBuffer
	iSrc = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	iDst = STATIC_CAST(CMMFDataBuffer*, &aDst);

	const TUint srcLen = iSrc->Data().Length();
	const TUint dstMaxLen = iDst->Data().MaxLength();
	const TUint sourceRemain = srcLen - iSrc->Position();

	if (dstMaxLen < (iSamplesPerBlock * 2))
		User::Leave(KErrArgument);

	if ((iSrc->FrameNumber() == 0) && (iDst->Position() == 0))
		{
		ResetL();
		}

	//reset data if not a consecutive frame number
	if ((iSrc->FrameNumber() != iLastFrameNumber) && (iSrc->FrameNumber() != (iLastFrameNumber+1)))
		{
		iTempSrcBufferPtr = iTempSrcBuffer;
		iTempSrcBufferCount = 0;
		}
	iLastFrameNumber = iSrc->FrameNumber();

	TUint dstRemain = (dstMaxLen - iDst->Position());
	TUint srcToFillTempBuffer = 0;

	//take account of src to be added to temporary buffer
	if (iTempSrcBufferCount > 0)
		{
		srcToFillTempBuffer = iBlockAlign - iTempSrcBufferCount;
		
		if (srcToFillTempBuffer < sourceRemain) //enough source to fill temporary buffer
			dstRemain -= (iSamplesPerBlock * 2);
		else //not enough source to fill the temporary buffer
			srcToFillTempBuffer = sourceRemain;
		}

	//calculate how much source is required to fill the destination buffer
	TUint blocksRemaining = dstRemain / (iSamplesPerBlock * 2);
	TUint maxUsableDst = blocksRemaining * iSamplesPerBlock * 2;
	TUint srcToUse = blocksRemaining * iBlockAlign;

	srcToUse += srcToFillTempBuffer;
	srcToUse = (srcToUse < sourceRemain) ? srcToUse : sourceRemain;
	
	//we need to cast away CONST even on the source, as the TClass needs a TUint8*
	TUint8* pSrc = CONST_CAST(TUint8*,iSrc->Data().Ptr());
	pSrc += iSrc->Position();
	TUint8* pDst = CONST_CAST(TUint8*,iDst->Data().Ptr());
	pDst += iDst->Position();

	TUint dstBytesAdded = 0;
	TUint srcLeft = srcToUse;

	//convert remaining source from previous call to ProcessL
	if (iTempSrcBufferCount > 0)
		{
		//Fill temp buffer from source buffer
		while((iTempSrcBufferCount < iBlockAlign) && (srcLeft))
			{
			*iTempSrcBufferPtr++ = *pSrc++;
			iTempSrcBufferCount++;
			srcLeft --;
			}

		if (iTempSrcBufferCount == iBlockAlign) //temp buffer full
			{
			//reset
			iTempSrcBufferCount = 0;
			iTempSrcBufferPtr = iTempSrcBuffer;

			iImaAdpcmTo16Pcm.Convert(iTempSrcBufferPtr, pDst, iSamplesPerBlock);

			pDst += (iSamplesPerBlock * 2);
			dstBytesAdded += (iSamplesPerBlock * 2);
			}
		}

	//convert full blocks
	while (srcLeft >= iBlockAlign) 
		{
		iImaAdpcmTo16Pcm.Convert(pSrc, pDst, iSamplesPerBlock);

		pSrc += iBlockAlign;
		pDst += (iSamplesPerBlock * 2);

		dstBytesAdded += (iSamplesPerBlock * 2);	
		srcLeft -= iBlockAlign;	
		}

	while (srcLeft)
		{
		*iTempSrcBufferPtr++ = *pSrc++;
		iTempSrcBufferCount++;
		srcLeft--;
		}
	
	//if the source buffer is consumed
	if ((srcLen == srcToUse + iSrc->Position()))
		{
		if (dstBytesAdded < maxUsableDst)
			result.iStatus = TCodecProcessResult::EDstNotFilled;
		else
			result.iStatus = TCodecProcessResult::EProcessComplete;
		}

	result.iSrcBytesProcessed = srcToUse;
	result.iDstBytesAdded = dstBytesAdded;

	iDst->Data().SetLength( iDst->Position() + result.iDstBytesAdded);

	return result;
	}

/**
Sets codec configuration.

@param  aConfigType
        The UID of the codec to configure.
@param  aConfigData
        The configuration information.
*/
void CMMFImaAdPcmPcm16Codec::ConfigureL(TUid aConfigType, const TDesC8& aConfigData)
	{
	if (aConfigType != KUidCodecAudioConfig)
		{
		User::Leave(KErrArgument);
		}
		
	const TMMFAudioConfig& audioConfig = static_cast<const TPckgBuf<TMMFAudioConfig>&>(aConfigData)();
	
	iChannels = audioConfig.iChannels;
	iSamplesRate = audioConfig.iSampleRate;
	
	switch (iSamplesRate * iChannels)
		{
		case 8000: // fall through, same as 11025
		case 11025:
		case 16000:
			iBlockAlign = 256;
			break;
		case 22050:
			iBlockAlign = 512;
			break;
			
		case 44100:
			iBlockAlign = 1024;
			break;
			
		case 88200:
			iBlockAlign = 2048;
			break;
			
		default:
			User::Leave(KErrArgument);
		}
	
	
	// SamplesPerBlock = [(BlockAlign - 4 * Channels) * 8] / (BitsPerSample * Channels) + 1
	iSamplesPerBlock = (iBlockAlign - 4 * iChannels) * 8 / (KImaAdpcmBitsPerSample * iChannels) + 1;
	}

TInt CMMFImaAdPcmPcm16Codec::Extension_(TUint aExtensionId, TAny*& aExtPtr, TAny*)
	{
	if (aExtensionId == KUidCustomInterfaceDevSoundFileBlockLength.iUid)	
		{
		aExtPtr = static_cast<MMMFDevSoundCustomInterfaceFileBlockLength*>(this);
		return KErrNone;
		}
	else
		{
		return CMMFCodec::Extension_(aExtensionId, aExtPtr, NULL);	
		}
	}

void CMMFImaAdPcmPcm16Codec::SetFileBlockLength(TUint aBlockAlign)
	{	
    iBlockAlign = aBlockAlign;		
  	iSamplesPerBlock = (iBlockAlign - 4 * iChannels) * 8 / (KImaAdpcmBitsPerSample * iChannels) + 1;
  		    
	}

