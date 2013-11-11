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



#include "MMFPcm16ToImaAdPcmCodec.h"

// __________________________________________________________________________
// Implementation

CMMFCodec* CMMFPcm16ImaAdPcmCodec::NewL(TAny* aInitParams)
	{
	CMMFPcm16ImaAdPcmCodec* self=new(ELeave) CMMFPcm16ImaAdPcmCodec();
	CleanupStack::PushL(self);
	self->ConstructL(aInitParams);
	CleanupStack::Pop(self);
	return STATIC_CAST( CMMFCodec*, self );
	}

CMMFPcm16ImaAdPcmCodec::~CMMFPcm16ImaAdPcmCodec()
	{
	}

CMMFPcm16ImaAdPcmCodec::CMMFPcm16ImaAdPcmCodec() : i16PcmToImaAdpcm(1)
	{
	}

void CMMFPcm16ImaAdPcmCodec::ConstructL(TAny*  /*aInitParams*/)
	{
	iTempSrcBufferPtr = iTempSrcBuffer;
	iTempSrcBufferCount = 0;
	}

void CMMFPcm16ImaAdPcmCodec::ResetL()
	{
	//Reset the actual codec
	TMMFImaAdpcmCodecStateOld state;
	state.iIndex = 0;
	state.iPredicted = 0;
	i16PcmToImaAdpcm.SetState(state);
	iTempSrcBufferPtr = iTempSrcBuffer;
	iTempSrcBufferCount = 0;
	}

/*************************************************************
CMMFPcm16ImaAdPcmCodec::ProcessL

This function converts PCM samples to IMA ADPCM samples in
blocks of KImaAdpcmBlockAlign (256) bytes. 1010 source 
bytes are required to fill a 256 byte block.
**************************************************************/
TCodecProcessResult CMMFPcm16ImaAdPcmCodec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	TCodecProcessResult result;
	result.iStatus = TCodecProcessResult::EProcessIncomplete;

	//convert from generic CMMFBuffer to CMMFDataBuffer
	iSrc = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	iDst = STATIC_CAST(CMMFDataBuffer*, &aDst);

	// If the Destination is set to zero, ensure the Codec is fully reset,
	// as this may be a reposition
	if ((iSrc->FrameNumber() == 0) && (iDst->Position() == 0))
		{
		ResetL();
		}


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
		srcToFillTempBuffer = KImaAdpcmTempBufferSize - iTempSrcBufferCount;
		if (srcToFillTempBuffer<sourceRemain) //enough source to fill temporary buffer
			dstRemain -= KImaAdpcmBlockAlign;
		else //not enough source to fill the temporary buffer
			srcToFillTempBuffer = sourceRemain;
		}

	//calculate how much source is required to fill the destination buffer
	TUint blocksRemaining = dstRemain/KImaAdpcmBlockAlign;
	TUint maxUsableDst = blocksRemaining * KImaAdpcmBlockAlign;
	TUint srcToUse = blocksRemaining * KImaAdpcmSamplesPerBlock * 2;

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
		while((iTempSrcBufferCount < KImaAdpcmTempBufferSize) && (srcLeft))
			{
			*iTempSrcBufferPtr++ = *pSrc++;
			iTempSrcBufferCount++;
			srcLeft--;
			}

		if (iTempSrcBufferCount == KImaAdpcmTempBufferSize)
			{
			//reset
			iTempSrcBufferCount = 0;
			iTempSrcBufferPtr = iTempSrcBuffer;

			i16PcmToImaAdpcm.Convert(iTempSrcBufferPtr, pDst, KImaAdpcmSamplesPerBlock);

			pDst += KImaAdpcmBlockAlign;
			dstBytesAdded += KImaAdpcmBlockAlign;
			}
		}

	//convert full blocks
	while (srcLeft >= (KImaAdpcmSamplesPerBlock*2)) 
		{
		i16PcmToImaAdpcm.Convert(pSrc, pDst, KImaAdpcmSamplesPerBlock);

		pSrc += KImaAdpcmSamplesPerBlock*2;
		pDst += KImaAdpcmBlockAlign;

		dstBytesAdded += KImaAdpcmBlockAlign;	
		srcLeft -= KImaAdpcmSamplesPerBlock*2;	
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
