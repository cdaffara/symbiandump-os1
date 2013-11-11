/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#include "e32base.h"
#include "cprocesslaunch.h"
#include "tprocessstartparams.h"

TInt E32Main()
	{
	/*
	God & Lorna need to create process like this:
	
	RProcess p;
	_LIT(KProcessName, "process.exe");
	CreateCommandLine
	TPtr commandline(CreateCommandLine(ERendezvousRequired));
	p.Create(KProcessName, commandLine);
	
	in the process launch code we need to figure out what the command buffer was
	and complete the rendezvous if required
	*/
	TProcessStartParams params;
	//_LIT(KDummyFileName, "\\sys\\bin\\ExFrameworkClient.exe");
	_LIT(KDummyFileName, "ExFrameworkClient.exe");
	_LIT(KDummyProcessName, "DummyProcessName");
	params.SetProcessFileName(KDummyFileName);
	params.SetProcessName(KDummyProcessName);
	params.SetRendezvousRequired(ETrue);
	TInt r = CProcessLaunch::ProcessLaunch(params);
	return r;
	}
 