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
 @file te_locmonitorstep1.cpp
*/
 
#include "te_geolocmonitorstep1.h"
#include "te_geolocmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>
#include <lbs/lbslocationinfo.h>

CTe_LocMonitorStep1::~CTe_LocMonitorStep1()
	{
	}

CTe_LocMonitorStep1::CTe_LocMonitorStep1()
	{
	SetTestStepName(KLocMonitorStep1);
	}

TVerdict CTe_LocMonitorStep1::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


void CTe_LocMonitorStep1::ReadIniFileValues()
    {
	
	_LIT(KMCC,"MCC");
	_LIT(KMNC,"MNC");
	_LIT(KLAC,"LAC");
	_LIT(KCID,"CID");
	_LIT(KTA,"TA");
	_LIT(KEXPECTEDERROR,"EXPECTEDERROR");
	

	iMCC	= -1; 
	iMNC	= -1; 
	iLAC	= -1; 
	iCID	= -1; 
	iTA		= -1;
	iExpectedError = KErrNone;
	
    GetIntFromConfig(ConfigSection(), KMCC, iMCC);
    GetIntFromConfig(ConfigSection(), KMNC, iMNC);
    GetIntFromConfig(ConfigSection(), KLAC, iLAC);
    GetIntFromConfig(ConfigSection(), KCID, iCID);
    GetIntFromConfig(ConfigSection(), KTA,  iTA);
    GetIntFromConfig(ConfigSection(), KEXPECTEDERROR,  iExpectedError);

    }


TVerdict CTe_LocMonitorStep1::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
		// here, read from .ini file
		ReadIniFileValues();
		
	
		INFO_PRINTF6(_L("TestValidateGSMInfoL(%d,%d,%d,%d,%d)"), iMCC, iMNC, iLAC, iCID, iTA);

		TInt result;
 	    TRAP( result, TestValidateGSMInfoL(iMCC, iMNC, iLAC, iCID, iTA); );
		if (iExpectedError == result)
			{
			INFO_PRINTF2(_L("TestValidateGSMInfoL passed. Got %d as expected"),iExpectedError);

			}
		else
			{
			INFO_PRINTF3(_L("TestValidateGSMInfoL failed, got error %d and was expecting %d "), result, iExpectedError);
			TESTL(0);
			}
	
 		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep1::doTestStepPostambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}

// -----------------------------------------------------------------------------
// Clocationinfodatatypestest::TestValidateGSMInfoL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CTe_LocMonitorStep1::TestValidateGSMInfoL(TInt iMCC, TInt iMNC, TInt iLAC, TInt iCID, TInt iTA)
   {
   CLbsGsmCellInfo* gsmInfo = CLbsGsmCellInfo::NewL();
   CleanupDeletePushL( gsmInfo );
   
   TLbsGsmNeighbourCellInfo nCell;
   nCell.SetArfcn( 100 );
   nCell.SetBsic( 30 );
   nCell.SetRxLevel( 20 );
   gsmInfo->AddNeighbouringCellInfoL( &nCell );
   
   gsmInfo->SetMobileCountryCode( iMCC );
   gsmInfo->SetMobileNetworkCode( iMNC );
   gsmInfo->SetLocationAreaCode( iLAC);
   gsmInfo->SetCellId( iCID);
   gsmInfo->SetTimingAdvance( iTA);
   
   gsmInfo->ValidateDataL();
   
   CleanupStack::PopAndDestroy();   // gsmInfo
   return KErrNone;
   }
