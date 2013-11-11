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
// processlaunch.cpp
// 
//

#include <f32file.h>
#include "cprocesslaunch.h"

//----------------------------------------------------------------------------	

/** called by the launcher! Launch the process by specified parameters
find the process and launch it

@param aParams A reference to TProcessStartParams object
@return Symbian error code
@see TProcessStartParams
@internalTechnology
@released
 */
TInt CProcessLaunch::ProcessLaunch(TProcessStartParams& aParams)
// now prevents multiple instances of the same image being launched!
// So P&S servers now look like realCServer2 servers.
	{
	TFullName fullName;
	_LIT(KStar, "*");
	TParse parser;
	parser.SetNoWild(aParams.GetProcessFileName(), NULL, NULL);
	fullName.Append(parser.NameAndExt());
	fullName.Append(KStar);
	
	TFindProcess processFinder(fullName);
	TInt err = KErrNotFound;
	while (err = processFinder.Next(fullName), err == KErrNone)
		{
		RProcess process;
		TInt processOpenErr = process.Open(processFinder);
		if (processOpenErr == KErrNone)
			{
			TExitType exitType = process.ExitType();
			if (exitType == EExitPending)
				{
				// Found a running instance of the process,
				// so return without starting another instance.
				process.Close();
				return KErrNone;
				}
			}
			process.Close();		
		}
	
	// No running instance found, so start one.
	return (DoProcessLaunch(aParams));
	}

/** Create the process by specified parameters and wait for the request

@param aParams A reference to TProcessStartParams object
@return Symbian error code
@see TProcessStartParams
@internalTechnology
@released
 */
TInt CProcessLaunch::DoProcessLaunch(TProcessStartParams& aParams)
	{
	RProcess process;
 	
 	TInt r=process.Create(aParams.GetProcessFileName(),aParams.GetAsCommandLine());
	if (r!=KErrNone)
		{
		return r;
		}
	// are we expecing to rendezvous with the process we have created above
	if(aParams.GetRendezvousRequired())
		{
		TRequestStatus stat;
		process.Rendezvous(stat);
		if (stat!=KRequestPending)
			{
			process.Kill(0);
			}
		else
			{
			process.Resume();
			}
		User::WaitForRequest(stat);
		r=(process.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
		}
	process.Close();
	return r;
	}

