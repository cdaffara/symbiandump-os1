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
 @file cleanallStep.cpp
*/

#include "SocketServer.TestSteps.h"

#include <ecom/ecom.h>

CCleanAllStep::CCleanAllStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCleanAllStep);
	}

TVerdict CCleanAllStep::doTestStepL()
	{
    iEsockTest->CleanAll();
    REComSession::FinalClose();
	return EPass;
	}

TInt CCleanAllStep::ConfigureFromIni()
    {
	//dummy
    return KErrNotFound;
    }

TVerdict CCleanAllStep::doSingleTestStep()
	{
	//dummy
	return EFail;
	}


