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
// @file ctlbsx3pserver.h
// This contains the header file for LBS Client Test server
// 
//

#ifndef __CT_LBS_X3P_SERVER_H__
#define __CT_LBS_X3P_SERVER_H__

// System includes

// User includes
#include "ctlbsserver.h"
#include "ctlbsx3pshareddata.h"

//Literals Used
_LIT(KLbsX3PTestServer,"LbsX3PTestServer");

//Common defines
_LIT(KLbsRefPos, "RefPos");

#define X3P_PRIORITY_PUSH			0x02
#define X3P_PRIORITY_MENU			0x08
#define X3P_PRIORITY_TIMER			0x10


// Implements the LBS Client Test server
class CT_LbsX3PServer : public CT_LbsServer
	{
public :
	static CT_LbsX3PServer* NewL();
	// Creates the test steps based on the test step name 
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CT_LbsX3PServer();
	CT_LbsX3PServer();
public:

protected:
	void ConstructL(const TDesC& aName);
	};

#endif //__CT_LBS_X3P_SERVER_H__
