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
// This contains CTestMMFVCLNTSuite 
// 
//

#ifndef TS_MMFVCLNT_SUITE_H_
#define TS_MMFVCLNT_SUITE_H_

/**
 *
 * @class CTestMmfVclntSuite
 *
 */
class  CTestMmfVclntSuite : public CTestSuite
	{
public:
	void InitialiseL();
	virtual ~CTestMmfVclntSuite();
	void AddTestStepL(CTestMmfVclntStep* ptrTestStep);
	TPtrC GetVersion();

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

#endif /* TS_MMFVCLNT_SUITE_H_ */

