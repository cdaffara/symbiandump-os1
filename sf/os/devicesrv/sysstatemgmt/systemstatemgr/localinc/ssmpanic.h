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
// This header file defines all the possible panic categories and codes within the System State Management component.
// When adding panic codes to the System State Management component,
// identify the module concerned and create a category if necessary.
// The category should contain the name of the class where the panic is occuring.
// Try to reuse an existing panic code, otherwise add the new panic code 
// to the end of the enum list.
//

#if !defined (__SSMPANICCODES_H__)
#define __SSMPANICCODES_H__

#include <e32std.h>

/** 
Defines all the modules (categories) within System State Management component which can throw panics
Strings must not be longer than 16 characters or they will be truncated by User::Panic()
@internalComponent
@released
*/
_LIT(KPanicCmdList, 				"CCmdList");
_LIT(KPanicCmdAmastarter, 			"CCmdAmastarter");
_LIT(KPanicCmdStarterBase, 			"CCmdStarterBase");
_LIT(KPanicCmdStartApp, 			"CCmdStartApp");
_LIT(KPanicCmdStartProcess, 		"CCmdStartProcess");
_LIT(KPanicDeferredDeleter, 		"CSsmDeferredDeleter");
_LIT(KPanicCmdCustomCommand, 		"CCmdCustomCmd");
_LIT(KPanicCmdMultipleWait, 		"CCmdMultipleWait");
_LIT(KPanicCmdWaitForApparcInit, 	"CCmdApparcInit");
_LIT(KPanicCmdResourceReader,		"CmdResourceRead");
_LIT(KPanicCmdFinaliseDrives,		"CmdFinaliseDrives");
_LIT(KPanicCmdPersistHalAttributes,	"CmdPersistHalAttributes");
_LIT(KPanicSsmCommand,				"BadSsmCommand");
_LIT(KPanicSusAdaptionServer,		"SusAdaptionSrv");
_LIT(KPanicCustomCmdSecurityCheck, "CCustomCmdSecurityCheck");
_LIT(KPanicCustomCmdCoopSysPerformRfsActions, "CCustomCmdCoopSysPerformRfsActions");
_LIT(KPanicSsmUiSpecific, "CSsmUiSpecific");
_LIT(KPanicDeferrableCommand, "CSsmDeferrableCommand");
_LIT(KPanicStartupProperties, "CSsmStartupProperties");


/** Defines all possible panic codes within System State Management component
@internalComponent
@released
*/
enum TSsmPanicCodes
	{
	EIllegalMethodCallInClass			= 1,
	EInvalidRunLAction 					= 2,
	ECmdListBadIdx						= 3,
	ECmdBadCommand						= 4,
	ECmdNullPtr							= 5,
	ENotInitialized						= 6,
	ENotInitialized2					= 7,
	ENotInitialized3					= 8,
	EUnknownError						= 9,	
	EInternalStateError					= 10,
	ENonNullResourceFileEntries			= 11,
	ENullResourceFileEntries			= 12,
	ENoPreparedCommandList				= 13,
	EInUse1								= 14,
	EInUse2								= 15,
	EInUse3								= 16,
	EPropertyRetrieveError				= 17,
	EInvalidUid							= 18,
	EInvalidArgument					= 19,
	EInvalidDeferrebleCmdRegistration	= 20,
	EInvalidCmdType						= 21,
	EDuplicateSubstateInCommandList		= 22,
	ECmdExecutionFailed					= 23
	};

/** Global panic function for use within System State Management component
@internalComponent
@released
*/
extern void PanicNow(const TDesC &aCategory, TInt aReason);

#endif // __SSMPANICCODES_H__
