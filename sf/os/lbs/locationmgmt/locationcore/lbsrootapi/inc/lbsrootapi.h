// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LBSROOTAPI_H__
#define __LBSROOTAPI_H__

#include "lbspropertykeydefs.h"
#include "lbsprocesssupervisor.h"
#include "lbssystemcontroller.h"
#include "lbsprocessuiddefs.h"

enum
    {
	KLbsStartupRequestKey = KLbsRootProcessKeyBase,
	KLbsStartupCompleteKey,
	KLbsCloseDownRequestKey,
	KLbsCloseDownCompleteKey,
	/**
	 * Make sure this is the last one, as it's used as the base for 
	 * the definition of n * 2 properties
	 */
    KLbsCloseDownBaseKey 
    };

_LIT(KLbsRootProcessName, "lbsroot.exe");

// Full path of lbsroot executable
_LIT(KLbsRootFileName, "\\sys\\bin\\lbsroot.exe");

// Need a wildcard when checking to see whether a process is open, to cope
// with system-derived accretions e.g. lbsroot.exe[0001]
_LIT(KLbsRootProcessSearchString, "lbsroot.exe*");

#endif //__LBSROOTAPI_H__
