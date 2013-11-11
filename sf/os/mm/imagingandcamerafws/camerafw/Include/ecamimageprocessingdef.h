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
 @internalTechnology
 @released
*/
#ifndef  ECAMIMAGEPROCESSING_DEF_H
#define  ECAMIMAGEPROCESSING_DEF_H

/** Contrast Enhancement

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L().

@internalTechnology
*/
static const TUid  KUidECamEventCIPAdjustContrastEnhancement 		= {KUidECamEventCIPAdjustContrastEnhancementUidValue};
/** Software based Red Eye Reduction

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L()

@internalTechnology
*/
static const TUid  KUidECamEventCIPAdjustRedEyeReduction 		    = {KUidECamEventCIPAdjustRedEyeReductionUidValue};

/**
Image distortion correction.

This uid value is available from the 'supported' or 'getter' methods only to the API clients using CCamera::New2L() or 
CCamera::NewDuplicate2L()

@internalTechnology
*/
static const TUid KUidECamEventCIPImageDistortionCorrection         = {KUidECamEventCIPImageDistortionCorrectionUidValue};


#endif//ECAMIMAGEPROCESSING_DEF_H



