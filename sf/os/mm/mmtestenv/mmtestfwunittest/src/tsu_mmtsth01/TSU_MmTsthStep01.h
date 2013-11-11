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

#ifndef __TSU_MMTSTHSTEP01_H__
#define __TSU_MMTSTHSTEP01_H__

class CTestSuite;
class CTSUMmTsthSuite01;

class CParseLine;
class CScript;
class CTestUtils;
class CLog;

// base class for all the TSU_MMTSTH01 suite test steps.
class RTSUMmTsthStep01 : public RTestStep
	{
public:
	virtual TVerdict OpenL();
	virtual void Close();

protected:
	CParseLine* iParseLine;
	CScript*	iTestScript;
	CTestSuite* iTestSuite;
	CTestUtils* iTestUtils;

	CLog* iLogClient;	// not owned
	};

#endif //__TSU_MMTSTHSTEP01_H__
