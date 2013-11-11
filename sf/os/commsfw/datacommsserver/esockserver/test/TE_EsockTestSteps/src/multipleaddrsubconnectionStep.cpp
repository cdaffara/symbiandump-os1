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
 @file multipleaddrsubconnectionStep.cpp
*/

#include "Sockets.TestSteps.h"
#include "SubConnections.TestSteps.h"

#include <cdbcols.h>

CMultipleAddRSubConnectionStep::CMultipleAddRSubConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KMultipleAddRSubConnectionStep);
	}

TInt CMultipleAddRSubConnectionStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    if (GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1
        || iParams.iSocketName.Length()==0)
            return KErrNotFound;

    if (GetStringFromConfig(iSection,KTe_SubConnectionName,iParams.iSubConnectionName)!=1
        || iParams.iSubConnectionName.Length()==0)
            {
            INFO_PRINTF2(_L("%S: SubConnection name missing."),&iParams.iSocketName);
            return KErrNotFound;
            }

    return KErrNone;
    }

TVerdict CMultipleAddRSubConnectionStep::doSingleTestStep()
	{
/*
        TRequestStatus reqStat,reqStat2;
		TInt error, error2;
        error = iEsockTest->AddSocketToSubConnection(sockParams);
		if (error!=KErrNone)
            {
            INFO_PRINTF2(_L("Could not add socket (%d)!"),error);
            SetTestStepResult(EFail);
            }
        error2 = iEsockTest->AddSocketToSubConnection(sockParams,reqStat2);
        if (error==KErrNone && error2==KErrNone && iCancelFlag)
            {
            error = iEsockTest->CancelAddSocketToSubConnection(sockParams.iSubConnectionName);
            if (error!=KErrNone)
                {
                INFO_PRINTF2(_L("Could not cancel adding socket (%d)!"),error);
                SetTestStepResult(EFail);
                }
            }
		if (error==KErrNone)
			{
			User::WaitForRequest(reqStat);
			if (reqStat.Int()!=KErrNone)
				{
				INFO_PRINTF2(_L("Could not add socket (%d)!"),error);
				SetTestStepResult(EFail);
				}
			}
		if (error2==KErrNone)
			{
			User::WaitForRequest(reqStat2);
			if (reqStat2.Int()==KErrNone)
				{
				INFO_PRINTF1(_L("KErrNone returned on multiple RSubConnection::Add()!"));
				SetTestStepResult(EFail);
				}
			}
  */
  return TestStepResult();
	}

