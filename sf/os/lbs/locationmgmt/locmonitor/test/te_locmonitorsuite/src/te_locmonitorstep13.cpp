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
 @file te_locmonitorstep13.cpp
*/
 
#include "te_locmonitorstep13.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep13::~CTe_LocMonitorStep13()
	{
	}

CTe_LocMonitorStep13::CTe_LocMonitorStep13()
	{
	SetTestStepName(KLocMonitorStep13);
	}

TVerdict CTe_LocMonitorStep13::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	
	// This test requires two positions...
	iPositions.Append(new(ELeave) TPositionSatelliteInfo());
	iPositions.Append(new(ELeave) TPositionSatelliteInfo());
	
	// ... and one of these
	iAreaInfo.Append(new(ELeave) TPositionAreaExtendedInfo());
	iAreaReqParams.Append(new(ELeave) TPosAreaReqParams());
	iAreaReqParams[0]->iPositionAreaType =  iAreaInfo[0]->PositionClassType();
	
	return TestStepResult();
	}

//
// This test checks that GetLastKnownPositionArea, 
// GetLastKnownPosition and SetLastKnowPosition can be
// issued simultaneously from the same subsession
// all of them are completed correctly.
//
TVerdict CTe_LocMonitorStep13::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
 		TPosition receivedPosition;
 		// Prepare a position for sending the Location Monitor		
 		TPositionInfo positionInfo;
 		TPosition position;
		TReal64 latitude(11), longitude(21);
		position.SetCoordinate(latitude, longitude);
		positionInfo.SetPosition(position);
 				
		RLbsLocMonitorSession locMonSession;
 		User::LeaveIfError(locMonSession.Connect()); 		
 		RLbsLocMonitorAreaPositioner areaPositioner;
 		areaPositioner.OpenL(locMonSession);
 		CleanupClosePushL(areaPositioner);
 		

		// Provoke a delay in the processing of the first message by the Location
		// Monitor DB to allow the messages to be queued in the server (this is
		// for testing the server queuing)
		SetShortDbResponseDelay(); // 1 second delay


		TRequestStatus status1;
 		areaPositioner.GetLastKnownPositionArea(*iPositions[0], *iAreaInfo[0], *iAreaReqParams[0], status1);
  		
 		TRequestStatus status2;
 		areaPositioner.GetLastKnownPosition(*iPositions[1], status2);
 		
		iLocMonDbListener->ListenForLocMonDbFeedback();
 		areaPositioner.SetLastKnownPosition(positionInfo);
 

 		User::WaitForRequest(status1);
 		TESTL(KErrNone == status1.Int());
 		// Check that the position and area information are as expected
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

		User::WaitForRequest(status2);
 		TESTL(KErrNone == status2.Int());		
 		// Check that the position and area information are as expected
 		iPositions[1]->GetPosition(receivedPosition);
 		TESTL(KLatGetLast2 == receivedPosition.Latitude());
 		TESTL(KLongGetLast2 == receivedPosition.Longitude());
 		
 		// Wait for feedback from the DB about the call to SetLastKnownPosition
 		iLocMonDbListener->WaitForDbFeedback();
		// Check that the position received by the DB is the position
		// sent by the test
		TESTL(latitude == iLocMonDbListener->iDbData.iPosition.Latitude());
		TESTL(longitude == iLocMonDbListener->iDbData.iPosition.Longitude()); 		

		// Check values
		CleanupStack::PopAndDestroy(&areaPositioner);
 		locMonSession.Close();
 		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep13::doTestStepPostambleL()
	{
	iPositions.ResetAndDestroy();
	iAreaInfo.ResetAndDestroy();
	iAreaReqParams.ResetAndDestroy();
	
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
