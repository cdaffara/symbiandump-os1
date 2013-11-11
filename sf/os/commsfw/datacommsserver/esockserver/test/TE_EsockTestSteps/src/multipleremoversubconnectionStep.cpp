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
 @file multipleremoversubconnectionStep.cpp
*/

#include "SubConnections.TestSteps.h"
#include "Sockets.TestSteps.h"

#include <cdbcols.h>

CMultipleRemoveRSubConnectionStep::CMultipleRemoveRSubConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KMultipleRemoveRSubConnectionStep);
	}

TInt CMultipleRemoveRSubConnectionStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    if (GetStringFromConfig(iSection,KTe_SocketName, iParams.iSocketName)!=1
        || iParams.iSocketName.Length()==0)
            return KErrNotFound;

    if (GetStringFromConfig(iSection,KTe_SubConnectionName,iParams.iSubConnectionName)!=1
        || iParams.iSubConnectionName.Length()==0)
            {
            INFO_PRINTF2(_L("%S: SubConnection name missing."),&iParams.iSubConnectionName);
            return KErrNotFound;
            }

    return KErrNone;
    }

TVerdict CMultipleRemoveRSubConnectionStep::doSingleTestStep()
	{
/*
        TRequestStatus reqStat,reqStat2;
		TInt error, error2;
        error = iEsockTest->RemoveSocketFromSubConnection(sockParams,reqStat);
		if (error!=KErrNone)
            {
            INFO_PRINTF2(_L("Could not remove socket (%d)!"),error);
            SetTestStepResult(EFail);
            }
        error2 = iEsockTest->RemoveSocketFromSubConnection(sockParams,reqStat2);
        if (error==KErrNone && error2==KErrNone && iCancelFlag)
            {
            error = iEsockTest->CancelRemoveSocketFromSubConnection(sockParams.iSubConnectionName);
            if (error!=KErrNone)
                {
                INFO_PRINTF2(_L("Could not cancel removeing subconnection (%d)!"),error);
                SetTestStepResult(EFail);
                }
            }
		if (error==KErrNone)
			{
			User::WaitForRequest(reqStat);
			if (reqStat.Int()!=KErrNone)
				{
				INFO_PRINTF2(_L("Could not remove socket (%d)!"),error);
				SetTestStepResult(EFail);
				}
			}
		if (error2==KErrNone)
			{
			User::WaitForRequest(reqStat2);
			if (reqStat2.Int()==KErrNone)
				{
				INFO_PRINTF1(_L("KErrNone returned on multiple RSubConnection::Remove()!"));
				SetTestStepResult(EFail);
				}
			}
	*/
	return TestStepResult();
	}

