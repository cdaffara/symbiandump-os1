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
// include\mmf\utils\rateconvert.h
// 
//

#ifndef RATECONVERT_H
#define RATECONVERT_H

/**
@internalTechnology
@file
*/

#include <e32base.h>

/**
Base utility class to change the sample rate of audio data in a buffer
*/
class CChannelAndSampleRateConverter : public CBase
	{
protected:
	CChannelAndSampleRateConverter();
	
public:
	/*
	Create a converter that will do rate and channel conversion as requested
	*/
	IMPORT_C static CChannelAndSampleRateConverter* CreateL(TInt aFromRate,TInt aFromChannels,
												 TInt aToRate,TInt aToChannels);
	/**
	Reads the audio data from the source buffer,
	converts the number of channels and the sample rate
	and copies the result to the destination buffer

	@param  aSrcBuffer
	        The source buffer containing the audio data to convert.
	@param  aDstBuffer
	        The destination buffer - assumed big enough

	@return The length of the source buffer that was consumed.
	*/
	virtual TInt Convert(const TDesC8& aSrcBuffer, TDes8& aDstBuffer)=0;
	virtual void Reset()=0;

	/*
	Indicates what buffer size is required to hold the converted data.
	Support size of original buffer. 
	aRoundUpToPower means give next power of 2 size up from raw size.
	NB. May be bigger than strictly necessary.
	*/
	virtual TInt MaxConvertBufferSize(TInt aSrcBufferSize, TBool aRoundUpToPower=EFalse)=0;
	};


#endif

