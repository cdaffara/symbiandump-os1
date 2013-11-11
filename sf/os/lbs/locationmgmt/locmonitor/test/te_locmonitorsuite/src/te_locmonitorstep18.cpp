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
 @file te_locmonitorstep18.cpp
*/
 
#include "te_locmonitorstep18.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep18::~CTe_LocMonitorStep18()
	{
	}

CTe_LocMonitorStep18::CTe_LocMonitorStep18()
	{
	SetTestStepName(KLocMonitorStep18);
	}

TVerdict CTe_LocMonitorStep18::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	
	// This test requires one position...
	iPositions.Append(new(ELeave) TPositionSatelliteInfo());
	iPositions.Append(new(ELeave) TPositionSatelliteInfo());
		
	return TestStepResult();
	}

//
// This test verifies that is possible to
// cancel a GetLastKnownPosition request.
//
TVerdict CTe_LocMonitorStep18::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
 		// Ensure the test DB is handling the GetLastKnownPositionArea
 		// request when the cancel arrives.
 		SetShortDbResponseDelay();
 		
  		RLbsLocMonitorSession locMonSession;
 		User::LeaveIfError(locMonSession.Connect());
 		
 		RLbsLocMonitorAreaPositioner areaPositioner;
 		
 		areaPositioner.OpenL(locMonSession);
 		CleanupClosePushL(areaPositioner);
 		
 		TRequestStatus status;
 		
 		areaPositioner.GetLastKnownPosition(*iPositions[0], status);
 		areaPositioner.CancelGetLastKnownPosition();
 		
 		User::WaitForRequest(status);
 		
 		TESTL(KErrCancel == status.Int());
 		
 		// Check values
 		CleanupStack::PopAndDestroy(&areaPositioner);
 		locMonSession.Close();
		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep18::doTestStepPostambleL()
	{
	iPositions.ResetAndDestroy();

	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
