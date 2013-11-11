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

#ifndef TGRAPHICSRESOURCESERVER_H
#define TGRAPHICSRESOURCESERVER_H

#include <test/testexecuteserverbase.h>

class CTSgServer : public CTestServer
	{
public:
	static CTSgServer* NewL();
//from 	CTestServer
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif
