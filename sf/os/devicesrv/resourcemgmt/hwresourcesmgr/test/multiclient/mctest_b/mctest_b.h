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
//

/**
 @file
*/



#ifndef MCTEST_B_H
#define MCTEST_B_H

#include <f32file.h>
#include <s32file.h>
#include "testcasecontroller.h"

class CHwrmMcTest_B : public CBase, public MHwrmTestCaseObserver
	{
public:
	static CHwrmMcTest_B* NewL(CConsoleBase& aConsole);
	static CHwrmMcTest_B* NewLC(CConsoleBase& aConsole);
	~CHwrmMcTest_B();

	void RunMulticlientTestCases_B();
		
private:
	CHwrmMcTest_B(CConsoleBase& aConsole);
	void ConstructL();

private: // from MHwrmTestCaseObserver
	void InfoMessage(const TDesC& aMessage);
	void LogMessage(const TDesC& aMessage);
	void DebugMessage(const TDesC& aMessage);
	
private:
	CConsoleBase& iConsole;
	CHwrmTestCaseController* iTestController;
	RFs iFs;
	RFileWriteStream iLogStream;
	};

#endif // MCTEST_B_H
