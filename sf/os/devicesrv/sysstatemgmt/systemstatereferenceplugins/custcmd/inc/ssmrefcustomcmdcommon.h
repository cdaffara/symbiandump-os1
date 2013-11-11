/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Declaration of CStrtSecurityCheckTask class
*
*/




/**
 @file
 @internalComponent
 @released
*/

#ifndef __SSMREFCUSTOMCMDCOMMON_H__
#define __SSMREFCUSTOMCMDCOMMON_H__

#include <e32std.h>
#include <e32cmn.h>

//Starter security phase PS key
const TUint32 KStarterSecurityPhase = 0x00000001;

	/**
	* Identifies the status of the security checks. Can be used to check if
	* boot-time security checks have been partially or fully executed.
	*/
	enum TStarterSecurityPhase
	   {
	   EStarterSecurityPhaseUninitialized = 0,
	   EStarterSecurityPhaseSimOk = 1,     // SIM card security checks are OK (PIN etc.)
	   EStarterSecurityPhaseSimNok = 2,    // SIM card security checks have failed
	   EStarterSecurityPhaseSecOk = 3,     // Device security checks are OK (password)
	   EStarterSecurityPhaseSecNok = 4     // Device security checks have failed
	   };

#endif // __SSMREFCUSTOMCMDCOMMON_H__
