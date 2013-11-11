/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#include "TSU_MMF_ACOD_AAAABBBBTestCodec.h"


// __________________________________________________________________________
// Implementation

/** @xxxx
 *	This function instantiates a CMMFAAAABBBBTestCodec
 *
 * @param	"TAny* aInitParams" 
 *			pointer to initial codec parameters
 *
 * @return	pointer to newly instantiated codec
 */
CMMFCodec* CMMFAAAABBBBTestCodec::NewL(TAny* aInitParams)
	{
	CMMFAAAABBBBTestCodec* self=new(ELeave) CMMFAAAABBBBTestCodec();
	CleanupStack::PushL(self);
	self->ConstructL(aInitParams);
	CleanupStack::Pop(self);
	return STATIC_CAST( CMMFCodec*, self );
	}

CMMFAAAABBBBTestCodec::~CMMFAAAABBBBTestCodec()
	{
	}

CMMFAAAABBBBTestCodec::CMMFAAAABBBBTestCodec()
	{
	}

void CMMFAAAABBBBTestCodec::ConstructL(TAny* /*aInitParams*/)
	{
	}

/** @xxxx
 *	This function writes '4's to the destination buffer, regardless of what is in the source buffer.
 *	This codec only exists to test the preferred supplier codec instantiation.
 *
 * @param	"const CMMFBuffer& aSrc" 
 *			reference to source buffer
 *			"CMMFBuffer& aDst" 
 *			reference to destination buffer
 *
 * @return	result of operation.
 */
TCodecProcessResult CMMFAAAABBBBTestCodec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
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
	
	TUint8* pDst = CONST_CAST(TUint8*,iDst->Data().Ptr());
	pDst += iDst->Position();

	TUint samplesToProcess = srcUse;
	while (samplesToProcess--)
		{ 
		*pDst++ = 4;
		}

	TCodecProcessResult result;
	result.iStatus = TCodecProcessResult::EProcessComplete;

	result.iSrcBytesProcessed = srcUse;
	result.iDstBytesAdded = srcUse;

	iDst->Data().SetLength(iDst->Position() + result.iDstBytesAdded);

	return result;
	}

