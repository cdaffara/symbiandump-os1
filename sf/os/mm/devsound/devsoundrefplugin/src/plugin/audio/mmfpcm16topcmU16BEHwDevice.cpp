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

#include "mmfpcm16topcmU16BEHwDevice.h"

/**
 *
 *	Returns the created hw device for passing audio through audio.
 *  for the wins implementation this would always be pcm16 although
 *  this is effectively a null hw device that will pass any datatype through
 *	@return	"CMMFPcm16ToPcmU16BEHwDevice"
 *
 */
CMMFPcm16ToPcmU16BEHwDevice* CMMFPcm16ToPcmU16BEHwDevice::NewL()
	{
    CMMFPcm16ToPcmU16BEHwDevice* self = new (ELeave) CMMFPcm16ToPcmU16BEHwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*
* ~CMMFPcm16ToPcmU16BEHwDevice
*/
CMMFPcm16ToPcmU16BEHwDevice::~CMMFPcm16ToPcmU16BEHwDevice()
	{
	}

/**
*
* ConstructL
*
*/
void CMMFPcm16ToPcmU16BEHwDevice::ConstructL()
	{
	iCodec = new (ELeave) CMMFPcm16ToPcmU16BECodec();
	}

/**
*
* Codec
* @return 'CMMFSwCodec&'
*/
CMMFSwCodec& CMMFPcm16ToPcmU16BEHwDevice::Codec()
	{
	return *iCodec;
	}

/**
*
* ProcessL
* @param aSrc 
* @param aDst
* @pre position of buffer aSrc is 0
* @pre position of buffer aDst is 0
* @pre sufficient bytes in output to consume input
* @return 'CMMFSwCodec::TCodecProcessResult'
* this codec has a does not expand or shrink destination data
*/
CMMFSwCodec::TCodecProcessResult CMMFPcm16ToPcmU16BECodec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	TCodecProcessResult result;
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	
	//convert from generic CMMFBuffer to CMMFDataBuffer
	const CMMFDataBuffer* src = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	CMMFDataBuffer* dst = STATIC_CAST(CMMFDataBuffer*, &aDst);
	
	//[ Check preconsitions ]
	if( !CheckPreconditions( src, dst ))
		{
		//[ precondition violation ]
		User::Leave( KErrArgument );
		}
	
	//we need to cast away CONST even on the source, as the TClass needs a TUint8*	
	TUint8* pSrc = CONST_CAST(TUint8*,src->Data().Ptr());
	TUint8* pDst = CONST_CAST(TUint8*,dst->Data().Ptr());
    
	TInt srcUse = src->Data().Length();
	
	TInt noSamples = srcUse/2; // for pcm16 TInt16
	iAudioPcm16ToPcmU16Be.Convert(pSrc, pDst, noSamples );
	
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	result.iSrcBytesProcessed  = srcUse;
	result.iDstBytesAdded      = srcUse;
	
	dst->Data().SetLength(srcUse);
	
	//[ Check post conditions ]
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
TBool CMMFPcm16ToPcmU16BECodec::CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer )
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
		 ( aSrcBuffer->Data().Length() % 2 != 0 ) ) // must have pcm16 samples on input
		 {
		 return result;
		 }
	 
	 result = ETrue;  // preconditions have been satisfied
	 
	 return result;
	}

