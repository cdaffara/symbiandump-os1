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
 @file ConfirmSocketDataTransferStep.cpp
*/

#include "Sockets.TestSteps.h"

CConfirmSocketDataTransferStep::CConfirmSocketDataTransferStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KConfirmSocketDataTransferStep);
	}

TInt CConfirmSocketDataTransferStep::ConfigureFromIni()
	{
	iParams.Reset();

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_Name, iParams.iSocketCheckName) != 1)
		|| (iParams.iSocketCheckName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

	// Fetch the name of the socket
	if (GetStringFromConfig(iSection, KTe_SocketName, iParams.iSocketName) != 1)
		{
		INFO_PRINTF2(_L("%S: Associated socket name missing."), &iParams.iSocketCheckName);
		return KErrNotFound;
		}

	// Perhaps we are supposed to find an errored socket
	GetBoolFromConfig(iSection, KErrorExpected, iParams.iErrorExpected);

	TPtrC dataTransform;
	
	// Also fetch the type of transformation we should do on the data
	if (!iParams.iErrorExpected && (GetStringFromConfig(iSection, KSocketDataCheckTransformType, dataTransform) != 1))
		{
		INFO_PRINTF2(_L("%S: Associated data transformation type missing."), &iParams.iSocketCheckName);
		return KErrNotFound;
		}
	
	if(dataTransform.Length() != 0)
		{
		// Decode the transform type
		if(dataTransform.Compare(_L("none")))
			iParams.iTransformType = ETransformDataNone;
		else if (dataTransform.Compare(_L("xor")))
			iParams.iTransformType = ETransformDataXor;
		else if (dataTransform.Compare(_L("one")))
			iParams.iTransformType = ETransformDataAddOne;
		else if (dataTransform.Compare(_L("two")))
			iParams.iTransformType = ETransfromDataAddTwo;
		else
			{
			INFO_PRINTF2(_L("%S: Associated data transformation type missing."), &iParams.iSocketCheckName);
			return KErrNotFound;
			}
		}
	
    // All ok if we got this far
    return KErrNone;
	}

TVerdict CConfirmSocketDataTransferStep::doSingleTestStep()
{
#ifdef __1060_TESTCODE__
    // Call the esock library function to do this check for us
    TInt error = iEsockTest->SendAndReceiveDataAndVerifyL(iParams);

    if (error != KErrNone)
    {
        INFO_PRINTF2(_L("Data on socket was not as expected (%S)."), &iParams.iSocketCheckName);
        INFO_PRINTF2(_L("Error: %d."), error);
		SetTestStepResult(EFail);
    }
#endif

	return TestStepResult();
}



