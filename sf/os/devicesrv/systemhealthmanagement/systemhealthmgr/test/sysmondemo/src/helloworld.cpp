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

#include "e32cons.h"
#include "e32std.h"
#include "e32cmn.h"
#include "e32property.h"
#include <e32uid.h>
 
void HelloWorldL()
	{
	
	//Get startup mode	
	TInt bootMode;
	RProperty::Get(KUidSystemCategory, KSystemStartupModeKey, bootMode) ;
	
	//Get current time
	TTime time;
	time.HomeTime();
	TBuf<40> timeString; // Holds the formatted date and time
    _LIT(KFormat1,"%:0%H%:1%T%:2%S%:3");
    time.FormatL(timeString,KFormat1);

	//create display message
    TBuf<255> buf;
	_LIT(KText, "Hello World, Mode %d, Current time is %S ");
	buf.Format(KText, bootMode, &timeString);

	User::InfoPrint(buf);

	// This app doesn't finish
	for(;;)
		{
		User::After(1000000) ;
		}
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	
	TInt err = KErrNone ;
	
	if (cleanup)
		{
		TRAP(err,HelloWorldL());
		delete cleanup;
		}
	
				
	__UHEAP_MARKEND;

	return (err);
	}


