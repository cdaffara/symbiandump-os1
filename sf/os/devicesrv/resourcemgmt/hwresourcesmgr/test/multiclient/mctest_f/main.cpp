// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mctest_f.h"
#include <e32cons.h>

CConsoleBase* console;

_LIT(KProgramName,"McTest_F");
_LIT(KSlaveProcessName,"mctest_b");
#define PANIC(XXX)     User::Panic(KProgramName, XXX)

//
// Main executable functions
//
void mainL()
	{
	// launch slave program (mctest_b.exe)
	RProcess slaveProcess;
	User::LeaveIfError(slaveProcess.Create(KSlaveProcessName,KNullDesC));;
	CleanupClosePushL(slaveProcess);
	slaveProcess.Resume();
	
	CHwrmMcTest_F* mcTestF = CHwrmMcTest_F::NewL(*console);
	CleanupStack::PushL(mcTestF);
	
	mcTestF->RunMulticlientTestCases();
	
	CleanupStack::PopAndDestroy(mcTestF);
	
	slaveProcess.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&slaveProcess);
	}

void consoleMainL()
	{	
	console = Console::NewL(KProgramName, TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	CActiveScheduler* sched = new(ELeave) CActiveScheduler();
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);


	TRAPD(err,mainL());
	if (err)
		console->Printf(_L("\nSetting up the test failed with error code %d.\n"), err);

	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2, console);
	}

GLDEF_C TInt E32Main()
	{  
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();
	TRAPD(error,consoleMainL());
	// consoleMainL only leaves if creation of console / active scheduler fails
	__ASSERT_ALWAYS(!error,PANIC(error));
	delete cleanupStack;
	__UHEAP_MARKEND;
	return 0;
	}
