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
// Constants and structs specific to VC1.  See the DevVideo specs for more details.
// 
//

#ifndef __DEVVIDEO_VC1_H__
#define __DEVVIDEO_VC1_H__

#include <e32base.h>

/**
Specifies the picture type.
@publishedAll
@released
*/
enum TVC1PictureType
    {
	/**
	INTRA picture, consisting of at least one I slice and zero or more SI slices
	*/
	EVC1PictureTypeI   = 0x01,

	/**
	INTER picture, consisting of at least one P slice and zero or more I, SI and SP slices
	*/
	EVC1PictureTypeP   = 0x02,

	/**
	Bi-predictive picture, consisting of at least one B slice and zero or more I, P, SI, 
	and SP slices 
	*/
	EVC1PictureTypeB   = 0x04,

	/**
	SI picture or switching I picture consisting of SI slices
	*/
	EVC1PictureTypeBI  = 0x08
    };




#endif
