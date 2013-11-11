// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ctlbsbtgpspsysuiteserver.h
// Testexecute test server for the BT GPS PSY Integration Tests
//

#if (!defined __CT_LBSBTGPSPSY_SERVER_H__)
#define __CT_LBSBTGPSPSY_SERVER_H__

#include <test/TestExecuteServerBase.h>

/** Testexecute server for BT GPS PSY Integration tests
 */
class CT_LbsBtGpsPsyTestServer : public CTestServer
	{
public:
	static CT_LbsBtGpsPsyTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

protected:
	void ConstructL();	
	};

#endif
