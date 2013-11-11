// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Constants and structs specific to Sorenson Spark.  See the DevVideo specs for more details.
// 
//

#ifndef __DEVVIDEO_SORENSENSPARK_H__
#define __DEVVIDEO_SORENSENSPARK_H__

#include <e32base.h>

/**
Specifies the picture type.
@publishedAll
@released
*/
enum TSparkPictureType
    {
    /**
    Indicates a keyframe
    */
    ESparkPictureTypeKey   = 0x01,
    /**
    Indicates a predicted or inter frame
    */
    ESparkPictureTypeP     = 0x02,
    /**
    Indicates a predicted frame that is disposable
    */
    ESparkPictureTypeDisp  = 0x04
    };

/**
Specifies action of deblocking filter
@publishedAll
@released
*/
enum TSparkDeblocking
	{
    /**
    Never deblock
    */
	EDeblockNever		= 0x1,
    /**
    Always deblock
    */
	EDeblockAlways		= 0x2,
    /**
    Apply deblocking as requested by video packet deblocking flag
    */
	EDeblockAsIndicated = 0x4
	};


/**
Settings to describe decompression
@publishedAll
@released
*/
class TSparkDecompressSettings
	{
public:
	/**
	If this member is set to EDeblockNever the decompressor will never deblock the image even 
	if the deblocking bit is set in the bitstream. If this member is set to EDeblockAlways the
	decompressor will deblock the image even if the deblocking bit not set in the bitstream.
	If this member is set to EDeblockAsIndicated then deblocking will only occur when the
	deblocking bit in the bitstream is set to 1.
	*/
    TSparkDeblocking iDeblockingMethod;
	};


/**
Sorenson Spark video packet header
@publishedAll
@released
*/
class TSparkPictureHeader
	{
public:
	/**
	Indicates the temporal reference, which is formed by incrementing the value in the previously transmitted
	picture header by one, plus the number of non-transmitted pictures since the previously transmitted one.
	*/
	TUint iTemporalReference;
	/**
	Indicates the picture coding type. iPictureType is set according to the picture header as one of
	the values defined in TSparkPictureType
	*/
	TSparkPictureType iPictureType;
	/**
	Requests use of deblocking filter
	*/
	TBool Deblocking;
	/**
	Indicates the initial value of the quantization parameter
	*/
	TUint iQuantizer;
	};

#endif