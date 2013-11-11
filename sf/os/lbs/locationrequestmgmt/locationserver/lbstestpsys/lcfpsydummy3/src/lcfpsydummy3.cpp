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

#include "lcfpsydummy3.h"
#include "lcfpsydummy3.hrh"


const TImplementationProxy KFactoryPtr =
        {{KLcfPsyDummy3ImplUid}, (TProxyNewLPtr)CLcfPsyDummy3::NewL};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount = 1;
    return &KFactoryPtr;
    }


CLcfPsyDummy3* CLcfPsyDummy3::NewL(TAny* aConstructionParameters)
    {
    CLcfPsyDummy3* self = new (ELeave) CLcfPsyDummy3;

    CleanupStack::PushL(self);
    self->ConstructL(aConstructionParameters);
    CleanupStack::Pop(self);

    return self;
    }

void CLcfPsyDummy3::ConstructL(TAny* aConstructionParameters)
    {
    // Must call BaseConstructL first thing during
    // construction.
    BaseConstructL(aConstructionParameters);
    }


CLcfPsyDummy3::~CLcfPsyDummy3()
    {
    // No resources to free
    }



void CLcfPsyDummy3::NotifyPositionUpdate(TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus)
    {
    iStatusPtr = &aStatus; // iStatusPtr is a TRequestStatus*

    // NotifyPositionUpdate must return quickly. It should
    // be implemented as asynchronous but since Example PSY
    // is very fast, we fetch the position synchronously.
    TRAPD(err, GetPositionInfoL(aPosInfo));

    if (err)
        {
        User::RequestComplete(iStatusPtr, err);
        }
    }


void CLcfPsyDummy3::GetPositionInfoL(TPositionInfoBase& aPosInfo)
    {
    // check that requested class is supported
    if (aPosInfo.PositionClassType() &
        ~(EPositionInfoClass|
          EPositionGenericInfoClass|
          EPositionCourseInfoClass|
          EPositionSatelliteInfoClass))
        {
        // some more bits are set
        User::RequestComplete(iStatusPtr, KErrArgument);
        return;
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

    if (aPosInfo.PositionClassType() &
        EPositionCourseInfoClass) // was EPositionClassInfo
        {
        GetPositionCourseInfoL(aPosInfo);
        }

    if (aPosInfo.PositionClassType() &
        EPositionSatelliteInfoClass) // was EPositionClassInfo
        {
        GetPositionSatelliteInfoL(aPosInfo);
        }

    // Complete the request
    User::RequestComplete(iStatusPtr, KErrNone);
    }


void CLcfPsyDummy3::GetBasicPositionInfoL(TPositionInfoBase& aPosInfo)
    {
    // The position info object is at least a TPositionInfo
    TPositionInfo* posInfo =
        static_cast<TPositionInfo*>(&aPosInfo);

    TPosition pos;
    // Calculate the position and fill in the position info
    // object
    pos.SetCoordinate(67.567, -12.34, 45.32);

    // set horizontal and vertical accuracy
    pos.SetAccuracy(150.0, 500.0);

    // set time of fix
    pos.SetCurrentTime();

    // Set position in position info.
    posInfo->SetPosition(pos);

    // Set the implementation uid

    posInfo->SetModuleId(ImplementationUid());
    }


void CLcfPsyDummy3::GetGenericPositionInfoL(TPositionInfoBase& /*aPosInfo*/)
    {
    // Empty implementation
    }

void CLcfPsyDummy3::GetPositionCourseInfoL(TPositionInfoBase& aPosInfo)
    {
    TPositionCourseInfo* courseInfo = static_cast<TPositionCourseInfo*>(&aPosInfo);
    TCourse course;

    // fill in course data
    course.SetHeading(DUMMY_HEADING);
    course.SetSpeed(DUMMY_SPEED);
    course.SetHeadingAccuracy(DUMMY_HEADING_ACCURACY);
    course.SetSpeedAccuracy(DUMMY_SPEED_ACCURACY);

    // store it in course info
    courseInfo->SetCourse(course);

    courseInfo->SetModuleId(ImplementationUid());
    }

void CLcfPsyDummy3::GetPositionSatelliteInfoL(TPositionInfoBase& aPosInfo)
    {
    TPositionSatelliteInfo* satInfo = static_cast<TPositionSatelliteInfo*>(&aPosInfo);

    // prepare data
    satInfo->ClearSatellitesInView();

    satInfo->SetHorizontalDoP(DUMMY_SAT_INFO_HDOP);
    satInfo->SetVerticalDoP(DUMMY_SAT_INFO_VDOP);
    satInfo->SetTimeDoP(DUMMY_SAT_INFO_TDOP);
    satInfo->SetSatelliteTime(DUMMY_SAT_INFO_TIME);

    TSatelliteData satData;

    // first satellite
    satData.SetAzimuth(DUMMY_AZIMUTH1);
    satData.SetElevation(DUMMY_ELEVATION1);
    satData.SetIsUsed(DUMMY_IS_USED1);
    satData.SetSatelliteId(DUMMY_SATELLITE_ID1);
    satData.SetSignalStrength(DUMMY_SIGNAL_STRENGTH1);
    // add to info
    satInfo->AppendSatelliteData(satData);

    // second satellite
    satData.SetAzimuth(DUMMY_AZIMUTH2);
    satData.SetElevation(DUMMY_ELEVATION2);
    satData.SetIsUsed(DUMMY_IS_USED2);
    satData.SetSatelliteId(DUMMY_SATELLITE_ID2);
    satData.SetSignalStrength(DUMMY_SIGNAL_STRENGTH2);
    // add to info
    satInfo->AppendSatelliteData(satData);
    }

void CLcfPsyDummy3::CancelNotifyPositionUpdate()
    {
    // No asynchronous operations, i.e. nothing to cancel
    }


// End of file

