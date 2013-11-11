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



#include <e32std.h>              // basic types

#include <ecom/implementationproxy.h> // for TImplementationProxy
#include <lbspositioninfo.h>     // for TPositionInfoBase
#include <lbssatellite.h>        // for TPositionSatelliteInfo

#include <lbs/epos_mpositionerstatus.h>

#include "lcfpsy6.h"
#include "lcfpsy6.hrh"


const TImplementationProxy KFactoryPtr = {{KLcfPsy6ImplUid}, (TProxyNewLPtr)CLcfPsy6::NewL};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount = 1;
    return &KFactoryPtr;
    }

CLcfPsy6* CLcfPsy6::NewL(TAny* aConstructionParameters)
    {
    CLcfPsy6* self = new(ELeave) CLcfPsy6;

    CleanupStack::PushL(self);
    self->ConstructL(aConstructionParameters);
    CleanupStack::Pop(self);

    return self;
    }

void CLcfPsy6::ConstructL(TAny* aConstructionParameters)
{
    BaseConstructL(aConstructionParameters);
    iUid = ImplementationUid();
}


CLcfPsy6::~CLcfPsy6()
{
    iEvents.Close();
}

void CLcfPsy6::NewEvent(EEventType aEventType)
{
    TQueryEvent event;
    event.iEventType = aEventType;
    event.iTime.UniversalTime();
    if (iEvents.Count() > DEF_MAX_EVENT_COUNT) iEvents.Remove(0);
    iEvents.Append(event);
}

void CLcfPsy6::NewRequest(
                TPositionInfoBase* aPosInfo,
                TRequestStatus* aRequestStatus)
{
    iRequest.iRequestStatus = aRequestStatus;
    iRequest.iPositionInfoBase = aPosInfo;
    iRequest.iPostponed = EFalse;
    iRequest.iDelayed = EFalse;
    iRequest.iDelayTimer = NULL;
}

void CLcfPsy6::NotifyPositionUpdate(
                TPositionInfoBase& aPosInfo,
                TRequestStatus& aStatus)
{
    TRequestStatus* status = &aStatus;

    // save current information
    NewRequest(&aPosInfo, status);
    NewEvent(EUpdate);

    // check that requested class is supported
    if (aPosInfo.PositionClassType() &
        ~(EPositionInfoClass | EPositionGenericInfoClass))
    {
        // some more bits are set
        CompleteRequest(KErrArgument);
        return;
    }

    if (aPosInfo.PositionClassType() & EPositionInfoClass)
    {
        // provide basic functionality for checking purposes
        GetBasicPositionInfo(aPosInfo);
    }

    if (aPosInfo.PositionClassType() & EPositionGenericInfoClass)
    {
        // The position info object is at least a HPositionGenericInfo
        HPositionGenericInfo* posInfo =
            static_cast<HPositionGenericInfo*>(iRequest.iPositionInfoBase);

        if (posInfo->IsRequestedField(KPSY6FieldEventLogReset))
        {
            iEvents.Reset();
            NewEvent(EUpdate);
        }

        ProcessRequest();
    }
    else
    {
        CompleteRequest(KErrNone);
    }
}

void CLcfPsy6::GetBasicPositionInfo(TPositionInfoBase& aPosInfo)
{
    // The position info object is at least a TPositionInfo
    TPositionInfo* posInfo = static_cast<TPositionInfo*>(&aPosInfo);

    TPosition pos;
    // Just for the purpose to know that the PSY is working
    // we return 1 in all fields
    pos.SetCoordinate(1.0, 1.0, 1.0);
    pos.SetAccuracy(1.0, 1.0);
    posInfo->SetPosition(pos);
}

void CLcfPsy6::GetMandatoryPositionInfo(TPositionInfoBase& aPosInfo)
    {
    TPositionInfo* posInfo = static_cast<TPositionInfo*>(&aPosInfo);
    TPosition pos;
    posInfo->GetPosition(pos);
    pos.SetCurrentTime();
    posInfo->SetPosition(pos);
    posInfo->SetModuleId(iUid);
    }
    
void CLcfPsy6::ProcessRequest()
{
    // Here is the main stuff

    // The position info object is at least a HPositionGenericInfo
    HPositionGenericInfo* posInfo =
        static_cast<HPositionGenericInfo*>(iRequest.iPositionInfoBase);

    TInt returnCode = KErrNone;

    // Check commands from test client
    // do everything it says

    if (posInfo->IsRequestedField(KPSY6FieldDelayProcessing) && !returnCode)
    {
        if (!iRequest.iDelayed)
        {
            // delay completing of the request by the given timeout
            TTimeIntervalMicroSeconds delay;
            posInfo->GetValue(KPSY6FieldDelayProcessing, delay);

            TRAP(returnCode,
                iRequest.iDelayTimer = new(ELeave) CDelayTimer;
                iRequest.iDelayTimer->ConstructL();
            )
            if (returnCode == KErrNone)
            {
                iRequest.iDelayTimer->Start(0, delay.Int64(), this);
                iRequest.iDelayed = ETrue;
                *iRequest.iRequestStatus = KRequestPending;
            }
            return;
        }
    }
    if (posInfo->IsRequestedField(KPSY6FieldProcessingStartTime) && !returnCode)
    {
        // set current time in the field to check at client side
        TTime time;
        time.UniversalTime();
        posInfo->SetValue(KPSY6FieldProcessingStartTime, time);
    }

    if (posInfo->IsRequestedField(KPSY6FieldGetMaxAge) && !returnCode)
    {
        // obtain and return MaxUpdateAge parameter

        TTime maxAge(TInt64(0));
        this->GetMaxAge(maxAge);

        returnCode = posInfo->SetValue(KPSY6FieldGetMaxAge, maxAge);
    }

    if (posInfo->IsRequestedField(KPSY6FieldEventLog) && !returnCode)
    {
        // pack and return the log of events
        TRAP(returnCode,
            User::LeaveIfError(posInfo->SetValue(KPSY6FieldEventLogSize, TInt32(iEvents.Count())));

            TInt bufferSize = sizeof(TQueryEvent) * iEvents.Count();
            HBufC8* hbuf = HBufC8::NewLC(bufferSize);
            TQueryEvent* eventArray =
                    reinterpret_cast<TQueryEvent*>(const_cast<TUint8*>(hbuf->Ptr()));
            for (TInt i = 0; i < iEvents.Count(); i++)
            {
                eventArray[i] = iEvents[i];
            }
            TPtrC8 data(hbuf->Ptr(), bufferSize); // descriptor of the data
            User::LeaveIfError(posInfo->SetValue(KPSY6FieldEventLog, data));

            CleanupStack::PopAndDestroy(hbuf);
        );
    }

    if (posInfo->IsRequestedField(KPSY6FieldRequestedUid) && !returnCode)
    {
        // get requested uid from posInfo
        TInt32 requestedUid = KErrNone;
        posInfo->GetValue(KPSY6FieldRequestedUid, requestedUid);
        iUid.iUid = requestedUid;
    }
    else
    {
        iUid = ImplementationUid();
    }

    if (posInfo->IsRequestedField(KPSY6FieldLeaveOnStartTracking) && !returnCode)
    {
        iLeaveOnStartTracking = ETrue;
    }

    if (posInfo->IsRequestedField(KPSY6FieldRequestedResult) && !returnCode)
    {
        // get requested complete code from posInfo
        TInt32 requestedResult = KErrNone;
        posInfo->GetValue(KPSY6FieldRequestedResult, requestedResult);
        returnCode = requestedResult;
    }

    if (posInfo->IsRequestedField(KPSY6FieldReportStatus) && !returnCode)
    {
        // get requested module status from posInfo
        TPositionModuleStatus modStatus;
        TPckg<TPositionModuleStatus> status(modStatus);
        posInfo->GetValue(KPSY6FieldReportStatus, status);

        MPositionerStatus* observer = PositionerStatus();
        observer->ReportStatus(modStatus);
    }
    CompleteRequest(returnCode);
}

void CLcfPsy6::CompleteRequest(TInt aCompletionCode)
{
    // timer must be deleted
    if (iRequest.iDelayTimer)
    {
        delete iRequest.iDelayTimer;
        iRequest.iDelayTimer = NULL;
    }
    GetMandatoryPositionInfo(*iRequest.iPositionInfoBase);
    User::RequestComplete(iRequest.iRequestStatus, aCompletionCode);
}

void CLcfPsy6::CancelNotifyPositionUpdate()
{
    NewEvent(ECancel);
    CompleteRequest(KErrCancel);
}

void CLcfPsy6::TimerCompleted(TInt /*aTimerId*/)
{
    ProcessRequest();
}

TBool CLcfPsy6::TrackingOverridden() const
{
    return ETrue;
}

void CLcfPsy6::StartTrackingL(const TTimeIntervalMicroSeconds& /*aInterval*/)
{
    NewEvent(ETrackingStart);
    if (iLeaveOnStartTracking)
    {
        iLeaveOnStartTracking = EFalse;
        User::Leave(KErrGeneral);
    }
}

void CLcfPsy6::StopTracking()
{
    NewEvent(ETrackingStop);
}

// End of file

