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

#include "MmfBtPcm16ToMulawHwDevice.h"
#include "../../MmfBtFileDependencyUtil.h"

/**
*
* NewL
*
*/
CMMFPcm16ToMulawHwDevice* CMMFPcm16ToMulawHwDevice::NewL()
	{
	CMMFPcm16ToMulawHwDevice* self=new(ELeave) CMMFPcm16ToMulawHwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*
* ~CMMFPcm16ToMulawHwDevice
*
*/
CMMFPcm16ToMulawHwDevice::~CMMFPcm16ToMulawHwDevice()
	{
	}

/**
*
* ConstructL
*
*/
void CMMFPcm16ToMulawHwDevice::ConstructL()
	{
	iCodec = new (ELeave) CMMFPcm16ToMuLawCodec();
	}

/**
*
* Codec
*
*/
CMMFSwCodec &CMMFPcm16ToMulawHwDevice::Codec()
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
CMMFSwCodec::TCodecProcessResult CMMFPcm16ToMuLawCodec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	CMMFSwCodec::TCodecProcessResult result;
	result.iCodecProcessStatus = result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	
	//convert from generic CMMFBuffer to CMMFDataBuffer
	const CMMFDataBuffer* src = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	CMMFDataBuffer* dst = STATIC_CAST(CMMFDataBuffer*, &aDst);
	
	if( !CheckPreconditions( src, dst ) )
		{
		//[ precondition(s) violation ]
		User::Leave(KErrArgument);
		}
	
	TUint8* pSrc = CONST_CAST(TUint8*,src->Data().Ptr());
	TUint8* pDst = CONST_CAST(TUint8*,dst->Data().Ptr());
	
	TUint destUse = src->Data().Length()/2;
	
	//compress TWO bytes (16-bit PCM word) into a to 1 byte sample
	iPcm16ToMuLaw.Convert(pSrc, pDst, destUse );
	
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	result.iSrcBytesProcessed  = src->Data().Length();
	result.iDstBytesAdded      = destUse;
	
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
* This method tests the preconditions of the ProcessL method
* @return TBool ETrue for sucess and EFalse for failure of the preconditions
*
**/
TBool CMMFPcm16ToMuLawCodec::CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer )
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
	if( ( aSrcBuffer->Data().Length()/2 > aDestBuffer->Data().MaxLength()) ||
		( aSrcBuffer->Data().Length() % 2 != 0 ))
		{
		return result;
		}
	
	result = ETrue;  // preconditions have been satisfied
	
	return result;
	}
