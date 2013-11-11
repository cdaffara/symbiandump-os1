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
 @file te_locmonitorstep4.cpp
*/
 
#include "te_locmonitorstep4.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep4::~CTe_LocMonitorStep4()
	{
	}

CTe_LocMonitorStep4::CTe_LocMonitorStep4()
	{
	SetTestStepName(KLocMonitorStep4);
	}

TVerdict CTe_LocMonitorStep4::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	iPosition = new(ELeave) TPositionSatelliteInfo();
	return TestStepResult();
	}

//
// Repeatible call to GetLastKnownPosition using a TPositionSatelliteInfo.
//
// This test checks that it is possible to request the last known
// position from the Location Monitor using the above mentioned data type.
//
// The call to GetLastKnowPosition is conducted twice to check
// repeatibility.
//
TVerdict CTe_LocMonitorStep4::doTestStepL()
	{
	TPosition expectedPosition, receivedPosition;

 	if (TestStepResult()==EPass)
 		{
 		RLbsLocMonitorSession locMonSession;
 		User::LeaveIfError(locMonSession.Connect());
 		RLbsLocMonitorAreaPositioner areaPositioner;
 		
 		areaPositioner.OpenL(locMonSession);
 		CleanupClosePushL(areaPositioner);	
 		
 		TRequestStatus status;
 		
 		areaPositioner.GetLastKnownPosition(*iPosition, status);
 			
 		User::WaitForRequest(status);
 		
 		// Check that the position and area information are as expected
 		iPosition->GetPosition(receivedPosition);
 		TESTL(KLatGetLast1 == receivedPosition.Latitude());
 		TESTL(KLongGetLast1 == receivedPosition.Longitude());

		// Try again
		areaPositioner.GetLastKnownPosition(*iPosition, status);	
 		User::WaitForRequest(status);

 		// Check that the position and area information are as expected
 		iPosition->GetPosition(receivedPosition);
 		TESTL(KLatGetLast2 == receivedPosition.Latitude());
 		TESTL(KLongGetLast2 == receivedPosition.Longitude());

 		CleanupStack::PopAndDestroy(&areaPositioner);
 		locMonSession.Close();
		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep4::doTestStepPostambleL()
	{
	delete iPosition;
	
	//delete iWaiter;
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
