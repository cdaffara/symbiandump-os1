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
 @file te_locmonitorstep24.cpp
*/
 
#include "te_locmonitorstep24.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep24::~CTe_LocMonitorStep24()
	{
	}

CTe_LocMonitorStep24::CTe_LocMonitorStep24()
	{
	SetTestStepName(KLocMonitorStep24);
	}

TVerdict CTe_LocMonitorStep24::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	
	// Instantiate objects required by this test
	for(TInt index = 0; index < 12; index++)
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
// This test checks that after 10 request
// have been queued in the Location Monitor,
// the following requests are inmediately completed
// with KErrServerBusy.
//
TVerdict CTe_LocMonitorStep24::doTestStepL()
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
 		RLbsLocMonitorAreaPositioner areaPositioner5;
 		areaPositioner5.OpenL(locMonSession2);
 		CleanupClosePushL(areaPositioner5);
 		RLbsLocMonitorAreaPositioner areaPositioner6;
 		areaPositioner6.OpenL(locMonSession2);
 		CleanupClosePushL(areaPositioner6);

		// Provoke a delay in the processing of the first message by the Location
		// Monitor DB to ensure all of the following requests are queued in the
		// server (this is for testing the server queuing mechanism).
		SetShortDbResponseDelay();
 		
 		
 		// Send 10 requests. That should fill up the Location Monitor's request queue.
		TRequestStatus status1;
 		areaPositioner1.GetLastKnownPositionArea(*iPositions[0], *iAreaInfo[0], *iAreaReqParams[0], status1);
 		
 		TRequestStatus status11;
 		areaPositioner1.GetLastKnownPosition(*iPositions[1],status11);
  		
  		TRequestStatus status2;
 		areaPositioner2.GetLastKnownPositionArea(*iPositions[2], *iAreaInfo[1], *iAreaReqParams[1], status2);
 		
 		TRequestStatus status21;
 		areaPositioner2.GetLastKnownPosition(*iPositions[3], status21);
  		
  		TRequestStatus status3;
 		areaPositioner3.GetLastKnownPositionArea(*iPositions[4], *iAreaInfo[2], *iAreaReqParams[2], status3);
 		
 		TRequestStatus status31;
 		areaPositioner3.GetLastKnownPosition(*iPositions[5], status31);
   		
		TRequestStatus status4;
 		areaPositioner4.GetLastKnownPositionArea(*iPositions[6], *iAreaInfo[3], *iAreaReqParams[3], status4);
 		
 		TRequestStatus status41;
 		areaPositioner4.GetLastKnownPosition(*iPositions[7], status41);
  
		TRequestStatus status5;
 		areaPositioner5.GetLastKnownPositionArea(*iPositions[8], *iAreaInfo[4], *iAreaReqParams[4], status5);
 		
 		TRequestStatus status51;
 		areaPositioner5.GetLastKnownPosition(*iPositions[9], status51);

		// Send two more requests. They should be rejected with error code KErrServerBusy.
		TRequestStatus status6;
 		areaPositioner6.GetLastKnownPositionArea(*iPositions[10], *iAreaInfo[5], *iAreaReqParams[5], status6);
 		
 		TRequestStatus status61;
 		areaPositioner6.GetLastKnownPosition(*iPositions[11], status61);

		User::WaitForRequest(status6);
		TESTL(KErrServerBusy == status6.Int());
		
		User::WaitForRequest(status61);
		TESTL(KErrServerBusy == status61.Int());		

		// Check that first session's request
		// completes normally 		
 		User::WaitForRequest(status1);
 		TESTL(KErrNone == status1.Int());
 		iPositions[0]->GetPosition(receivedPosition);
 		TESTL(KLatGetPos1 == receivedPosition.Latitude());
 		TESTL(KLongGetPos1 == receivedPosition.Longitude());
 		INFO_PRINTF2(_L("receivedarea %d"), (int)iAreaInfo[0]->Area());
 		INFO_PRINTF2(_L("Actualarea %d"), TPositionAreaInfo::EAreaCity);
		TESTL(TPositionAreaInfo::EAreaCity == iAreaInfo[0]->Area());
		TESTL(iAreaInfo[0]->MobileNetworkCodeMatch());
		TESTL(iAreaInfo[0]->MobileCountryCodeMatch());
		TESTL(iAreaInfo[0]->LocationAreaCodeMatch());
		TESTL(iAreaInfo[0]->CellIdMatch());

		// Check that the 10th request completes with expected values.
		User::WaitForRequest(status51);
		User::WaitForRequest(status5);
 		TESTL(KErrNone == status51.Int());
 
 		// Check that the third client is terminated with the same 
 		// results as the first (the server should complete all of the
 		// queued clients for the same request type with the results
 		// of the same read operation)
 		iPositions[9]->GetPosition(receivedPosition);
 		TESTL(KLatGetLast2 == receivedPosition.Latitude());
 		TESTL(KLongGetLast2 == receivedPosition.Longitude());

		
		User::WaitForRequest(status2);
		User::WaitForRequest(status21);
		User::WaitForRequest(status3);
		User::WaitForRequest(status31);
		User::WaitForRequest(status4);
		User::WaitForRequest(status41);
		User::WaitForRequest(status11);


		CleanupStack::PopAndDestroy(&areaPositioner6);
		CleanupStack::PopAndDestroy(&areaPositioner5);
		CleanupStack::PopAndDestroy(&areaPositioner4);
		CleanupStack::PopAndDestroy(&areaPositioner3);
		CleanupStack::PopAndDestroy(&areaPositioner2);
		CleanupStack::PopAndDestroy(&areaPositioner1);
 		locMonSession.Close();
 		locMonSession2.Close();
 		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep24::doTestStepPostambleL()
	{
	iPositions.ResetAndDestroy();
	iAreaInfo.ResetAndDestroy();
	iAreaReqParams.ResetAndDestroy();
	
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
