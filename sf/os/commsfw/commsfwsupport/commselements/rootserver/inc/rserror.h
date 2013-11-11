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

#if !defined(__RSERROR_H__)
#define __RSERROR_H__

/**
@file 

@publishedPartner
@released
*/
/** A module has died, voluntarily or otherwise.
 */
const TInt KErrRSSuddenDeath                        = -17103;
/** A request was cancelled.
 */
const TInt KErrRSRequestCancelled					= -17104;

/** A parameter was in some way invalid.
 */
const TInt KErrRSInvalidParameter					= -17105;

/** The file could not be found, read or was in some way corrupt.
 */
const TInt KErrRSInvalidParameterFile				= -17106;

/** The name was invalid.
 */
const TInt KErrRSInvalidParameterName				= -17107;

/** The stack size was invalid.
 */
const TInt KErrRSInvalidParameterStackSize			= -17108;

/** The heap size was invalid.
 */
const TInt KErrRSInvalidParameterHeapSize			= -17109;

/** The second UID of the DLL was not the UID of a Comms Provider Module.
For a DLL to be a CPM it must have the second UID value of 0x101f7418.
 */
const TInt KErrRSInvalidUidType						= -17110;

/** A Comms Provider Module with this name is already loaded.
 */
const TInt KErrRSModuleAlreadyExist					= -17111;

/** The size of the MBuf pool is invalid.
 */
const TInt KErrRSInvalidMBufPoolSize				= -17112;

/** The specified module is not running.
 */
const TInt KErrRSModuleNotRunning					= -17113;

/** The specified module is not loaded.
 */
const TInt KErrRSModuleNotLoaded					= -17114;

/** The specified binding is already active.
 */
const TInt KErrRSAlreadyBound						= -17115;

/** There are running modules.
 */
const TInt KErrRSModulesStillRunning				= -17116;

/** Unable to create more new heaps in the Comms Process.
Consider sharing a heap between modules.
 */
const TInt KErrRSNoNewHeapsAvailable				= -17117;

/** A request timed out. This can happen when a module stops
responding to requests for some reason.
 */
const TInt KErrRSRequestTimedOut					= -17118;

/** The specified module does not exist..
 */
const TInt KErrRSModuleUnknown						= -17119;

/** The specified submodule does not exist.
 */
const TInt KErrRSSubModuleUnknown					= -17120;

/** The specified binding does not exist.
 */
const TInt KErrRSBindingUnknown						= -17121;

/** The status is unknown.
 */
const TInt KErrRSStatusUnknown						= -17122;

/** The specified binding exist.
 */
const TInt KErrRSBindingExists						= -17123;

/** The module is not ready for requests.
 */
const TInt KErrRSModuleNotReady						= -17124;

/** The specified binding is not yet completed.
 */
const TInt KErrRSBindingInProgress					= -17125;

/** Found heap but unable to open it for sharing.
 */
const TInt KErrRSUnableToOpenHeap					= -17126;

/** Unable to find specified heap.
 */
const TInt KErrRSUnableToFindHeap					= -17127;

/** Unable to create communication channels for new module.
 */
const TInt KErrRSUnableToCreateQueues				= -17128;

/** This Comms Provider Module exist but is not responding.
 */
const TInt KErrRSZombie								= -17129;

/** The specified ordinal does not exist for the DLL.
 */
const TInt KErrRSInvalidParameterThreadFuncOrdinal	= -17130;

/** Invalid binding.
 */
const TInt KErrRSInvalidBinding						= -17131;

/** Queue length of binding invalid. Must be between TRSBindingInfo::EMinQueueLength and TRSBindingInfo::EMaxQueueLength.
@see TRSBindingInfo
 */
const TInt KErrRSInvalidQueueLength					= -17132;
/** A module has died through a panic, Kill(), or other exception
 */
const TInt KErrRSSuddenViolentDeath                 = -17133;

/** A module (boot-time/OnDemand) involved in binding failed, hence if any of the two module(s) is/are On-Demand, 
the message needs to be completed with this error code to signal to caller that there are icomplete bindings
 */
const TInt KErrRSEitherModuleInBindingFailedToLoad 	= -17134;
#endif // __RSERROR_H__

