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
 @file te_locmonitorstep10.cpp
*/
 
#include "te_locmonitorstep10.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep10::~CTe_LocMonitorStep10()
	{
	}

CTe_LocMonitorStep10::CTe_LocMonitorStep10()
	{
	SetTestStepName(KLocMonitorStep10);
	}

TVerdict CTe_LocMonitorStep10::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	iBadPosition = new(ELeave) TPositionGpsMeasurementInfo();
	iAreaInfo = new(ELeave) TPositionAreaExtendedInfo();
	iAreaReqParams = new(ELeave) TPosAreaReqParams();
	iAreaReqParams->iPositionAreaType =  iAreaInfo->PositionClassType();
	
	//iWaiter = CLocMonWaiter::NewL();
	return TestStepResult();
	}

//
// Use of incorrect parameter in calls to RLbsLocMonitorAreaPositioner::GetLastKnownPositionArea, 
// RLbsLocMonitorAreaPositioner::GetLastKnownPositon and RLbsLocMonitorAreaPositioner::SetLastKnownPosition.
//
// The incorrect parameter is the "position" passed in, which is a TPositionGpsMeasurementInfo.
//
TVerdict CTe_LocMonitorStep10::doTestStepL()
	{
	TPosition receivedPosition;

 	if (TestStepResult()==EPass)
 		{ 
 		TInt err;
 		RLbsLocMonitorSession locMonSession;
 		User::LeaveIfError(locMonSession.Connect());
 		
 		RLbsLocMonitorAreaPositioner areaPositioner;
 		
 		areaPositioner.OpenL(locMonSession);
 		CleanupClosePushL(areaPositioner);
 		
 		TRequestStatus status;
 		
 		areaPositioner.GetLastKnownPositionArea(*iBadPosition, *iAreaInfo, *iAreaReqParams, status);
 		User::WaitForRequest(status);
 		// Check that the asynchronous request completed with KErrArgument
 		TESTL(KErrArgument == status.Int());
 		
 		areaPositioner.GetLastKnownPosition(*iBadPosition, status);
 		User::WaitForRequest(status);
 		// Check that the asynchronous request completed with KErrArgument
 		TESTL(KErrArgument == status.Int());
 		
		err = areaPositioner.SetLastKnownPosition(*iBadPosition);
 		// Check that the asynchronous request completed with KErrArgument
 		TESTL(KErrArgument == err);
		
 		CleanupStack::PopAndDestroy(&areaPositioner);
 		
 		locMonSession.Close();
		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep10::doTestStepPostambleL()
	{
	delete iBadPosition;
	delete iAreaInfo;
	delete iAreaReqParams;
	
	//delete iWaiter;
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
