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
//

/**
 @file
*/

#include <amastartcli.h>
#include "amastartconst.h"
#include "amastartsrv.h"



EXPORT_C RAmaStartSession::RAmaStartSession()
: RSessionBase(), iVersion( KAmaStartCliVersionMajor, KAmaStartCliVersionMinor, KAmaStartCliVersionBuild ), iClientStatus( NULL )
	{
	DEBUGPRINT1A( ">RAmaStartSession::RAmaStartSession> (Empty constructor)");
	} //lint !e1401 not initialized by constructor (iSpare)


	
/**
 * Connect to the AmaStart server. If the server is not found, this function will attempt to start it
 * and then connect.
 */
EXPORT_C TInt RAmaStartSession::Connect()
	{
	DEBUGPRINT1A( ">RAmaStartSession::Connect") ;
	
	TInt err = CreateSession(KAmaStartSrvName, iVersion, KAmaStartSrvMsgSlots);
	if( (KErrNotFound == err) || (KErrServerTerminated == err) )
		{
		err = CAmaStartServer::StartAmaStartSrv();
		if (err != KErrNone) 
			{
			DEBUGPRINT1A( "RAmaStartSession::Connect> (Error starting server thread)") ;
			return err;
			}
		
		err = CreateSession(KAmaStartSrvName, iVersion, KAmaStartSrvMsgSlots);
		}

	DEBUGPRINT1A( "RAmaStartSession::Connect>") ;
	return err;
	}
	
	
	
EXPORT_C void RAmaStartSession::Close()
	{
	DEBUGPRINT1A( ">RAmaStartSession::Close") ;
	// Guard against illegal operations if non-open.
	if( Handle() )
		{
		StartDscCancel();
		}
	
	RSessionBase::Close();
	DEBUGPRINT1A( "RAmaStartSession::Close>") ;
	}
	
	

/**
 * Sends the UID of a DSC to the server for its items to be launched.
 * The system currently only supports one call per client session. 
 * 
 * @param aDscId The UID for the DSC to be launched.
 * @param aStatus The TRequestStatus which will be completed by server-side code when
 * list execution is complete. KErrNone if list execution is successful. A system-wide error code 
 * in case of error. KErrCancel if a call is made to StartDscCancel() during list processing. 
 */
EXPORT_C void RAmaStartSession::StartDsc( const TUid& aDscId, TRequestStatus& aStatus )
	{
	DEBUGPRINT1A( ">RAmaStartSession::StartDsc") ;
	iClientStatus = &aStatus;
	
	TBool dscExists = ETrue;
	
	TRAPD(err, dscExists = CheckDscExistsL(aDscId));
	if (KErrNone != err)
		{
		User::RequestComplete(iClientStatus, err);
		DEBUGPRINT1A( "RAmaStartSession::StartDsc> (Error checking that DSC exists") ;
		return;
		}
	if (!dscExists)
		{
		User::RequestComplete(iClientStatus, KErrArgument);
		DEBUGPRINT1A( "RAmaStartSession::StartDsc> (DSC does not exist)") ;
		return;
		}
	


	TIpcArgs args( aDscId.iUid );
	SendReceive( EAmaStartSrvStartDsc, args, aStatus );
	
	DEBUGPRINT1A( "RAmaStartSession::StartDsc>") ;
	}



/**
 * Cancels a currently executing DSC.
 *
 * @see StartDscL()
 */	
EXPORT_C void RAmaStartSession::StartDscCancel()
	{
	DEBUGPRINT1A( ">RAmaStartSession::StartDscCancel") ;
	__ASSERT_DEBUG( Handle(), User::Panic( KAmaStartHandleNotOpen, KAmaStartSrvHandleNotOpen) );
	
	if( iClientStatus && (iClientStatus->Int() == KRequestPending) && Handle() )
		{
		SendReceive( EAmaStartSrvStartDscCancel );
		}
	
	iClientStatus = NULL;
	
	DEBUGPRINT1A( "RAmaStartSession::StartDscCancel>") ;
	}

TBool RAmaStartSession::CheckDscExistsL(const TUid& aDscId)
	{
	DEBUGPRINT1A( ">RAmaStartSession::CheckDscExistsL") ;
	
	RDscStore store;
	store.OpenL();
	CleanupClosePushL(store);
	TBool result = store.DscExistsL(aDscId);
	CleanupStack::PopAndDestroy(/*store*/);
	
	DEBUGPRINT1A( "RAmaStartSession::CheckDscExistsL>");
	return result;
	}
