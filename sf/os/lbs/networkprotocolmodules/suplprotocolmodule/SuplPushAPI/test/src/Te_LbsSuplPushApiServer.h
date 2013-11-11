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
// This is the test server for all Lbs SUPL Push API unit tests.
// 
//

#if (!defined __TE_LBSSUPLPUSHAPI_SERVER_H__)
#define __TE_LBSSUPLPUSHAPI_SERVER_H__
#include <test/testexecuteserverbase.h>

/**
The test server for the SUPL Push API unit tests.

@see CTestServer
*/
class CTe_LbsSuplPushApiSuite : public CTestServer
	{
public:
	static CTe_LbsSuplPushApiSuite* NewL();
	virtual ~CTe_LbsSuplPushApiSuite();

	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
protected:
	void ConstructL();
	
private:
	CTestStep* CreateTestStepL(const TDesC& aStepName);
	};

#endif //__TE_LBSSUPLPUSHAPI_SERVER_H__
