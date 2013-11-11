/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef __CT_LBS_SIMULATION_TEST_PSY_SERVER_H__
#define __CT_LBS_SIMULATION_TEST_PSY_SERVER_H__

// User includes
#include "ctlbsserver.h"
#include <test/testexecuteserverbase.h>

_LIT(KTestProcedure115_0, "TP115-0");
_LIT(KTestProcedure115_1, "TP115-1");
_LIT(KTestProcedure115_2, "TP115-2");
_LIT(KTestProcedure115_3, "TP115-3");
_LIT(KTestProcedure115_4, "TP115-4");
_LIT(KTestProcedure115_5, "TP115-5");
_LIT(KTestProcedure115_6, "TP115-6");
_LIT(KTestProcedure115_7, "TP115-7");
_LIT(KTestProcedure115_8, "TP115-8");
_LIT(KTestProcedure115_9, "TP115-9");
_LIT(KTestProcedure115_10, "TP115-10");
_LIT(KTestProcedure115_11, "TP115-11");
_LIT(KTestProcedure115_12, "TP115-12");
_LIT(KTestProcedure221, "TP221");
_LIT(KTestProcedure242, "TP242");
_LIT(KTestProcedure254, "TP254");
_LIT(KTestProcedure256, "TP256");
_LIT(KTestProcedure262, "TP262");
_LIT(KTestProcedure300_0, "TP300-0");
_LIT(KTestProcedure300_1, "TP300-1");
_LIT(KTestProcedure300_2, "TP300-2");
_LIT(KTestProcedure300_3, "TP300-3");
_LIT(KTestProcedure300_4, "TP300-4");
_LIT(KTestProcedure300_5, "TP300-5");
_LIT(KTestProcedure300_6, "TP300-6");
_LIT(KTestProcedure300_7, "TP300-7");
_LIT(KTestProcedure300_8, "TP300-8");
_LIT(KTestProcedure400_0, "TP400-0");
_LIT(KTestProcedure400_1, "TP400-1");
_LIT(KTestProcedure400_2, "TP400-2");
_LIT(KTestProcedure400_3, "TP400-3");
_LIT(KTestProcedure400_4, "TP400-4");
_LIT(KTestProcedure400_5, "TP400-5");
_LIT(KTestProcedure500_0, "TP500-0");
_LIT(KTestProcedure500_1, "TP500-1");
_LIT(KTestProcedure500_2, "TP500-2");
_LIT(KTestProcedure500_3, "TP500-3");
_LIT(KTestProcedure500_4, "TP500-4");
_LIT(KTestProcedure500_5, "TP500-5");
_LIT(KTestProcedure500_6, "TP500-6");
_LIT(KTestProcedure500_7, "TP500-7");
_LIT(KTestProcedure600_0, "TP600-0");
_LIT(KTestProcedure600_1, "TP600-1");
_LIT(KTestProcedure600_2, "TP600-2");
_LIT(KTestProcedure600_3, "TP600-3");

// Implements the LBS Client Test server
class CT_LbsSimulationTestPSYServer : public CT_LbsServer
	{
public :
	static CT_LbsSimulationTestPSYServer* NewL();
	// Creates the test steps based on the test step name 
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

public:

protected:
	};

#endif //__CT_LBS_SIMULATION_TEST_PSY_SERVER_H__
