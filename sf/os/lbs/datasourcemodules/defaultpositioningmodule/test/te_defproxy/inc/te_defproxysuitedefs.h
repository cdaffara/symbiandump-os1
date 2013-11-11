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
 @file Te_defproxySuiteDefs.h
 @internalTechnology
*/
#ifndef __TE_DEFPROXY_SUITEDEFS_H__
#define __TE_DEFPROXY_SUITEDEFS_H__

// For test step panics
_LIT(KTe_defproxySuitePanic,"Te_defproxySuite");

_LIT(KErrorCode, "Error code: %d");
_LIT(KOOMTextPath, "c:\\fr.txt");

_LIT(KValueReturned, "The returned value was: %d");
_LIT(KValueExpected, "We expected this value: %d");

_LIT(KWrongRequestResult, "Wrong request result");
_LIT(KWrongModuleIdReturned, "Wrong ModuleId returned");
_LIT(KWrongEventType, "Wrong event type");
_LIT(KWrongAmountOfEvent, "Wrong amount of events");
_LIT(KWrongEventNumberPSY6, "Wrong number of events in PSY6");
_LIT(KWrongUidAccepted, "Wrong Uid accepted");
_LIT(KWrongNullUidAccepted, "Null Uid accepted");
// For the base step
_LIT(KFailedConnectLocServer, "Failed to connect to LocServer");
_LIT(KFailedConnectDefProxy, "Failed to connect to DefProxy");
_LIT(KFailedPositionRequest, "Position request failed");
_LIT(KFailedSetRequestor, "Failed to set requestor");
_LIT(KFailedCancelInterval, "Failed to cancel during interval");
_LIT(KFailedCancelPsyDelay, "Failed to cancel during psy delay");
_LIT(KFailedStopTracking, "Failed to stop tracking");
_LIT(KFailedStartTracking, "Failed to start tracking");
_LIT(KFailedStartTracking2, "Failed to start 2nd tracking");
_LIT(KFailedSetOptions, "Failed to set options");
_LIT(KFailedReadLogsize, "Failed to read logsize");

_LIT(KBadResults, "Bad result");
_LIT(KBadFirstEventPSY6, "Bad first event in PSY6");
_LIT(KBadSecondEventPSY6, "Bad second event in PSY6");
_LIT(KBadThirdEventPSY6, "Bad third event in PSY6");

_LIT(KFirstDelayTooLong, "1st delay two long");
_LIT(KSecondDelayOutOfRange, "2nd delay out of range");
_LIT(KThirdDelayOutOfRange, "3rd delay out of range");
_LIT(KNormalDelayTooBig, "Normal delay too big");

_LIT(KRequestedModuleNotFound, "Requested module not found");
_LIT(KRequestNotCancelled, "Request wasn't cancelled");
_LIT(KUnexpectedEvent, "Unexpected event reported");
_LIT(KCannotCheckEventTimes, "Cannot check events times");
_LIT(KStartEventBeyondExpectendRange, "Tracking start event is beyond expected range");
_LIT(KStopEventBeyondExpectendRange, "Tracking stop event is beyond expected range");
_LIT(KResponseTimeDoesNotMatch, "Response time does not match");

_LIT(KLocationRequestSucceededTooSoon, "Location request succeeded from ExtGpsPsy1 too soon: %d");
_LIT(KLocationRequestStillNotCompleted, "Location request is still not completed from ExtGpsPsy1: %d");
_LIT(KLocationRequestStillCompleted, "Location request is still completed from ExtGpsPsy1: %d");

#endif
