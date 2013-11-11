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
// te_spudRubConSvr.h
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TE_SPUD_RSUBCON_H
#define TE_SPUD_RSUBCON_H

#include <test/testexecuteserverbase.h>

class CSpudRSubConTestServer : public CTestServer
	{
public:
	static CSpudRSubConTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};


#endif //TE_SPUD_RSUBCON_H
