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
// MmfBtMulawToPcm16HwDevice.cpp
// 
//

#include "MmfBtAudioCodec.h"
#include <mmfpaniccodes.h>
#include "MmfBtPcm16SwapEndianHwDevice.h"
#include "../../MmfBtFileDependencyUtil.h"

/**
*
* NewL
* @return 'CMMFPcm16SwapEndianHwDevice*'
*
*/
CMMFPcm16SwapEndianHwDevice* CMMFPcm16SwapEndianHwDevice::NewL()
	{
	CMMFPcm16SwapEndianHwDevice* self=new(ELeave) CMMFPcm16SwapEndianHwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*
* ~CMMFPcm16SwapEndianHwDevice
*/
CMMFPcm16SwapEndianHwDevice::~CMMFPcm16SwapEndianHwDevice()
	{
	}

/**
*
* ConstructL
*
*/
void CMMFPcm16SwapEndianHwDevice::ConstructL()
	{
	iCodec = new (ELeave)CMMFPcm16SwapEndianCodec();
	}

/**
*
* Codec
*
*/
CMMFSwCodec &CMMFPcm16SwapEndianHwDevice::Codec()
	{
	return *iCodec;
	}

/**
*
* ProcessL
* @param aSrc
* @param aDst
* @return 'CMMFSwCodec::TCodecProcessResult'
*
*/
CMMFSwCodec::TCodecProcessResult CMMFPcm16SwapEndianCodec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	CMMFSwCodec::TCodecProcessResult result;
	result.iCodecProcessStatus = result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	
	//convert from generic CMMFBuffer to CMMFDataBuffer
	const CMMFDataBuffer* src = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	CMMFDataBuffer* dst = STATIC_CAST(CMMFDataBuffer*, &aDst);
	
	//[ check the preconditions ]
	if( !CheckPreconditions( src, dst ))
		{
		//[ precondition violation ]
		User::Leave( KErrArgument );
		}
	
	TInt srcUse = src->Data().Length();
	
	//we need to cast away CONST even on the source, as the TClass needs a TUint8*	
	TUint8* pSrc = CONST_CAST(TUint8*,src->Data().Ptr());
	TUint8* pDst = CONST_CAST(TUint8*,dst->Data().Ptr());
	
	TInt noSamples = srcUse / 2; 	//divide by TWO as 2 bytes are equal to 1 sample
	iPcm16EndianSwap.Convert(pSrc, pDst, noSamples );
	
	result.iCodecProcessStatus  = TCodecProcessResult::EProcessComplete;
	result.iSrcBytesProcessed   = srcUse;
	result.iDstBytesAdded       = srcUse;
	
	dst->Data().SetLength(srcUse);
	
	//[ check post conditions ]
	__ASSERT_DEBUG( (src->Position() == 0), TMmfAudioCodecPanicsNameSpace::Panic( 	TMmfAudioCodecPanicsNameSpace::EPostConditionViolation ));
	__ASSERT_DEBUG( (dst->Position() == 0), TMmfAudioCodecPanicsNameSpace::Panic( 	TMmfAudioCodecPanicsNameSpace::EPostConditionViolation ));
	__ASSERT_DEBUG( src->Data().Length() == dst->Data().Length(), TMmfAudioCodecPanicsNameSpace::Panic( 	TMmfAudioCodecPanicsNameSpace::EPostConditionViolation )); 
	__ASSERT_DEBUG( dst->Data().Length() % 2 == 0, TMmfAudioCodecPanicsNameSpace::Panic( 	TMmfAudioCodecPanicsNameSpace::EPostConditionViolation )); // pcm output
	
	return result;
	}

/**
*
* Preconditions
* This methos tests the preconditions of the ProcessL method
* @return TBool ETrue for sucess and EFalse for failure of the preconditions
*
**/
TBool CMMFPcm16SwapEndianCodec::CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer )
	{
	TBool result = EFalse;
	
	if(! aSrcBuffer )
		{
		return result;
		}
	
	if( ! aDestBuffer )
		{
		return result;
		}
	
	// Check position of src and dest are 0
	if( aSrcBuffer->Position() )
		{
		return result;
		}
	
	// Check position of src and dest are 0
	if( aDestBuffer->Position() )
		{
		return result;
		}
	
	// check there are sufficient bytes in the output to consume the input
	if( ( aSrcBuffer->Data().Length() > aDestBuffer->Data().MaxLength() ) ||
		( aSrcBuffer->Data().Length() % 2 != 0 ))
		{
		return result;
		}
	
	result = ETrue;  // preconditions have been satisfied
	
	return result;
	}
