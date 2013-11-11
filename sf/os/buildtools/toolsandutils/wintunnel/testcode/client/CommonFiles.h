// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CommonFiles_H
#define __CommonFiles_H

#include <e32cons.h>

// public
LOCAL_D CConsoleBase* console; // write all your messages to this
LOCAL_C void doExampleL(); // code this function for the real example

// private
LOCAL_C void callExampleL(); // initialize with cleanup stack, then do example

GLDEF_C TInt E32Main() // main function called by E32
    {
	_LIT(KSymbianEx,"SymbianEx");
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,callExampleL()); // more initialization, then do example
	__ASSERT_ALWAYS(!error,User::Panic(KSymbianEx,error));
	delete cleanup; // destroy clean-up stack
	return 0; // and return
    }

LOCAL_C void callExampleL() // initialize and call example code under cleanup stack
    {
	_LIT(KStatus1,"Symbian OS Example Code (comms)");
	_LIT(KStatus2,"failed: leave code=%d");
	_LIT(KStatus3,"ok");
	_LIT(KStatus4," [press any key]");

	console=Console::NewL(KStatus1,TSize(KConsFullScreen,KConsFullScreen));
	//console=Console::NewL(KStatus1, TSize(KDefaultConsWidth,KDefaultConsHeight));
	CleanupStack::PushL(console);
	TRAPD(error,doExampleL()); // perform example function
	if (error) console->Printf(KStatus2, error);
	else console->Printf(KStatus3);
	console->Printf(KStatus4);
	console->Getch(); // get and ignore character
	CleanupStack::Pop(); // close console
    }

#endif
