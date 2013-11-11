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



#include "MMFAudioU8ToPcmS16Codec.h"

// __________________________________________________________________________
// Implementation

CMMFCodec* CMMFAudioU8PcmS16Codec::NewL(TAny* aInitParams)
	{
	CMMFAudioU8PcmS16Codec* self=new(ELeave) CMMFAudioU8PcmS16Codec();
	CleanupStack::PushL(self);
	self->ConstructL(aInitParams);
	CleanupStack::Pop(self);
	return STATIC_CAST( CMMFCodec*, self );
	}

CMMFAudioU8PcmS16Codec::~CMMFAudioU8PcmS16Codec()
	{
	}

CMMFAudioU8PcmS16Codec::CMMFAudioU8PcmS16Codec()
	{
	}

void CMMFAudioU8PcmS16Codec::ConstructL(TAny* /*aInitParams*/)
	{
	}


	
//this codec converts 1 byte to 2 bytes
TCodecProcessResult CMMFAudioU8PcmS16Codec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{

	TCodecProcessResult result;
	result.iStatus = TCodecProcessResult::EProcessIncomplete;

	//convert from generic CMMFBuffer to CMMFDataBuffer
	iSrc = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	iDst = STATIC_CAST(CMMFDataBuffer*, &aDst);

	const TUint dstMaxLen = iDst->Data().MaxLength();

	if (!dstMaxLen)
		User::Leave(KErrArgument);

	//don't scribble Destination (pDst) by only consuming enough source to fill pDst
	TUint srcUse = (dstMaxLen - iDst->Position()) / 2;
	const TUint srcLen = iSrc->Data().Length();
	const TUint sourceRemain = srcLen - iSrc->Position();

	//make sure we don't blow source by checking against remaining
	//and clipping to minimum remaining if necessary
	srcUse = (srcUse<sourceRemain ? srcUse : sourceRemain);
	
	//we need to cast away CONST even on the source, as the TClass needs a TUint8*
	TUint8* pSrc = CONST_CAST(TUint8*,iSrc->Data().Ptr());
	pSrc += iSrc->Position();
	TUint8* pDst = CONST_CAST(TUint8*,iDst->Data().Ptr());
	pDst += iDst->Position();


	iAudioU8ToS16Pcm.Convert(pSrc, pDst, srcUse );

	if ((srcUse * 2) + iDst->Position() < dstMaxLen)
		result.iStatus = TCodecProcessResult::EDstNotFilled;

	else if (srcUse + iSrc->Position() >= srcLen)
		result.iStatus = TCodecProcessResult::EProcessComplete;

	result.iSrcBytesProcessed = srcUse;
	result.iDstBytesAdded = srcUse * 2;

	iDst->Data().SetLength(iDst->Position() + (srcUse * 2));

	return result;
	}

