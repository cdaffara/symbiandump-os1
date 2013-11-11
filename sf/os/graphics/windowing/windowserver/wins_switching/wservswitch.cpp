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
// For the Winscw Emulator only, selects between NGA and Non-NGA version of wsgraphicdrawer.
// The default is the non-GCE based version.
// To select the NGA version do one of:
// 1. Add a line to the epoc.ini file in \epoc32\data like this:
// symbian_graphics_use_gce ON
// or
// 2. Start epoc.exe with these parameters, (the "--" IS necessary):
// -Dsymbian_graphics_use_gce=ON --
// or
// 3. epoc.exe can be told to switch to a different initialisation file than epoc.ini, with the -M parameter.
// Progress chaining to the real Wserv is logged in epocwind.out.
// 
//

#include <e32svr.h>
#include <u32hal.h>


_LIT(KProcessName, "WservSwitch");


TInt ChainWservSwitch()
	{
	_LIT(KWservSwitchNga,"z:\\sys\\bin\\wserv_nga.exe");
	_LIT(KWservSwitchNonNga,"z:\\sys\\bin\\wserv_nonnga.exe");
	// need to forward the command parameters to the chained version of Wserv
	TInt argLen = User::CommandLineLength();
	HBufC* arg = HBufC::New(argLen);
	if (arg == NULL)
		{
		return KErrNoMemory;
		}
	TPtr argPtr = arg->Des();
	User::CommandLine(argPtr);

	// NGA or Non-NGA WServ version?
	TBool nga = EFalse;
	UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalBoolProperty,  (TAny*)"symbian_graphics_use_gce",  &nga);

	// try to launch it
	const TPtrC exeName = nga ?  KWservSwitchNga() : KWservSwitchNonNga();

#ifdef _DEBUG
	RDebug::Print(_L("%S: Starting \"%S\" \"%S\""), &KProcessName, &exeName, &argPtr);
#endif

	RProcess server;
	TInt err = server.Create(exeName, argPtr, EOwnerThread);
	delete arg;
	arg = NULL;
	if (err)
		{
		return err;
		}

	// wait for server to start
#ifdef _DEBUG
	RDebug::Print(_L("%S: waiting on process ..."), &KProcessName);
#endif
	TRequestStatus stat;
	server.Rendezvous(stat);

	if (stat!=KRequestPending)
		{ // abort startup
		server.Kill(0);
		}
	else
		{ // logon OK - start the server
		server.Resume();
		}

	User::WaitForRequest(stat);
	return (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	}


TInt E32Main()
	{
	TInt error = ChainWservSwitch();
	// propagate error to SysStart
	if (error)
		{
#ifdef _DEBUG
		RDebug::Print(_L("%S: Failed with error %i"), &KProcessName, error);
#endif
		}
	RProcess::Rendezvous(error);
	return error;
	}

