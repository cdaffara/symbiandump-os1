/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* sysstartpaniccodes.h
* This header file defines all the possible panic categories and codes within the System Starter component.
* When adding panic codes to the System Starter component and related modules,
* identify the module concerned and create a category if necessary.
* The category should contain the name of the class where the panic is occuring.
* Try to reuse an existing panic code otherwise add the new panic code 
* to the end of the enum list.
* 
*
*/



#if !defined (__SYSSTARTPANICCODES_H__)
#define __SYSSTARTPANICCODES_H__

#include <e32std.h>

/** 
Defines all the modules (categories) within System Starter which can throw panics
Strings must not be longer than 16 characters or they will be truncated by User::Panic()
@internalComponent
@deprecated
*/
_LIT(KPanicSysStart, "CSystemStarter");
_LIT(KPanicRestartSys, "RestartSys");
_LIT(KPanicAppStart, "CAppStarter");
_LIT(KPanicMultipleWait, "CMultipleWait");
_LIT(KPanicDllStarter, "CDllStarter");
_LIT(KPanicResourceFileReader, "CResourceReader");
_LIT(KPanicStartupState, "CStartupState");
_LIT(KPanicStartSafe, "CStartSafe");
_LIT(KPanicStartupProperties, "CStartupProperties");
_LIT(KPanicMonitor, "CMonitor");
_LIT(KPanicDsc, "CDscStore");

/** Defines all possible panic codes within System Starter component
@internalComponent
@deprecated
*/
enum TSysStartPanicCodes
	{
	ERestartNotSupportedOnEmulator 	= 1,
	ENULLPropertiesPassed			= 2,
	EInvalidCommandType				= 3,
	EInvalidStartMethod				= 4,
	EInvalidTimeout					= 5,
	EInvalidNoOfRetries				= 6,
	EInvalidRecoveryMethod			= 7,
	ERestartSystemCallFailed		= 8,
	EPropertyRetrieveError			= 9,
	EAddDomainHierarchyError		= 10,
	EInvalidMultipleWaitFailOnError	= 11,
	EInvalidDLLFailOnError			= 12,
	EInvalidOrdinal0				= 13,
	EInvalidDataLink0				= 14,
	EInvalidStateId					= 15,
	EInvalidActionOnFailure			= 16,
	EExtraMultipleWaitCommand		= 17,
	EMissingMultipleWaitCommand		= 18,
	EMissingMaxStartupMode			= 19,
	EInvalidStartupMode				= 20,
	ETimerValueInvalid				= 21,
	ESSCProcessFailure				= 22,
	ERestartSystemCallWithMode		= 23,
	EDomainManagerConnectionFailure = 24,
	EIdCannotChange 				= 25
	};

/** Global panic function for use within System Starter component
@internalComponent
@deprecated
*/
extern void PanicNow(const TDesC &aCategory, TInt aReason);

#endif // __SYSSTARTPANICCODES_H__
