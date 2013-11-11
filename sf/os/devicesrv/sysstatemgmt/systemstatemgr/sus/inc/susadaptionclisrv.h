// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SUSADAPTIONCLISRV_H__
#define __SUSADAPTIONCLISRV_H__

/**
@internalComponent
@released
*/
_LIT(KSusAdaptionServerName, "!SusAdaptionSrv");

/**
@internalComponent
@released
*/
/**
A Major version must be specifyed when creating a session with the server
@internalComponent
@released
*/
const TUint KSsmAdaptationMajorVersionNumber=0;
/**
A Minor version must be specifyed when creating a session with the server
@internalComponent
@released
*/
const TUint KSsmAdaptationMinorVersionNumber=1;
/**
@internalComponent
@released
*/
const TUint KSsmAdaptationBuildVersionNumber=1;
/**
@internalComponent
@released
*/
const TInt  KSsmFirstUsedAdaptationOpCode = 1;

/**
Op-codes used in message passing between client and server.
@internalComponent
@released
@capability 
*/
enum TSusAdaptionServerRequests
	{
	//State Adaptation related opcodes 
	ERequestCoopSysStateChange = KSsmFirstUsedAdaptationOpCode,
	ERequestCoopSysSelfTest,
	ERequestCoopSysPerformRestartActions,
	ERequestCoopSysPerformShutdownActions,
	ERequestCoopSysPerformRfsActions,
	ERequestCoopSysCancel,
	ENotifyCoopSysEvent,
	ENotifyCoopSysCancel,
	EGetLastCoopSysEvent,
	//SIM Adaptation related opcodes
	EGetSimOwned,
	EGetSimCancel,
	ENotifySimEvent,
	ENotifySimCancel,
	EGetLastSimEvent,
	// RTC Adaptation related opcodes
	EValidateRtc,
	ESetWakeupAlarm,
	EUnsetWakeupAlarm,
	ERtcCancel,
	//Misc Adaptation opcodes
	ESecurityStateChange,
	EGetGlobalStartupMode,
	EPrepareSimLanguages,
	EGetHiddenReset,
	EMiscCancel,
	EGetSimLanguagesArray,
	//Emergency call Rf Adaptation opcodes
	EActivateRfForEmergencyCall,
	EDeactivateRfForEmergencyCall,
	EEmergencyCallRfCancel,	
	//Opcodes used for testing purposes
	EDebugMarkHeap,
	EDebugMarkHeapEnd,
	EDebugCleanupAdaptations,
	EDebugSetHeapFailure,
	EDebugUnSetHeapFailure,
	//opcode for setting client as priorityclient
	ESetAsPriorityClient,
	//End of supported opcodes
	EEndOfSusAdaptionOpCodes	
	};
	
#endif
