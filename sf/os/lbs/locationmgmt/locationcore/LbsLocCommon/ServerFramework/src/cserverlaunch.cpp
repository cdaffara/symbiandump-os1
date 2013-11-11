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
// serverlaunch.cpp
// 
//

#include "cserverlaunch.h"
#include "tserverstartparams.h"

/** Launch the server by specified parameters
find the process and launch it

@param aParams A reference to TProcessStartParams object
@return Symbian error code
@see TProcessStartParams
@internalTechnology
@released
 */
TInt CServerLaunch::ServerLaunch(TServerStartParams& aParams)
	{
	TFindServer serverSearcher(aParams.GetServerName());
	TFileName matchingFileName;
	TInt error = serverSearcher.Next(matchingFileName); // we haev asked for a exact match, so the retunred filename can be ignored
	if(error==KErrNone)
		{
		return KErrNone; // found it, so some other kind process has just started the server for us
		}
	else
		{
		error = CreateServerProcess(aParams);		
		}
	return error;
	}

/** create server process by specified parameters
signal the clients and start the process, wait for any request.

@param aParams A reference to TProcessStartParams object
@return Symbian error code
@see TProcessStartParams
@internalTechnology
@released
 */
TInt CServerLaunch::CreateServerProcess(TServerStartParams& aParams)
	{
	RProcess server;
 	
 	TInt r=server.Create(aParams.GetServerFileName(),aParams.GetAsCommandLine());
	if (r!=KErrNone)
		return r;
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		{
		server.Kill(0);
		}
	else
		{
		server.Resume();
		}
	User::WaitForRequest(stat);
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}
	
