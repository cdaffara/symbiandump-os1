/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file Te_integration_stltsySuiteServer.h
 @internalTechnology
*/

#if (!defined __TE_INTEGRATION_STLTSY_SERVER_H__)
#define __TE_INTEGRATION_STLTSY_SERVER_H__

#include <TestExecuteServerBase.h>

// For shared data
#include "TestLtsySharedData.h"

#include "TestLtsyModel.h"

class CTe_integration_stltsySuite : public CTestServer
	{
public:
	static CTe_integration_stltsySuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CTe_integration_stltsySuite();

// Please Add/modify your class members
private:
	CTe_integration_stltsySuite();
	void ConstructL(const TDesC& aName);
public:
	CTestLtsySharedData* iSharedData;
	CTestLtsyModel* iTestModel;
	};

#endif
