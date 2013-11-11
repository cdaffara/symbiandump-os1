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
// @file ctlbsfeaturesserver.h
// 
//

#ifndef __CT_LBS_FEATURES_SERVER_H__
#define __CT_LBS_FEATURES_SERVER_H__

// System includes
#include <test/testexecuteserverbase.h>

//Literals Used
_LIT(KLbsFeaturesTestServer,"ctlbsfeaturesserver");


// Implements the LBS Client Test server
class CT_LbsFeaturesServer : public CTestServer
	{
public :
	static CT_LbsFeaturesServer* NewL();
	// Creates the test steps based on the test step name 
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CT_LbsFeaturesServer();
	
protected:
	CT_LbsFeaturesServer();
	void ConstructL(const TDesC& aName);
	};

#endif //__CT_LBS_FEATURES_SERVER_H__
