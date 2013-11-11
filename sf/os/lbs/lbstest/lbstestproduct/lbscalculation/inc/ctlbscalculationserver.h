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
// Test server for calculation tests
// 
//

#ifndef __CT_LBS_CALCULATION_SERVER_H__
#define __CT_LBS_CALCULATION_SERVER_H__

// User includes
#include "ctlbsserver.h"

//Literals Used
_LIT(KLbsCalculationTestServer,"lbscalculationtestserver");


// Implements the LBS Calculation Test server
class CT_LbsCalculationServer : public CT_LbsServer
	{
public :
	static CT_LbsCalculationServer* NewL();
	// Creates the test steps based on the test step name 
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CT_LbsCalculationServer();
	CT_LbsCalculationServer();
public:

protected:
	};

#endif //__CT_LBS_CALCULATION_SERVER_H__
