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

/**
 @file
 @test
*/

#ifndef EGLTESTREFSERVER_H
#define EGLTESTREFSERVER_H

#include <test/testexecuteserverbase.h>

// Entry point
GLDEF_C TInt E32Main(void);

/**
CTestEGLRefServer	Server encapsulates the functionality required to execute Test Execute based test cases for the EGL component.
*/
class CTestEGLRefServer : public CTestServer
	{
public:
	static CTestEGLRefServer* NewL();
	
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif	// EGLTESTREFSERVER_H
