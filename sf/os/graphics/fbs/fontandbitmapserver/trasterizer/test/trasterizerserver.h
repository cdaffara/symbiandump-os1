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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef TRASTERIZERSERVER_H
#define TRASTERIZERSERVER_H

#include <test/testexecuteserverbase.h>

class CTRasterizerServer : public CTestServer
	{
public:
	static CTRasterizerServer* NewL();
	
	// from CTestServer	
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif // TRASTERIZERSERVER_H


