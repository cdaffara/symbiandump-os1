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

#ifndef __TSU_MMTSTHSTEP13_H__
#define __TSU_MMTSTHSTEP13_H__

class CTestSuite;
class CTSUMmTsthSuite13;

// base class for all the TSU_MMTSTH13 suite test steps.

class RTSUMmTsthStep13 : public RTestStep
	{
public:
	virtual TVerdict OpenL();
	virtual void Close();

protected:
	void CleanupFileSystem();

protected:
	// the CTestUtils object which we will test
	CTestUtils* iTestUtils;

	};

#endif //__TSU_MMTSTHSTEP13_H__
