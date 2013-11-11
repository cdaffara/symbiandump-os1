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
// @file ctlbsagpshaiassdataserver.h
// This contains the header file for LBS AGPS HAI Validation Test Server
// 
//

#ifndef __CT_LBSAGPSHAIASSDATASERVER_H__
#define __CT_LBSAGPSHAIASSDATASERVER_H__

// User includes
#include "ctlbsagpshaiserverroot.h"

// Implements the LBS Client Test server
class CT_LbsAGPSHAIAssDataServer : public CT_LbsAGPSHAIServerRoot
	{
public:
	static CT_LbsAGPSHAIAssDataServer* NewL();
	// Creates the test steps based on the test step name 
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CT_LbsAGPSHAIAssDataServer();
	CT_LbsAGPSHAIAssDataServer();
private:
	const TDesC& ServerName();
	};

#endif //__CT_LBSAGPSHAIASSDATASERVER_H__
