// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ETel Test server header file.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TE_ETELSERVER_H
#define TE_ETELSERVER_H

#include "TE_EtelBase.h"
#include <test/testexecuteserverbase.h>

class CETelTestServer : public CTestServer
	{
public:
	static CETelTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif // TE_ETELSERVER_H
