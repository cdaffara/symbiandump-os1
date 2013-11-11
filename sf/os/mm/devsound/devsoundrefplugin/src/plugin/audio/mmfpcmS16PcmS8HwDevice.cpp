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

#include "mmfpcmS16PcmS8HwDevice.h"

/**
 *
 *	Returns the created hw device for passing audio through audio.
 *  for the wins implementation this would always be pcm16 although
 *  this is effectively a null hw device that will pass any datatype through
 *	@return	"CMMFPcm16ToPcmU16BEHwDevice"
 *
 */
CMMFPcmS16ToPcmS8HwDevice* CMMFPcmS16ToPcmS8HwDevice::NewL()
	{
    CMMFPcmS16ToPcmS8HwDevice* self = new (ELeave) CMMFPcmS16ToPcmS8HwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*
* ~CMMFPcmS16ToPcmS8HwDevice
*/
CMMFPcmS16ToPcmS8HwDevice::~CMMFPcmS16ToPcmS8HwDevice()
	{
	}

/**
*
* ConstructL
*
*/
void CMMFPcmS16ToPcmS8HwDevice::ConstructL()
	{
	iCodec = new (ELeave) CMMFPcmS16ToPcmS8Codec();
	}

/**
*
* Codec
* @return 'CMMFSwCodec&'
*
*/
CMMFSwCodec& CMMFPcmS16ToPcmS8HwDevice::Codec()
	{
	return *iCodec;
	}

/**
*
* ProcessL
* @param aSrc
* @param aDst
* @param aSamples
*
*/
CMMFSwCodec::TCodecProcessResult CMMFPcmS16ToPcmS8Codec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	TCodecProcessResult result;
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	
	//convert from generic CMMFBuffer to CMMFDataBuffer
	const CMMFDataBuffer* src = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	CMMFDataBuffer* dst = STATIC_CAST(CMMFDataBuffer*, &aDst);
	
	//[Check Preconditions]
	if( !CheckPreconditions( src, dst ))
		{
		//[ precondition violation ]
		User::Leave( KErrArgument );
		}
	
	//we need to cast away CONST even on the source, as the TClass needs a TUint8*
	TUint8* pSrc = CONST_CAST(TUint8*,src->Data().Ptr());
	TUint8* pDst = CONST_CAST(TUint8*,dst->Data().Ptr());
    
	TInt srcUse = src->Data().Length();
	TInt noSamples = srcUse/2;
	
	//compress TWO bytes (16-bit PCM word) into a to 1 byte sample
	iAudioPcmS16ToPcmS8.Convert(pSrc, pDst, noSamples );
	
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	result.iSrcBytesProcessed  = srcUse;
	result.iDstBytesAdded      = srcUse/2;
	
	dst->Data().SetLength(result.iDstBytesAdded);
	
	//[ post conditions
    // srcbytes/2 == destbytes added
	// pos src == 0
	// pos dest == 0 ]
	__ASSERT_DEBUG( (src->Position() == 0), TMmfAudioCodecPanicsNameSpace::Panic( 	TMmfAudioCodecPanicsNameSpace::EPostConditionViolation ));
	__ASSERT_DEBUG( (dst->Position() == 0), TMmfAudioCodecPanicsNameSpace::Panic( 	TMmfAudioCodecPanicsNameSpace::EPostConditionViolation ));
	__ASSERT_DEBUG( src->Data().Length()/2 == dst->Data().Length(), TMmfAudioCodecPanicsNameSpace::Panic( 	TMmfAudioCodecPanicsNameSpace::EPostConditionViolation ));
	
	return result;
	}

/**
*
* Preconditions
* This methos tests the preconditions of the ProcessL method
* @return TBool ETrue for sucess and EFalse for failure of the preconditions
*
**/
TBool CMMFPcmS16ToPcmS8Codec::CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer )
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
	if( ( aSrcBuffer->Data().Length()/2 > aDestBuffer->Data().MaxLength() ) &&
		( aSrcBuffer->Data().Length() % 2 == 0 ) ) // must have pcm16 samples on input
		{
		return result;
		}
	
	result = ETrue;  // preconditions have been satisfied
	
	return result;
	}

