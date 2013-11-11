/**
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
*
*/



/**
 @file Te_locsrvSuiteDefs.h
 @internalTechnology
*/
#ifndef __TE_LOCSRV_SUITEDEFS_H__
#define __TE_LOCSRV_SUITEDEFS_H__

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

#endif
