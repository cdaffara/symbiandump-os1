// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
_LIT(KPanicSsmCommand,				"BadSsmCommand");
_LIT(KPanicSusAdaptionServer,		"SusAdaptionSrv");
_LIT(KPanicCustomCmdSimSecurityCheck, "CCustomCmdSimSecurityCheck");
_LIT(KPanicCustomCmdCoopSysPerformRfsActions, "CCustomCmdCoopSysPerformRfsActions");
_LIT(KPanicSsmUiSpecific, "CSsmUiSpecific");
_LIT(KPanicCustomCmdDeviceSecurityCheck, "CCustomCmdDeviceSecurityCheck");
_LIT(KPanicSecurityCheckNotifier, "CSsmSecurityCheckNotifier");
_LIT(KPanicSecurityNoteController, "CStrtSecurityNoteController");


/** Defines all possible panic codes within System State Management component
@internalComponent
@released
*/
enum TSsmPanicCodes
	{
	EIllegalMethodCallInClass 	= 1,
	ENoCustomCommandObject,
	EInvalidRunLAction,
	ECmdListBadIdx,
	ECmdBadCommand,
	ECmdNullPtr,
	ENotInitialized,
	ENotInitialized2,
	ENotInitialized3,
	EUnknownError,
	EInternalStateError,
	ENonNullResourceFileEntries,
	ENullResourceFileEntries,
	ENoPreparedCommandList,
	EInUse1,
	EInUse2,
	EInUse3,
	EPendingRequest,
	EPropertyRetrieveError,
	EInvalidUid,
	EInvalidArgument,
	EInvalidSecuritySubState,
	EInvalidDeviceSecurityState,
	};

/** Global panic function for use within System State Management component
@internalComponent
@released
*/
extern void PanicNow(const TDesC &aCategory, TInt aReason);

#endif // __SSMPANICCODES_H__
