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
// te_smsstoreserver.h
// 
//

/**
 @file
*/

#ifndef __TE_SMSPRTSERVER_H__
#define __TE_SMSPRTSERVER_H__

#include "TE_smsstorbase.h"
#include <test/testexecuteserverbase.h>

class CSmsStorTestServer : public CTestServer
	{
public:
	static CSmsStorTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};
#endif // __TE_SMSPRTSERVER_H__
