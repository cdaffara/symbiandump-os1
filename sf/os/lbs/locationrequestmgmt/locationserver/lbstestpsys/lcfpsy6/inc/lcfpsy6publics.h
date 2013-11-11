/*
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
*
*/



#ifndef LCFPSY6_PUBLICS
#define LCFPSY6_PUBLICS

#include <lbsfieldids.h>

/** HPositioneGenericInfo Field IDs used to request behavior
    or information from the PSY */
enum TPSY6PositionCommandFieldIds
{
    KPSY6FieldStart =                   EPositionFieldProprietaryFieldsBegin + 100,
    KPSY6FieldDelayProcessing,          /** PSY delays processing of the request 
                                            for timeout, defined in 
                                            TTimeIntervalMicroSeconds */
    KPSY6FieldGetMaxAge,                /** PSY returns result of GetMaxAge function.
                                            type of the field is TTime */
    KPSY6FieldEventLog,                 /** PSY returns list of last-logged events in
                                            a form of TQueryEvent[] as TDesC field */
    KPSY6FieldEventLogSize,             /** The Size of Event Log is return in the field */
    KPSY6FieldEventLogReset,            /** PSY will start event log from scratch */
    KPSY6FieldProcessingStartTime,      /** PSY sets home time when the request processing
                                            has been started */
    KPSY6FieldRequestedResult,          /** PSY will complete with request code. Use to get
                                            KPositionQualityLoss or 
                                            KPositionPartialUpdate */
    KPSY6FieldRequestedUid,             /** PSY will set requested Module Id. TUint32 */
    KPSY6FieldLeaveOnStartTracking,     /** PSY will leave on next StartTrackingL call */
    KPSY6FieldReportStatus,             /** PSY will ReportStatus before request 
                                            handling. Field must contain prepared
                                            TPositionModuleStatus object */
};

/** Maximum number of logged events in the PSY 
    The log Can Be Retrieved by KPSY6FieldEventLog command
 */
#define DEF_MAX_EVENT_COUNT             10

/** Event types */
enum EEventType 
{
    EUpdate,                            /** when NotifyPositionUpdate called */
    ECancel,                            /** when CancelNotifyPositionUpdate called */
    ETrackingStart,                     /** when StartTracking called */
    ETrackingStop                       /** when StopTracking called */
};

/** Event properties */
struct TQueryEvent
{
    EEventType      iEventType;
    TTime           iTime;
};

#endif // LCFPSY6_PUBLICS
