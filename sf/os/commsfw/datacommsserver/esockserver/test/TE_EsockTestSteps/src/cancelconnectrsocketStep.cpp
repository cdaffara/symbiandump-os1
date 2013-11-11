// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file cancelconnectrsocketStep.cpp
*/
#include "Sockets.TestSteps.h"
#include "cancelconnectrsocketStep.h"

CCancelConnectrsocketStep::CCancelConnectrsocketStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCancelConnectrsocketStep);
	}

TInt CCancelConnectrsocketStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1)
		|| (iParams.iSocketName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_SocketName);
		return KErrNotFound;
		}
		
    return KErrNone;
    }


TVerdict CCancelConnectrsocketStep::doSingleTestStep()
	{
    TInt error = iEsockTest->CancelConnectSocket(iParams);

    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not cancel connecting socket (%S)."),&iParams.iSocketName);
        INFO_PRINTF2(_L("Error: %d."),error);
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}



