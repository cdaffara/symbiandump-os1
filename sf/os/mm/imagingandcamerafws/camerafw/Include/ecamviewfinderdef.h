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

/**
 @file
 @internalTechnology
 @prototype
*/
#ifndef  ECAMVIEWFINDER_DEF_H
#define  ECAMVIEWFINDER_DEF_H

/**
This will be used to specify the number of cross fading frames when the viewfinder fading type is not EVFFadingTypeCross.

@internalTechnology
*/
static const TInt KECamCrossFadingFramesNotUsed = 0;

/**
Special handle which represents that a different format is being used by frame for EVFFadingTypeStaticFrameFading.

@internalTechnology
*/
static const TInt KECamSpecialStaticFrameFormatHandle = -1;

/**
Special handle value to represent handle for frame used if the type of viewfinder fading is not 
EVFFadingTypeStaticFrameFading.

@internalTechnology
*/
static const TInt KECamStaticFrameFadingTypeNotUsed = -2;

/** 
The current Version of the TViewFinderFadingCapabilities class.

@internalTechnology
*/
static const TUint KECamViewFinderFadingCapabilitiesCurrentVersion = 1;

/** 
The current Version of the TViewFinderFadingEffect class.

@internalTechnology
*/
static const TUint KECamViewFinderFadingEffectCurrentVersion = 1;
	 
#endif // ECAMVIEWFINDER_DEF_H
