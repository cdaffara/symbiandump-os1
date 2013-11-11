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

#ifndef CAMERAOVERLAY_CONST_H
#define CAMERAOVERLAY_CONST_H

/** 
The current Version for the TOverlaySupportInfo class.
@publishedPartner
@prototype
*/
static const TUint KECamOverlaySupportInfoCurrentVersion = 1;

/** 
The current Version for the TOverlayParameters class.
@publishedPartner
@prototype
*/
static const TUint KECamOverlayParametersCurrentVersion = 1;

/** 
Constant used to specify that no specific viewfinder of the given type (either direct or client based) shall be considered.
Rather, every viewfinder should be taken into consideration.
@publishedPartner
@prototype
*/
static const TInt  KECamOverlayNoSpecificViewFinderHandle = -1;

/**
Special handle value used for TOverlaySupportInfo::iViewFinderHandle when TOverlaySupportInfo::iDesiredCameraMode is other
than viewfinder mode but not EModeNone.
If TOverlaySupportInfo::iDesiredCameraMode is EModeNone, this indicates that OverlaySupportInfo should be used 
as default, where support information was being provided to the client without distinguishing between different camera modes.
Also used as default value for TOverlayParameters::iViewFinderHandle, when TOverlayParameters::iCurrentModes is not 
neglected.
@publishedPartner
@prototype
*/
static const TInt  KECamOverlayInvalidViewFinderHandle = -2;


#endif //CAMERAOVERLAY_CONST_H
