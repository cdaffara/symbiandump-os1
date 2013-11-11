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
 @file te_locmonitorstep20.cpp
*/
 
#include "te_locmonitorstep20.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep20::~CTe_LocMonitorStep20()
	{
	}

CTe_LocMonitorStep20::CTe_LocMonitorStep20()
	{
	SetTestStepName(KLocMonitorStep20);
	}

TVerdict CTe_LocMonitorStep20::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	
	// Instantiate objects required by this test
	for(TInt index = 0; index < 5; index++)
		{
		iPositions.Append(new(ELeave) TPositionSatelliteInfo());
		// ... and one of these
		iAreaInfo.Append(new(ELeave) TPositionAreaExtendedInfo());
		iAreaReqParams.Append(new(ELeave) TPosAreaReqParams());
		iAreaReqParams[index]->iPositionAreaType =  iAreaInfo[index]->PositionClassType();
		}

	return TestStepResult();
	}

//
// This test checks that it is possible to cancel
// a GetLastKnownPosition request without affecting
// the completion of the same request in other subsessions.
// In this test, the request cancelled is not the one that
// triggered the DB access but a requet queued by the server.
//
TVerdict CTe_LocMonitorStep20::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
 		TPosition receivedPosition;
				
		// Open sessions and subsessions
		RLbsLocMonitorSession locMonSession;
 		User::LeaveIfError(locMonSession.Connect()); 		
 		RLbsLocMonitorAreaPositioner areaPositioner1;
 		areaPositioner1.OpenL(locMonSession);
 		CleanupClosePushL(areaPositioner1);
		RLbsLocMonitorAreaPositioner areaPositioner2;
 		areaPositioner2.OpenL(locMonSession);
 		CleanupClosePushL(areaPositioner2);
		RLbsLocMonitorAreaPositioner areaPositioner3;
 		areaPositioner3.OpenL(locMonSession);
 		CleanupClosePushL(areaPositioner3);

 		RLbsLocMonitorSession locMonSession2;
 		User::LeaveIfError(locMonSession2.Connect()); 		
 		RLbsLocMonitorAreaPositioner areaPositioner4;
 		areaPositioner4.OpenL(locMonSession2);
 		CleanupClosePushL(areaPositioner4);

		// Provoke a delay in the processing of the first message by the Location
		// Monitor DB to allow the messages to be queued in the server (this is
		// for testing the server queuing mechanism)
		SetShortDbResponseDelay();
 		
		TRequestStatus status1;
 		areaPositioner1.GetLastKnownPosition(*iPositions[0], status1);
  		
 		TRequestStatus status2;
 		areaPositioner2.GetLastKnownPosition(*iPositions[1], status2);
 		
 		TRequestStatus status3;
 		areaPositioner3.GetLastKnownPosition(*iPositions[2], status3);
 		
 		TRequestStatus status31;
 		areaPositioner3.GetLastKnownPositionArea(*iPositions[3], *iAreaInfo[0], *iAreaReqParams[0], status31);
 		
 		TRequestStatus status4;
 		areaPositioner4.GetLastKnownPosition(*iPositions[4], status4);

 		
 		//********
 		// Cancel the third request
 		//********
 		areaPositioner3.CancelGetLastKnownPosition();
 		
 		User::WaitForRequest(status3);
 		TESTL(KErrCancel == status3.Int());
 		
 		User::WaitForRequest(status1);
 		TESTL(KErrNone == status1.Int());
 		// Check that the position and area information are as expected
 		iPositions[0]->GetPosition(receivedPosition);
 		TESTL(KLatGetPos1 == receivedPosition.Latitude());
 		TESTL(KLongGetPos1 == receivedPosition.Longitude());

		User::WaitForRequest(status2);
 		TESTL(KErrNone == status2.Int());		
 		// Check that the second client is terminated with the same 
 		// results as the first (the server should complete all of the
 		// queued clients for the same request type with the results
 		// of the same read operation)
 		iPositions[1]->GetPosition(receivedPosition);
 		TESTL(KLatGetPos1 == receivedPosition.Latitude());
 		TESTL(KLongGetPos1 == receivedPosition.Longitude());
		
 		User::WaitForRequest(status31);
 		TESTL(KErrNone == status31.Int());		
 		// Check that the position and area information are as expected
 		iPositions[3]->GetPosition(receivedPosition);
 		INFO_PRINTF2(_L("receivedLatitude %d"),(int)receivedPosition.Latitude());
 		INFO_PRINTF2(_L("receivedLongitude %d"),(int)receivedPosition.Longitude());
 	    INFO_PRINTF2(_L("ActualLatitude %d"), KLatGetPos2);
 	    INFO_PRINTF2(_L("ActualLongitude() %d"), KLongGetPos2);
 		TESTL(KLatGetPos2 == receivedPosition.Latitude());
 		TESTL(KLongGetPos2 == receivedPosition.Longitude());
		TESTL(TPositionAreaInfo::EAreaCountry == iAreaInfo[0]->Area());


 		User::WaitForRequest(status4);
 		TESTL(KErrNone == status4.Int());
 		// Check that the fourth queued client is also terminated with the same 
 		// results as the first (the server should complete all of the
 		// queued clients for the same request type with the results
 		// of the same read operation)
 		iPositions[4]->GetPosition(receivedPosition);
 		TESTL(KLatGetPos1 == receivedPosition.Latitude());
 		TESTL(KLongGetPos1 == receivedPosition.Longitude());

		// Check values
 		CleanupStack::PopAndDestroy(&areaPositioner4);
 		CleanupStack::PopAndDestroy(&areaPositioner3);
 		CleanupStack::PopAndDestroy(&areaPositioner2);
 		CleanupStack::PopAndDestroy(&areaPositioner1);
  		locMonSession.Close();
 		locMonSession2.Close();
 		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep20::doTestStepPostambleL()
	{
	iPositions.ResetAndDestroy();
	iAreaInfo.ResetAndDestroy();
	iAreaReqParams.ResetAndDestroy();
	
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
