// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the RTestServ and RTestSession API's
// 
//

/**
 @file TestClient.cpp
*/
//#include "..\..\SampleServer\src\SampleServer.h"
#include <simulprocserver.h>
#include <simulprocclient.h>


TVersion RTestServ::Version() const
	{
	return(TVersion(1,0,1));
	}

EXPORT_C TInt RTestServ::Connect(const TDesC& aServerName)
/**
 * @param aServerName - Human readable name of the test server
 * @param aDebugMode - Set to true for just in time debugging
 * @return int - Standard error codes
 * Secure version of the API call. Expects the server binary to be
 * ServerXXX.exe
 */
	{
	if(aServerName.Length() > iServerName.MaxLength())
		return KErrTooBig;
	iServerName = aServerName;
	// Assume the server is already running and attempt to create a session
	// 4 message slots
	TInt err = CreateSession(aServerName,Version(),4);
	if(err == KErrNotFound)
		{
		// Server not running
		// Construct the server binary name
		TBuf<KMaxServerNameLength> serverFile;
		RProcess server;
		_LIT(KEmpty,"");
		_LIT(KExe,".exe");

		serverFile.Copy(aServerName);
		serverFile.Append(KExe);
		err = server.Create(serverFile,KEmpty);
		if(err != KErrNone)
			return err;
		// Synchronise with the server
		TRequestStatus reqStatus;
		server.Rendezvous(reqStatus);

		if (reqStatus!=KRequestPending)
			{
			server.Kill(0);
			}
		else
			{
			// Start the test harness
			server.Resume();
			// Server will call the reciprocal static synchronise call		
			}
		User::WaitForRequest(reqStatus); // wait for start or death
		server.Close();
		if(reqStatus.Int() != KErrNone)
			return reqStatus.Int();
		
		// Create the root server session
		err = CreateSession(aServerName,Version(),4);
		}
	return err;
	}

EXPORT_C const TDesC& RTestServ::ServerName() const
/**
 * @return - The human readable server name
 */
	{
	return iServerName;
	}

//
EXPORT_C TInt RTestSession::Open(RTestServ& aServ, const TDesC& aStepName, TBool aSharedData)
/**
 * @param aServ - Reference to the root server session
 * @param aStepName - Reference to the name of the test step owned by the server
 * @return Standard error codes
 * Secure and Non-secure variants
 * Synchronously open a server test step session
 */
	{
	if(aStepName.Length() > KMaxServerNameLength)
		return KErrTooBig;
	TIpcArgs args;
	args.Set(0,&aStepName);
	args.Set(1,aSharedData);
	return CreateSubSession(aServ,EMMTSOpenTestStep,args);
	}

EXPORT_C void RTestSession::StartProcessing(TRequestStatus& aStatus)
/**
 *
 * @param aCommandString - The arguments to the RUN_TEST_STEP command
 * @return aPanicString  - If the test step panics, this member is set up with the panic string
 * @param aStatus - For completion to the caller
 * Secure and Non-secure variants
 * Send the RUN_TEST_STEP arguments to the test server
 */
	{
	SendReceive(EMMTSStartProcessing,aStatus);
	}

EXPORT_C TVerdict RTestSession::EndProcessingAndReturnResult(TDes8& aMessage)
/**
 *
 * @return - Standard Epoc error codes 
 * Secure and Non-secure variants
 * Synchronous abort of the the test step
 */
	{

	TPckgBuf<TVerdict> v;
	TIpcArgs args;
	args.Set(0,&v);
	args.Set(1,&aMessage);
	SendReceive(EMMTSStopProcessing, args);
	return v();
	}

EXPORT_C void RTestSession::Close()
	{
	CloseSubSession(EMMTSClose);
	}
