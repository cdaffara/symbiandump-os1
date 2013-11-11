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
//

#include "MmfUtilities.h"
#include "mmfutilitiespriv.h"
#include "mmf/utils/rateconvert.h"

const TInt KMaxInt16Bit = 65536 ; //Maximum for a 16bit int

EXPORT_C CMMFChannelAndSampleRateConverterFactory::~CMMFChannelAndSampleRateConverterFactory()
	{
	delete iConverter;
	}

	
EXPORT_C CMMFChannelAndSampleRateConverter* CMMFChannelAndSampleRateConverterFactory::CreateConverterL(TInt aFromRate,TInt aFromChannels,
													   TInt aToRate,TInt aToChannels)
	{
	if(aFromRate<0 || aFromChannels<0 || aToRate<0 || aToChannels<0)
		{
		User::Leave(KErrArgument);
		}

	iFromRate=aFromRate;
	iToRate=aToRate;
	iFromChannels=aFromChannels;
	iToChannels=aToChannels;
	return CreateConverterL();
	}

EXPORT_C CMMFChannelAndSampleRateConverter* CMMFChannelAndSampleRateConverterFactory::CreateConverterL()
	{
	if (iConverter
		&&(iFromRate==iConverter->iFromRate)
		&&(iToRate==iConverter->iToRate)
		&&(iFromChannels==iConverter->iFromChannels)
		&&(iToChannels==iConverter->iToChannels))
		return iConverter;

	delete iConverter;
	iConverter=NULL;
	
	iConverter = CMMFForwardingChannelAndSampleRateConverter::NewL(iFromRate, iFromChannels, iToRate, iToChannels);

	return iConverter;
	}
	
// SetRates kept for BC reasons - populates publically visible properties
	
void CMMFChannelAndSampleRateConverter::SetRates(TInt aFromRate,TInt aFromChannels,
											 TInt aToRate,TInt aToChannels)
	{
	iFromRate=aFromRate;
	iToRate=aToRate;
	iFromChannels=aFromChannels;
	iToChannels=aToChannels;

	iRatio = (TReal)aFromRate / (TReal)aToRate;

	TInt quotient = (TInt)iRatio;
	TReal remainder = iRatio - (TReal)quotient;
	iFraction = quotient * KMaxInt16Bit + (TInt32)(remainder * KMaxInt16Bit);

	Reset();
	}
	
//
// Forwarding converter - adapt to the implementation in audioutils.
//
	
CMMFForwardingChannelAndSampleRateConverter* CMMFForwardingChannelAndSampleRateConverter::NewL(TInt aFromRate,
																							   TInt aFromChannels,
										    			      								   TInt aToRate,
										    			      								   TInt aToChannels)
	{
	CMMFForwardingChannelAndSampleRateConverter* self = new (ELeave) CMMFForwardingChannelAndSampleRateConverter;
	CleanupStack::PushL(self);
	self->ConstructL(aFromRate,aFromChannels,aToRate,aToChannels);
	CleanupStack::Pop(self);
	return self;
	}
	
CMMFForwardingChannelAndSampleRateConverter::CMMFForwardingChannelAndSampleRateConverter()
	{
	}

void CMMFForwardingChannelAndSampleRateConverter::ConstructL(TInt aFromRate,
															 TInt aFromChannels,
										    			     TInt aToRate,
										    			     TInt aToChannels)
	{
	SetRates(aFromRate, aFromChannels, aToRate, aToChannels); 
		// delib called before iRealConverter created. Reset() will be no-op.
		// this is preserved solely to keep BC with original API that exposes too much
	iRealConverter = CChannelAndSampleRateConverter::CreateL(aFromRate,aFromChannels,aToRate,aToChannels);
	}
	
CMMFForwardingChannelAndSampleRateConverter::~CMMFForwardingChannelAndSampleRateConverter()
	{
	delete iRealConverter;
	}
	
TInt CMMFForwardingChannelAndSampleRateConverter::Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer)
	{
	TInt ignore = iRealConverter->Convert(aSrcBuffer.Data(), aDstBuffer.Data());
	// CChannelAndSampleRateConverter returns the source length converted
	// while the old API wants the destination generated, so change. 
	TInt result = aDstBuffer.Data().Length(); 
	return result;
	}
	
void CMMFForwardingChannelAndSampleRateConverter::Reset()
	{
	// need to check iRealConverter created. Won't be true during construction
	if (iRealConverter)
		{
		iRealConverter->Reset();
		}
	}
	
TUint CMMFForwardingChannelAndSampleRateConverter::MaxConvertBufferSize(TUint aSrcBufferSize)
	{
	return TUint(iRealConverter->MaxConvertBufferSize(aSrcBufferSize));
	}
	
//
// Old derivatives of CMMFChannelAndSampleRateConverter. These were previously returned by
// CreateConverterL(). Kept for BC, but no longer used by main code.
//

TInt CMMFStereoToStereoRateConverter::Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer)
	{
	TInt32* aSrc = (TInt32*)aSrcBuffer.Data().Ptr();
	TInt32* aDst = (TInt32*)aDstBuffer.Data().Ptr();
	TUint aSamples = aSrcBuffer.Data().Length()/4;

	TInt32* src = aSrc;
	TInt32* dst = aDst;
	TInt32* limit=src+aSamples;
	
	// add left over from last buffer
	TUint index = iIndex;
	src = aSrc + (index>>16);

	while(src<limit)
		{
		*dst++ = *src;
		index += iFraction;
		src = aSrc + (index>>16);
		}
	
	// get amount by which index exceeded end of buffer
	// so that we can add it back to start of next buffer
	iIndex = index - (aSamples << 16);

	// return sample byte count and setup output buffer
	TInt length = (dst-(TInt32*)aDst)*4;	//dealing with 32bit values so multiply by 4 for bytes
	aDstBuffer.Data().SetLength(length); //adjust length of destination buffer
	return (length);
	}	

TInt CMMFMonoToStereoRateConverter::Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer)
	{
	TInt16* aSrc = (TInt16*)aSrcBuffer.Data().Ptr();
	TInt16* aDst = (TInt16*)aDstBuffer.Data().Ptr();
	TUint aSamples = aSrcBuffer.Data().Length()/2;

	TInt16* src = aSrc;
	TInt16* dst = aDst;
	TInt16* limit=aSrc+aSamples;

	// add left over from last buffer
	TUint index = iIndex;
	src = aSrc + (index>>16);

	while(src<limit)
		{
		*dst++ = *src;
		*dst++ = *src;
		index += iFraction;
		src = aSrc + (index>>16);
		}

	// get amount by which index exceeded end of buffer
	// so that we can add it back to start of next buffer
	iIndex = index - (aSamples << 16);

	// return sample byte count and setup output buffer
	TInt length = (dst-aDst) * 2;		// size in bytes
	aDstBuffer.Data().SetLength(length); //adjust length of destination buffer
	return (length);
	}	
	

TUint CMMFMonoToStereoRateConverter::MaxConvertBufferSize(TUint aSrcBufferSize)
	{
	return aSrcBufferSize*2;
	}



TInt CMMFMonoToMonoRateConverter::Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer)
	{
	TInt16* aSrc = (TInt16*)aSrcBuffer.Data().Ptr();
	TInt16* aDst = (TInt16*)aDstBuffer.Data().Ptr();
	TUint aSamples = aSrcBuffer.Data().Length()/2;

	TInt16* src = aSrc;
	TInt16* dst = aDst;
	TInt16* limit=aSrc+aSamples; //*2 ???

	// add left over from last buffer
	TUint index = iIndex;
	src = aSrc + (index>>16);

	while(src<limit)
		{
  		*dst++ = *src;
		index += iFraction;
		src = aSrc + (index>>16);
		}

	// get amount by which index exceeded end of buffer
	// so that we can add it back to start of next buffer
	iIndex = index - (aSamples << 16);

	// return sample byte count and setup output buffer
	TInt length = (dst-aDst)*2;			// size in bytes
	aDstBuffer.Data().SetLength(length); //adjust length of destination buffer
	return (length);
	}	

//This method takes the left and right sample of interleaved PCM and sums it, then divides by 2
TInt CMMFStereoToMonoRateConverter::Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer)
	{
	TInt16* aSrc = (TInt16*)aSrcBuffer.Data().Ptr();
	TInt16* aDst = (TInt16*)aDstBuffer.Data().Ptr();
	TUint aSamples = aSrcBuffer.Data().Length()/4;

	TInt16* src = aSrc;
	TInt16* limit=aSrc+aSamples*2;	//because 1 sample = two TInt16s

	// add left over from last buffer
	TUint index = iIndex;
	src = aSrc + (index>>16)*2;

	TInt length = 0;

	while(src<limit)
		{
		*aDst++ = (TInt16)((src[0] + (src[1])) / 2);
		index += iFraction;
		src = aSrc + (index>>16)*2;
		length++;
		}
	// get amount by which index exceeded end of buffer
	// so that we can add it back to start of next buffer
	iIndex = index - (aSamples << 16);

	// return sample byte count and setup output buffer
	aDstBuffer.Data().SetLength(length * 2); //adjust length of destination buffer
	return (length);
	}


TUint CMMFStereoToMonoRateConverter::MaxConvertBufferSize(TUint aSrcBufferSize)
	{
	TUint size = aSrcBufferSize/2;
	size += aSrcBufferSize & 1; //avoid round down error
	return size;
	}

//This method takes the left and right sample of interleaved PCM and sums it, then divides by 2
TInt CMMFStereoToMonoConverter::Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer)
	{
	TInt16* aSrc = (TInt16*)aSrcBuffer.Data().Ptr();
	TInt16* aDst = (TInt16*)aDstBuffer.Data().Ptr();
	TUint aSamples = aSrcBuffer.Data().Length()/4;
	for (TUint i=0;i<aSamples;i++)
		{
		*aDst++ = (TInt16)((aSrc[0] + (aSrc[1])) / 2);
		aSrc+=2;
		}
	aDstBuffer.Data().SetLength(aSamples*2); //adjust length of destination buffer
	return aSamples*2;
	}	

TUint CMMFStereoToMonoConverter::MaxConvertBufferSize(TUint aSrcBufferSize)
	{
	TUint size = aSrcBufferSize/2;
	size += aSrcBufferSize & 1; //avoid round down error
	return size;
	}



TInt CMMFMonoToStereoConverter::Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer)
	{
	TInt16* aSrc = (TInt16*)aSrcBuffer.Data().Ptr();
	TInt16* aDst = (TInt16*)aDstBuffer.Data().Ptr();
	TUint aSamples = aSrcBuffer.Data().Length()/2;
	for (TUint i=0;i<aSamples;i++)
		{
		*aDst++ = *aSrc;
		*aDst++ = *aSrc++;
		}
	aDstBuffer.Data().SetLength(aSamples*4); //adjust length of destination buffer
	return aSamples*4;
	}	

TUint CMMFMonoToStereoConverter::MaxConvertBufferSize(TUint aSrcBufferSize)
	{
	return aSrcBufferSize*2;
	}





