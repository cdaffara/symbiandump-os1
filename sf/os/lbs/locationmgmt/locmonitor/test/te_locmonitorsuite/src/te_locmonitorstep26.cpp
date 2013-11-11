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
 @file te_locmonitorstep26.cpp
*/
 
#include "te_locmonitorstep26.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep26::~CTe_LocMonitorStep26()
	{
	}

CTe_LocMonitorStep26::CTe_LocMonitorStep26()
	{
	SetTestStepName(KLocMonitorStep26);
	}

TVerdict CTe_LocMonitorStep26::doTestStepPreambleL()
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
// This test exercises the code in the Location Monitor 
// that handles an error code different from KErrNone 
// returned by the Database. The client (test) never
// gets to know that there was an error writing.
//
TVerdict CTe_LocMonitorStep26::doTestStepL()
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

		// Ask the DB to complete the next request it gets with
		// a negative error code.
		SetDbFailResponse();

 		areaPositioner.SetLastKnownPosition(positionInfo);

		User::After(1000); // Give the Db a chance to see the request sent above

		CleanupStack::PopAndDestroy(&areaPositioner);
 		locMonSession.Close();
 		}

	  return TestStepResult(); 		
	}

TVerdict CTe_LocMonitorStep26::doTestStepPostambleL()
	{
	iPositions.ResetAndDestroy();
	iAreaInfo.ResetAndDestroy();
	iAreaReqParams.ResetAndDestroy();
	
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
