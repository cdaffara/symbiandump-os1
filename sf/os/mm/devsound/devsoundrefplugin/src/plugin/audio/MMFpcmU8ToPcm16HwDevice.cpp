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

#include "MMFpcmU8ToPcm16HwDevice.h"

/**
 *
 *	Returns the created hw device for unsigned pcm8 to signed pcm16 audio.
 *
 *	@return	"CMMFPcmU8ToPcm16HwDevice"
 *
 */
CMMFPcmU8ToPcm16HwDevice* CMMFPcmU8ToPcm16HwDevice::NewL()
	{
    CMMFPcmU8ToPcm16HwDevice* self = new (ELeave) CMMFPcmU8ToPcm16HwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
 *
 *	Second phase constructor.
 *
 */
void CMMFPcmU8ToPcm16HwDevice::ConstructL()
	{
    iCodec = new (ELeave) CMMFPcmU8ToPcm16Codec();
	}

/**
*
* ~CMMFPcmU8ToPcm16HwDevice
*
*/
CMMFPcmU8ToPcm16HwDevice::~CMMFPcmU8ToPcm16HwDevice()
	{
	//The codec is deleted in the base class CMMFSwCodecWrapper
	}

/**
*
* Codec
* @return 'CMMFSwCodec&'
*
*/
CMMFSwCodec& CMMFPcmU8ToPcm16HwDevice::Codec()
	{
	return *iCodec;
	}

/**
*
* ProcessL
* @param aSource
* @param aDst 
* @return 'CMMFSwCodec::TCodecProcessResult'
*
*/
CMMFSwCodec::TCodecProcessResult CMMFPcmU8ToPcm16Codec::ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest)
	{
	TCodecProcessResult result;
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	
	//convert from generic CMMFBuffer to CMMFDataBuffer
	const CMMFDataBuffer* src = STATIC_CAST(const CMMFDataBuffer*, &aSource);
	CMMFDataBuffer* dst = STATIC_CAST(CMMFDataBuffer*, &aDest);
	
	//[ check preconditions ]
	if( !CheckPreconditions( src, dst ))
		{
		//[ precondition violation ]
		User::Leave( KErrArgument );
		}
	
	//we need to cast away CONST even on the source, as the TClass needs a TUint8*
	TUint8* pSrc = CONST_CAST(TUint8*,src->Data().Ptr());
	TUint8* pDst = CONST_CAST(TUint8*,dst->Data().Ptr());
	
	TInt srcUse = src->Data().Length();
	TInt noSamples = srcUse;
	iAudioU8ToS16Pcm.Convert(pSrc, pDst, noSamples );
	
	TInt dstBytesAdded = srcUse * 2;
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	result.iSrcBytesProcessed  = srcUse;
	result.iDstBytesAdded      = dstBytesAdded;
	
	dst->Data().SetLength(dstBytesAdded);
	
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
TBool CMMFPcmU8ToPcm16Codec::CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer )
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
	if( aSrcBuffer->Data().Length() * 2 > aDestBuffer->Data().MaxLength() )
		{
		return result;
		}
	
	result = ETrue;  // preconditions have been satisfied
	
	return result;
	}
