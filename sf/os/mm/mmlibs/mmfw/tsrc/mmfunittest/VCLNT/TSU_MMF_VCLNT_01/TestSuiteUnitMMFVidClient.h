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

#if (!defined __TEST_SUITE_MMF_VID_CLNT_H_)
#define __TEST_SUITE_MMF_VID_CLNT_H_

#include <testframework.h>

/**
 *
 * CTestSuiteUnitMMFVidClient, which is the base class for 
 * the UnitMMFVidClient suite.
 *
 * @lib "TSU_MMF_VCLNT_01.lib"
 *
 * @xxxx
 *
 */
class  CTestSuiteUnitMMFVidClient : public CTestSuite
	{
public:
	void InitialiseL();
	virtual ~CTestSuiteUnitMMFVidClient();
	void AddTestStepL( CTestStepUnitMMFVidClient * aTestStep );
	TPtrC GetVersion() const;

#ifdef __WINS__
protected:
	TBool WaitUntilWindowServerReadyL();
	void InitWservL();
	void DeInitWserv();
protected:
	CWsScreenDevice* iScreen;
	RWindow* iWindow;
	RWsSession iWs;
	RWindowGroup iRootWindow;
#endif // __WINS__
	};


#endif //__TEST_SUITE_MMF_VID_CLNT_H_
