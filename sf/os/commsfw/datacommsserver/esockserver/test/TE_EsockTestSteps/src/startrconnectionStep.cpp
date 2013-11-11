// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file startrconnectionStep.cpp
*/

#include "Connections.TestSteps.h"
#include "Sockets.TestSteps.h"
#include "cdbcols.h"



CStartRConnectionStep::CStartRConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KStartRConnectionStep);
	}

TInt CStartRConnectionStep::ConfigureFromIni()
	{
	iParams.Reset();
	
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_ConnectionName, iParams.iConnectionName) != 1)
		|| (iParams.iConnectionName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
		
	TPtrC16 bearerSet;
	TBool bearerPresent = ((GetStringFromConfig(iSection,KTe_BearerName,bearerSet)==1)
		&& (bearerSet.Length()>0));

	TBool IAPpresent = (GetIntFromConfig(iSection,KTe_IAPIndexName,iParams.iIAP)==1);
	TBool SNAPpresent = (GetIntFromConfig(iSection,KTe_SNAPIndexName,iParams.iSNAP)==1);

	iParams.iStartWithOldPreferences = bearerPresent && IAPpresent;
	iParams.iStartWithSnapPreferences = SNAPpresent;

	if (iParams.iStartWithOldPreferences && iParams.iStartWithSnapPreferences)
		{
		INFO_PRINTF2(_L("Connection preferences for (%S) have both snap and IAP."),&iParams.iConnectionName);
		return KErrNotFound;
		}

	//with preferences?
	if (iParams.iStartWithOldPreferences)
		{
		if (bearerSet.Compare(KTe_CommDbBearerCSD)==0)
			{ iParams.iBearer = KCommDbBearerCSD; }
		else if (bearerSet.Compare(KTe_CommDbBearerWcdma)==0)
			{ iParams.iBearer = KCommDbBearerWcdma; }
		else if (bearerSet.Compare(KTe_CommDbBearerLAN)==0)
			{ iParams.iBearer = KCommDbBearerLAN; }
		else
			{
			INFO_PRINTF3(_L("%S: Bearer type (%S) not recognised."),&iParams.iConnectionName,&bearerSet);
			return KErrNotFound;
			}
		}

	if (!GetIntFromConfig(iSection,KTe_ExpectedError,iExpectedError))  
		{
		iExpectedError = KErrNone;
		}

	else 
		{
		INFO_PRINTF2(_L("Error to be expected: %d"),iExpectedError);
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CStartRConnectionStep::doSingleTestStep()
	{
    TInt error = iEsockTest->StartConnection(iParams);

	if (error!=KErrNone)
          {
          INFO_PRINTF2(_L("Could not start connection (%S)."),&iParams.iConnectionName);
          INFO_PRINTF2(_L("Error: %d."),error);
          } 

    if (error!=iExpectedError)
          {
          INFO_PRINTF3(_L("Returned error (%d) differs from expected (%d)."), error, iExpectedError);
          return EFail;
          }
          
         return EPass;
         
	}


