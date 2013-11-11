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
// @file ctlbsconflictserver.h
// This contains the header file for LBS Conflict Test server
// 
//

#ifndef __CT_LBS_CONFLICT_SERVER_H__
#define __CT_LBS_CONFLICT_SERVER_H__

// System includes

// User includes
#include "ctlbsserver.h"
#include "ctlbsconflictshareddata.h"

//Literals Used
_LIT(KLbsConflictTestServer,"LbsConflictTestServer");


// Implements the LBS Client Test server
class CT_LbsConflictServer : public CT_LbsServer
	{
public :
	static CT_LbsConflictServer* NewL();
	// Creates the test steps based on the test step name 
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CT_LbsConflictServer();
	CT_LbsConflictServer();
public:

protected:
	void ConstructL(const TDesC& aName);
	};

#endif //__CT_LBS_CONFLICT_SERVER_H__
