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
// include\mmf\utils\rateconvert.cpp
// Note this code used to be in mmfutilities.cpp but split off here to allow
// scaling of descriptors instead of just CMMFBuffers.
// 
//

#include "rateconvertimpl.h" // includes rateconvert.h itself
#include <e32const.h>

const TInt KMaxInt16Bit = 65536 ; 

/*
The rate conversion algorithms used here are extremely basic, using nearest neighbour, not
even interpolation. An increment is created - initially a real, but converted to 16.16 fixed
point notation for efficiency purposes. For example, from 8000 to 11025 this increment is set
at 8000/11025 (~ 0.73). Each increment to the destination buffer conceptually increments the
src pointer by this value (0.73). On each iteration the nearest src sample is used.

The idea is that successive buffers run on from each other. The index is adjusted so at the end
of the run it is correct for the next buffer, and this is saved from one iteration to the next.
If the client wants to convert separate buffers, it should call Reset(), where the index is reset
to 0. 

Note the algorithm is even then not ideal, as it effectively truncates and not rounds the 
fixed-point index. However, a feature of this is that the nearest src sample is always behind the
conceptual fixed-point index. This makes it easy to ensure that processing of the next buffer
never needs data from the previous cycle - except this index value.

*/

enum TPanicCodes
	{
	EPanicNoDestinationBuffer=1,
	EPanicNoSourceConsumed
	};

#ifdef _DEBUG

static void Panic(TInt aPanicCode)
	{
	_LIT(KRateConvert,"RateConvert");
	User::Panic(KRateConvert, aPanicCode);
	}
	
#endif // _DEBUG

//
// CChannelAndSampleRateConverter 
//

CChannelAndSampleRateConverter::CChannelAndSampleRateConverter()
	{
	// constructor does nothing but ensures can't derive from outside dll
	}

// Factory function
EXPORT_C CChannelAndSampleRateConverter* CChannelAndSampleRateConverter::CreateL(TInt aFromRate,
																				 TInt aFromChannels,
										 										 TInt aToRate,
										 										 TInt aToChannels)
	{
	// check that the params in range so we can assume OK later on
	if (aFromChannels <= 0 || aFromChannels > 2 ||
		aToChannels <= 0 || aToChannels > 2 ||
		aFromRate <= 0 || aToRate <= 0)
		{
		User::Leave(KErrArgument);
		}
		
	CChannelAndSampleRateConverterCommon* converter = NULL;

	if (aFromChannels==aToChannels)
		{
		if (aFromChannels==1)
			{
			converter = new (ELeave) CMonoToMonoRateConverter;			
			}
		else
			{
			converter = new (ELeave) CStereoToStereoRateConverter;
			}
		}
	else
		{
		if (aFromChannels==1)
			{
			if (aFromRate!=aToRate)
				{
				converter = new (ELeave) CMonoToStereoRateConverter;				
				}
			else
				{
				converter = new (ELeave) CMonoToStereoConverter;				
				}
			}
		else
			{
            ASSERT(aFromChannels>1 && aToChannels==1);
			if (aFromRate!=aToRate)
				{
				converter = new (ELeave) CStereoToMonoRateConverter;				
				}
			else
				{
 				converter = new (ELeave) CStereoToMonoConverter;				
				}
			}
		}
	converter->SetRates(aFromRate,aToRate);
	return converter;
	}
	
//
// CChannelAndSampleRateConverterImpl
//

CChannelAndSampleRateConverterCommon::CChannelAndSampleRateConverterCommon()
	{
	}

void CChannelAndSampleRateConverterCommon::Reset()
	{
	// default does nothing
	}
	
void CChannelAndSampleRateConverterCommon::SetRates(TInt /*aFromRate*/, TInt /*aToRate*/)
	{
	// in default no need to know so don't cache
	}

TInt CChannelAndSampleRateConverterCommon::MaxConvertBufferSize(TInt aSrcBufferSize, TBool aRoundUpToPower)
	{
	// assume aSrcBuffer takes channel change into account
	TInt rawValue = aSrcBufferSize;
	if (aRoundUpToPower)
		{
		return NextPowerUp(rawValue); 
		}
	else
		{
		return rawValue;
		}
	}
	
TInt CChannelAndSampleRateConverterCommon::NextPowerUp(TInt aValue)
	{
	TInt power = 128; // no need to start lower
	while (power<aValue)
		{
		power *= 2;
		}
	return power;
	}
	
//
// CChannelAndSampleRateConvert
//

CChannelAndSampleRateConvert::CChannelAndSampleRateConvert()
	{
	}

	
TInt CChannelAndSampleRateConvert::MaxConvertBufferSize(TInt aSrcBufferSize, TBool aRoundUpToPower)
	{
	// take rate conversion into account. Assumed channel mismatch handled by the child class.
	TInt rawValue = aSrcBufferSize;
	if (iFromRate < iToRate)
		{
		TInt result = SizeOfUpsampleBuffer(rawValue);
		return result; // always rounded up to next size
		}
	else
		{
		if (aRoundUpToPower)
			{
			return NextPowerUp(rawValue); 
			}
		else
			{
			return rawValue;
			}
		}
	}
	
void CChannelAndSampleRateConvert::SetRates(TInt aFromRate, TInt aToRate)
	{
	iFromRate=aFromRate;
	iToRate=aToRate;

	TReal ratio = TReal(aFromRate) / TReal(aToRate);
	TInt quotient = TInt(ratio);
	TReal remainder = ratio - TReal(quotient);
	iFraction = (quotient << 16) + TInt32(remainder * KMaxInt16Bit);

	Reset();
	}
	
void CChannelAndSampleRateConvert::Reset()
	{
	iIndex = 0;
	}
	
TInt CChannelAndSampleRateConvert::SizeOfUpsampleBuffer(TInt aBufferLength)
	{
	TInt rawValue = aBufferLength;
	ASSERT(iFromRate < iToRate); // should not be called otherwise
	// upsample - will generate more data. use floats to avoid extra round error
	rawValue = TInt(rawValue * TReal(iToRate) / TReal(iFromRate) + 0.5) + 4*sizeof(TInt16); // add some buffer extra buffer
	rawValue = NextPowerUp(rawValue); // when upscaling always give nice power
	return rawValue;	
	}

//
// Specific converter code
//

CStereoToStereoRateConverter::CStereoToStereoRateConverter()
	{
	}

TInt CStereoToStereoRateConverter::Convert(const TDesC8& aSrcBuffer, TDes8& aDstBuffer)
	{
	const TInt32* srcPtr = reinterpret_cast<const TInt32*>(aSrcBuffer.Ptr());
	TInt32* dstPtr = const_cast<TInt32*>(reinterpret_cast<const TInt32*>(aDstBuffer.Ptr()));
	
	const TInt32* srcLimit=srcPtr+LengthBytesTo32Bit(aSrcBuffer.Length()); // ptr+n does *4 for TInt32* ptr
	TInt32* dstLimit=dstPtr+LengthBytesTo32Bit(aDstBuffer.MaxLength()); // ditto

	// add left over from last buffer
	TUint index = iIndex;
	const TInt32* src = srcPtr + (index>>16);
	TInt32* dst = dstPtr;

	if (dst>=dstLimit)
		{
		__ASSERT_DEBUG(EFalse,Panic(EPanicNoDestinationBuffer));
		return 0;
		}

	while (src<srcLimit && dst<dstLimit)
		{
  		*dst++ = *src;
		index += iFraction;
		src = srcPtr + (index>>16); // truncate fix-point index
		}

	// get amount by which index exceeded end of buffer
	// so that we can add it back to start of next buffer
	const TInt32* conceptualLastSrc = Min(src, srcLimit); // ptr to last src we have we'd _not_ used next iteration
	TInt srcSamplesCopied = conceptualLastSrc - srcPtr;
	__ASSERT_DEBUG(srcSamplesCopied>0, Panic(EPanicNoSourceConsumed)); // should always be ok if we have some destination space
	iIndex = index - (srcSamplesCopied << 16); 

	// return sample byte count and setup output buffer
	TInt dstLength = Length32BitToBytes(dst-dstPtr);
	aDstBuffer.SetLength(dstLength); //adjust length of destination buffer
	return Length32BitToBytes(srcSamplesCopied);
	}
	
CMonoToStereoRateConverter::CMonoToStereoRateConverter()
	{
	}

TInt CMonoToStereoRateConverter::Convert(const TDesC8& aSrcBuffer, TDes8& aDstBuffer)
	{
	const TInt16* srcPtr = reinterpret_cast<const TInt16*>(aSrcBuffer.Ptr());
	TInt32* dstPtr = const_cast<TInt32*>(reinterpret_cast<const TInt32*>(aDstBuffer.Ptr()));
	
	const TInt16* srcLimit=srcPtr+LengthBytesTo16Bit(aSrcBuffer.Length()); // as ptr+n does *2 for TInt16* ptr
	TInt32* dstLimit=dstPtr+LengthBytesTo32Bit(aDstBuffer.MaxLength()); // ditto but does *4 for TInt32*

	// add left over from last buffer
	TUint index = iIndex;
	const TInt16* src = srcPtr + (index>>16);
	TInt32* dst = dstPtr;

	if (dst>=dstLimit)
		{
		__ASSERT_DEBUG(EFalse,Panic(EPanicNoDestinationBuffer));
		return 0;
		}

	while (src<srcLimit && dst<dstLimit-1)
		{
		TInt16 sample = *src;
		TInt32 stereoSample = MonoToStereo(sample);
		*dst++ = stereoSample;
		index += iFraction;
		src = srcPtr + (index>>16); // truncate fix-point index
		}

	// get amount by which index exceeded end of buffer
	// so that we can add it back to start of next buffer
	const TInt16* conceptualLastSrc = Min(src, srcLimit); // ptr to last src we have we'd _not_ used next iteration
	TInt srcSamplesCopied = conceptualLastSrc - srcPtr;
	__ASSERT_DEBUG(srcSamplesCopied>0, Panic(EPanicNoSourceConsumed)); // should always be ok if we have some destination space
	iIndex = index - (srcSamplesCopied << 16); 

	// return sample byte count and setup output buffer
	TInt dstLength = Length32BitToBytes(dst-dstPtr);			// size in bytes
	aDstBuffer.SetLength(dstLength); //adjust length of destination buffer
	return Length16BitToBytes(srcSamplesCopied);
	}	
	

TInt CMonoToStereoRateConverter::MaxConvertBufferSize(TInt aSrcBufferSize, TBool aRoundUpToPower)
	{
	return CChannelAndSampleRateConvert::MaxConvertBufferSize(aSrcBufferSize*2, aRoundUpToPower);
	}
	
CMonoToMonoRateConverter::CMonoToMonoRateConverter()
	{
	}

TInt CMonoToMonoRateConverter::Convert(const TDesC8& aSrcBuffer, TDes8& aDstBuffer)
	{
	const TInt16* srcPtr = reinterpret_cast<const TInt16*>(aSrcBuffer.Ptr());
	TInt16* dstPtr = const_cast<TInt16*>(reinterpret_cast<const TInt16*>(aDstBuffer.Ptr()));
	
	const TInt16* srcLimit=srcPtr+LengthBytesTo16Bit(aSrcBuffer.Length()); // ptr+n does *2 for TInt16* ptr
	TInt16* dstLimit=dstPtr+LengthBytesTo16Bit(aDstBuffer.MaxLength()); // ditto

	// add left over from last buffer
	TUint index = iIndex;
	const TInt16* src = srcPtr + (index>>16);
	TInt16* dst = dstPtr;

	if (dst>=dstLimit)
		{
		__ASSERT_DEBUG(EFalse,Panic(EPanicNoDestinationBuffer));
		return 0;
		}

	while (src<srcLimit && dst<dstLimit)
		{
  		*dst++ = *src;
		index += iFraction;
		src = srcPtr + (index>>16); // truncate fix-point index
		}

	// get amount by which index exceeded end of buffer
	// so that we can add it back to start of next buffer
	const TInt16* conceptualLastSrc = Min(src, srcLimit); // ptr to last src we have we'd _not_ used next iteration
	TInt srcSamplesCopied = conceptualLastSrc - srcPtr;
	__ASSERT_DEBUG(srcSamplesCopied>0, Panic(EPanicNoSourceConsumed)); // should always be ok if we have some destination space
	iIndex = index - (srcSamplesCopied << 16); 

	// return sample byte count and setup output buffer
	TInt dstLength = Length16BitToBytes(dst-dstPtr);		// size in bytes
	aDstBuffer.SetLength(dstLength); //adjust length of destination buffer
	return Length16BitToBytes(srcSamplesCopied);
	}	
	
CStereoToMonoRateConverter::CStereoToMonoRateConverter()
	{
	}

//This method takes the left and right sample of interleaved PCM and sums it, then divides by 2
TInt CStereoToMonoRateConverter::Convert(const TDesC8& aSrcBuffer, TDes8& aDstBuffer)
	{
	const TInt32* srcPtr = reinterpret_cast<const TInt32*>(aSrcBuffer.Ptr());
	TInt16* dstPtr = const_cast<TInt16*>(reinterpret_cast<const TInt16*>(aDstBuffer.Ptr()));
	
	const TInt32* srcLimit=srcPtr+LengthBytesTo32Bit(aSrcBuffer.Length()); // ptr+n does *4 for TInt32* ptr
	TInt16* dstLimit=dstPtr+LengthBytesTo16Bit(aDstBuffer.MaxLength()); // ditto but *2 for TInt16*

	// add left over from last buffer
	TUint index = iIndex;
	const TInt32* src = srcPtr + (index>>16);
	TInt16* dst = dstPtr;

	if (dst>=dstLimit)
		{
		__ASSERT_DEBUG(EFalse,Panic(EPanicNoDestinationBuffer));
		return 0;
		}

	while (src<srcLimit && dst<dstLimit)
		{
		TInt32 sample = *src;
		TInt16 monoSample = StereoToMono(sample);
  		*dst++ =  monoSample;
		index += iFraction;
		src = srcPtr + (index>>16); // truncate fix-point index
		}

	// get amount by which index exceeded end of buffer
	// so that we can add it back to start of next buffer
	const TInt32* conceptualLastSrc = Min(src, srcLimit); // ptr to last src we have we'd _not_ used next iteration
	TInt srcSamplesCopied = conceptualLastSrc - srcPtr;
	__ASSERT_DEBUG(srcSamplesCopied>0, Panic(EPanicNoSourceConsumed)); // should always be ok if we have some destination space
	iIndex = index - (srcSamplesCopied << 16); 

	// return sample byte count and setup output buffer
	TInt dstLength = Length16BitToBytes(dst-dstPtr);			// size in bytes
	aDstBuffer.SetLength(dstLength); //adjust length of destination buffer
	return Length32BitToBytes(srcSamplesCopied);
	}


TInt CStereoToMonoRateConverter::MaxConvertBufferSize(TInt aSrcBufferSize, TBool aRoundUpToPower)
	{
	TUint size = aSrcBufferSize/2;
	size += aSrcBufferSize & 1; //avoid round down error
	return CChannelAndSampleRateConvert::MaxConvertBufferSize(size, aRoundUpToPower);
	}
	
CStereoToMonoConverter::CStereoToMonoConverter()
	{
	}

//This method takes the left and right sample of interleaved PCM and sums it, then divides by 2
TInt CStereoToMonoConverter::Convert(const TDesC8& aSrcBuffer, TDes8& aDstBuffer)
	{
	const TInt32* src = reinterpret_cast<const TInt32*>(aSrcBuffer.Ptr());
	TInt16* dst = const_cast<TInt16*>(reinterpret_cast<const TInt16*>(aDstBuffer.Ptr()));

	TInt srcCount = LengthBytesTo32Bit(aSrcBuffer.Length());
	TInt dstCount = LengthBytesTo16Bit(aDstBuffer.MaxLength());
	TInt count = Min(srcCount, dstCount); // if aDstBuffer is short, just copy that much
	
	for (TUint i=0;i<count;++i)
		{
		TInt32 sample = *src++;
		TInt16 monoSample = StereoToMono(sample);
  		*dst++ = monoSample;
		}
		
	aDstBuffer.SetLength(Length16BitToBytes(count)); // *2 because is mono
	return Length32BitToBytes(count); // *4 as is stereo
	}	

TInt CStereoToMonoConverter::MaxConvertBufferSize(TInt aSrcBufferSize, TBool aRoundUpToPower)
	{
	TUint size = aSrcBufferSize/2;
	size += aSrcBufferSize & 1; //avoid round down error
	return CChannelAndSampleRateConverterCommon::MaxConvertBufferSize(size, aRoundUpToPower);
	}
	
CMonoToStereoConverter::CMonoToStereoConverter()
	{
	}

TInt CMonoToStereoConverter::Convert(const TDesC8& aSrcBuffer, TDes8& aDstBuffer)
	{
	const TInt16* src = reinterpret_cast<const TInt16*>(aSrcBuffer.Ptr());
	TInt32* dst = const_cast<TInt32*>(reinterpret_cast<const TInt32*>(aDstBuffer.Ptr()));

	TInt srcCount = LengthBytesTo16Bit(aSrcBuffer.Length());
	TInt dstCount = LengthBytesTo32Bit(aDstBuffer.MaxLength());
	TInt count = Min(srcCount, dstCount); // if aDstBuffer is short, just copy that much
	
	for (TUint i=0;i<count;i++)
		{
		TInt16 sample = *src++;
		TInt32 stereoSample = MonoToStereo(sample);
  		*dst++ =  stereoSample;
		}
		
	aDstBuffer.SetLength(Length32BitToBytes(count)); // *4 because is stereo
	return Length16BitToBytes(count); // *2 as is mono
	}	

TInt CMonoToStereoConverter::MaxConvertBufferSize(TInt aSrcBufferSize, TBool aRoundUpToPower)
	{
	return CChannelAndSampleRateConverterCommon::MaxConvertBufferSize(aSrcBufferSize*2, aRoundUpToPower);
	}





