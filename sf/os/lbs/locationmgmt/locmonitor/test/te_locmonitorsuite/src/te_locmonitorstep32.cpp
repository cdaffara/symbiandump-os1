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
*
*/


/**
 @file te_locmonitorstep32.cpp
*/
 
#include "te_locmonitorstep32.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>
#include <simtsy.h>

CTe_LocMonitorStep32::~CTe_LocMonitorStep32()
	{
	}

CTe_LocMonitorStep32::CTe_LocMonitorStep32()
	{
	SetTestStepName(KLocMonitorStep32);
	}

TVerdict CTe_LocMonitorStep32::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	
	// This test requires one position...
	iPositions.Append(new(ELeave) TPositionSatelliteInfo());
	
	// ... and one of these
	iAreaInfo.Append(new(ELeave) TPositionAreaExtendedInfo());
	iAreaReqParams.Append(new(ELeave) TPosAreaReqParams());
	iAreaReqParams[0]->iPositionAreaType =  iAreaInfo[0]->PositionClassType();
	
	return TestStepResult();
	}

//
// This test checks that when the client of the 
// Location Monitor calls GetLastKnownPositionArea
// but the Location Monitor has no Area information
// it will return the Last Known Position with 
// area info set to unknown.
// (PRECONDITION FOR THIS TEST TO PASS: the script
// must have set a SIM TSY configuration file that
// supplies an invalid Global Cell ID to the Location
// Monitor)
//
TVerdict CTe_LocMonitorStep32::doTestStepL()
	{
	if (TestStepResult()==EPass)
 		{
 		TPosition receivedPosition;
 
		// Open session and subsessions
		RLbsLocMonitorSession locMonSession1;
 		User::LeaveIfError(locMonSession1.Connect());
 
 		RLbsLocMonitorAreaPositioner areaPositioner1;
 		areaPositioner1.OpenL(locMonSession1);
		CleanupClosePushL(areaPositioner1);
		
		TRequestStatus status1;
 		areaPositioner1.GetLastKnownPositionArea(*iPositions[0], *iAreaInfo[0], *iAreaReqParams[0], status1);
  		
 		User::WaitForRequest(status1);
 		TESTL(KErrNone == status1.Int());
 		// Check that the position and area information are as expected
 		iPositions[0]->GetPosition(receivedPosition);
 		
 		// *** The returned latitude and longitude correspond to those
 		// the the DB returns when the Loc Mon calls GetLast(). Therefore
 		// expect KLatGetLast1/KLongGetLast1 instead of 
 		// KLastGetPos1/KLongGetPos1 *********************************
		INFO_PRINTF2(_L("received area info %d and expected 0"), iAreaInfo[0]->Area());
		
 		INFO_PRINTF2(_L("KLongGetLast1 %d"), KLongGetLast1);
		INFO_PRINTF2(_L("receivedLatitude %d"),(int)receivedPosition.Latitude());
 		INFO_PRINTF2(_L("receivedLongitude %d"),(int)receivedPosition.Longitude());
 		INFO_PRINTF2(_L("KLatGetLast1 %d"), KLatGetLast1);
 		INFO_PRINTF2(_L("KLongGetLast1 %d"), KLongGetLast1);
 
 		TESTL(KLatGetLast1 == receivedPosition.Latitude());
 		TESTL(KLongGetLast1 == receivedPosition.Longitude());
		
		TESTL(TPositionAreaInfo::EAreaUnknown == iAreaInfo[0]->Area());

		// Check values
		CleanupStack::PopAndDestroy(&areaPositioner1);
 		locMonSession1.Close();
 		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep32::doTestStepPostambleL()
	{
	iPositions.ResetAndDestroy();
	iAreaInfo.ResetAndDestroy();
	iAreaReqParams.ResetAndDestroy();
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	
	
	//Move to next cell in sim tsy config file to ensure
	// the Location Monitor DB has at least information about
	// one area/cell Id.
	RProperty::Set(KUidPSSimTsyCategory,
					KPSSimTsyTimersReduceTime, KReduceSimTsyTimers);
					User::After(4000000);
	
	
	return TestStepResult();
	}
