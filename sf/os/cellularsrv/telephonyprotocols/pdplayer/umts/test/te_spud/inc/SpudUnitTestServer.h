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
//

/**
 @file
 @internalComponent
*/

#ifndef SPUD_UNIT_TEST_SERVER_H
#define SPUD_UNIT_TEST_SERVER_H

#include <test/testexecuteserverbase.h>
#include "reteldriverinput.h"

class CSpudUnitTestServer : public CTestServer
	{
public:
	static CSpudUnitTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
private:
	};

REtelDriverInput* NewEtelDriverInputL();

#endif
// SPUD_UNIT_TEST_SERVER_H

