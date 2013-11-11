// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file CT_LbsServer.h
// This contains the header file for LBS Test server
// 
//

#ifndef __CT_LBS_SERVER_H__
#define __CT_LBS_SERVER_H__

// System includes
#include <test/testexecuteserverbase.h>

// User includes
#include "ctlbsshareddata.h"

//Literals Used
_LIT(KTestCaseId, "tc_id");

/** Configuration variable to state which module is in use for the test.

This does not select the module to use, it's used to switch on test notifcation
messages from the test step to the test module.

Values:	"real" for use with the real A-GPS module.
		"test" for use with the test module.
*/
_LIT(KLbsModuleType, "module_type");
_LIT(KLbsModuleType_Real, "real");
_LIT(KLbsModuleType_Test, "test");


// Implements the LBS Test server
class CT_LbsServer : public CTestServer
	{
public :
	static CT_LbsServer* NewL();
	// Creates the test steps based on the test step name 
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	virtual ~CT_LbsServer();
	CT_LbsServer();
public:
	CT_LbsSharedData* iSharedData;	

	/**
	iScheduler is a pointer to the active scheduler for use in the tests
	@see CActiveScheduler
	*/ 
	CActiveScheduler* iScheduler;

protected:
	virtual void ConstructL(const TDesC& aName);
	};

#endif //__CT_LBS_SERVER_H__
