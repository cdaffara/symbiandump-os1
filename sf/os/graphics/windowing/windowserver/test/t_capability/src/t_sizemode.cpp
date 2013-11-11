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

/**
 @file
 @test
 @internalComponent
*/

#include <w32std.h>
#include <e32debug.h>
#include "t_sizemode.h"

/************************************************************************/
/* A window server client application used to test calls to             */
/* CWsScreenDevice::SetCurrentScreenModeAttributes()                    */
/* Controlled from test step CT_WServCapCheckStepTSizeMode()          */
/* Refer to this test step for details of operation                     */
/************************************************************************/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK; // mark heap state

	CTrapCleanup* TheTrapCleanup = CTrapCleanup::New();

	TRAPD(err, StartTestL());

	//If the call to SetCurrentScreenModeAttributes() panics,
	//execution will not reach this point

    delete TheTrapCleanup;

	__UHEAP_MARKEND; //Check for memory leaks

	return err;
	}

LOCAL_C void StartTestL(void)
	{
	//Create a window server session
	RWsSession session;
	User::LeaveIfError(session.Connect());

	//Create a screen device for screen 0
	CWsScreenDevice* device = new(ELeave) CWsScreenDevice(session);

	CleanupStack::PushL(device);
	User::LeaveIfError(device->Construct(0));	//Screen 0

	//Read the current TSizeMode settings from screen 0 and attempt to write them back
	TSizeMode originalModeData = device->GetCurrentScreenModeAttributes();

	RDebug::Print(_L("Calling CWsScreenDevice::SetCurrentScreenModeAttributes()\n"));
	device->SetCurrentScreenModeAttributes(originalModeData);
	RDebug::Print(_L("Calling CWsScreenDevice::SetCurrentScreenModeAttributes() caused no panic\n"));

	//Tidy up
	CleanupStack::PopAndDestroy(device);
	session.Close();
	}
