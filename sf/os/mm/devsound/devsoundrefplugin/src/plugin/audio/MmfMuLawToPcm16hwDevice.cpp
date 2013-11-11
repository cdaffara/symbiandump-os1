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

#include "MMFAudioCodec.h"
#include <mmf/common/mmfpaniccodes.h>
#include "MmfMuLawToPcm16hwDevice.h"

/**
*
* NewL
*
*/
CMMFMulawToPcm16CodecHwDevice* CMMFMulawToPcm16CodecHwDevice::NewL()
	{
	CMMFMulawToPcm16CodecHwDevice* self=new(ELeave) CMMFMulawToPcm16CodecHwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*
* ~CMMFMulawPcm16HwDevice
*
*/
CMMFMulawToPcm16CodecHwDevice::~CMMFMulawToPcm16CodecHwDevice()
	{
	}

/**
*
* ConstructL
*
*/
void CMMFMulawToPcm16CodecHwDevice::ConstructL()
	{
	iCodec = new (ELeave) CMMFMulawToPcm16Codec();
	}

/**
*
* Codec
*
*/
CMMFSwCodec &CMMFMulawToPcm16CodecHwDevice::Codec()
	{
	return *iCodec;
	}

/**
*
* ProcessL
* @param aSrc
* @param aDst
* @return TCodecProcessResult
*
*/
CMMFSwCodec::TCodecProcessResult CMMFMulawToPcm16Codec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	CMMFSwCodec::TCodecProcessResult result;
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	
	//convert from generic CMMFBuffer to CMMFDataBuffer
	const CMMFDataBuffer* src = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	CMMFDataBuffer* dst = STATIC_CAST(CMMFDataBuffer*, &aDst);
	
	//[check preconditions hold ]
	if( !CheckPreconditions( src, dst ))
		{
		//[ precondition violation ]
		User::Leave( KErrArgument );
		}
	
	//we need to cast away CONST even on the source, as the TClass needs a TUint8*
	TUint8* pSrc = CONST_CAST(TUint8*,src->Data().Ptr());
	TUint8* pDst = CONST_CAST(TUint8*,dst->Data().Ptr());
	TInt noSamples = src->Data().Length();
	
	iMulawTo16Pcm.Convert(pSrc, pDst, noSamples );
	
	TUint dstBytesAdded = noSamples*sizeof(TInt16);
	
	result.iCodecProcessStatus  = TCodecProcessResult::EProcessComplete;
	result.iSrcBytesProcessed   = src->Data().Length();
	result.iDstBytesAdded       = dstBytesAdded;
	
	dst->Data().SetLength( dstBytesAdded );
	
	//[ check post conditions ]
	__ASSERT_DEBUG( (src->Position() == 0), TMmfAudioCodecPanicsNameSpace::Panic( 	TMmfAudioCodecPanicsNameSpace::EPostConditionViolation ));
	__ASSERT_DEBUG( (dst->Position() == 0), TMmfAudioCodecPanicsNameSpace::Panic( 	TMmfAudioCodecPanicsNameSpace::EPostConditionViolation ));
	__ASSERT_DEBUG( src->Data().Length() == dst->Data().Length()/2, TMmfAudioCodecPanicsNameSpace::Panic( 	TMmfAudioCodecPanicsNameSpace::EPostConditionViolation )); // 2 here signifies sizeof TInt16
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
TBool CMMFMulawToPcm16Codec::CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer )
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
	if( aSrcBuffer->Data().Length() > aDestBuffer->Data().MaxLength()/2 )
		{
		return result;
		}
	
	result = ETrue;  // preconditions have been satisfied
	
	return result;
	}
