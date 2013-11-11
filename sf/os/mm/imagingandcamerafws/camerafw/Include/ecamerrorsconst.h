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
// Camera specific errors
// 
//

/**
 @publishedPartner
 @released
 @file
*/

#ifndef  ECAMERRORS_CONST_H
#define  ECAMERRORS_CONST_H

/**
This error code is supposed to be used whenever any fatal error occurs from which the client cannot recover.

@publishedPartner
@prototype
*/
static const TInt KErrECamFatalError = -12108;

/**
If error is KErrECamImageResourceNotReleased and client wants to successfully call the Prepare method again, 
client needs to delete all CCameraImageCapture objects and any Snapshot and Histogram objects associated with
it as well. 

@publishedPartner
@prototype
*/
static const TInt KErrECamImageResourceNotReleased = -12109;

/**
If error is KErrECamVideoResourceNotReleased and client wants to successfully call the Prepare method again,
client needs to call ReleaseVideoResource to unprepare video and then delete any Snapshot and Histogram 
objects associated with it as well.

@publishedPartner
@prototype
*/
static const TInt KErrECamVideoResourceNotReleased = -12110;

#endif // ECAMERRORS_CONST_H
