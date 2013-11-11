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
// @file ctlbshybridx3pserver.h
// 
//

#ifndef __CT_LBS_HYBRID_X3P_SERVER_H__
#define __CT_LBS_HYBRID_X3P_SERVER_H__

// System includes

// User includes
#include "ctlbsserver.h"

//Literals Used
_LIT(KLbsHybridX3PTestServer,"LbsHybridX3PTestServer");


// Implements the LBS Client Test server
class CT_LbsHybridX3PServer : public CT_LbsServer
	{
public :
	static CT_LbsHybridX3PServer* NewL();
	// Creates the test steps based on the test step name 
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CT_LbsHybridX3PServer();
	CT_LbsHybridX3PServer();
public:

protected:
	void ConstructL(const TDesC& aName);
	};

#endif //__CT_LBS_HYBRID_X3P_SERVER_H__
