// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Camera specific constants
// 
//

/**
 @file
 @publishedAll
 @released
*/

#ifndef  ECAMCONSTANTS_H
#define  ECAMCONSTANTS_H

/** 
	Multiplier value used to re-scale various parameters which would typically 
	require floating point representation.
	For example to represent aperture (F-stop) F5.6, all values are re-scaled using 
	this factor. In this way F5.6 will be 560, F22 will be 2200.
	Functions and settings which use such factor are clearly identified in 
	individual comments. 
*/
static const TInt KECamFineResolutionFactor 	= 100;

/** 
	Value info settings. This data type provides additional information 
	to specify the meaning of the returned array of values and 
	the related data set it describes. 
*/
enum TValueInfo
	{	
	/** 
		Data set consists of discrete, magnitude ordered values, 
		not necessary at uniform intervals.
		The array contains each of these individual values, magnitude ordered. 
		The first value in the array is the minimum and the last is the maximum value of the range. 
		Naturally it can be just one element or a bitfield. 
		*/
	/** Not active. The data array should be empty. */
	ENotActive,		
	/** Data is represented by one element TInt in the form of bitfields. */ 
	EBitField,
	/** Data is represented by individual elements. */
	EDiscreteSteps,
	/** Data set consists of discrete, magnitude ordered values, at uniform intervals.
		It can be described by the minimum, maximum value and the step. */
	EDiscreteRangeMinMaxStep,
	/** Data set is continuous.
		It can be described by the minimum and maximum value. The step is 1. */
	EContinuousRangeMinMax
	};

#endif // ECAMCONSTANTS_H
