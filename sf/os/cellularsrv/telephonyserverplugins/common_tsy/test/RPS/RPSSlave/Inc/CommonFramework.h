// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef __CommonFramework_H
#define __CommonFramework_H

#include <e32base.h>
#include <e32cons.h>
#include <bacline.h>

#include "RmPartySmlS.h"
#include "rpsasciirqstdefs.h"
#include "RpsSFlogger.h"

_LIT(KTxtExampleCode,"RPS Slave");
_LIT(KFormatFailed,"failed: leave code=%d");
_LIT(KTxtOK,"ok");


// public
GLREF_D CConsoleBase* console; // write all your messages to this
LOCAL_D CRmPartySmlS* rmPartySml;
LOCAL_C void doExampleL(); 

// private
LOCAL_C void callExampleL(); // initialize with cleanup stack, then do example

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error, callExampleL()); // more initialization, then do example
	__ASSERT_ALWAYS(!error,User::Panic(KPanicMsgFormat,KInitError));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
    }

LOCAL_C void callExampleL() // initialize and call example code under cleanup stack
    {
    
	console=Console::NewL(KTxtExampleCode,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);

	// Create Active Scheduler
    CActiveScheduler* sched = NULL;
    sched = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    CleanupStack::PushL(sched);
 //   sched->Start();


	// Call the RPSSlave.
	// Restart it if it ever panics.
	TBool keepUp(ETrue);
	while(keepUp)
		{
		TRAPD(error, doExampleL());
		
		if (error)
			{
			LOGLINE2(KFormatFailed, error);
			LOGLINE1(_L("Restaring RPSSlave....."));
			if(rmPartySml != NULL)
				delete rmPartySml;
			}		
		else
			{
			LOGLINE1(KTxtOK);
			keepUp = EFalse;
			LOGLINE1(_L("RPSSlave exited normally."));
			}
		}
		
	CleanupStack::PopAndDestroy(sched);  // sched
	CleanupStack::PopAndDestroy(console);  // close console

    }

#endif
