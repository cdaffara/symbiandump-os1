// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// RStartProcess Client side header
// Implements the R class to start server process
// 
//

/**
 @file 
*/

#include "startprocess.h"
#include <e32math.h>

/*
* Start process has to be different for EPOC32 (EKA1/EKA2 - started as a process) and emulator 
* (should be started as a DLL in a separate thread)
*/
TInt RStartProcess::StartProcess()
/** 
 * Starts the process and waits for a completion
 *
 * @return system error code
 */
	{
	//
	// on EPOC platform. Simultaneous launching
	// of two such processes should be detected when the second one attempts to
	// create the process object, failing with KErrAlreadyExists.
	//
	TBuf<KMaxFileNameLength> serverFile;
	_LIT(KExe, ".exe");
	
	serverFile.Copy( iName );
	serverFile.Append(KExe);

 	// Remove any "!" prefix when forming the server binary filename.  This is because only
 	// a single string is passed into CStartServer and used to form both the server name
 	// and server binary filename.  However, for protected servers, the string will have
 	// a "!" prefix - this is used for the server name but removed for the binary filename.
 	_LIT(KExclamationMark, "!");
 	if (serverFile.Left(1).Compare(KExclamationMark()) == 0)
 		serverFile.Delete(0,1);

	TInt res = iProcess.Create( serverFile,KNullDesC );
	return res == KErrNone ? WaitForCompletion() : res;
	}

TInt RStartProcess::StartProcessThreadFn(TAny* aStartProcess)
	{//logon completes with the return code
	return static_cast<RStartProcess*>(aStartProcess)->StartProcess();	
	}
	

TInt RStartProcess::WaitForCompletion()
	{
	TRequestStatus started;
	iProcess.Rendezvous(started);
	
	TRequestStatus died;
	iProcess.Logon(died);
	if (died!=KRequestPending)
		{
		// logon failed - process is not yet running, so cannot have terminated
		User::WaitForRequest(died);	// eat signal
		iProcess.Kill(0);				// abort startup
		iProcess.Close();
		return died.Int();
		}
	//
	// logon OK - start the process
	iProcess.Resume();
	User::WaitForRequest(started,died);		// wait for start or death
	if (started==KRequestPending)
		{
		// process has died, never made it to the startup signal
		iProcess.Close();
		return died.Int();
		}
	//
	// process started (at last). Cancel and consume the death-notification
	// before reporting success
	iProcess.LogonCancel(died);
	User::WaitForRequest(died);		// eat the signal (from the cancel)
	iProcess.Close();
	return iErrorCode;
	}

EXPORT_C void RStartProcess::Cancel()
/**
* Intended to cancel asynchronous process startup.
*
*/
	{//not sure what this can do => kill the thread maybe
	//for the time being when cancel occures nothing happens and user must still wait
	//for start-up to finish
   //for the time being we end up blocking our thread until the startup has finished
   //and than we call. 
   //NOTE At the minute we don't kill the process when cancel is called we just set the error code
   //and wait for the process to synchronise anyway
   //iProcess.Kill( KErrCancel );
	iErrorCode = KErrCancel; //indicate the error code
	}

EXPORT_C TInt RStartProcess::Start(const TDesC& aName, TRequestStatus& aStatus)
/**
* Starts the process asynchronously (using a thread)
*
* @param aName name of the process DLL/EXE
* @param aStatus
* @return TInt - An error code
*/
	{
	iErrorCode = KErrNone;
	iName.Des().Copy(aName);
	TName name(iName);
	TTime now;
	now.HomeTime();
	TInt64 seed = now.Int64();
	name.AppendNum(Math::Rand(seed), EHex);
	
	//start the process process using a thread => asynch
	RThread t;
	const TUint KStackSize=0x1000;
	if ((iErrorCode=t.Create(name, StartProcessThreadFn, KStackSize, NULL/*reuses our heap*/, this)) == KErrNone)
		{
		t.Logon(aStatus);
		t.Resume(); //RStartProcess::StartProcessThreadFn completes the logon
		t.Close();
		}
	else
		{
		TRequestStatus* p=&aStatus;
		User::RequestComplete( p, iErrorCode );
		}
	return iErrorCode;
	}

EXPORT_C TInt RStartProcess::Start(const TDesC& aName)
/**
* Starts the process synchronously
*
* @param aName name of the process DLL/EXE
* @return TInt - An error code
*/
	{
	iName.Des().Copy(aName);
	iErrorCode = KErrNone;
	return StartProcessThreadFn(this);		
	}

