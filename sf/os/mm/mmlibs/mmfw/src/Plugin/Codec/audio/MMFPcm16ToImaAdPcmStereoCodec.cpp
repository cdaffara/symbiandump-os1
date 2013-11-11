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



#include "MMFPcm16ToImaAdPcmStereoCodec.h"

// __________________________________________________________________________
// Implementation

CMMFCodec* CMMFPcm16ImaAdPcmStereoCodec::NewL(TAny* aInitParams)
	{
	CMMFPcm16ImaAdPcmStereoCodec* self=new(ELeave) CMMFPcm16ImaAdPcmStereoCodec();
	CleanupStack::PushL(self);
	self->ConstructL(aInitParams);
	CleanupStack::Pop(self);
	return STATIC_CAST( CMMFCodec*, self );
	}

CMMFPcm16ImaAdPcmStereoCodec::~CMMFPcm16ImaAdPcmStereoCodec()
	{
	}

CMMFPcm16ImaAdPcmStereoCodec::CMMFPcm16ImaAdPcmStereoCodec() : i16PcmToImaAdpcm(2)
	{
	}

void CMMFPcm16ImaAdPcmStereoCodec::ConstructL(TAny*  /*aInitParams*/)
	{
	iTempSrcBufferPtr = iTempSrcBuffer;
	iTempSrcBufferCount = 0;
	}

/*************************************************************
CMMFPcm16ImaAdPcmStereoCodec::ProcessL

This function converts stereo PCM samples to IMA ADPCM samples 
in blocks of KImaAdpcmBlockAlign (256) bytes. Any left 
over bytes that do not fill a block are currently discarded. 
996 source bytes are required to fill a 256 byte block.
**************************************************************/
TCodecProcessResult CMMFPcm16ImaAdPcmStereoCodec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	TCodecProcessResult result;
	result.iStatus = TCodecProcessResult::EProcessIncomplete;

	//convert from generic CMMFBuffer to CMMFDataBuffer
	iSrc = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	iDst = STATIC_CAST(CMMFDataBuffer*, &aDst);

	const TUint srcLen = iSrc->Data().Length();
	const TUint dstMaxLen = iDst->Data().MaxLength();
	const TUint sourceRemain = srcLen - iSrc->Position();

	if (dstMaxLen < KImaAdpcmBlockAlign)
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
		srcToFillTempBuffer = KImaAdpcmStereoTempBufferSize - iTempSrcBufferCount;

		if (srcToFillTempBuffer<sourceRemain) //enough source to fill temporary buffer
			dstRemain -= KImaAdpcmBlockAlign;
		else //not enough source to fill the temporary buffer
			srcToFillTempBuffer = sourceRemain;
		}

	//calculate how much source is required to fill the destination buffer
	TUint blocksRemaining = dstRemain/KImaAdpcmBlockAlign;
	TUint maxUsableDst = blocksRemaining * KImaAdpcmBlockAlign;
	TUint srcToUse = blocksRemaining * KImaAdpcmStereoSamplesPerBlock * 4;

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
		while((iTempSrcBufferCount < KImaAdpcmStereoTempBufferSize) && (srcLeft))
			{
			*iTempSrcBufferPtr++ = *pSrc++;
			iTempSrcBufferCount++;
			srcLeft--;
			}

		if (iTempSrcBufferCount == KImaAdpcmStereoTempBufferSize)
			{
			//reset
			iTempSrcBufferCount = 0;
			iTempSrcBufferPtr = iTempSrcBuffer;

			i16PcmToImaAdpcm.Convert(iTempSrcBufferPtr, pDst, KImaAdpcmStereoSamplesPerBlock);

			pDst += KImaAdpcmBlockAlign;
			dstBytesAdded += KImaAdpcmBlockAlign;
			}
		}

	//convert full blocks
	while (srcLeft >= (KImaAdpcmStereoSamplesPerBlock*4)) 
		{
		i16PcmToImaAdpcm.Convert(pSrc, pDst, KImaAdpcmStereoSamplesPerBlock);

		pSrc += KImaAdpcmStereoSamplesPerBlock*4;
		pDst += KImaAdpcmBlockAlign;

		dstBytesAdded += KImaAdpcmBlockAlign;	
		srcLeft -= KImaAdpcmStereoSamplesPerBlock*4;	
		}

	//save remaining source in iTempSrcBuffer
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

	iDst->Data().SetLength(iDst->Position() + result.iDstBytesAdded);

	return result;
	}
