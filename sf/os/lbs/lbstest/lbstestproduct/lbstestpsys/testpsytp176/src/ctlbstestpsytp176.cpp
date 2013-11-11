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
//



// INCLUDE FILES
#include <lbspositioninfo.h>
#include "ctlbstestpsytp176.h"
#include "testpsytp176.hrh"


// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsyTp176::CT_LbsTestPsyTp176() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestPsyTp176::ConstructL(TAny* aConstructionParameters)
    {
    numberOfReq = 0;
    iHorAccuracy = 200;
    BaseConstructL(aConstructionParameters);
    }



// Two-phased constructor.
CT_LbsTestPsyTp176* CT_LbsTestPsyTp176::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestPsyTp176* self = new (ELeave) CT_LbsTestPsyTp176;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsyTp176* CT_LbsTestPsyTp176::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestPsyTp176* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestPsyTp176::~CT_LbsTestPsyTp176()
    {
    }

// ---------------------------------------------------------
// CT_LbsTestPsyTp176::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsyTp176::NotifyPositionUpdate(
	TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus)
	{
    TPositionInfo* position = static_cast<TPositionInfo*> (&aPosInfo);
    TRequestStatus* status = &aStatus;


    TTimeIntervalMicroSeconds someTime(1000000); // Add some timedifference

    if (numberOfReq == 0)
        {
        User::After(500000);
        }
    else if (numberOfReq == 1)
        {
        User::After(1000000);
        }
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
    // Just decrement (cant be less than zero, this causes the Eposserver to crash with
    // lbserror -13, EPositionBadAccuracy)
    iHorAccuracy-=10;
    

    newPosition.SetTime(tt);
    // set horizontal and vertical accuracy
    newPosition.SetAccuracy(iHorAccuracy, verAcc);

    position -> SetPosition(newPosition);


    // Check that classtype is supported

    //Since the test should be automatic a magic number is used as the error code
    //(very unlikely that EPos will send this error code).
    TUint32 classType = position->PositionClassType();
    if (classType != (classType & EPositionInfoClass))
        {
        User::RequestComplete(status, KErrHardwareNotAvailable); //Magic number -44
        return;
        }

    TUid implUid = { KPosImplementationUid };
    position->SetModuleId(implUid);
    

    // will verify CR052 that error code -2 will be returned if the uid is not correct.
    if ( numberOfReq == 2)
        {
	    TUid implUid;//totaly wrong value
	implUid.iUid = 0x4564556;
	    position->SetModuleId(implUid);
        }
    if ( numberOfReq == 3)
        {
	    TUid implUid;
        implUid.iUid = 0x01000001; //existing PSY
	    position->SetModuleId(implUid);
        }
    if ( numberOfReq == 4)
        {
	    TUid implUid;
        implUid.iUid = 0x01046001; //non existing PSY
	    position->SetModuleId(implUid);
        }
    numberOfReq++;
	User::RequestComplete(status, KErrNone);
    }



// ---------------------------------------------------------
// CT_LbsTestPsyTp176::CancelNotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsyTp176::CancelNotifyPositionUpdate()
	{
	}

