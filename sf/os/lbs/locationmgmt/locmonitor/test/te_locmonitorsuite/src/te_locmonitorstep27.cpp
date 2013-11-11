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
 @file te_locmonitorstep27.cpp
*/
 
#include "te_locmonitorstep27.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep27::~CTe_LocMonitorStep27()
	{
	}

CTe_LocMonitorStep27::CTe_LocMonitorStep27()
	{
	SetTestStepName(KLocMonitorStep27);
	}

TVerdict CTe_LocMonitorStep27::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	
	// Instantiate objects required by this test
	for(TInt index = 0; index < 1; index++)
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
// This test checks that if the database takes too
// long to respond the user is informed of a timeout.
//
TVerdict CTe_LocMonitorStep27::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
 		RLbsLocMonitorSession locMonSession;
 		User::LeaveIfError(locMonSession.Connect()); 		
 		RLbsLocMonitorAreaPositioner areaPositioner;
 		areaPositioner.OpenL(locMonSession);
 		CleanupClosePushL(areaPositioner);
		// Ask the DB to complete the next request after a delay
		// that is longer than the Location Monitor timeout.
		SetLongDbResponseDelay();
		
		TRequestStatus status;
  		areaPositioner.GetLastKnownPositionArea(*iPositions[0], *iAreaInfo[0], *iAreaReqParams[0], status);

		// User is informed of a timeout
		User::WaitForRequest(status);
		INFO_PRINTF2(_L("expected status KErrTimedOut but getting %d"), status.Int());
		TESTL(KErrTimedOut== status.Int());
		
		
		// Try again, this time without delaying the DB response
		areaPositioner.GetLastKnownPositionArea(*iPositions[0], *iAreaInfo[0], *iAreaReqParams[0], status);

		User::WaitForRequest(status);
		TESTL(KErrNone== status.Int());
		
		CleanupStack::PopAndDestroy(&areaPositioner);
 		locMonSession.Close();
 		}

	  return TestStepResult(); 		
	}

TVerdict CTe_LocMonitorStep27::doTestStepPostambleL()
	{
	iPositions.ResetAndDestroy();
	iAreaInfo.ResetAndDestroy();
	iAreaReqParams.ResetAndDestroy();
	
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
