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
 @file te_locmonitorstep12.cpp
*/
 
#include "te_locmonitorstep12.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep12::~CTe_LocMonitorStep12()
	{
	}

CTe_LocMonitorStep12::CTe_LocMonitorStep12()
	{
	SetTestStepName(KLocMonitorStep12);
	}

TVerdict CTe_LocMonitorStep12::doTestStepPreambleL()
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
// This test checks that GetLastKnownPositionArea and 
// GetLastKnownPosition can be issued simultaneously from
// the same subsession and both are completed correctly.
//
TVerdict CTe_LocMonitorStep12::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{		
 		TPosition receivedPosition;
		
		RLbsLocMonitorSession locMonSession;
 		User::LeaveIfError(locMonSession.Connect()); 		
 		RLbsLocMonitorAreaPositioner areaPositioner;
 		areaPositioner.OpenL(locMonSession);
 		CleanupClosePushL(areaPositioner);
 		
		// One second DB processing delay to ensure the
		// second request is received by the server before
		// the first terminates (test of server's queues).
		SetShortDbResponseDelay();

		TRequestStatus status1;
 		areaPositioner.GetLastKnownPositionArea(*iPositions[0], *iAreaInfo[0], *iAreaReqParams[0], status1);
  		
 		TRequestStatus status2;
 		areaPositioner.GetLastKnownPosition(*iPositions[1], status2);
 
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

		// Check values
 		CleanupStack::PopAndDestroy(&areaPositioner);
 		locMonSession.Close();
 		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep12::doTestStepPostambleL()
	{
	iPositions.ResetAndDestroy();
	iAreaInfo.ResetAndDestroy();
	iAreaReqParams.ResetAndDestroy();
	
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
