// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/
#ifndef  ECAMIMAGEPROCESSING_CONST_H
#define  ECAMIMAGEPROCESSING_CONST_H
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamadvsettingsuidsconst.hrh>
#endif

/** Relative Orientation 

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L().

@publishedPartner
@prototype
*/
static const TUid  KUidECamEventImageProcessingTransformRelativeOrientation = {KUidECamEventImageProcessingTransformRelativeOrientationUidValue};

/**
Uid used to notify that the current picture orientation is unachievable after setting the relative custom orientation.

@see CCamera::CCameraImageProcessing::SetRelativeCustomOrientationL(TOrientationReference aOrientationReference, 
												TRelativeCustomOrientation aRelativeCustomOrientation);

@publishedPartner
@prototype
*/
static const TUid  KUidECamEventPictureOrientationUnachievable		= {KUidECamEventPictureOrientationUnachievableUidValue};

#endif//ECAMIMAGEPROCESSING_CONST_H



