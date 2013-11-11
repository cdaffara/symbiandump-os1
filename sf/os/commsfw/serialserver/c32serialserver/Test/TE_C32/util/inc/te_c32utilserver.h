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

#if (!defined TE_C32UTILSERVER_H)
#define TE_C32UTILSERVER_H
#include <test/testexecuteserverbase.h>

class CC32UtilServer : public CTestServer
	{
public:
	static CC32UtilServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif // TE_C32UTILSERVER_H


