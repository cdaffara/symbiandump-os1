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
 @file te_locmonitorstep31.cpp
*/
 
#include "te_locmonitorstep31.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep31::~CTe_LocMonitorStep31()
	{
	}

CTe_LocMonitorStep31::CTe_LocMonitorStep31()
	{
	SetTestStepName(KLocMonitorStep31);
	}

TVerdict CTe_LocMonitorStep31::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	
	return TestStepResult();
	}

//
// This test checks that when an invalid position
// (coordinates set to NaN) is set on the GPS bus 
// the position is not sent by the Location Monitor
// to the database.
//
TVerdict CTe_LocMonitorStep31::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
 		
 		TPosition position; // no coordinates => invalid position

		RLbsLocMonitorSession locMonSession;
 		User::LeaveIfError(locMonSession.Connect()); 		
 		RLbsLocMonitorAreaPositioner areaPositioner;
 		areaPositioner.OpenL(locMonSession);

 		iLocMonDbListener->ListenForLocMonDbFeedback();
		iPositionInjector->InjectGpsPosition(position);
		
		// This should time out because the DB shouldn't be
		// asked to store the position and therefore it
		// won't send any feedback to the test.
		iLocMonDbListener->WaitForDbFeedback(); 
		
		CleanupStack::PopAndDestroy(&areaPositioner);
 		locMonSession.Close();
 		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep31::doTestStepPostambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
