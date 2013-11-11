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
 @publishedAll
 @released
 @file
*/

#ifndef  ECAMERRORS_H
#define  ECAMERRORS_H

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamerrorsconst.h>
#include <ecamerrorsdef.h>
#endif

/** The camera has been disabled, hence calls do not succeed  */
static const TInt KErrECamCameraDisabled			= -12100;

/** This parameter or operation is supported, but presently is disabled. */ 
static const TInt KErrECamSettingDisabled			= -12101;

/** This value is out of range. */
static const TInt KErrECamParameterNotInRange 		= -12102;

/**
@deprecated Use sysytem wide error code KErrNotSupported, since there is no clear cut difference between the two 
*/ 
static const TInt KErrECamSettingNotSupported		= -12103;

/** The optimum focus is lost */
static const TInt KErrECamNotOptimalFocus 		= -12104;

/** The flash is not charged */
static const TInt KErrECamFlashNotCharged		= -12105;

/**
This error code is supposed to be used with new event KUidECamEventCIPSetColorSwapEntry 
and KUidECamEventCIPRemoveColorSwapEntry.
This would occur when the particular color is being targetted by 2 color entries for 
conversion to different colors.

*/
static const TInt KErrECamColorOperationConflict	= -12106;

#endif // ECAMERRORS_H
