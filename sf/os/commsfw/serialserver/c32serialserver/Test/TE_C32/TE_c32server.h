// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// 
//

#ifndef TE_C32SERVER_H
#define TE_C32SERVER_H

#include "TE_c32base.h"
#include <test/testexecuteserverbase.h>
//C32 Multi Threading header files included
#include<te_c32multithreading.h>

class CC32TestServer : public CTestServer
	{
public:
	static CC32TestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif

