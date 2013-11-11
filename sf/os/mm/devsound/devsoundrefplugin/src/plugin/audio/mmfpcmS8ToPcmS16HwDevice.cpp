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


#include "mmfpcmS8ToPcmS16HwDevice.h"

/**
 *
 *	Returns the created hw device for passing audio through audio.
 *  for the wins implementation this would always be pcm16 although
 *  this is effectively a null hw device that will pass any datatype through
 *	@return	"CMMFPcm16ToPcmU16BEHwDevice"
 *
 */
CMMFPcm8ToPcm16HwDevice* CMMFPcm8ToPcm16HwDevice::NewL()
	{
    CMMFPcm8ToPcm16HwDevice* self = new (ELeave) CMMFPcm8ToPcm16HwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*
* ~CMMFPcm8ToPcm16HwDevice
*/
CMMFPcm8ToPcm16HwDevice::~CMMFPcm8ToPcm16HwDevice()
	{
	}

/**
*
* ConstructL
*
*/
void CMMFPcm8ToPcm16HwDevice::ConstructL()
	{
	iCodec = new (ELeave) CMMFPcm8ToPcm16Codec();
	}

/**
*
* Codec
* @return 'CMMFSwCodec&'
*
*/
CMMFSwCodec& CMMFPcm8ToPcm16HwDevice::Codec()
	{
	return *iCodec;
	}

/**
*
* ProcessL
* @param aSrc
* @param aDst
* @return 'CMMFSwCodec::TCodecProcessResult'
* this codec expands 1 byte to 2 bytes
*/
CMMFSwCodec::TCodecProcessResult CMMFPcm8ToPcm16Codec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	TCodecProcessResult result;
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	
	//convert from generic CMMFBuffer to CMMFDataBuffer
	const CMMFDataBuffer* src = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	CMMFDataBuffer* dst = STATIC_CAST(CMMFDataBuffer*, &aDst);
	
	//[ check preconditions ]
	if( !CheckPreconditions( src, dst ))
		{
		//[ precondition violation ]
		User::Leave( KErrArgument );
		}
	
	//we need to cast away CONST even on the source, as the TClass needs a TUint8*
	TUint8* pSrc = CONST_CAST(TUint8*,src->Data().Ptr());
	TUint8* pDst = CONST_CAST(TUint8*,dst->Data().Ptr());
	
	TInt noSamples = src->Data().Length();
	iAudioPcm8ToPcm16.Convert(pSrc, pDst, noSamples );
	
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	
	TInt dstBytesAdded         = noSamples *sizeof(TInt16);
	result.iSrcBytesProcessed = src->Data().Length();
	result.iDstBytesAdded     = dstBytesAdded;
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
TBool CMMFPcm8ToPcm16Codec::CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer )
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



