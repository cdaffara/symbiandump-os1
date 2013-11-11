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
//

#if !defined (__SSMSERVERPANICCODES_H__)
#define __SSMSERVERPANICCODES_H__

#include <e32std.h>
#include "ssmpanic.h"

_LIT(KPanicSysStateMgr, "!SysStateMgr");

/** Defines all possible panic codes within System State Management Server
@internalComponent
@released
*/
enum TSsmServerPanicCodes
	{
	//
	// Generic errors
	//

	// CSsmServer errors
	// Failed to set SsmServer thread critical
	ESsmServerError1,
	// Failed to set SsmServer thread name
	ESsmServerError2,
	// NULL pointer in RequestStateTransitionL()
	ESsmServerError3,

	// CCleSessionProxy errors
	// Tried to connect an already connected session
	ESsmCCleSessionProxy1,

	//
	// Swp errors
	//

	// CSsmSwpPolicyResolver errors
	// The policy resolver wasn't able to load the DLL
	ESwpPolicyResolverLibraryNotLoaded,

	// CSsmSwpPolicyFrame errors
	// The internal policy pointer is not initialised in CallInitialize()
	ESwpPolicyFrameError1,
	// The internal policy pointer is not initialised in CallHandleCleReturnValueCancel()
	ESwpPolicyFrameError2,
	// The internal policy pointer is not initialised in CallTransitionAllowed()
	ESwpPolicyFrameError3,
	// The policy frame has not been initialised in CallTransitionAllowed()
	ESwpPolicyFrameError4,
	// The internal state was not correct for this operation in CallPrepareCommandList()
	ESwpPolicyFrameError5,
	// The requested key does not correspond to the loaded policy in CallPrepareCommandList()
	ESwpPolicyFrameError6,
	// The internal policy pointer is not initialised in CallPrepareCommandList()
	ESwpPolicyFrameError7,
	// The policy frame has not been initialised in CallPrepareCommandList()
	ESwpPolicyFrameError8,
	// The internal state was not correct for this operation in CallCommandList()
	ESwpPolicyFrameError9,
	// The internal policy pointer is not initialised in CallCommandList()
	ESwpPolicyFrameError10,
	// The policy frame has not been initialised in CallCommandList()
	ESwpPolicyFrameError11,
	// The internal state was not correct for this operation in CallHandleCleReturnValue()
	ESwpPolicyFrameError12,
	// The internal policy pointer is not initialised in CallHandleCleReturnValue()
	ESwpPolicyFrameError13,
	// The policy frame has not been initialised in CallHandleCleReturnValue()
	ESwpPolicyFrameError14,
	// The internal policy pointer is not initialised in CallInitializeCancel()
	ESwpPolicyFrameError15,
	// The internal policy pointer is not initialised in CallPrepareCommandListCancel()
	ESwpPolicyFrameError16,
	// The internal state was not correct for this operation in CallHandleCleReturnValueCancel()
	ESwpPolicyFrameError17,
	// The internal policy session proxy pointer is not initialised in CallInitialize()
	ESwpPolicyFrameError18,
	// The internal policy session proxy pointer is not initialised in CallPrepareCommandList()
	ESwpPolicyFrameError19,
	// The internal policy session proxy pointer is not initialised in CallCommandList()
	ESwpPolicyFrameError20,
	// The internal policy session proxy pointer is not initialised in CallHandleCleReturnValue()
	ESwpPolicyFrameError21,
	// The internal policy session proxy pointer is not initialised in CallInitializeCancel()
	ESwpPolicyFrameError22,
	// The internal policy session proxy pointer is not initialised in CallPrepareCommandListCancel()
	ESwpPolicyFrameError23,
	// The internal policy session proxy pointer is not initialised in CallHandleCleReturnValueCancel()
	ESwpPolicyFrameError24,

	// CSsmSwpRequestHandler errors
	// The internal transition scheduler pointer is not initialised in SubmitRequestL()
	ESwpRequestHandlerError1,
	// The internal transition scheduler pointer is not initialised in SubmitRequestL()
	ESwpRequestHandlerError2,
	// The internal transition scheduler pointer is not initialised in Cancel()
	ESwpRequestHandlerError3,
	// The internal transition scheduler pointer is not initialised in DoSubmitRequestL()
	ESwpRequestHandlerError4,
	// The internal CLE session proxy pointer is not initialised in DoSubmitRequestL()
	ESwpRequestHandlerError5,

	// CSsmSwpTransitionEngine errors
	// The CLE session proxy pointer parameter is null in SetCleSessionProxy()
	ESwpTransitionEngineError1,
	// The policy frame pointer parameter is null in SetSwpPolicyFrame()
	ESwpTransitionEngineError2,
	// The engine is already active in SubmitL()
	ESwpTransitionEngineError3,
	// The engine is already active in SubmitL()
	ESwpTransitionEngineError4,
	// The internal client status pointer is not null in SubmitL()
	ESwpTransitionEngineError5,
	// The internal state was not correct for this operation in DoSubmit()
	ESwpTransitionEngineError6,
	// The internal policy frame pointer is not initialised in DoCancel()
	ESwpTransitionEngineError7,
	// The internal policy frame pointer is not initialised in DoInitialize()
	ESwpTransitionEngineError8,
	// The internal policy frame pointer is not initialised in DoTransitionAllowed()
	ESwpTransitionEngineError9,
	// The internal policy frame pointer is not initialised in DoPrepareCommandList()
	ESwpTransitionEngineError10,
	// The internal policy frame pointer is not initialised in DoExecuteCommandListL()
	ESwpTransitionEngineError11,
	// The internal policy frame pointer is not initialised in DoHandleCleReturnValue()
	ESwpTransitionEngineError12,
	// The internal iCleSession pointer is not initialised in DoExecuteCommandListL()
	ESwpTransitionEngineError13,
	// The internal iCleSession pointer is not initialised in DoSubmit()
	ESwpTransitionEngineError14,
	// The internal iPolicyFrame pointer is not initialised in DoSubmit()
	ESwpTransitionEngineError15,
	// The command list returned by the Swp DLL implementation is not a valid Swp command list
	ESwpTransitionEngineError16,
	// iClientStatus unexpectedly not NULL when engine idle in DoCancel()
	ESwpTransitionEngineError17,
	// CSsmSwpTransitionEngine destructed before iClientMessage was completed
	ESwpTransitionEngineError18,
	// CSsmSwpTransitionEngine invalid state in RunL()
	ESwpTransitionEngineError19,
	// CSsmSwpTransitionEngine unexpected error in RunError()
	ESwpTransitionEngineError20,
	// The SsmSwpPolicySession proxy pointer parameter is null in SetSsmSwpPolicySessionProxy()
	ESwpTransitionEngineError21,
	// The internal policy frame pointer is not initialised in DoConnectSwpPolicySessionL()
	ESwpTransitionEngineError22,
	// The internal iSsmSwpPolicySession pointer is not initialised in DoSubmit()
	ESwpTransitionEngineError23,
	// CSsmSwpTransitionEngine unexpected error in  DoSubmit()
	ESwpTransitionEngineError24,
	
	// CSsmSwpTransitionScheduler errors
	// The queue is not empty on destruction in ~CSsmSwpTransitionScheduler()
	ESwpTransitionSchedulerError1,
	// The internal current transaction pointer is not null in DoSubmitL()
	ESwpTransitionSchedulerError2,
	// The scheduler is already active or not added to ActiveScheduler in ScheduleTransitionL()
	ESwpTransitionSchedulerError3,
	// The internal current transaction pointer is null in ScheduleTransitionL()
	ESwpTransitionSchedulerError4,
	// An ActiveScheduler return status error occured that caused RunError() to be called
	ESwpTransitionSchedulerError5,

	//
	// State errors
	//

	// CSsmStateTransitionEngine errors
	// An operation was attempted when a State Policy was not available
	ESsmStateEngineError1,
	// An attempt to create a CSsmStateTransitionEngine without MCleSessionProxy was detected
	ESsmStateEngineError2,
	// Failed to connect session to CleSrv in DoConnectCleSessionL()
	ESsmStateEngineError3,
	// Leave detected from virtual implementaion of MSsmStatePolicy::GetNextState
	ESsmStateEngineError4,
	// An attempt to create CSsmStateTransitionEngine without MSsmStatePolicyResolverProxy was detected
	ESsmStateEngineError5,
	// State Policy DLL failed to initialize
	ESsmStateEngineError6,
	// Failed to start transition, most likely due to a policy file error
	ESsmStateEngineError7,
	// State Policy DLL failed to prepare command list
	ESsmStateEngineError8,
	// Invalid state command list returned from policy DLL
	ESsmStateEngineError9,
	// DoSubmit called with intention EReplaceCurrentClearQueue without a preceding call to Cancel
	ESsmStateEngineError10,
	// Invalid state in RunL
	ESsmStateEngineError11,
	// Tried to connect an already connected session in DoConnectCleSessionL()
	ESsmStateEngineError12,
	// Attempt to dereference a NULL pointer in DoPrepareCommandList()
	ESsmStateEngineError13,
	// Attempt to dereference a NULL pointer in FurtherSubTransition()
	ESsmStateEngineError14,
	// Unknown error in CSsmStateTransitionEngine::RunError
	ESsmStateEngineError15,

	// CSsmStatePolicyFrame errors
	// CallInitialize called when the CSsmStatePolicyFrame was in another state than EInitialize
	ESsmStatePolicyError1,
	// CallInitialize called but CSsmStatePolicyFrame was already initialized
	ESsmStatePolicyError2,
	// CallPrepareCommandList called when the CSsmStatePolicyFrame was in another state than EPrepare
	ESsmStatePolicyError3,
	// CallPrepareCommandList called on the wrong MSsmStatePolicy implementation
	ESsmStatePolicyError4,
	// CallCommandList called when the CSsmStatePolicyFrame was in another state than EGet
	ESsmStatePolicyError5,
	// CallGetNextState called when the CSsmStatePolicyFrame was in another state than ENext
	ESsmStatePolicyError6,
	// Trying to use a Null pointer in CallInitialize
	ESsmStatePolicyError7,
	// Trying to use a Null pointer in CallInitializeCancel
	ESsmStatePolicyError8,
	// Trying to use a Null pointer in CallRelease
	ESsmStatePolicyError9,
	// Trying to use a Null pointer in CallTransitionAllowed
	ESsmStatePolicyError10,
	// Trying to use a Null pointer in CallPrepareCommandList
	ESsmStatePolicyError11,
	// Trying to use a Null pointer in CallPrepareCommandListCancel
	ESsmStatePolicyError12,
	// Trying to use a Null pointer in CallCommandList
	ESsmStatePolicyError13,
	// Trying to use a Null pointer in CallGetNextState
	ESsmStatePolicyError14,

	// CSsmStatePolicyResolver errors
	// Tried to use an RLibrary before it was loaded in CreatePolicyLC
	ESsmStateResolverError1,

	// CSsmStateTransitionRequest errors
	// CSsmStateTransitionRequest deleted before iMessage was completed
	ESsmTransitionRequestError1,
	
	// SSM failed to add the system state/startup domain hierarchy when starting
	EAddDomainHierarchyError,
	
	//SSM failed to create SUS 
	ESsmCreateSUSError,

	// CSsmSwpPolicyProxy errors
	// Tried to connect an already connected session
	ESsmCSsmSwpPolicySession1,
	
	// CSsmSwpPolicyServer errors
	// The internal policy pointer is not initialised in CallInitializeCancel()
	ESsmSwpPolicyServerError1,
	// The internal policy pointer is not initialised in CallPrepareCommandList()
	ESsmSwpPolicyServerError2,
	// The internal policy pointer is not initialised in CallPrepareCommandListCancel()
	ESsmSwpPolicyServerError3,
	// The internal policy pointer is not initialised in CallCommandList()
	ESsmSwpPolicyServerError4,
	// The internal policy pointer is not initialised in CallHandleCleReturnValue()
	ESsmSwpPolicyServerError5,
	// The internal policy pointer is not initialised in CallHandleCleReturnValueCancel()
	ESsmSwpPolicyServerError6,

	// CallInitialize called when CSsmSwpPolicyStepCompletion is not in ENull state
	ESsmSwpPolicySrvStepError1,
	// CallPrepareCommandList called when CSsmSwpPolicyStepCompletion is not in EInitialize state
	ESsmSwpPolicySrvStepError2,
	// CallCommandListL called when CSsmSwpPolicyStepCompletion is not in EPrepareCommandList state
	ESsmSwpPolicySrvStepError3,
	// CallHandleCleReturnValue called when is not in ECallCommandList state
	ESsmSwpPolicySrvStepError4
	};

#endif // __SSMSERVERPANICCODES_H__
