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
#include "MmfImaAdpcmtopcm16hwdevice.h"
#include <mmf/server/mmfswcodecwrappercustominterfacesuids.hrh>

/**
*
* NewL
*
*/
CMMFImaAdpcmToPcm16CodecHwDevice* CMMFImaAdpcmToPcm16CodecHwDevice::NewL()
	{
	CMMFImaAdpcmToPcm16CodecHwDevice* self=new(ELeave)CMMFImaAdpcmToPcm16CodecHwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*
* ConstructL
*
*/
void CMMFImaAdpcmToPcm16CodecHwDevice::ConstructL()
	{
	iCodec = new (ELeave) CMMFImaAdpcmToPcm16Codec();
	}

/**
*
* ~CMMFMulawPcm16HwDevice
*
*/
CMMFImaAdpcmToPcm16CodecHwDevice::~CMMFImaAdpcmToPcm16CodecHwDevice()
	{
	}

/**
*
* Codec
*
*/
CMMFSwCodec &CMMFImaAdpcmToPcm16CodecHwDevice::Codec()
	{
	return *iCodec;
	}

/**
Retrieves a custom interface to the device.
The reference CMMFImaAdpcmToPcm16CodecHwDevice supports one FileBlockLength custom interface,
or else calls two standard (CMMFSwCodecWrapper) custom interfaces,TPlayCustomInterface and TRecordCustomInterface.

@param	aInterface
		Interface UID, defined with the custom interface.
		aInterface = KUidCustomInterfaceDevSoundFileBlockLength,
		aInterface = KMmfPlayCustomInterface for TPlayCustomInterface,
		aInterface = KMmfRecordCustomInterface for TRecordCustomInterface.
@return A pointer to the interface implementation, The return value must be cast to the
		correct type by the user.
*/
TAny* CMMFImaAdpcmToPcm16CodecHwDevice::CustomInterface(TUid aInterfaceId)
	{
	if(aInterfaceId == KUidCustomInterfaceDevSoundFileBlockLength)
		{
		return static_cast<MMMFDevSoundCustomInterfaceFileBlockLength*>(this);
		}
	else
		{
		return CMMFSwCodecWrapper::CustomInterface(aInterfaceId);
		}
	}

/**
@see CMMFSwCodecWrapper::Start

this function sets SampleRate and Channels for CMMFImaAdpcmToPcm16Codec
*/
TInt CMMFImaAdpcmToPcm16CodecHwDevice::Start(TDeviceFunc aFuncCmd, TDeviceFlow aFlowCmd)
	{
	TInt err = static_cast<CMMFImaAdpcmToPcm16Codec*>(iCodec)->Configure(iChannels, iSampleRate, iBlockAlign);
	if (err == KErrNone)
		{
		err = CMMFSwCodecWrapper::Start(aFuncCmd, aFlowCmd);
		}

	return err;
	}

/**
SetFileBlockLength

This function sets file's block length for CMMFImaAdpcmToPcm16CodecHwDevice

@param aBlockAlign
       The file's block length 
*/
void CMMFImaAdpcmToPcm16CodecHwDevice::SetFileBlockLength(TUint aBlockAlign)
	{
	iBlockAlign = aBlockAlign;
	}

CMMFImaAdpcmToPcm16Codec::CMMFImaAdpcmToPcm16Codec()
	{
	iChannels = 1;
	iSampleRate = 0;
	iBlockAlign = KImaAdpcmBlockAlign;
	iSamplesPerBlock = KImaAdpcmSamplesPerBlock;
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
* This function converts IMA ADPCM samples to PCM samples.
*
*/
CMMFSwCodec::TCodecProcessResult CMMFImaAdpcmToPcm16Codec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{
	CMMFSwCodec::TCodecProcessResult result;
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;
	
	//convert from generic CMMFBuffer to CMMFDataBuffer
	const CMMFDataBuffer* src = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	CMMFDataBuffer* dst = STATIC_CAST(CMMFDataBuffer*, &aDst);
	
	if( !CheckPreconditions( src, dst ) )
		{
		//[ precondition(s) violation ]
		User::Leave(KErrArgument);
		}
	
	//calculate how much source is required to fill the destination buffer
	TUint blocksRemaining = src->Data().Length() / iBlockAlign;
	
	//we need to cast away CONST even on the source, as the TClass needs a TUint8*
	TUint8* pSrc = CONST_CAST(TUint8*,src->Data().Ptr());
	TUint8* pDst = CONST_CAST(TUint8*,dst->Data().Ptr());
	
	//[ [process full blocks ]
	TUint dstBytesAdded = 0;
	for( TUint count = 0; count < blocksRemaining; count++ )
		{
		iImaAdpcmTo16Pcm.Convert(pSrc, pDst, iSamplesPerBlock);
		pSrc += iBlockAlign;
		pDst += (iSamplesPerBlock * sizeof(TInt16));
		dstBytesAdded += (iSamplesPerBlock * sizeof(TInt16));	
		}
	
	result.iCodecProcessStatus = TCodecProcessResult::EProcessComplete;	
	result.iSrcBytesProcessed = blocksRemaining * iBlockAlign;
	result.iDstBytesAdded = dstBytesAdded;
	dst->Data().SetLength(result.iDstBytesAdded);
	
	//[ check post conditions
	__ASSERT_DEBUG( (src->Position() == 0), TMmfAudioCodecPanicsNameSpace::Panic( 	TMmfAudioCodecPanicsNameSpace::EPostConditionViolation ));
	__ASSERT_DEBUG( (dst->Position() == 0), TMmfAudioCodecPanicsNameSpace::Panic( 	TMmfAudioCodecPanicsNameSpace::EPostConditionViolation ));
	TInt r1 = src->Data().Length();
	r1 /= iBlockAlign;
	TInt r2 =  dst->Data().Length();
	r2 /=(iSamplesPerBlock * sizeof(TInt16));
	__ASSERT_DEBUG(  r1== r2, TMmfAudioCodecPanicsNameSpace::Panic(TMmfAudioCodecPanicsNameSpace::EPostConditionViolation ));
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
TBool CMMFImaAdpcmToPcm16Codec::CheckPreconditions( const CMMFDataBuffer* aSrcBuffer, CMMFDataBuffer* aDestBuffer )
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
	const TUint KTempBufferSize = iSamplesPerBlock * 2;
    TInt numInputSubFrames  = aSrcBuffer->Data().Length() / iBlockAlign;
	TInt numOutputSubFrames = aDestBuffer->Data().MaxLength() / KTempBufferSize;

	//[ we need modulo 1010 bytes on all src frames that are not the last
	// frame
	// For the last frame we will code only whole frames and effectively
	// drop any remaining samples]	
	TBool validInputDataLength = (aSrcBuffer->Data().Length() % iBlockAlign == 0) ;
    
	if( (numInputSubFrames > numOutputSubFrames) ||  // sufficient space in the output for the input
        (aSrcBuffer->Position() > 0 )  ||                   // position must be zero since we can eat all the data
		(aDestBuffer->Position() > 0 ) ||
		(!validInputDataLength))                         //position must be zero
		{
		return result;
		}
	
	result = ETrue;  // preconditions have been satisfied
	
	return result;
	}

/**
Configure

This function sets file's block length, channels & sample rate.

@param aChannels
       The file's number of channels
@param aSampleRate
       The file's sample rate 
@param aBlockAlign
       The file's block length 
*/
TInt CMMFImaAdpcmToPcm16Codec::Configure(TUint aChannels, TUint aSampleRate, TUint aBlockAlign)
	{
	iChannels = aChannels;
	iSampleRate = aSampleRate;
	
	if (aBlockAlign < 256)	
		{
		switch (iSampleRate * iChannels)
			{
			case 8000: // fall through, same as 11025
			case 11025:
			case 16000:
				iBlockAlign = 256;
				break;
			case 22050:
				iBlockAlign = 512;
				break;
				
			case 44100:
				iBlockAlign = 1024;
				break;
				
			case 88200:
				iBlockAlign = 2048;
				break;
				
			default:
				return KErrArgument;
			}
		}
	else
		{
		iBlockAlign = aBlockAlign;
		}
	
	const TUint KImaAdpcmBitsPerSample = 4;
	iSamplesPerBlock = (iBlockAlign - 4 * iChannels) * 8 / (KImaAdpcmBitsPerSample * iChannels) + 1;
	
	return KErrNone;
	}
	





