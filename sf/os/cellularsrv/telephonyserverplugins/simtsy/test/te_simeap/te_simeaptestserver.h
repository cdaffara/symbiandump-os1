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
*/

#ifndef _TE_SIMEAP_TESTSERVER_H_
#define _TE_SIMEAP_TESTSERVER_H_

#include <test/testexecuteserverbase.h>

class CSimEapTestServer : public CTestServer
	{
public:
	static CSimEapTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif // _TE_SIMEAP_TESTSERVER_H_
