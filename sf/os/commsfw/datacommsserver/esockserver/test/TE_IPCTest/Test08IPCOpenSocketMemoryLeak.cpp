// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains IPC Test 08
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "Test08IPCOpenSocketMemoryLeak.h"

const TDesC& CTest08IPCOpenSocketMemoryLeak::GetTestName()
	{
	_LIT(ret,"IPCTest08");
	return ret;
	}

enum TVerdict CTest08IPCOpenSocketMemoryLeak::InternalDoTestStepL()
	{
	
#if defined (_DEBUG_SOCKET_FUNCTIONS)
	//-------------------substep 00-----------------------------	
	_LIT(aLog00,"00 Open socket server:"); Logger().Write(aLog00);
	TInt err;
	RSocketServ ss;
	CleanupClosePushL(ss);
	if ( (err = OptimalConnect(ss)) != KErrNone )
		{
		Logger().WriteFormat(_L("Connecting to socket server failed with %d"), err);
		User::Leave(EFail);
		}
	
	//-------------------substep 01-----------------------------	
	Logger().Write(_L("01 Set heap to fails after N=0 allocations..."));
	Logger().Write(_L("   ...Try to Open a socket. If it returns error, check memory leak."));
	Logger().Write(_L("   ...Increase N by one and repeat the same untill open-socket returns NoError:"));

	RSocket aSock;
	CleanupClosePushL(aSock);
	RSocket aSockets[16];
	
	TInt aFailure = 0;
	TInt i;
	for (i=0; i < 16; i++)
		CleanupClosePushL(aSockets[i]);
	
	for (i=0;i<16;i++)
		{
		if ((err = aSockets[i].Open(ss, iProt)) != KErrNone)
			{
			Logger().WriteFormat(_L("Error: Open socket error %d"), err);
			User::Leave(EFail);
			}
		}

	CleanupStack::Pop(&aSockets[15]);
	for (i=14; i >= 0; i--)	
		{
		CleanupStack::PopAndDestroy(&aSockets[i]);
		}
	
	do 
		{
		ss.__DbgFailNext(aFailure);
		ss.__DbgMarkHeap();
		
		err = aSock.Open(ss, iProt);
		
		if (err != KErrNone)     
			{
			ss.__DbgMarkEnd(0);
			}
		aFailure++;
		} while  (err != KErrNone);
		
		// To flush the hanging __DbgFailNext
		ss.__DbgFailNext(-1);
		Logger().WriteFormat(_L("Info: Loop has passed %d times"), aFailure - 1);
		
		aSockets[15].Close();
		
		//-------------------substep 02-----------------------------	
		Logger().WriteFormat(_L("02 Close the socket & socket server"));
		CleanupStack::PopAndDestroy(2, &ss);
		return EPass;
#else
		Logger().WriteFormat(_L("Info: Test Disabled on release build"));
		return EPass;	
#endif
		
	}

