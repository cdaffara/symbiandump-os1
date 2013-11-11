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
 @file te_locmonitorstep2.cpp
*/
 
#include "te_locmonitorstep2.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep2::~CTe_LocMonitorStep2()
	{
	}

CTe_LocMonitorStep2::CTe_LocMonitorStep2()
	{
	SetTestStepName(KLocMonitorStep2);
	}

TVerdict CTe_LocMonitorStep2::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	iPosition = new(ELeave) TPositionSatelliteInfo();
	iAreaInfo = new(ELeave) TPositionAreaExtendedInfo();
	iAreaReqParams = new(ELeave) TPosAreaReqParams();
	iAreaReqParams->iPositionAreaType =  iAreaInfo->PositionClassType();
	
	//iWaiter = CLocMonWaiter::NewL();
	return TestStepResult();
	}

//
// Repeatible call to GetLastKnownPositionArea using a TPositionSatelliteInfo
// and a TPositionAreaExtendedInfo object.
//
// This test checks that it is possible to request the last known
// position with area information from the Location Monitor using the above
// mentioned data types.
//
// The call to GetLastKnowPositionArea is conducted twice to check
// repeatibility.
//
TVerdict CTe_LocMonitorStep2::doTestStepL()
	{
	TPosition receivedPosition;

 	if (TestStepResult()==EPass)
 		{
 
 		RLbsLocMonitorSession locMonSession;
 		User::LeaveIfError(locMonSession.Connect());
 		
 		RLbsLocMonitorAreaPositioner areaPositioner;
 		
 		areaPositioner.OpenL(locMonSession);
 		
 		TRequestStatus status;
 		
 		areaPositioner.GetLastKnownPositionArea(*iPosition, *iAreaInfo, *iAreaReqParams, status);
 			
 		User::WaitForRequest(status);
 		
 		TESTL(status.Int() == KErrNone);
 		
 		// Check that the position and area information are as expected
 		iPosition->GetPosition(receivedPosition);

 		INFO_PRINTF2(_L("received area info %d expected EAreaCity 700"),(int)iAreaInfo->Area());
 
 		INFO_PRINTF2(_L("receivedLatitude %d"),(int)receivedPosition.Latitude());
 		INFO_PRINTF2(_L("receivedLongitude %d"),(int)receivedPosition.Longitude());
 		INFO_PRINTF2(_L("ActualLatitude %d"), KLatGetPos1);
 		INFO_PRINTF2(_L("ActualLongitude %d"), KLongGetPos1);
 		TESTL(KLatGetPos1 == receivedPosition.Latitude());
 		TESTL(KLongGetPos1 == receivedPosition.Longitude());
		
		TESTL(TPositionAreaInfo::EAreaCity == iAreaInfo->Area());
		TESTL(iAreaInfo->MobileNetworkCodeMatch());
		TESTL(iAreaInfo->MobileCountryCodeMatch());
		TESTL(iAreaInfo->LocationAreaCodeMatch());
		TESTL(iAreaInfo->CellIdMatch());

		// Try again
		areaPositioner.GetLastKnownPositionArea(*iPosition, *iAreaInfo, *iAreaReqParams, status);	
 		User::WaitForRequest(status);
		
 		// Check that the position and area information are as expected
 		iPosition->GetPosition(receivedPosition);
 		TESTL(KLatGetPos2 == receivedPosition.Latitude());
 		TESTL(KLongGetPos2 == receivedPosition.Longitude());
 
		TESTL(TPositionAreaInfo::EAreaCountry == iAreaInfo->Area());
		TESTL(iAreaInfo->MobileCountryCodeMatch());
		TESTL(!iAreaInfo->MobileNetworkCodeMatch());
		TESTL(!iAreaInfo->LocationAreaCodeMatch());
		TESTL(!iAreaInfo->CellIdMatch());		
		
 		areaPositioner.Close();
 		
 		locMonSession.Close();
 		
 		// Wait 3 seconds for the server to go down
 		//User::After(3000000);
		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep2::doTestStepPostambleL()
	{
	delete iPosition;
	delete iAreaInfo;
	delete iAreaReqParams;
	
	//delete iWaiter;
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
