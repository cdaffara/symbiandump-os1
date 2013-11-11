// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __TESTSUITEVIRTUALSTUB_H__
#define __TESTSUITEVIRTUALSTUB_H__

_LIT(KTestSuiteVirtualStub, "TestSuiteVirtualStub");

// CTestSuiteVirtualStub : a test suite stub to run testing on.

class  CTestSuiteVirtualStub : public CTestSuite
	{
public:
	void InitialiseL();
	virtual ~CTestSuiteVirtualStub();
	TPtrC GetVersion() const;
	};


#endif //__TESTSUITEVIRTUALSTUB_H__
