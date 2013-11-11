/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32std.h>              // basic types

#include <ecom/implementationproxy.h> // for TImplementationProxy
#include <lbspositioninfo.h>     // for TPositionInfoBase

#include <lbs/epos_mpositionerstatus.h>

#include "lcfpsydummy1.h"
#include "lcfpsydummy1.hrh"


const TImplementationProxy KFactoryPtr =
	{{KLcfPsyDummy1ImplUid}, (TProxyNewLPtr)CLcfPsyDummy1::NewL};

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }
#endif

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount = 1;
    return &KFactoryPtr;
    }


CLcfPsyDummy1* CLcfPsyDummy1::NewL(TAny* aConstructionParameters)
    {
    CLcfPsyDummy1* self = new (ELeave) CLcfPsyDummy1;

    CleanupStack::PushL(self);
    self->ConstructL(aConstructionParameters);
    CleanupStack::Pop(self);

    return self;
    }

void CLcfPsyDummy1::ConstructL(TAny* aConstructionParameters)
    {
    // Must call BaseConstructL first thing during
    // construction.
    BaseConstructL(aConstructionParameters);
    }


CLcfPsyDummy1::~CLcfPsyDummy1()
    {
    // No resources to free
    }

//////////////////////////
// Methods overridden from CPositioner
//////////////////////////

void CLcfPsyDummy1::ServiceL(RMessage2& aMessage)
{
    switch (aMessage.Function())
    {
        default:
            break;
    }
}

TBool CLcfPsyDummy1::TrackingOverridden() const
{
    return ETrue;
}
void CLcfPsyDummy1::StartTrackingL(const TTimeIntervalMicroSeconds& aInterval)
{
    TTimeIntervalMicroSeconds interval(aInterval);
}
void CLcfPsyDummy1::StopTracking()
{
}

void CLcfPsyDummy1::NotifyPositionUpdate(TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus)
    {
    iStatusPtr = &aStatus; // iStatusPtr is a TRequestStatus*

    // NotifyPositionUpdate must return quickly. It should
    // be implemented as asynchronous but since Example PSY
    // is very fast, we fetch the position synchronously.
    TRAPD(err, GetPositionInfoL(aPosInfo));

    // test call to all methods from CPositioner
    TPositionQuality quality;
    GetRequiredPositionQuality(quality);

    TBool partial = IsPartialUpdateAllowed();

    TTime maxAge;
    GetMaxAge(maxAge);

    TPositionModuleStatus status;
    status.SetDataQualityStatus(TPositionModuleStatus::EDataQualityNormal);
    MPositionerStatus* observer = PositionerStatus();
    observer->ReportStatus(status);

    if (err)
        {
        User::RequestComplete(iStatusPtr, err);
        }
    } 


void CLcfPsyDummy1::GetPositionInfoL(TPositionInfoBase& aPosInfo)
    {
    // check that requested class is supported
    if (aPosInfo.PositionClassType() & 
        ~(EPositionInfoClass|
          EPositionGenericInfoClass))
        {
        // some more bits are set
        User::RequestComplete(iStatusPtr, KErrArgument);
        return;
        }   

    // PSY Tester bug fix: First of all clear HPositionGenericInfo fields,
    // if the parameter was of type HPositionGenericInfo
    if (aPosInfo.PositionClassType() &
        EPositionGenericInfoClass) // was EPositionClassInfo
        {
        HPositionGenericInfo* posInfo =
            static_cast<HPositionGenericInfo*>(&aPosInfo);
        
        // Clear old data 
        posInfo->ClearPositionData();
        }



    // Check which position information has been requested
    // and provide that information.
    if (aPosInfo.PositionClassType() &
        EPositionInfoClass) // was EPositionClassInfo
        {
        GetBasicPositionInfoL(aPosInfo);
        }

    if (aPosInfo.PositionClassType() &
        EPositionGenericInfoClass) // was EPositionClassInfo
        {
        GetGenericPositionInfoL(aPosInfo);
        }

    // Complete the request
    User::RequestComplete(iStatusPtr, KErrNone);
    }


void CLcfPsyDummy1::GetBasicPositionInfoL(TPositionInfoBase& aPosInfo)
    {
    // The position info object is at least a TPositionInfo
    TPositionInfo* posInfo =
        static_cast<TPositionInfo*>(&aPosInfo);

    TPosition pos;
    // Calculate the position and fill in the position info
    // object
    // Latitude, Longtitude, altitude
    pos.SetCoordinate(57.1, 11.3, 32.5);
    
    // set horizontal and vertical accuracy
    pos.SetAccuracy(40.0, 40.0);
    
    // set time of fix
    pos.SetCurrentTime();

    // Set position in position info.
    posInfo->SetPosition(pos);

    // Set the implementation uid     
    posInfo->SetModuleId(ImplementationUid());
    }


void CLcfPsyDummy1::GetGenericPositionInfoL(TPositionInfoBase& aPosInfo)
    {
    _LIT(KStreetDescriptor,  "Tieteenkatu 1");
    _LIT(KCityDescriptor,    "Tampere");
    _LIT(KCountryDescriptor, "Finland");

    // The position info object is at least a
    // HPositionGenericInfo
    HPositionGenericInfo* posInfo =
        static_cast<HPositionGenericInfo*>(&aPosInfo);

    // This PSY can deliver street, city and country
    // information. Attach this data if it is requested by
    // the client
    if (posInfo->IsRequestedField(EPositionFieldStreet))
        {
        User::LeaveIfError(posInfo->SetValue(
            EPositionFieldStreet, KStreetDescriptor));
        }
    if (posInfo->IsRequestedField(EPositionFieldCity))
        {
        User::LeaveIfError(posInfo->SetValue(
            EPositionFieldCity, KCityDescriptor));
        }
    if (posInfo->IsRequestedField(EPositionFieldCountry))
        {
        User::LeaveIfError(posInfo->SetValue(
            EPositionFieldCountry, KCountryDescriptor));
        }

    }


void CLcfPsyDummy1::CancelNotifyPositionUpdate()
    {
    // No asynchronous operations, i.e. nothing to cancel
    }


// End of file

