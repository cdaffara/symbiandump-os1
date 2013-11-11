// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll 
*/


#if !defined(__SMSPVER_H__)
#define __SMSPVER_H__

#include <e32def.h>

/**
 *  @internalComponent
 */
const TUint KSmsPrtMajorVersionNumber=6;
const TUint KSmsPrtMinorVersionNumber=1;
const TUint KSmsPrtBuildVersionNumber=0;

#ifdef _DEBUG
/**
 *  Key for SMS Stack P&S Free Disk Space 
 */
const TInt KUidPSSMSStackFreeDiskSpaceKey = 0x10282FB0;

#endif 

#endif	// __SMSPVER_H__
