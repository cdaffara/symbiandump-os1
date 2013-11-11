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

#ifndef HWRMTESTS_H
#define HWRMTESTS_H

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include "testcasecontroller.h"

class CHwrmTests : 	public CBase, public MHwrmTestCaseObserver
	{
public:
	static CHwrmTests* NewL(CConsoleBase& aConsole);
	static CHwrmTests* NewLC(CConsoleBase& aConsole);
	~CHwrmTests();

	void RunTests();		
private:
	CHwrmTests(CConsoleBase& aConsole);
	void ConstructL();

	void RunBasicTests_SDK();
	void RunBasicTests_Extended();
    void RunBasicTests_Enhanced();
	void RunVibraTests();
	void RunInvalidParamTests();
	void RunWaitChargingNotifyTests();

private: // from MHwrmTestCaseObserver
	void InfoMessage(const TDesC& aMessage);
	void LogMessage(const TDesC& aMessage);
	void DebugMessage(const TDesC& aMessage);
	
	// helper log functions
	void DoLogMessageL(const TDesC& aMessage);
	void InfoAndLogMessage(const TDesC& aMessage);
	
private:
	CConsoleBase& iConsole;
	CHwrmTestCaseController* iTestController;
	RFs iFs;
	RFileWriteStream iLogStream;
	};

#endif
