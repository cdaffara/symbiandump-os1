// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "clecli.h"
#include "clesrv.h"

/**
 Construct the client-side CLE session.

*/
RCleSession::RCleSession()
: RSessionBase(),iVersion( KCleCliVersionMajor, KCleCliVersionMinor, KCleCliVersionBuild )
	{
	} //lint !e1401 not initialized by constructor (iSpare)


	
/**
 Connect to the CLE server. If the server is not found, this function will attempt to start it
 and retry connecting to it.

@leave KErrNoMemory if no memory.
@leave One of the system-wide error codes.
*/
void RCleSession::ConnectL()
	{
	// Guard against multiple calls.
	if( KNullHandle != iHandle )
		{
		User::Leave( KErrAlreadyExists );
		}

	TInt err = CreateSession(KCleSrvName, iVersion, KCleSrvMsgSlots);
	if( (KErrNotFound == err) || (KErrServerTerminated == err) )
		{
		err = CCleServer::StartCleSrv(KCleSrvName);
		DEBUGPRINT2(_L("Starting CleSrv completed with %d"),err);
		if (KErrNone !=  err && KErrAlreadyExists != err)
			{
			User::Leave(err);
			}

		User::LeaveIfError( CreateSession(KCleSrvName, iVersion, KCleSrvMsgSlots) );
		}
	else
		{
		User::LeaveIfError( err );
		}	
	}
	
/**
 Connect to the CLE server with the specified name. If the server is not found, this function will attempt to start it
 and retry connecting to it.
 
@param aServerName The name of the CLE server to connect to
@leave KErrNoMemory if no memory.
@leave One of the system-wide error codes.
*/
void RCleSession::ConnectL(const TDesC& aServerName)
	{
	// Guard against multiple calls.
	if( KNullHandle != iHandle )
		{
		User::Leave( KErrAlreadyExists );
		}

	TInt err = CreateSession(aServerName, iVersion, KCleSrvMsgSlots);
	if( (KErrNotFound == err) || (KErrServerTerminated == err) )
		{
		err = CCleServer::StartCleSrv(aServerName);

		DEBUGPRINT2(_L("Starting CleSrv completed with %d"),err);
		if (KErrNone !=  err && KErrAlreadyExists != err)
			{
			User::Leave(err);
			}

		User::LeaveIfError( CreateSession(aServerName, iVersion, KCleSrvMsgSlots) );
		}
	else
		{
		User::LeaveIfError( err );
		}	
	}	
	
void RCleSession::Close()
	{
	// Guard against illegal operations if non-open.
	if( Handle() )
		{
		ExecuteCommandListCancel();
		}
	RSessionBase::Close();
	}


/**
  Sends the pointer of the command list to the CLE server for reconstruction and execution 
  of the commands.
  The system currently only supports one call per client session. 

  @param aCmdList A constructed command list to be passed to the CLE server for execution.
  @param aStatus The TRequestStatus is completed by server-side code when
  all commands in a list have been initiated. KErrNone if list execution is successful. A system-wide error code 
  in case of error. KErrCancel if a call is made to ExecuteCommandListCancel() during list processing. 
  
*/
 void RCleSession::ExecuteCommandList( const CSsmCommandList& aCmdList, TRequestStatus& aStatus ,TCmdErrorSeverity& /*aSeverity*/)
	{
	TRequestStatus* clientStatus = &aStatus;
	
	if( aCmdList.Count() == 0 )
		{
		// Empty cmd list? Notify the client benignly and don't bother the server.
		User::RequestComplete( clientStatus, KErrArgument );
		
		return;
		}

	if( !Handle() )
		{
		// Suggests that a call to ConnectL() might have been overlooked.
		User::RequestComplete( clientStatus, KErrDisconnected );
		
		return;
		}
	TIpcArgs args( &aCmdList );
    SendReceive( ECleSrvExecuteCmdList, args, aStatus );
	}



/**
 Cancels a currently executing command list.
 
 @see ExecuteCommandList()

*/	
void RCleSession::ExecuteCommandListCancel()
	{
	
	if( Handle() )
		{
		SendReceive( ECleSrvExecuteCmdListCancel );
		}
	
	}
