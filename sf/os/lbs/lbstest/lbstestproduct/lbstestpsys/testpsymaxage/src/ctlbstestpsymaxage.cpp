// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name     : CT_LbsTestPsyMaxAge.cpp
//




// INCLUDE FILES
#include <e32base.h> 
#include <lbs/epos_cpositioner.h>
#include "ctlbstestpsymaxage.h"
#include "testpsymaxage.hrh"

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsyMaxAge::CT_LbsTestPsyMaxAge() 
    {
    iMaxAge = TTime(0);
    // Set to 20 as in .rss file
    iHorAccuracy = 20;
    iEveryOther = ETrue;
    }

// EPOC default constructor can leave.
void CT_LbsTestPsyMaxAge::ConstructL(TAny* aConstructionParameters)
    {
	BaseConstructL(aConstructionParameters);
    }

// Two-phased constructor.
CT_LbsTestPsyMaxAge* CT_LbsTestPsyMaxAge::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestPsyMaxAge* self = new (ELeave) CT_LbsTestPsyMaxAge;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
	return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsyMaxAge* CT_LbsTestPsyMaxAge::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestPsyMaxAge* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestPsyMaxAge::~CT_LbsTestPsyMaxAge()
    {
    }

// ---------------------------------------------------------
// CT_LbsTestPsyMaxAge::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsyMaxAge::NotifyPositionUpdate(
	TPositionInfoBase& aPosInfo, 
	TRequestStatus& aStatus)
{
	TInt err = KErrNone;
    TTime now;
    now.UniversalTime();

    TTime maxAgeTime;
    GetMaxAge(maxAgeTime);
    TTimeIntervalMicroSeconds diff = now.MicroSecondsFrom(maxAgeTime);
    TTimeIntervalMicroSeconds someTime(1); // Add some timedifference

    TTime tt;
    tt.UniversalTime();
    //Request ID must be unique, use UniversalTime() as seed
    // to give a random number
    TInt64 seed = tt.Int64();
    TReal lat = 90 * Math::FRand(seed);
    TReal lon = 90 * Math::FRand(seed);
    TReal32 alt = (TReal32)(90 * Math::FRand(seed));
    TReal32 verAcc = 0;
    TPosition newPosition;

    newPosition.SetCoordinate(lat, lon, alt);
    tt = tt + someTime;
    // Just decrement (cant be less than zero, this causes the Eposserver to crash with
    // lbserror -13, EPositionBadAccuracy)
    iHorAccuracy-=5;
    if (iHorAccuracy<0)
        iHorAccuracy = 20;

    newPosition.SetTime(tt);
    // set horizontal and vertical accuracy
    newPosition.SetAccuracy(iHorAccuracy, verAcc);

    // Check if MaxAge is enabled
    if (maxAgeTime != TTime(0))
        {
        // Add a little extra (1.5 sec) to newTime just to be able to 
        // do test with only one client. The update interval is always at least
        // as big as MaxAge
        TTime newTime = iMaxAge + diff + TTimeIntervalMicroSeconds(1500000);

        if ( newTime < now)
            {
            // New position needed
            TPositionInfo* position = static_cast<TPositionInfo*> (&aPosInfo);
            TUid implUid = { KPosImplementationUid };
            position->SetModuleId(implUid);
            position->SetPosition(newPosition);
            iOldPositionInfo = *position;
            iMaxAge = now;
            }
        else 
            {
            // Keep old position
            TPosition pos = TPosition();
            iOldPositionInfo.GetPosition(pos);

            TReal64 lat = pos.Latitude();
            if (Math::IsFinite(lat))
                {
                // Keep old position
                TPositionInfo* position = static_cast<TPositionInfo*> (&aPosInfo);
                TUid implUid = { KPosImplementationUid };
                position->SetModuleId(implUid);
                TPosition pos = TPosition();
                iOldPositionInfo.GetPosition(pos);

                // set horizontal and vertical accuracy and check in TP that
                // this value is not changed in dynamic quality for maxage                
                if (!iEveryOther)
                    {
                    pos.SetAccuracy(iHorAccuracy, verAcc);
                    iEveryOther = ETrue;
                    }
                else 
                    {
                    iEveryOther = EFalse;
                    }
                position->SetPosition(pos);
                }
            else
                {
                // New position needed the first time
                TPositionInfo* position = static_cast<TPositionInfo*> (&aPosInfo);
                TUid implUid = { KPosImplementationUid };
                position->SetModuleId(implUid);
                position->SetPosition(newPosition);
                iOldPositionInfo = *position;
                iMaxAge = now;                
                }
            }
        }
    else
        {
        // MaxAge disabled
        // New position needed
        TPositionInfo* position = static_cast<TPositionInfo*> (&aPosInfo);
        TUid implUid = { KPosImplementationUid };
        position->SetModuleId(implUid);
        position->SetPosition(newPosition);
        iOldPositionInfo = *position;
        iMaxAge = now;
        }

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
}

// ---------------------------------------------------------
// CT_LbsTestPsyMaxAge::CancelNotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsyMaxAge::CancelNotifyPositionUpdate()
	{
	}

//  End of File  
