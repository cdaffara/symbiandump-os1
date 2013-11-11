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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#if (!defined __TBITGDISERVER_H__)
#define __TBITGDISERVER_H__

#include <test/testexecuteserverbase.h>
#include <test/tgraphicsharness.h>

class CTBitgdiServer : public CTestServer
	{
public:
	static CTBitgdiServer* NewL();
//from 	CTestServer
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
private:
	};

#endif


