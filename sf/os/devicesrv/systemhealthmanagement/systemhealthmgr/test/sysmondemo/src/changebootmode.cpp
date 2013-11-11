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
// changemode.cpp
// 
//

#include "e32base.h"
#include "e32std.h"

#include <hal.h>
#include <hal_data.h>

LOCAL_C void ChangeModeL()
{


	TBuf<40> command;
	User::CommandLine(command) ;
	TPtr8 data = command.Collapse() ;

	TInt mode = 0 ;
		
	if (data != KNullDesC8)
	{
		TLex8 txt = TLex8(const_cast<TUint8*> (data.PtrZ())) ;
		User::LeaveIfError(txt.Val(mode)) ;		
	}

	HAL::Set(HALData::EPersistStartupModeKernel, mode);	

	// Initialisation complete, now signal 
	RProcess::Rendezvous(KErrNone);

}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK; // mark heap state

	CTrapCleanup* theTrapCleanup=CTrapCleanup::New();

	if (theTrapCleanup == NULL)
		return KErrNoMemory;
	else
		{
		TRAPD(error, ChangeModeL());

    	delete theTrapCleanup;
		__UHEAP_MARKEND; // check no memory leak

		return(error);
		}
	}
