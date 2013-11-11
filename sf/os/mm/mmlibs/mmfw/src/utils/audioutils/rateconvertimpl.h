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

#ifndef RATECONVERTIMPL_H
#define RATECONVERTIMPL_H

#include "mmf/utils/rateconvert.h"

/**
Abstract class that all specific converters derive from.
*/
NONSHARABLE_CLASS( CChannelAndSampleRateConverterCommon ) : public CChannelAndSampleRateConverter
	{
public:
	virtual void SetRates(TInt aSrcRate,TInt aDstRate);
	
	// from CChannelAndSampleRateConverter
	void Reset();
	TInt MaxConvertBufferSize(TInt aSrcBufferSize, TBool aRoundUpToPower);
	
protected:
	CChannelAndSampleRateConverterCommon();
	
	static TInt NextPowerUp(TInt aValue);
	inline static TInt StereoToMono(TInt aSample);
	inline static TInt MonoToStereo(TInt aSample);
	
	inline static TInt LengthBytesTo16Bit(TInt aLength);
	inline static TInt LengthBytesTo32Bit(TInt aLength);
	inline static TInt Length16BitToBytes(TInt aLength);
	inline static TInt Length32BitToBytes(TInt aLength);
	};
	
/* functions for copying from mono to stereo and from stereo to mono */

inline TInt CChannelAndSampleRateConverterCommon::StereoToMono(TInt aSample)
	{
	TInt topChannel = aSample>>16; // will naturally keep sign
	TInt bottomChannel = TInt16(aSample&0xffff); // need to sign extend
	TInt monoSample = (topChannel + bottomChannel)/2; // add two channels and divide
	return monoSample;
	}
	
inline TInt CChannelAndSampleRateConverterCommon::MonoToStereo(TInt aSample)
	{
	TInt32 stereoSample = (aSample<<16) | (aSample&0xffff);
#ifdef _DEBUG
	TInt monoSample = StereoToMono(stereoSample);
	ASSERT(monoSample==aSample);
#endif
	return TInt(stereoSample);
	}
	
// Convert various lengths by multipling or dividing by 2 or 4

inline TInt CChannelAndSampleRateConverterCommon::LengthBytesTo16Bit(TInt aLength)
	{
	return aLength/sizeof(TInt16);
	}
	
inline TInt CChannelAndSampleRateConverterCommon::LengthBytesTo32Bit(TInt aLength)
	{
	return aLength/sizeof(TInt32);
	}
	
inline TInt CChannelAndSampleRateConverterCommon::Length16BitToBytes(TInt aLength)
	{
	return aLength*sizeof(TInt16);
	}
	
inline TInt CChannelAndSampleRateConverterCommon::Length32BitToBytes(TInt aLength)
	{
	return aLength*sizeof(TInt32);
	}

/**
Abstract class that all specific rate converters derive from - as opposed to channel converters
*/
NONSHARABLE_CLASS( CChannelAndSampleRateConvert ) : public CChannelAndSampleRateConverterCommon
	{
public:
	
	// from CChannelAndSampleRateConverterCommon
	void Reset();
	TInt MaxConvertBufferSize(TInt aSrcBufferSize, TBool aRoundUpToPower);
	void SetRates(TInt aSrcRate,TInt aDstRate);
	
protected:
	CChannelAndSampleRateConvert();
	
	TInt SizeOfUpsampleBuffer(TInt aBufferLength);
protected:
	/*
	The sample rate of the data in the source buffer
	*/
	TInt iFromRate;
	/*
	The sample rate of the data in the destination buffer
	*/
	TInt iToRate;

	TInt iFraction;
	TInt iIndex;
	};

NONSHARABLE_CLASS( CStereoToMonoRateConverter ) : public CChannelAndSampleRateConvert
	{
public:
	CStereoToMonoRateConverter();
	
protected:
	// from CChannelAndSampleRateConvert
	TInt Convert(const TDesC8& aSrcBuffer, TDes8& aDstBuffer);
	TInt MaxConvertBufferSize(TInt aSrcBufferSize, TBool aRoundUpToPower);
	};

NONSHARABLE_CLASS( CStereoToMonoConverter ) : public CChannelAndSampleRateConverterCommon
	{
public:
	CStereoToMonoConverter();
	
protected:
	// from CChannelAndSampleRateConverterCommon
	TInt Convert(const TDesC8& aSrcBuffer, TDes8& aDstBuffer);
	TInt MaxConvertBufferSize(TInt aSrcBufferSize, TBool aRoundUpToPower);
	};

NONSHARABLE_CLASS( CStereoToStereoRateConverter ) : public CChannelAndSampleRateConvert
	{
public:
	CStereoToStereoRateConverter();
	
protected:
	// from CChannelAndSampleRateConvert
	TInt Convert(const TDesC8& aSrcBuffer, TDes8& aDstBuffer);
	};

NONSHARABLE_CLASS( CMonoToMonoRateConverter ) : public CChannelAndSampleRateConvert
	{
public:
	CMonoToMonoRateConverter();
	
protected:
	// from CChannelAndSampleRateConvert
	TInt Convert(const TDesC8& aSrcBuffer, TDes8& aDstBuffer);
	};

NONSHARABLE_CLASS( CMonoToStereoConverter ) : public CChannelAndSampleRateConverterCommon
	{
public:
	CMonoToStereoConverter();
	
protected:
	// from CChannelAndSampleRateConverterCommon
	TInt Convert(const TDesC8& aSrcBuffer, TDes8& aDstBuffer);
	TInt MaxConvertBufferSize(TInt aSrcBufferSize, TBool aRoundUpToPower);
	};

NONSHARABLE_CLASS( CMonoToStereoRateConverter ) : public CChannelAndSampleRateConvert
	{
public:
	CMonoToStereoRateConverter();
	
protected:
	// from CChannelAndSampleRateConvert
	TInt Convert(const TDesC8& aSrcBuffer, TDes8& aDstBuffer);
	TInt MaxConvertBufferSize(TInt aSrcBufferSize, TBool aRoundUpToPower);
	};

#endif

