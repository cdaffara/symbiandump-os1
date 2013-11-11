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
//

#include "MmfBtAudioCodec.h"
#include <mmfpaniccodes.h>
#include "MmfBtALawToPcm16HwDevice.h"
#include "../../MmfBtFileDependencyUtil.h"


/**
*
* NewL
*
*/
CMMFAlawToPcm16CodecHwDevice* CMMFAlawToPcm16CodecHwDevice::NewL()
	{
	CMMFAlawToPcm16CodecHwDevice* self=new(ELeave) CMMFAlawToPcm16CodecHwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*
* ~CMMFAlawPcm16HwDevice
*
*/
CMMFAlawToPcm16CodecHwDevice::~CMMFAlawToPcm16CodecHwDevice()
	{
	}

/**
*
* ConstructL
*
*/
void CMMFAlawToPcm16CodecHwDevice::ConstructL()
	{
	iCodec = new (ELeave) CMMFAlawToPcm16Codec();
	}

/**
*
* Codec
*
*/
CMMFSwCodec &CMMFAlawToPcm16CodecHwDevice::Codec()
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
* @return TCodecProcessResult
*
*/
CMMFSwCodec::TCodecProcessResult CMMFAlawToPcm16Codec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	CMMFSwCodec::TCodecProcessResult result;
	result.iCodecProcessStatus = 	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	
	//convert from generic CMMFBuffer to CMMFDataBuffer
	const CMMFDataBuffer* src = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	CMMFDataBuffer* dst = STATIC_CAST(CMMFDataBuffer*, &aDst);
	
	if( !CheckPreconditions( src, dst ))
		{
		//[ precondition violation ]
		User::Leave( KErrArgument );
		}
		
	//we need to cast away CONST even on the source, as the TClass needs a TUint8*
	TUint8* pSrc = CONST_CAST(TUint8*,src->Data().Ptr());
	TUint8* pDst = CONST_CAST(TUint8*,dst->Data().Ptr());
	TInt noSamples = src->Data().Length();
	iAlawTo16Pcm.Convert(pSrc, pDst, noSamples );
	
	TUint dstBytesAdded = noSamples*sizeof(TInt16);
	
	result.iCodecProcessStatus  = TCodecProcessResult::EProcessComplete;	
	result.iSrcBytesProcessed   = src->Data().Length();
	result.iDstBytesAdded       = dstBytesAdded;
	
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
TBool CMMFAlawToPcm16Codec::CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer )
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
