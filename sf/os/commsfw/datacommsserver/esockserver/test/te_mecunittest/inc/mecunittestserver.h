// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// mecunittestserver.h
//

/**
@file
*/

#ifndef MECUNITTESTSERVER_H
#define MECUNITTESTSERVER_H

#include <test/testexecuteserverbase.h>

class CMecUnitTestServer : public CTestServer
	{
public:
	static CMecUnitTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CMecUnitTestServer();

protected:
    CMecUnitTestServer();

private:
	const TPtrC ServerName() const;
	};

#endif
// MECUNITTESTSERVER_H

