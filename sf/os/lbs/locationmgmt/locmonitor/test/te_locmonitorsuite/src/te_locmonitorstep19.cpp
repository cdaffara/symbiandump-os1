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
 @file te_locmonitorstep19.cpp
*/
 
#include "te_locmonitorstep19.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep19::~CTe_LocMonitorStep19()
	{
	}

CTe_LocMonitorStep19::CTe_LocMonitorStep19()
	{
	SetTestStepName(KLocMonitorStep19);
	}

TVerdict CTe_LocMonitorStep19::doTestStepPreambleL()
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
// a GetLastKnownPositionArea request without affecting
// the completion of the same request in other subsessions.
// In this test, the request cancelled is not the one that
// triggered the DB access but a request queued by the server.
//
TVerdict CTe_LocMonitorStep19::doTestStepL()
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
 		areaPositioner1.GetLastKnownPositionArea(*iPositions[0], *iAreaInfo[0], *iAreaReqParams[0], status1);
  		
 		TRequestStatus status2;
 		areaPositioner2.GetLastKnownPositionArea(*iPositions[1], *iAreaInfo[1], *iAreaReqParams[1], status2);
 		
 		TRequestStatus status3;
 		areaPositioner3.GetLastKnownPositionArea(*iPositions[2], *iAreaInfo[2], *iAreaReqParams[2], status3);
 		
 		TRequestStatus status31;
 		areaPositioner2.GetLastKnownPosition(*iPositions[3],status31);
 		
 		TRequestStatus status4;
 		areaPositioner4.GetLastKnownPositionArea(*iPositions[4], *iAreaInfo[3], *iAreaReqParams[3], status4);
 		
 		
 		//********
 		// Cancel the third request
 		//********
 		areaPositioner3.CancelGetLastKnownPositionArea();
 		
 		User::WaitForRequest(status3);
 		TESTL(KErrCancel == status3.Int());
 		
 		User::WaitForRequest(status1);
 		TESTL(KErrNone == status1.Int());
 		// Check that the position and area information are as expected
 		iPositions[0]->GetPosition(receivedPosition);
 		TESTL(KLatGetPos1 == receivedPosition.Latitude());
 		TESTL(KLongGetPos1 == receivedPosition.Longitude());
 		INFO_PRINTF2(_L("receivedarea %d"),(int)iAreaInfo[0]->Area());
 		INFO_PRINTF2(_L("Actualarea %d"), TPositionAreaInfo::EAreaCity);
		TESTL(TPositionAreaInfo::EAreaCity == iAreaInfo[0]->Area());
		TESTL(iAreaInfo[0]->MobileNetworkCodeMatch());
		TESTL(iAreaInfo[0]->MobileCountryCodeMatch());
		TESTL(iAreaInfo[0]->LocationAreaCodeMatch());
		TESTL(iAreaInfo[0]->CellIdMatch());

		User::WaitForRequest(status2);
 		TESTL(KErrNone == status2.Int());		
 		// Check that the second client is terminated with the same 
 		// results as the first (the server should complete all of the
 		// queued clients for the same request type with the results
 		// of the same read operation)
 		iPositions[1]->GetPosition(receivedPosition);
 		TESTL(KLatGetPos1 == receivedPosition.Latitude());
 		TESTL(KLongGetPos1 == receivedPosition.Longitude());
		
		TESTL(TPositionAreaInfo::EAreaCity == iAreaInfo[1]->Area());
		TESTL(iAreaInfo[1]->MobileNetworkCodeMatch());
		TESTL(iAreaInfo[1]->MobileCountryCodeMatch());
		TESTL(iAreaInfo[1]->LocationAreaCodeMatch());
		TESTL(iAreaInfo[1]->CellIdMatch());

 		User::WaitForRequest(status31);
 		TESTL(KErrNone == status31.Int());		
 		// Check that the position and area information are as expected
 		iPositions[3]->GetPosition(receivedPosition);
 		TESTL(KLatGetLast2 == receivedPosition.Latitude());
 		TESTL(KLongGetLast2 == receivedPosition.Longitude());
 		
 		User::WaitForRequest(status4);
 		TESTL(KErrNone == status4.Int());
 		 // Check that the fourth client is terminated with the same 
 		// results as the first (the server should complete all of the
 		// queued clients for the same request type with the results
 		// of the same read operation)
 		iPositions[4]->GetPosition(receivedPosition);
 		TESTL(KLatGetPos1 == receivedPosition.Latitude());
 		TESTL(KLongGetPos1 == receivedPosition.Longitude());
		
		TESTL(TPositionAreaInfo::EAreaCity == iAreaInfo[3]->Area());
		TESTL(iAreaInfo[3]->MobileNetworkCodeMatch());
		TESTL(iAreaInfo[3]->MobileCountryCodeMatch());
		TESTL(iAreaInfo[3]->LocationAreaCodeMatch());
		TESTL(iAreaInfo[3]->CellIdMatch());
 		

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

TVerdict CTe_LocMonitorStep19::doTestStepPostambleL()
	{
	iPositions.ResetAndDestroy();
	iAreaInfo.ResetAndDestroy();
	iAreaReqParams.ResetAndDestroy();
	
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
