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


#ifndef MCTEST_F_H
#define MCTEST_F_H

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include "testcasecontroller.h"

class CHwrmMcTest_F : 	public CBase, public MHwrmTestCaseObserver
	{
public:
	static CHwrmMcTest_F* NewL(CConsoleBase& aConsole);
	static CHwrmMcTest_F* NewLC(CConsoleBase& aConsole);
	~CHwrmMcTest_F();

	void RunMulticlientTestCases();
		
private:
	CHwrmMcTest_F(CConsoleBase& aConsole);
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

#endif // MCTEST_F_H
