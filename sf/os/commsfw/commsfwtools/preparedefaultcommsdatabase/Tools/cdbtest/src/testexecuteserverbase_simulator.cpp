// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestServer which is the base class for all test servers
// This is a simulator of the TestExecuteStepBase.cpp file contained within TestExecute
// and is designed for use with the TOOLS2 platform.
// 
//

/**
 @file testexecuteserverbase_simulator.cpp
*/

#include <e32base.h>
#include "testexecuteserverbase.h"

CTestServer::~CTestServer()
    {
    }


void CTestServer::ConstructL(const TDesC& aName)
    {
    }

CTestStep* CreateTestStep(const TDesC& aStepName)
    {
	return NULL;
    }

