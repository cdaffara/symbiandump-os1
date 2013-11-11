// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// * INCLUDE FILES:
// 
//

// Standard includes
#include <e32std.h>
#include <mmfbtswcodecwrapper.h>

#include <ecom/implementationproxy.h>
#include "BtGsm610.h"
#include "gsm610fr.h"
#include <ecom/ecom.h>
#include <e32def.h>
#include <mmfbthwdeviceimplementationuids.hrh>
#include "../../../MmfBtFileDependencyUtil.h"

/**
*
* NewL
* @return CMmfGsm610ToPcm16HwDevice*
*
*/
CMmfGsm610ToPcm16HwDevice* CMmfGsm610ToPcm16HwDevice::NewL()
	{
	CMmfGsm610ToPcm16HwDevice* self=new(ELeave) CMmfGsm610ToPcm16HwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*
* Codec
*
*/
CMMFSwCodec& CMmfGsm610ToPcm16HwDevice::Codec()
	{						  
	return *iCodec;
	}

/**
*
* CMmfGsm610ToPcm16HwDevice
*
*/
CMmfGsm610ToPcm16HwDevice::~CMmfGsm610ToPcm16HwDevice()
	{
	}


/**
*
* ConstructL
*
*/
void CMmfGsm610ToPcm16HwDevice::ConstructL()
	{
	CMMFGsm610ToPcm16Codec* ptr= new(ELeave)CMMFGsm610ToPcm16Codec();
	CleanupStack::PushL(ptr); 
	ptr->ConstructL();
	iCodec = ptr;
	CleanupStack::Pop(ptr);
	}

/**
*
* CMmfPcm16ToGsm610HwDevice
*
*/
CMmfPcm16ToGsm610HwDevice* CMmfPcm16ToGsm610HwDevice::NewL()
	{
	CMmfPcm16ToGsm610HwDevice* self=new(ELeave) CMmfPcm16ToGsm610HwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*
* Codec
* @return CMMFSwCodec&
*
*/
CMMFSwCodec& CMmfPcm16ToGsm610HwDevice::Codec()
	{
	return *iCodec;
	}

/**
*
* ~CMmfPcm16ToGsm610HwDevice
*
*/
CMmfPcm16ToGsm610HwDevice::~CMmfPcm16ToGsm610HwDevice()
	{
	}

/**
*
* ConstructL
*
*/
void CMmfPcm16ToGsm610HwDevice::ConstructL()
	{
	CMMFPcm16ToGsm610Codec* ptr =new(ELeave) CMMFPcm16ToGsm610Codec();
	CleanupStack::PushL(ptr); 
	ptr->ConstructL();
	iCodec = ptr;
	CleanupStack::Pop(ptr);
	}

/**
*
* CMMFGsm610ToPcm16Codec
*
*/
CMMFGsm610ToPcm16Codec::CMMFGsm610ToPcm16Codec()
	{	
	}

/**
*
* ConstructL
*
*/
void CMMFGsm610ToPcm16Codec::ConstructL()
	{
    iGsmDecoder = new (ELeave) CGSM610FR_Decoder;
	iGsmDecoder->ConstructL();
	iGsmDecoder->StartL();
	}

/**
*
* NewL
*
*/
CMMFGsm610ToPcm16Codec*  CMMFGsm610ToPcm16Codec::NewL()
	{
	CMMFGsm610ToPcm16Codec* self=new(ELeave) CMMFGsm610ToPcm16Codec();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*
* ~CMMFGsm610ToPcm16Codec
*
*/
CMMFGsm610ToPcm16Codec::~CMMFGsm610ToPcm16Codec()
	{
	delete iGsmDecoder;
	}

/**
*
* ProcessL
* @param aSource
* @param aDest
* @pre input buffer length is mod 65
* @pre output buffer has sufficient space for coded input
*
*/
CMMFSwCodec::TCodecProcessResult CMMFGsm610ToPcm16Codec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDest)
	{
	CMMFSwCodec::TCodecProcessResult result;
	result.iCodecProcessStatus =	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;

	//convert from generic CMMFBuffer to CMMFDataBuffer
	CMMFBuffer* pSrcBuffer =const_cast<CMMFBuffer*>(&aSrc);
	if( !pSrcBuffer )
		{
		User::Leave( KErrArgument );
		}

	CMMFDataBuffer* src = static_cast<CMMFDataBuffer*>( pSrcBuffer );
    if( !src )
		{
		User::Leave( KErrArgument );
		}

	CMMFDataBuffer* dst = static_cast<CMMFDataBuffer*>(&aDest);
	if( !dst )
		{
		User::Leave( KErrArgument );
		}

	if(!CheckInputBuffers( *src, *dst ))
		{
		User::Leave( KErrArgument );
		}

	TInt numBuffersToProcess = NumBuffersToProcess( *src );
	TUint8* pSrc = CONST_CAST(TUint8*,src->Data().Ptr());
	TUint8* pDst = CONST_CAST(TUint8*,dst->Data().Ptr());
	
	for( TInt count = 0; count < numBuffersToProcess; count++ )
		{
		// Encode two frames of gsm data
		iGsmDecoder->ExecuteL( pSrc, pDst );
		pSrc                      += KGsmFrameSize;;
		pDst                      += KPcmDataForGsmFrame;
		result.iSrcBytesProcessed += KGsmFrameSize;
		result.iDstBytesAdded     += KPcmDataForGsmFrame;
		}

    dst->Data().SetLength( result.iDstBytesAdded );
	__ASSERT_DEBUG( ProcessPostCondition( result ), TMmfGsmCodecPanicsNameSpace::Panic( TMmfGsmCodecPanicsNameSpace::EPostConditionViolation ));
	return result;
	}

/**
*
* CheckInputBuffers
* @param aSrc
* @param aDest
* @return TBool
* This function returns ETrue if the preconditions of processL are met
*
*/
TBool CMMFGsm610ToPcm16Codec::CheckInputBuffers( CMMFDataBuffer& aSrc, CMMFDataBuffer& aDest )
	{
	TBool result = ETrue;    
    TInt numInputSubFrames     = aSrc.Data().Length() / KGsmFrameSize;
	TInt numOutputSubFrames    = aDest.Data().MaxLength() / KPcmDataForGsmFrame;
	TBool validInputDataLength = (aSrc.Data().Length() % KGsmFrameSize == 0);
    
	if( (numInputSubFrames > numOutputSubFrames) ||  // sufficient space in the output for the input
        (aSrc.Position() > 0 )  ||                   // position must be zero since we can eat all the data
		(aDest.Position() > 0 ) ||
		(!validInputDataLength))                         //position must be zero
		{
		result = EFalse;
		}

	return result;
	}

/**
*
* NumBuffersToProcess
* @param aSrc
* @return TBool
* This method returns the number of buffers to process
*
*/
TInt CMMFGsm610ToPcm16Codec::NumBuffersToProcess( const CMMFDataBuffer& aSrc )
	{
    TInt numBuffers = (aSrc.Data().Length() / KGsmFrameSize );
	return numBuffers;
	}

/**
*
* ProcessPostCondition
* @param aResult
* @return TBool Etrue if the post condition is satisfied
*
**/
TBool CMMFGsm610ToPcm16Codec::ProcessPostCondition( const CMMFSwCodec::TCodecProcessResult& aResult )
	{
     TBool status = ETrue;
	 if( (aResult.iSrcBytesProcessed / KGsmFrameSize ) != (aResult.iDstBytesAdded / KPcmDataForGsmFrame ) )
		 {
		 status = EFalse;
		 }
	 return status;
	}

/************************>----------------------------------<*****************************/

/**
*
* CMMFPcm16ToGsm610Codec
*
*/
CMMFPcm16ToGsm610Codec::CMMFPcm16ToGsm610Codec()
	{	
	}

/**
*
* ConstructL
*
*/
void CMMFPcm16ToGsm610Codec::ConstructL()
	{
	iGsmEncoder = new (ELeave) CGSM610FR_Encoder;
	iGsmEncoder->ConstructL();
	iGsmEncoder->StartL();
	}

/**
*
* NewL
*
*/
CMMFPcm16ToGsm610Codec* CMMFPcm16ToGsm610Codec::NewL()
	{
	CMMFPcm16ToGsm610Codec* self=new(ELeave) CMMFPcm16ToGsm610Codec();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*
* CMMFPcm16ToGsm610Codec
*
*/
CMMFPcm16ToGsm610Codec::~CMMFPcm16ToGsm610Codec()
	{
	delete iGsmEncoder;
	}

/**
*
* ProcessL
* @param aSource
* @param aDest TCodecProcessResult
* @return
* @pre input buffer length is mod 320
* @pre output buffer has sufficient space for coded input
*/
CMMFSwCodec::TCodecProcessResult CMMFPcm16ToGsm610Codec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDest)
	{
	CMMFSwCodec::TCodecProcessResult result;
	result.iCodecProcessStatus = 	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;

	//convert from generic CMMFBuffer to CMMFDataBuffer
	CMMFBuffer* pSrcBuffer =const_cast<CMMFBuffer*>(&aSrc);
	if( !pSrcBuffer )
		{
		User::Leave( KErrArgument );
		}

	CMMFDataBuffer* src = static_cast<CMMFDataBuffer*>( pSrcBuffer );
    if( !src )
		{
		User::Leave( KErrArgument );
		}

	CMMFDataBuffer* dst = static_cast<CMMFDataBuffer*>(&aDest);
	if( !dst )
		{
		User::Leave( KErrArgument );
		}


	if(!CheckInputBuffers( *src, *dst ))
		{
		User::Leave( KErrArgument );
		}

	TInt numBuffersToProcess = NumBuffersToProcess( *src );
	TUint8* pSrc = CONST_CAST(TUint8*,src->Data().Ptr());
	TUint8* pDst = CONST_CAST(TUint8*,dst->Data().Ptr());
	
	for( TInt count = 0; count < numBuffersToProcess; count++ )
		{
		// Encode two frames of gsm data
		iGsmEncoder->ExecuteL (pSrc, pDst);
		pSrc                      += KPcmDataForGsmFrame;
		pDst                      += KGsmFrameSize;
		result.iSrcBytesProcessed += KPcmDataForGsmFrame;
		result.iDstBytesAdded     += KGsmFrameSize;
		}

    dst->Data().SetLength( result.iDstBytesAdded ); 

	__ASSERT_DEBUG( ProcessPostCondition(result), TMmfGsmCodecPanicsNameSpace::Panic( TMmfGsmCodecPanicsNameSpace::EPostConditionViolation ));

    return result ;
	}

/**
*
* CheckInputBuffers
* @param aSrc
* @param aDest
* @return TBool
* This function returns ETrue if there is sufficient space
* in the output buffer for the coded input and
* the position of both input buffers is zero
*
*/
TBool CMMFPcm16ToGsm610Codec::CheckInputBuffers( CMMFDataBuffer& aSrc, CMMFDataBuffer& aDest )
	{
	TBool result = ETrue;    
    TInt numInputSubFrames     = aSrc.Data().Length() / KPcmDataForGsmFrame;
	TInt numOutputSubFrames    = aDest.Data().MaxLength() / KGsmFrameSize;
	TBool validInputDataLength = (aSrc.Data().Length() % KPcmDataForGsmFrame == 0);
    
	if( (numInputSubFrames > numOutputSubFrames) ||  // sufficient space in the output for the input
        (aSrc.Position() > 0 )  ||                   // position must be zero since we can eat all the data
		(aDest.Position() > 0 ) ||
		(!validInputDataLength))                         //position must be zero
		{
		result = EFalse;
		}

	return result;
	}

/**
*
* NumBuffersToProcess
* @param aSrc
* @return TBool
* This method returns the number of buffers to process
*
*/
TInt CMMFPcm16ToGsm610Codec::NumBuffersToProcess( const CMMFDataBuffer& aSrc )
	{
	TInt numBuffers = ( aSrc.Data().Length() / KPcmDataForGsmFrame );
	return numBuffers;
	}

