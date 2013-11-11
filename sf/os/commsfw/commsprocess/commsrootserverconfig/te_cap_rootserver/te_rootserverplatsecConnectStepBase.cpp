// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// RootServer Platform Security Test Suite Test Step Base Class
// 
//

/**
 @file 
*/

#include "te_rootserverplatsecConnectStepBase.h"
#include "te_rootserverplatsecConnectSuiteDefs.h"


/**
 @return - TVerdict
 Implementation of CTestStep base class virtual
 It is used for doing all initialisation common to derived classes in here.
 Make it being able to leave if there are any errors here as there's no point in
 trying to run a test step if anything fails.
 The leave will be picked up by the framework.
*/
TVerdict Cte_rootserverplatsecConnectStepBase::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
 @return - TVerdict
 Implementation of CTestStep base class virtual
 It is used for doing all after test treatment common to derived classes in here.
 Make it being able to leave
 The leave will be picked up by the framework.
*/
TVerdict Cte_rootserverplatsecConnectStepBase::doTestStepPostambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

Cte_rootserverplatsecConnectStepBase::~Cte_rootserverplatsecConnectStepBase()
	{
	}

Cte_rootserverplatsecConnectStepBase::Cte_rootserverplatsecConnectStepBase()
	{
	}


CSessionConnectionHelper::CSessionConnectionHelper()
	{
	}

CSessionConnectionHelper::~CSessionConnectionHelper()
	{
	}
	
/**
 @return - TInt
 Public method to provide access to protected RSessionBase::CreateSession() method
*/
TInt CSessionConnectionHelper::OpenSession (const TDesC& aServer, const TVersion& aVersion)
	{
	return CreateSession (aServer, aVersion);
	}

