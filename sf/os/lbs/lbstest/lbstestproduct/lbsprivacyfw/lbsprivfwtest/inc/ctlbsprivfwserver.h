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
// @file ctlbsprivfwserver.h
// This contains the header file for LBS Client Test server
// 
//

#ifndef __CT_LBS_PRIVFW_SERVER_H__
#define __CT_LBS_PRIVFW_SERVER_H__

// System includes

// User includes
#include "ctlbsserver.h"

//Literals Used
_LIT(KLbsPrivFwTestServer,"LbsPrivFwTestServer");


// Implements the LBS Client Test server
class CT_LbsPrivFwServer : public CT_LbsServer
	{
public :
	static CT_LbsPrivFwServer* NewL();
	// Creates the test steps based on the test step name 
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CT_LbsPrivFwServer();
	CT_LbsPrivFwServer();
public:

//	CActiveScheduler* iScheduler;

protected:
	void ConstructL(const TDesC& aName);
	};

#endif //__CT_LBS_PRIVFW_SERVER_H__
