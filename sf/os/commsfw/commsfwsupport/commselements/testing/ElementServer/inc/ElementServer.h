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

#if (!defined __ELEMENTSERVER_H__)
#define __ELEMENTSERVER_H__

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <TestExecuteServerBase.h>
#else
#include <test/TestExecuteServerBase.h>
#endif
class CElementServer : public CTestServer
	{
public:
	static CElementServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif	// __ELEMENTSERVER_H__
