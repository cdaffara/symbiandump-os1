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
//

#ifndef CONVERSIONCOEFFICIENT_H
#define CONVERSIONCOEFFICIENT_H

/**
Specifies the possible YUV-RGB conversion coefficients to use. 

The coefficients are determined by two factors: 
The YUV signal range used, and the actual conversion multipliers.
@publishedAll
@released
*/
enum TYuvCoefficients
	{
	/**
	The nominal Y range is [16…235] and the U and V ranges [16…240]. This corresponds to H.264 | MPEG-4 AVC video_range_flag=0.
	*/
	EYuvRange0	   = 0x01000000,

	/**
	The nominal Y, U, and V ranges are [0…255]. This corresponds to H.264 | MPEG-4 AVC video_range_flag=1.
	*/
	EYuvRange1	   = 0x02000000,

	/**
	The YUV-RGB conversion cofficients are those specified in ITU-R Recommendation BT.709. 
	This corresponds to H.264 | MPEG-4 AVC matrix_coefficients=1. BT.709 is the default YUV format used 
	for MPEG-4 Part 2 and H.264 | MPEG-4 AVC.
	*/
	EYuvBt709		= 0x00000001,

	/**
	YUV data with range 0 (Y [16…235]) and conversion coefficients according to BT.709. 
	This is the most typical configuration.
	*/
	EYuvBt709Range0  = EYuvRange0 | EYuvBt709,

	/**
	YUV data with range 1 (Y [0…255]) and conversion coefficients according to BT.709.
	*/
	EYuvBt709Range1  = EYuvRange1 | EYuvBt709,

	/**
	The YUV-RGB conversion cofficients are those specified in ITU-R Recommendation BT.601.5.
	*/
	EYuvBt601		= 0x00000002,

	/**
	YUV data with range 0 (Y [16…235]) and conversion coefficients according to BT.601.5. 
	This is the data format used in H.263.
	*/
	EYuvBt601Range0  = EYuvRange0 | EYuvBt601,

	/**
	YUV data with range 1 (Y [0…255]) and conversion coefficients according to BT.601.5.
	*/
	EYuvBt601Range1  = EYuvRange1 | EYuvBt601,

	/**
	A custom conversion matrix is used. 
	@see TYuvConversionMatrix.
	*/
	ECustomYuvMatrix = 0x00800000
	};

#endif // CONVERSIONCOEFFICIENT_H

