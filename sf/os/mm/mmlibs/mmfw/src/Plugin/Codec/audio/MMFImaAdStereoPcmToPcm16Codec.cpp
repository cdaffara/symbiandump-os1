/*
* Copyright (c) 1997-2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#include "MMFImaAdStereoPcmToPcm16Codec.h"


// __________________________________________________________________________
// Implementation

CMMFCodec* CMMFImaAdStereoPcmPcm16Codec::NewL(TAny* aInitParams)
	{
	CMMFImaAdStereoPcmPcm16Codec* self=new(ELeave) CMMFImaAdStereoPcmPcm16Codec();
	CleanupStack::PushL(self);
	self->ConstructL(aInitParams);
	CleanupStack::Pop(self);
	return STATIC_CAST( CMMFCodec*, self );
	}

CMMFImaAdStereoPcmPcm16Codec::~CMMFImaAdStereoPcmPcm16Codec()
	{
	}

CMMFImaAdStereoPcmPcm16Codec::CMMFImaAdStereoPcmPcm16Codec() : iImaAdpcmTo16Pcm(2)
	{
	}

void CMMFImaAdStereoPcmPcm16Codec::ConstructL(TAny*  /*aInitParams*/)
	{
	iTempSrcBufferPtr = iTempSrcBuffer;
	iTempSrcBufferCount = 0;
	}

/*************************************************************
CMMFImaAdStereoPcmPcm16Codec::ProcessL

This function converts IMA ADPCM stereo samples to PCM samples 
in blocks of KImaAdpcmBlockAlign (256) bytes. 256 
source bytes are required to fill a 996 byte block.
**************************************************************/
TCodecProcessResult CMMFImaAdStereoPcmPcm16Codec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	TCodecProcessResult result;
	result.iStatus = TCodecProcessResult::EProcessIncomplete;

	//convert from generic CMMFBuffer to CMMFDataBuffer
	iSrc = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	iDst = STATIC_CAST(CMMFDataBuffer*, &aDst);

	const TUint srcLen = iSrc->Data().Length();
	const TUint dstMaxLen = iDst->Data().MaxLength();
	const TUint sourceRemain = srcLen - iSrc->Position();

	if (dstMaxLen < (KImaAdpcmStereoSamplesPerBlock*4))
		User::Leave(KErrArgument);

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
		srcToFillTempBuffer = KImaAdpcmBlockAlign - iTempSrcBufferCount;
		
		if (srcToFillTempBuffer<sourceRemain) //enough source to fill temporary buffer
			dstRemain -= (KImaAdpcmStereoSamplesPerBlock*4);
		else //not enough source to fill the temporary buffer
			srcToFillTempBuffer = sourceRemain;
		}

	//calculate how much source is required to fill the destination buffer
	TUint blocksRemaining = dstRemain/(KImaAdpcmStereoSamplesPerBlock*4);
	TUint maxUsableDst = blocksRemaining * KImaAdpcmSamplesPerBlock * 4;
	TUint srcToUse = blocksRemaining * KImaAdpcmBlockAlign;

	srcToUse += srcToFillTempBuffer;
	srcToUse = (srcToUse<sourceRemain ? srcToUse : sourceRemain);
	
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
		while((iTempSrcBufferCount < KImaAdpcmBlockAlign) && (srcLeft))
			{
			*iTempSrcBufferPtr++ = *pSrc++;
			iTempSrcBufferCount++;
			srcLeft--;
			}

		if(iTempSrcBufferCount == KImaAdpcmBlockAlign)
			{
			//reset
			iTempSrcBufferCount = 0;
			iTempSrcBufferPtr = iTempSrcBuffer;

			iImaAdpcmTo16Pcm.Convert(iTempSrcBufferPtr, pDst, KImaAdpcmStereoSamplesPerBlock);

			pDst += (KImaAdpcmStereoSamplesPerBlock*4);
			dstBytesAdded += (KImaAdpcmStereoSamplesPerBlock*4);
			}
		}

	//convert full blocks
	while (srcLeft >= KImaAdpcmBlockAlign) 
		{
		iImaAdpcmTo16Pcm.Convert(pSrc, pDst, KImaAdpcmStereoSamplesPerBlock);

		pSrc += KImaAdpcmBlockAlign;
		pDst += (KImaAdpcmStereoSamplesPerBlock*4);

		dstBytesAdded += (KImaAdpcmStereoSamplesPerBlock*4);	
		srcLeft -= KImaAdpcmBlockAlign;	
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
