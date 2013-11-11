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
// The TEFUnit header file which tests the SessionManagement
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYSESSIONMANAGEMENTFU_H
#define CCTSYSESSIONMANAGEMENTFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsySessionManagementFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestInitialise0001L();
	void TestInitialise0004L();
	void TestOpen0001L();
	void TestOpen0003L();
	void TestGetTsyName0001L();
	void TestGetTsyName0003L();
	void TestEnumeratePhones0001L();
	void TestLoadPhoneModule0001L();
	void TestLoadPhoneModule0003L();
	void TestUnloadPhoneModule0001L();
	void TestUnloadPhoneModule0003L();
	void TestGetTsyVersionNumber0001L();
	void TestGetTsyVersionNumber0003L();
	void TestSetExtendedErrorGranularity0001L();
	void TestReferenceCount0001L();
	void TestReferenceCount0006L();
	void TestReferenceCount00011L();
	void TestOpenExistingCall0001L();
	void TestOpenExistingCall0003L();
	void TestOpenExistingCall0006L();
	void TestOpenExistingCall0008L();
	void TestOpenExistingCall00011L();
	void TestOpenExistingCall00013L();
	void TestInitialiseMM0001L();

private:

	void AuxReferenceCountL(const TDesC &aName);
	void AuxOpenExisting1L(const TDesC &aName);
	void AuxOpenExisting3L(const TDesC &aName);

	}; // class CCTsySessionManagementFU

#endif // CCTSYSESSIONMANAGEMENTFU_H

