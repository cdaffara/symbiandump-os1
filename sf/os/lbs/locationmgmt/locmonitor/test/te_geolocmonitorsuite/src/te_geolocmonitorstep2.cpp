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
 
#include "te_geolocmonitorstep2.h"
#include "te_geolocmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>
#include <lbs/lbslocationinfo.h>

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
	return TestStepResult();
	}


void CTe_LocMonitorStep2::ReadIniFileValues()
    {
	
	_LIT(KMCC,"MCC");
	_LIT(KMNC,"MNC");
	_LIT(KLAC,"LAC");
	_LIT(KUCID,"UCID");
	_LIT(KSC,"SC");
	_LIT(KEXPECTEDERROR,"EXPECTEDERROR");
	
	iMCC	= -1; 
	iMNC	= -1; 
	iLAC 	=-1;
	iUCID	= -1; 
	iSC		= -1; 
	iExpectedError = KErrNone;
		
    GetIntFromConfig(ConfigSection(), KMCC, iMCC);
    GetIntFromConfig(ConfigSection(), KMNC, iMNC);
    GetIntFromConfig(ConfigSection(), KLAC, iLAC);
    GetIntFromConfig(ConfigSection(), KUCID, iUCID);
    GetIntFromConfig(ConfigSection(), KSC, iSC);
    GetIntFromConfig(ConfigSection(), KEXPECTEDERROR,  iExpectedError);

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
		// here, read from .ini file
		ReadIniFileValues();
		
	
		INFO_PRINTF5(_L("TestValidateWCDMAInfoL(%d,%d,%d,%d)"), iMCC, iMNC, iUCID, iSC);

		TInt result;
		
		TRAP( result, TestValidateWCDMAInfoL(iMCC,iMNC,iUCID,iSC););
		if (iExpectedError == result)
			{
			INFO_PRINTF2(_L("TestValidateWCDMAInfoL passed. Got %d as expected"),iExpectedError);

			}
		else
			{
			INFO_PRINTF3(_L("TestValidateWCDMAInfoL failed, got error %d and was expecting %d"), result, iExpectedError);
			TESTL(0);
			}
		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep2::doTestStepPostambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}

TInt CTe_LocMonitorStep2::TestValidateWCDMAInfoL(TInt iMCC, TInt iMNC, TInt iUCID, TInt iSC )
   {
   CLbsWcdmaCellInfo* wcdmaInfo = CLbsWcdmaCellInfo::NewL();
   CleanupDeletePushL( wcdmaInfo );
   
   TLbsWcdmaNeighbourCellInfo nCell;
   nCell.SetUniqueCellId( 100 );
   nCell.SetScramblingCode( 30 );
   nCell.SetPathloss( 100 );
   nCell.SetSignalStrength( 90 );
   wcdmaInfo->AddNeighbouringCellInfoL( &nCell );
   
   wcdmaInfo->SetMobileCountryCode( iMCC);
   wcdmaInfo->SetMobileNetworkCode( iMNC);
   wcdmaInfo->SetUniqueCellId(iUCID);
   wcdmaInfo->SetScramblingCode( iSC  );
   
   wcdmaInfo->ValidateDataL();
   
   CleanupStack::PopAndDestroy();   // wcdmaInfo
   return KErrNone;
   }
