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



#include "MMFAudioS16ToPcms16Codec.h"


// __________________________________________________________________________
// Implementation

CMMFCodec* CMMFAudioS16PcmS16Codec::NewL(TAny* aInitParams)
	{
	CMMFAudioS16PcmS16Codec* self=new(ELeave) CMMFAudioS16PcmS16Codec();
	CleanupStack::PushL(self);
	self->ConstructL(aInitParams);
	CleanupStack::Pop(self);
	return STATIC_CAST( CMMFCodec*, self );
	}

CMMFAudioS16PcmS16Codec::~CMMFAudioS16PcmS16Codec()
	{
	}

CMMFAudioS16PcmS16Codec::CMMFAudioS16PcmS16Codec()
	{
	}

void CMMFAudioS16PcmS16Codec::ConstructL(TAny* /*aInitParams*/)
	{
	}


//this codec has a does not expand or shrink destination data
TCodecProcessResult CMMFAudioS16PcmS16Codec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
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
	TUint srcUse = dstMaxLen - iDst->Position();
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

	//divide by TWO as 2 bytes are equal to 1 sample
	Mem::Copy( pDst, pSrc, srcUse / 2 );

	if (srcUse + iDst->Position() < dstMaxLen)
		result.iStatus = TCodecProcessResult::EDstNotFilled;

	else if (srcUse + iSrc->Position() >= srcLen)
		result.iStatus = TCodecProcessResult::EProcessComplete;

	result.iSrcBytesProcessed = srcUse;
	result.iDstBytesAdded = srcUse;

	iDst->Data().SetLength(iDst->Position() + srcUse);

	return result;

	}
