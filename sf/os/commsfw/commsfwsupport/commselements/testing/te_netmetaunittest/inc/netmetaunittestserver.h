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
// netmetaunittestserver.h
//

/**
@file
*/

#ifndef NETMETAUNITTESTSERVER_H
#define NETMETAUNITTESTSERVER_H

#include <test/testexecuteserverbase.h>

class CNetMetaUnitTestServer : public CTestServer
	{
public:
	static CNetMetaUnitTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CNetMetaUnitTestServer();

protected:
    CNetMetaUnitTestServer();

private:
	const TPtrC ServerName() const;
	};

#endif
// NETMETAUNITTESTSERVER_H
