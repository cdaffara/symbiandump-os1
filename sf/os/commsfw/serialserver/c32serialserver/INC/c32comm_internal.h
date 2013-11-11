// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @Internal
 @released
*/

#ifndef __C32COMM_INTERNAL_H__
#define __C32COMM_INTERNAL_H__


#include <e32base.h>

/**
C32 major version number.
@internalAll
*/
const TUint KEC32MajorVersionNumber = 1;  
/**
C32 minor version number.
@internalAll
*/
const TUint KEC32MinorVersionNumber = 0;  
/**
C32 build version number.
@internalAll
*/
const TUint KEC32BuildVersionNumber = 605;

/**
@internalComponent
*/
const TDesC& KCommServerName(void);
#endif // __C32COMM_INTERNAL_H__

