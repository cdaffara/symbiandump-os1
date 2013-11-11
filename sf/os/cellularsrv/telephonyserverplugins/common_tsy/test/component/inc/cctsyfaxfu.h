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
// The TEFUnit header file which tests the Fax
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYFAXFU_H
#define CCTSYFAXFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyFaxFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestWaitForEndOfPage0001L();
	void TestTerminateFaxSession0001L();
	void TestGetFaxSettings0001L();
	void TestSetFaxSettings0001L();
	void TestAdoptFaxSharedHeaderFile0001L();
	void TestRead0001L();
	void TestWrite0001L();

protected:
	~CCTsyFaxFU();

private:
	void OpenLineVoiceLC(RLine& aLine);
	void OpenCallVoiceLC(RLine& aLine, RCall& aCall);
	void OpenLineDataLC(RLine& aLine);
	void OpenCallDataLC(RLine& aLine, RCall& aCall);
	void OpenLineFaxLC(RLine& aLine);
	void OpenCallFaxLC(RLine& aLine, RCall& aCall);
	void OpenFaxL();

	}; // class CCTsyFaxFU

#endif // CCTSYFAXFU_H

