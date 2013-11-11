// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "TSU_MMF_SWCDWRAP_TestDevice.h"

void Panic()
	{
	_LIT(KMMF_SWCDWRAP_UnitTestPanic, "TSU_MMF_SWCDWRAP");
	User::Panic(KMMF_SWCDWRAP_UnitTestPanic, 0);
	}

/**
 *
 *	test null hw device
 *	@return	"CMMFTestNullDevice"
 *
 */
CMMFTestNullDevice* CMMFTestNullDevice::NewL()
	{
    CMMFTestNullDevice* self = new (ELeave) CMMFTestNullDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
 *
 *	Second phase constructor.
 *
 */
void CMMFTestNullDevice::ConstructL()
	{
    // Create device codec
	iCodec = new (ELeave) CMMFTestNullCodec();
	}


CMMFTestNullDevice::~CMMFTestNullDevice()
	{
	}


CMMFSwCodec& CMMFTestNullDevice::Codec()
	{
	__ASSERT_ALWAYS(iCodec, Panic());
	return *iCodec;
	}


CMMFSwCodec::TCodecProcessResult CMMFTestNullCodec::ProcessL(const CMMFBuffer& /*aSource*/, CMMFBuffer& /*aDest*/)
	{//no processing required for null codec
	User::Leave(KErrNotSupported); 
	//to keep compiler happy
	TCodecProcessResult result;
	result.iCodecProcessStatus = TCodecProcessResult::EEndOfData;
	result.iSrcBytesProcessed = 0;
	result.iDstBytesAdded = 0;
	return result;
	};



/**
 *
 *	test null hw device
 *	@return	"CMMFTestNullDevice"
 *
 */
CMMFTest2To1Device* CMMFTest2To1Device::NewL()
	{
    CMMFTest2To1Device* self = new (ELeave) CMMFTest2To1Device();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
 *
 *	Second phase constructor.
 *
 */
void CMMFTest2To1Device::ConstructL()
	{
    // Create device codec
	iCodec = new (ELeave) CMMFTest2To1Codec();
	}


CMMFTest2To1Device::~CMMFTest2To1Device()
	{
	}


CMMFSwCodec& CMMFTest2To1Device::Codec()
	{
	__ASSERT_ALWAYS(iCodec, Panic());
	return *iCodec;
	}


CMMFSwCodec::TCodecProcessResult CMMFTest2To1Codec::ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest)
	{//no processing required for null codec
	
	//convert from generic CMMFBuffer to CMMFDataBuffer
	const CMMFDataBuffer* src = static_cast<const CMMFDataBuffer*>(&aSource);
	CMMFDataBuffer* dst = static_cast<CMMFDataBuffer*>(&aDest);

	const TUint dstMaxLen = dst->Data().MaxLength();

	if (!dstMaxLen)
	    {
		User::Leave(KErrArgument);
	    }

	const TUint srcLen = src->Data().Length();

	//we need to cast away CONST even on the source, as the TClass needs a TUint8*
	const TUint8* pSrc = src->Data().Ptr();
	TUint8* pDst = const_cast<TUint8*>(dst->Data().Ptr());
	
	TUint numberOfSamplesToGenerate = srcLen/4; // /2 to convert from bytes to samples and /2 as we only take every other
	
	ASSERT(numberOfSamplesToGenerate*2 <= dstMaxLen); // should be true for test
	
	TInt numberOfSamples = numberOfSamplesToGenerate;

	while (numberOfSamples--)
		{ 
		*pDst++ = *pSrc++;
		*pDst++ = *pSrc++;
		pSrc += 2; //only send every other sample to destination
		}

	TCodecProcessResult result;
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	result.iSrcBytesProcessed = numberOfSamplesToGenerate*4;
	result.iDstBytesAdded = numberOfSamplesToGenerate*2;

	dst->Data().SetLength(result.iDstBytesAdded);

	return result;
	};
