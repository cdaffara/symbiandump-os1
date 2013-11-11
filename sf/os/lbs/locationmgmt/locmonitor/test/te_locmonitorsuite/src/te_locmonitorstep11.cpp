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
 @file te_locmonitorstep11.cpp
*/
 
#include "te_locmonitorstep11.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep11::~CTe_LocMonitorStep11()
	{
	}

CTe_LocMonitorStep11::CTe_LocMonitorStep11()
	{
	SetTestStepName(KLocMonitorStep11);
	}

TVerdict CTe_LocMonitorStep11::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	
	return TestStepResult();
	}

//
// This test checks that a SetLastKnowPosition request
// results in a request to store a position sent to the DB
//
TVerdict CTe_LocMonitorStep11::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
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

 		iLocMonDbListener->ListenForLocMonDbFeedback();
 		areaPositioner.SetLastKnownPosition(positionInfo);
 		iLocMonDbListener->WaitForDbFeedback(); 		
		
		// Check that the position received by the DB is the position
		// sent by the test
		TESTL(latitude == iLocMonDbListener->iDbData.iPosition.Latitude());
		TESTL(longitude == iLocMonDbListener->iDbData.iPosition.Longitude());
		
		CleanupStack::PopAndDestroy(&areaPositioner);
 		locMonSession.Close();
 		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep11::doTestStepPostambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
