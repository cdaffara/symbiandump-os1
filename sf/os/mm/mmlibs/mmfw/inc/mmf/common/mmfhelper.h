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
// include\mmf\common\mmfutilities.h
// 
//

#ifndef __MMF_COMMON_HELPER_H__
#define __MMF_COMMON_HELPER_H__

#include <e32base.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmffourcc.h>


/**
@internalAll

Base utility class to change the sample rate of audio data in a buffer
*/
class CMMFChannelAndSampleRateConverter : public CBase
	{
public:
	/**
	Reads the audio data from the source buffer,
	converts the number of channels and the sample rate
	and copies the result to the destination buffer

	@param  aSrcBuffer
	        A pointer to a source buffer containing the audio data to convert.
	@param  aDstBuffer
	        A pointer to a destination buffer.

	@return The length of the destination buffer.
	*/
	virtual TInt Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer) =0;
	virtual void Reset() {};

	/*
	Indicates what buffer size is required to hold the converted data.
	*/
	virtual TUint MaxConvertBufferSize(TUint aSrcBufferSize) {return aSrcBufferSize;}

	void SetRates(TInt aSrcRate,TInt aSrcChannels,TInt aDstRate,TInt aDstChannels);
public:
	/*
	The sample rate of the data in the source buffer
	*/
	TInt iFromRate;
	/*
	The sample rate of the data in the destination buffer
	*/
	TInt iToRate;
	/*
	The number of channels of data in the source buffer
	*/
	TInt iFromChannels;
	/*
	The number of channels of data in the destination buffer
	*/
	TInt iToChannels;
protected:
	TReal iRatio;
	TInt iFraction;
	TInt iIndex;
	};

/**
@internalAll
*/
class CMMFStereoToMonoRateConverter : public CMMFChannelAndSampleRateConverter
	{
	public:
		virtual TInt Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer);
		virtual TUint MaxConvertBufferSize(TUint aSrcBufferSize);

	};

/**
@internalAll
*/
class CMMFStereoToMonoConverter : public CMMFChannelAndSampleRateConverter
	{
	public:
		virtual TInt Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer);
		virtual TUint MaxConvertBufferSize(TUint aSrcBufferSize);
	};

/**
@internalAll
*/
class CMMFStereoToStereoRateConverter : public CMMFChannelAndSampleRateConverter
	{
	public:
		virtual TInt Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer);
	};

/**
@internalAll
*/
class CMMFMonoToMonoRateConverter : public CMMFChannelAndSampleRateConverter
	{
	public:
		virtual TInt Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer);
	};

/**
@internalAll
*/
class CMMFMonoToStereoConverter : public CMMFChannelAndSampleRateConverter
	{
	public:
		virtual TInt Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer);
		virtual TUint MaxConvertBufferSize(TUint aSrcBufferSize);
	};

/**
@internalAll
*/
class CMMFMonoToStereoRateConverter : public CMMFChannelAndSampleRateConverter
	{
	public:
		virtual TInt Convert(const CMMFDataBuffer& aSrcBuffer, CMMFDataBuffer& aDstBuffer);
		virtual TUint MaxConvertBufferSize(TUint aSrcBufferSize);
	};

/**
@internalAll

Factory class to create the appropriate CMMFChannelAndSampleRateConverter-derived
class depending on the supplied number of channels and bit rate
*/
class CMMFChannelAndSampleRateConverterFactory : public CBase
	{

public:
	IMPORT_C CMMFChannelAndSampleRateConverter* CreateConverterL(TInt aFromRate,TInt aFromChannels,
												 TInt aToRate,TInt aToChannels);
	IMPORT_C CMMFChannelAndSampleRateConverter* CreateConverterL();
	CMMFChannelAndSampleRateConverter* Converter() {return iConverter;}
	IMPORT_C ~CMMFChannelAndSampleRateConverterFactory();
	TInt Rate() {return iToRate;}
	TInt Channels() {return iToChannels;}
public:
	/**
	The sample rate of the data in the source buffer
	*/
	TInt iFromRate;
	/**
	The sample rate of the data in the destination buffer
	*/
	TInt iToRate;
	/**
	The number of channels of data in the source buffer
	*/
	TInt iFromChannels;
	/**
	The number of channels of data in the destination buffer
	*/
	TInt iToChannels;
private:
	CMMFChannelAndSampleRateConverter* iConverter;
	};

#endif //__MMF_COMMON_HELPER_H__

