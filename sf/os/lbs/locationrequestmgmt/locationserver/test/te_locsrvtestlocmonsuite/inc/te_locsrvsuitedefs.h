/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file define all the common values thoughout your test project
*
*/


/**
 @file 
 @internalTechnology
 @test
*/
#ifndef __TE_LOCSRVTESTLOCMON_SUITEDEFS_H__
#define __TE_LOCSRVTESTLOCMON_SUITEDEFS_H__


#include <e32cmn.h>

//TODO Check if these values could be placed in this file ? & If they are valid
// Values used to vary the behaviour of the Test Location Monitor Interface
// for different unit test cases in the Location Server
/*
 * Key values for test control properties
 */
const TUint KLbsLocMonitorTestLKPAreaKey 		= 0x10000000;
const TUint KLbsLocMonitorTestLKPosKey 			= 0x20000000;
const TUint KLbsLocMonitorTestSessionKey 		= 0x30000000;
const TUint KLbsLocMonitorTestSubSessionKey 	= 0x40000000;

// Dummy Test Component UIDs
const TInt KLocSrvTestSuiteUid	= 0x102869E6;
const TUid KLocSrvTestSuite		= {KLocSrvTestSuiteUid};


const TTimeIntervalMicroSeconds32 KGetLKPAReqPropagateDelay =     500;
const TTimeIntervalMicroSeconds32 KGetLKPAMultipleReqDelay  =     100;
const TTimeIntervalMicroSeconds32 KClearDBReqPropagateDelay =     500;
const TTimeIntervalMicroSeconds32 KClearDBMultipleReqDelay  =     100;
const TTimeIntervalMicroSeconds32 KGetLKPosReqPropagateDelay =    500;
const TTimeIntervalMicroSeconds32 KGetLKPosMultipleReqDelay =     100;

const TTimeIntervalMicroSeconds32 KServerHandleCloseDelay  =   50000;



// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KTe_locsrvSuiteInt,"Test");

// For test step panics
_LIT(KTe_locsrvSuitePanic, "Te_locsrvSuite");
_LIT(KPanicThreadName, "TestPanicThread");

_LIT(KDone, "Done.");

_LIT(KFailedConnectServer, "Failed to connect to server");
_LIT(KFailedOepenDefaultPSY, "Failed to open default psy");
_LIT(KFailedCheck, "Check failed");
_LIT(KFailedEqualCheck, "Equal check failed");
_LIT(KFailedPropertyCheck, "Property check failed");
_LIT(KFailedOpenListedModule, "Failed to open listed module");
_LIT(KFailedOpenPositioner, "Failed to open positioner");
_LIT(KFailedOpenPositionerByModuleID, "Failed to open positioner by moduleId");
_LIT(KFailedOpenModuleError, "Failed to open module \"%S\", error %d");
_LIT(KFailedGetLastKnownPosition, "GetLastKnownPosition failed!");
_LIT(KFailedReadLogSize, "Failed to read logsize");
_LIT(KFailedGetTrackingFix, "Failed to get tracking fix");
_LIT(KFailedLocationRequest, "Location request failed");
_LIT(KFailedStartFirstTrackingSession, "Failed to start 1st tracking session");
_LIT(KFailedCancelDuringInterval, "Failed to cancel during interval");
_LIT(KFailedStartSecondTrackingSession, "Failed to start 2nd tracking session");
_LIT(KFailedCancelDuringPsyDelay, "Failed to cancel during psy delay");
_LIT(KFailedCancelAfterStoppedSession, "Failed to cancel after stopped session");
_LIT(KFailedModuleStatusRequest, "Module Status request failed");
_LIT(KFailedCancelModuleStatusRequest, "Failed to cancel module status request");
_LIT(KFailedGetDefaultModuleId, "Failed to get default module id");
_LIT(KFailedGetModulesAmount, "Failed to get amount of modules");
_LIT(KFailedGetModulesByIndex, "Failed to get module by index");
_LIT(KFailedGetModulesByID, "Failed to get module by ID");
_LIT(KFailedGetModulesInfoFromCenRep, "Failed to get module info from CenRep");
_LIT(KFailedGetNumModules, "GetNumModules failed");
_LIT(KFailedGetModInfoByIndex, "GetModInfoByIndex failed");
_LIT(KFailedGetModStatus, "GetModStatus failed");

_LIT(KValueReturned,"The returned value was: %d");
_LIT(KValueExpected,"We expected this value: %d");

_LIT(KWrongRequestResult, "Wrong request result");
_LIT(KWrongModuleIdReturned, "Wrong ModuleId returned");
_LIT(KWrongErrorCode, "Wrong error code");
_LIT(KWrongOpenByCriteriaResult, "Wrong result of open-by-criteria");
_LIT(KWrongHandle, "Wrong handle");
_LIT(KWrongResult, "Wrong result");
_LIT(KWrongEventAmount, "Wrong amount of events");
_LIT(KWrongStatus, "Wrong status");
_LIT(KWrongQualityStatus, "Wrong quality status");

_LIT(KBadInitialState, "Bad initial state");
_LIT(KBadResult, "Bad result");

_LIT(KOnlyOneNaN, "Only one is NaN");
_LIT(KComparingWithCenRep, "Comparing with CenRep");
_LIT(KModulesNamesDiffer, "Module names differ");
_LIT(KSimulationPSYIgnoredRequiresInputFile, "SimulationPSY ([0x%08x]), it requires input file. Ignored");
_LIT(KOpeningModule, "Opening module #%d \"%S\" [0x%08x]...");
_LIT(KCantOpenDisabledModule, "Module \"%S\" is disabled, can't open.");
_LIT(KCheckingModule, "Checking module %d (%S)...");
_LIT(KOpenedBadModule, "Opened with bad module");
_LIT(KOpenedWithNullModule, "Opened with NULL module");
_LIT(KLastKnownPositionCorrupted, "Last Known Position is corrupted");
_LIT(KRequestNotCancelled, "Request not canceled");
_LIT(KTooFastTrackingFix, "Too fast tracking fix");
_LIT(KTooSlowTrackingFix, "Too slow tracking fix");
_LIT(KDelayOutOfRange, "Delay out of range");
_LIT(KCourseInfoClassSupported, "Course info class supported!");
_LIT(KSatelliteInfoClassSupported, "Satellite Info class supported!");
_LIT(KUnsupportedClassSupported, "Unsupported class IS supported!");
_LIT(KUpdateEventExpected, "Update event expected");
_LIT(KTrackingStartEventExpected, "TrackingStart event expected");
_LIT(KETrackingStopEventExpected, "ETrackingStop event expected");
_LIT(KComparedValuesNaN, "One or two of compared values is NaN!");
_LIT(KCheckingDeviceStatus, "Checking DeviceStatus %d");
_LIT(KModuleStatusRequestTimedOut, "Module Status request timed out");
_LIT(KEDeviceDisabledReportedFromPSY, "EDeviceDisabled reported from PSY");

_LIT(KLeaveInPanicThread, "LeaveInPanicThread");
_LIT(KUnexpectedErrorCreatingPanicThread, "Unexpected error while creating panic thread");
_LIT(KThreadDidntDieKillIt, "Thread didn't die. Killing it...");
_LIT(KThreadWasKilled, "Thread was Killed with Reason %d");
_LIT(KThreadWasTerminated, "Thread was Terminated with Reason %d");
_LIT(KUnexpectedPanicCategory, "Unexpected panic category: Actual '%S', Expected '%S'");
_LIT(KUnexpectedPanicReason, "Unexpected panic reason");
_LIT(KUnexpectedThreadExitType, "Unexpected thread exit type!");
_LIT(KUnexpectedEvents, "Unexpected events");

_LIT(KRemoveLastKnownPosdat, "Remove LastKnownPos.dat for this test case!");

_LIT(KTieteenkatu1, "Tieteenkatu 1");

_LIT(KFailedGLKPArea, "Failed to get the last known position area");
_LIT(KFailedCancelGLKPArea, "LastKnownPositionArea CANCEL request failed");
_LIT(KFailedGLKPAreaReqCancel, "Failed to cancel an outstanding LKPA req");
_LIT(KFailedClearDB, "Failed to clear the last known position database");
_LIT(KFailedClearDBCancel, "Failed to CANCEL the oustanding clear database request");
_LIT(KFailedCancelClearDB, "Clear database CANCEL request failed");
_LIT(KFailedCancelReqAfterClearDB, "Outstanding requests not cancelled after a clear database request");
_LIT(KFailedSpotIncorrectArg, "Location server failed to spot the incorrect argument from the client");
_LIT(KFailedGLKPAreaReqCancelDBCleared, "Failed to clear oustanding LKPArea req when the database is cleared");

_LIT(KFailedGLKPos, "Failed to get the last known position");
_LIT(KFailedCancelGLKPos, "LastKnownPosition CANCEL request failed");
_LIT(KFailedGLKPosReqCancel, "Failed to cancel an outstanding LKPos req");
_LIT(KFailedGLKPosReqCancelDBCleared, "Failed to clear oustanding LKPosition req when the database is cleared");

#endif
