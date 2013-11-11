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
 @file te_locmonitorstep30.cpp
*/
 
#include "te_locmonitorstep30.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep30::~CTe_LocMonitorStep30()
	{
	}

CTe_LocMonitorStep30::CTe_LocMonitorStep30()
	{
	SetTestStepName(KLocMonitorStep30);
	}

TVerdict CTe_LocMonitorStep30::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	
	return TestStepResult();
	}

//
// This test checks that a valid position set in the
// final-network-position bus is correctly read by the Location Monitor
// and sent to the database.
//
TVerdict CTe_LocMonitorStep30::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
 		TPosition position;
		TReal64 latitude(30), longitude(21);
		position.SetCoordinate(latitude, longitude);

		RLbsLocMonitorSession locMonSession;
 		User::LeaveIfError(locMonSession.Connect()); 		
 		RLbsLocMonitorAreaPositioner areaPositioner;
 		areaPositioner.OpenL(locMonSession);
 		CleanupClosePushL(areaPositioner);

 		iLocMonDbListener->ListenForLocMonDbFeedback();
		iPositionInjector->InjectFinalNetworkPosition(position);
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

TVerdict CTe_LocMonitorStep30::doTestStepPostambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
