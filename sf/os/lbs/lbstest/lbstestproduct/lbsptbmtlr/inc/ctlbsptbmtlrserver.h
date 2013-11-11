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
// @file ctlbsptbmtlrserver.h
// This contains the header file for PTB LBS MTLR Test server
// 
//

#ifndef __CT_LBS_PTB_MTLR_SERVER_H__
#define __CT_LBS_PTB_MTLR_SERVER_H__

// System includes

// User includes
#include "ctlbsserver.h"
#include "ctlbsptbmtlrshareddata.h"

//Literals Used
_LIT(KLbsPTBMTLRTestServer,"LbsPTBMTLRTestServer");


// Implements the LBS Client Test server
class CT_LbsPTBMTLRServer : public CT_LbsServer
	{
public :
	static CT_LbsPTBMTLRServer* NewL();
	// Creates the test steps based on the test step name 
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CT_LbsPTBMTLRServer();
	
protected:
	CT_LbsPTBMTLRServer();
	void ConstructL(const TDesC& aName);
	};

#endif //__CT_LBS_PTB_MTLR_SERVER_H__
