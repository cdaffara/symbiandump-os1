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
 @file te_locmonitorstep3.cpp
*/
 
#include "te_locmonitorstep3.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep3::~CTe_LocMonitorStep3()
	{
	}

CTe_LocMonitorStep3::CTe_LocMonitorStep3()
	{
	SetTestStepName(KLocMonitorStep3);
	}

TVerdict CTe_LocMonitorStep3::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	iPosition = new(ELeave) TPositionSatelliteInfo();
	iAreaInfo = new(ELeave) TPositionAreaExtendedInfo();
	iAreaReqParams = new(ELeave) TPosAreaReqParams();
	iAreaReqParams->iPositionAreaType =  iAreaInfo->PositionClassType();
	
	return TestStepResult();
	}

//
// Call to GetLastKnownPositionArea using a TPositionInfo
// and a TPositionAreaInfo object.
//
// This test checks that it is possible to request the last known
// position with area information from the Location Monitor using
// the above mentioned data types.
//
TVerdict CTe_LocMonitorStep3::doTestStepL()
	{
	TPosition expectedPosition, receivedPosition;
	TPositionAreaInfo expectedAreaInfo;

 	if (TestStepResult()==EPass)
 		{
 
 		RLbsLocMonitorSession locMonSession;
 		User::LeaveIfError(locMonSession.Connect());
 		
 		RLbsLocMonitorAreaPositioner areaPositioner;
 		
 		areaPositioner.OpenL(locMonSession);
 		
 		TRequestStatus status;
 		
 		areaPositioner.GetLastKnownPositionArea(*iPosition, *iAreaInfo, *iAreaReqParams, status);
 			
 		User::WaitForRequest(status);
 		
 		// Check that the position and area information are as expected
 		iPosition->GetPosition(receivedPosition);
 		INFO_PRINTF2(_L("receivedLatitude %d"), (int)receivedPosition.Latitude());
 		INFO_PRINTF2(_L("receivedLongitude %d"), (int)receivedPosition.Longitude());
 		INFO_PRINTF2(_L("ActualLatitude %d"), KLatGetPos1);
 		INFO_PRINTF2(_L("ActualLongitude %d"), KLongGetPos1);
 		TESTL(KLatGetPos1 == receivedPosition.Latitude());
 		TESTL(KLongGetPos1 == receivedPosition.Longitude());
		
		TESTL(TPositionAreaInfo::EAreaCity == iAreaInfo->Area());

 		areaPositioner.Close();
 		
 		locMonSession.Close();
		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep3::doTestStepPostambleL()
	{
	delete iPosition;
	delete iAreaInfo;
	delete iAreaReqParams;
	
	//delete iWaiter;
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
