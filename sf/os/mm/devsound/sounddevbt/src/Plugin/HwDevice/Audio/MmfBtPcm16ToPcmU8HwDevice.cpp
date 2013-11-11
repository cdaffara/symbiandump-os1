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
// MmfPcm16ToPcmU8HwDevice.cpp
// 
//

#include "MmfBtPcm16ToPcmU8HwDevice.h"
#include "../../MmfBtFileDependencyUtil.h"

/**
 *
 *	Returns the created hw device for signed pcm16 to unsigned pcm8 audio.
 *
 *	@return	"CMMFPcm16ToPcmU8HwDevice"
 *
 */
CMMFPcm16ToPcmU8HwDevice* CMMFPcm16ToPcmU8HwDevice::NewL()
	{
    CMMFPcm16ToPcmU8HwDevice* self = new (ELeave) CMMFPcm16ToPcmU8HwDevice();
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
void CMMFPcm16ToPcmU8HwDevice::ConstructL()
	{
    iCodec = new (ELeave) CMMFPcm16ToPcmU8Codec();
	}

/**
*
* ~CMMFPcm16ToPcmU8HwDevice
*/
CMMFPcm16ToPcmU8HwDevice::~CMMFPcm16ToPcmU8HwDevice()
	{
	//The codec is deleted in the base class CMMFSwCodecWrapper
	}

/**
*
* Codec
* @return 'CMMFSwCodec&'
*
*/
CMMFSwCodec& CMMFPcm16ToPcmU8HwDevice::Codec()
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
CMMFSwCodec::TCodecProcessResult CMMFPcm16ToPcmU8Codec::ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest)
	{
	TCodecProcessResult result;
	result.iCodecProcessStatus = 	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	
	//convert from generic CMMFBuffer to CMMFDataBuffer
	const CMMFDataBuffer* src = STATIC_CAST(const CMMFDataBuffer*, &aSource);
	CMMFDataBuffer* dst = STATIC_CAST(CMMFDataBuffer*, &aDest);
	
	if( !CheckPreconditions( src, dst ))
		{
		//[ precondition violation ]
		User::Leave( KErrArgument );
		}
	
	//we need to cast away CONST even on the source, as the TClass needs a TUint8*
	TUint8* pSrc = CONST_CAST(TUint8*,src->Data().Ptr());
	TUint8* pDst = CONST_CAST(TUint8*,dst->Data().Ptr());
    
	TInt srcUse = src->Data().Length();
	
	TInt noSamples = srcUse /2;
	iAudioS16ToU8Pcm.Convert(pSrc, pDst, noSamples );
	
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	result.iSrcBytesProcessed  = srcUse;
	result.iDstBytesAdded      = srcUse / 2;
	
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
TBool CMMFPcm16ToPcmU8Codec::CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer )
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
	if( aSrcBuffer->Data().Length()/2 > aDestBuffer->Data().MaxLength() )
		{
		return result;
		}
	
	result = ETrue;  // preconditions have been satisfied
	
	return result;
	}
