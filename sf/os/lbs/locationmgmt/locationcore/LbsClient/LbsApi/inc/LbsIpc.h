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
//



// The file must not be modified.




#ifndef __LBSIPC_H__
#define __LBSIPC_H__

#include <lbs/lbsipc.hrh>

/**
Major version number

@publishedAll
@released
 */
const TInt8 KPosMajorVersionNumber = 1;

/**
Minor version number

@publishedAll
@released
 */
const TInt8 KPosMinorVersionNumber = 0;

/**
Build number

@publishedAll
@released
 */
const TInt16 KPosBuildVersionNumber = 0;

/**
Default message slots

@publishedAll
@released
 */
const TInt KLbsDefaultMessageSlots = 16;

/**
@publishedAll
@released
The name of the server held in the kernel
 */
_LIT(KPositionServerName, "!PosServer"); // the server name must start with a "!" because the server now has "ProtServ" capability

/**
@publishedAll
@released
The name of the server held in the kernel
 */
_LIT(KPositionServerImg, "EPosServer");

#endif // __LBSIPC_H__
