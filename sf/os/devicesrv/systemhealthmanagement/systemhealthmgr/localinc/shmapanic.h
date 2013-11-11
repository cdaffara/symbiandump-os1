// shmapaniccodes.h

// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This header file defines all the possible panic categories and codes within the System Health Monitor component.
// When adding panic codes to the System Health Monitor component and related modules,
// identify the module concerned and create a category if necessary.
// The category should contain the name of the class where the panic is occuring.
// Try to reuse an existing panic code otherwise add the new panic code 
// to the end of the enum list.
// 
//

#if !defined (__SHMAPANICCODES_H__)
#define __SHMAPANICCODES_H__

#include <e32std.h>

/** 
Defines all the modules (categories) within System Health Monitor which can throw panics
Strings must not be longer than 16 characters or they will be truncated by User::Panic()
@internalComponent
@released
*/
_LIT(KPanicRestartSys, "RestartSys");
_LIT(KPanicStartSafe, "CStartSafe");
_LIT(KPanicStartupProperties, "CStartupProperties");
_LIT(KPanicMonitor, "CMonitor");

/** Defines all possible panic codes within System Health Monitor component
@internalComponent
@released
*/
enum TShmaPanicCodes
	{
	ERestartNotSupportedOnEmulator 	= 1,
	EInvalidCommandType				= 2,
	EInvalidStartMethod				= 3,
	EInvalidTimeout					= 5,	// To maintain the compability for sysstart tests for v9.5
	EInvalidNoOfRetries				= 6,	// To maintain the compability for sysstart tests for v9.5
	EInvalidRecoveryMethod			= 7,
	ERestartSystemCallFailed		= 8,
	ETimerValueInvalid				= 9,
	ERestartSystemCallWithMode		= 10
#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
	,
	ERestartSysLibNotPresent		= 11
#endif
	};

/** Global panic function for use within System Health Monitor component
@internalComponent
@released
*/
extern void PanicNow(const TDesC &aCategory, TInt aReason);

#endif // __SHMAPANICCODES_H__
