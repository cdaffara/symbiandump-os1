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

#include "mmfpcm16ToImaAdpcm.h"

/**
*
* NewL
*
*/
CMMFPcm16ToImaAdpcmHwDevice* CMMFPcm16ToImaAdpcmHwDevice::NewL()
	{
	CMMFPcm16ToImaAdpcmHwDevice* self=new(ELeave) CMMFPcm16ToImaAdpcmHwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*
* ~CMMFPcm16ToAlawHwDevice
*
*/
CMMFPcm16ToImaAdpcmHwDevice::~CMMFPcm16ToImaAdpcmHwDevice()
	{
	}

/**
*
* ConstructL
*
*/
void CMMFPcm16ToImaAdpcmHwDevice::ConstructL()
	{
	iCodec = new (ELeave) CMMFPcm16ToImaAdpcmCodec();
	}

/**
*
* Codec
*
*/
CMMFSwCodec &CMMFPcm16ToImaAdpcmHwDevice::Codec()
	{
	return *iCodec;
	}

/**
*
* ResetL
*
*/
void CMMFPcm16ToImaAdpcmCodec::ResetL()
	{
	//Reset the actual codec
	TMMFImaAdpcmCodecState state;
	state.iIndex = 0;
	state.iPredicted = 0;
	iPcm16ToImaAdpcm.SetState(state);
	}
	
/**
*
* ProcessL
* @param aSrc src buffer
* @param aDst destination buffer
* @return CMMFSwCodec::TCodecProcessResult
*  This function converts PCM samples to IMA ADPCM samples in
*  blocks of KImaAdpcmBlockAlign (256) bytes. 1010 source 
*  bytes are required to fill a 256 byte block.
* @pre if last buffer and src contains < 1010 bytes discard input
* This function throws away the last buffer if it contains < 1010 bytes
* (ie we must have sufficient data to process an entire frame )
* All other src buffers must contain
* All destination buffer must contain
*
**/
CMMFSwCodec::TCodecProcessResult CMMFPcm16ToImaAdpcmCodec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	CMMFSwCodec::TCodecProcessResult result;
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	
	//convert from generic CMMFBuffer to CMMFDataBuffer
	const CMMFDataBuffer* source = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	CMMFDataBuffer* destination  = STATIC_CAST(CMMFDataBuffer*, &aDst);
	
	//[ source and destination must not be null ]
	if( !source || !destination )
		User::Leave( KErrArgument );
	
	//[ check preconditions ]
	if( !BuffersStatus( source, destination ))
		{
		User::Leave( KErrArgument );
		}
	
	//[ code the buffers ]
	ProcessBuffers( *source, *destination, result );
	
	return result;	
	}

/**
*
* ProcessBuffers
* @param aSource
* @param aDestination
* @param aResult 
* all we have to do is find out how many source frames there 
* are to process and process them
* finally returning process complete and fillin the status of the result
*
**/
void CMMFPcm16ToImaAdpcmCodec::ProcessBuffers(const CMMFDataBuffer& aSource, CMMFDataBuffer& aDestination, CMMFSwCodec::TCodecProcessResult& aResult )
	{
	//[ calculate how many full buffers are to be processed ]
    const TUint srcLen    = aSource.Data().Length();
    TInt numFullSrcFrames = srcLen/KSourceFrameSize;
    
	TUint8* pSrc = const_cast<TUint8*>(aSource.Data().Ptr());
	TUint8* pDst = const_cast<TUint8*>(aDestination.Data().Ptr());
    TInt dstBytesAdded = 0;
	// calculate number of pcm samples per source frame
	const TInt KSamplesPerFrame = KSourceFrameSize/(sizeof(TInt16)); 
	
	//[ convert all the buffers ]
	for( TInt count = 0; count < numFullSrcFrames; count++ )
		{
		i16PcmToImaAdpcm.Convert(pSrc, pDst, KSamplesPerFrame );				
		pSrc          += KSourceFrameSize;
		pDst          += KCodedFrameSize;
		dstBytesAdded += KCodedFrameSize;
		}
	aResult.iSrcBytesProcessed = numFullSrcFrames*KSourceFrameSize;
	aResult.iDstBytesAdded = dstBytesAdded;		
	aDestination.Data().SetLength( aResult.iDstBytesAdded);
	}

/**
*
* BuffersStatus
* @param source buffer containing the data to be coded
* @param destination buffer containing the coded data
* @return TBool EFalse indicates bad buffers
*
**/
TBool CMMFPcm16ToImaAdpcmCodec::BuffersStatus( const CMMFDataBuffer* source, const CMMFDataBuffer* destination )
	{
	TBool status = EFalse;
	
	//[ demand source and destination positions are zero ]
    CMMFDataBuffer* pDst = const_cast<CMMFDataBuffer*>( destination );
	if( source->Position() || destination->Position() )
		{
		return status;
		}
	
	//[ Have we got full buffers ]
	TInt sourceBuffers = source->Data().Length()/KSourceFrameSize;
	TInt destBuffers   = (pDst->Data().MaxLength())/KCodedFrameSize;
	
	if( sourceBuffers <= destBuffers )  // the sink can process the source
		{
		return ETrue;                       // note this precondition has been weakened in line with other codecs
		}                                   // such that it can process partially full buffers
	                                        // ie you can if you wish use larger buffers than needed and only partially
	                                        // fill them. We do however expect all the input to be processed.
	return status;
	}
