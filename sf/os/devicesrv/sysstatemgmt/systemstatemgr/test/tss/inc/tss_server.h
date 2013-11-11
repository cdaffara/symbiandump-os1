// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSS_SERVER_H__
#define __TSS_SERVER_H__

#include <test/testexecuteserverbase.h>
		
class CSsTestServer : public CTestServer
	{
public:
	static CSsTestServer* NewL();
	
public:
	// From CTestServer
	CTestStep* CreateTestStep(const TDesC& aStepName);
	};


#endif // __TSS_SERVER_H__
