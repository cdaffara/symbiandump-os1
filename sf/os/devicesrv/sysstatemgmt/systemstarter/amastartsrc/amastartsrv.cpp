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

#include <e32debug.h>

#include "amastartsrv.h"



/**
 * Used to create a new CAmaStartServer
 * @return A pointer to the CAmaStartServer
 * @leave One of the system-wide error codes if construction fails.
 */
EXPORT_C CAmaStartServer* CAmaStartServer::NewLC()
	{
	DEBUGPRINT1A( ">CAmaStartServer::NewLC" );
	CAmaStartServer* self = new(ELeave) CAmaStartServer();	
	CleanupStack::PushL( self );
	self->ConstructL();
	DEBUGPRINT1A( "CAmaStartServer::NewLC>" );
	return self;	
	}



/**
 * Static function used to create and start CAmaStartServer
 * 
 * @return KErrAlreadyExists if the server is already running
 */
EXPORT_C TInt CAmaStartServer::StartAmaStartSrv()
	{
	DEBUGPRINT1A( ">CAmaStartServer::StartAmaStartSrv" );
	
	const TInt KAmaStartSrvMinHeapSize = 0x2000;
	const TInt KAmaStartSrvMaxHeapSize = 10 * KAmaStartSrvMinHeapSize;
	RThread srvThread;

	TInt err = srvThread.Create( KAmaStartSrvName, &CAmaStartServer::AmaStartSrvThreadFn, 
								 KDefaultStackSize, KAmaStartSrvMinHeapSize, KAmaStartSrvMaxHeapSize, 
								 NULL, EOwnerProcess );

	if( KErrNone == err )
		{
		TRequestStatus trs;
		srvThread.Rendezvous( trs );
		if ( trs != KRequestPending)
			{
			srvThread.Kill(trs.Int());
			srvThread.Close();
			DEBUGPRINT1A( "CAmaStartServer::StartAmaStartSrv> (Not Request pending)" );
			return trs.Int();
			}
		srvThread.Resume();
		User::WaitForRequest( trs );
		srvThread.Close();
		err = trs.Int();
		}

	DEBUGPRINT1A( "CAmaStartServer::StartAmaStartSrv>" );
	return err;
	}



CAmaStartServer::~CAmaStartServer()
	{
	DEBUGPRINT1A( ">CAmaStartServer::~CAmaStartServer" );
	TInt i = 0;
	TInt count = iSessionInfoArray.Count();
	for(; i < count; i++ )
		{
		delete iSessionInfoArray[ i ].iStarter;
		iSessionInfoArray[ i ].iStarter = NULL;
		}
	iSessionInfoArray.Close();
	DEBUGPRINT1A( "CAmaStartServer::~CAmaStartServer>" );
	}



void CAmaStartServer::StartDscL( const TUid& aDscId, const RMessage2& aMessage, const TInt aSessionIndex )
	{
	DEBUGPRINT1A( ">CAmaStartServer::StartDscL" );
	
	if( !__IN_RANGE(aSessionIndex, iSessionInfoArray.Count()) )
		{
		User::Leave( KErrArgument );
		}
	iSessionInfoArray[ aSessionIndex ].iStarter = CAmaStartAsync::NewL( aMessage );
	iSessionInfoArray[ aSessionIndex ].iMessagePtr = aMessage;
	iSessionInfoArray[ aSessionIndex ].iStarter->StartL( aDscId );
	
	DEBUGPRINT1A( "CAmaStartServer::StartDscL>" );
	}



/**
 * Locate the CommandListExecutor associated with the session calling CommandListCancel.
 * Delete the Command list (which cancels its AO) then complete the client's RMessage with KErrCancel.
 */
void CAmaStartServer::StartDscCancel( const TInt aSessionIndex )
	{
	DEBUGPRINT1A( ">CAmaStartServer::StartDscCancel" );
	__ASSERT_DEBUG( __IN_RANGE( aSessionIndex, iSessionInfoArray.Count()), User::Panic( KAmaStartSrvBadIdx, KAmaStartSrvArrayIndexInvalid ) );

	iSessionInfoArray[ aSessionIndex ].iStarter->Cancel();
	
	DEBUGPRINT1A( "CAmaStartServer::StartDscCancel>" );
	}
	


/**
 * Iterate through iSessionInfoArray to find an unused array element
 * If found, use it. Otherwise, Append() a new TAmaStartSessionInfo.
 * Called during CSession construction.
 */
void CAmaStartServer::RegisterSessionL( TInt& aSessionIndex )
	{
	DEBUGPRINT1A( ">CAmaStartServer::RegisterSessionL" );
	
	TInt i = 0;
	TInt count = iSessionInfoArray.Count();
	TBool slotFound = EFalse;
	
	for(; i < count; i++ )
		{
		if( iSessionInfoArray[ i ].iInUse )
			{
			continue;
			}
		else
			{
			iSessionInfoArray[ i ].iInUse = ETrue;
			iSessionInfoArray[ i ].iStarter = NULL;
			aSessionIndex = i;
			slotFound = ETrue;
			iSessionCount++;
			break;
			}
		}
	
	if( !slotFound )
		{
		TAmaStartSessionInfo sessionInfo;
		sessionInfo.iStarter = NULL;
		sessionInfo.iInUse = ETrue;
		iSessionInfoArray.AppendL( sessionInfo );
		aSessionIndex = iSessionCount++;
		}
	
	DEBUGPRINT2A( "Registered in slot %d", aSessionIndex );
	DEBUGPRINT1A( "CAmaStartServer::RegisterSessionL>" );
	}



/**
 * Called from CSession destructor.
 */
void CAmaStartServer::DeregisterSession( const TInt& aSessionIndex )
	{
	DEBUGPRINT1A( ">CAmaStartServer::DeregisterSession" );
	__ASSERT_DEBUG( __IN_RANGE(aSessionIndex, iSessionInfoArray.Count()), User::Panic(KAmaStartSrvBadIdx, KAmaStartSrvArrayIndexInvalid) );
	
	if( iSessionInfoArray[ aSessionIndex ].iStarter )
		{
		delete iSessionInfoArray[ aSessionIndex ].iStarter;
		iSessionInfoArray[ aSessionIndex ].iStarter = NULL;
		}
	iSessionInfoArray[ aSessionIndex ].iInUse = EFalse;
	iSessionCount--;
	
	DEBUGPRINT2A( "De-registered slot %d", aSessionIndex );
	DEBUGPRINT1A( "CAmaStartServer::DeregisterSession>" );
	}



/**
 * Used to create a new server-side session.
 * @return A pointer to the new instance of CSession2.
 * @leave KErrNotSupported if versions are incompatible.
 */
EXPORT_C CSession2* CAmaStartServer::NewSessionL( const TVersion& aVersion, const RMessage2& /*aMessage*/ ) const
	{
	DEBUGPRINT1A( ">CAmaStartServer::NewSessionL" );
    CAmaStartServer& mutatedSelf = MUTABLE_CAST( CAmaStartServer&, *this );
	if( !User::QueryVersionSupported(iVersion, aVersion) )
		{
		User::Leave( KErrNotSupported );
		}
	DEBUGPRINT1A( "CAmaStartServer::NewSessionL>" );
	return CAmaStartSession::NewL( mutatedSelf ); 
	}



CAmaStartServer::CAmaStartServer()
: CServer2( EPriorityHigh, EUnsharableSessions ),
  iVersion( KAmaStartSrvVersionMajor, KAmaStartSrvVersionMinor, KAmaStartSrvVersionBuild )
	{
	DEBUGPRINT1A( ">CAmaStartServer::CAmaStartServer> (Empty constructor)" );
	}
	
	
	
void CAmaStartServer::ConstructL()
	{
	DEBUGPRINT1A( ">CAmaStartServer::ConstructL" );
	StartL( KAmaStartSrvName );
	// The starting thread is signalled in the thread function.
	DEBUGPRINT1A( "CAmaStartServer::ConstructL>" );
	}
	

// Static functions
	
TInt CAmaStartServer::AmaStartSrvThreadFn( TAny* /*aAny*/ )
	{
	DEBUGPRINT1A( ">CAmaStartServer::AmaStartSrvThreadFn" );
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TRAPD( err, AmaStartSrvThreadRunL() ); 
    delete cleanup;
    DEBUGPRINT1A( "CAmaStartServer::AmaStartSrvThreadFn>" );
    return err;
	}



void CAmaStartServer::AmaStartSrvThreadRunL()
	{
	DEBUGPRINT1A( ">CAmaStartServer::AmaStartSrvThreadRunL" );
    CActiveScheduler* sched = new(ELeave) CActiveScheduler();
    CleanupStack::PushL( sched );
    CActiveScheduler::Install( sched );
    CAmaStartServer* amaStartSrv = CAmaStartServer::NewLC();
    RThread::Rendezvous( KErrNone );
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy( 2, sched );
    DEBUGPRINT1A( "CAmaStartServer::AmaStartSrvThreadRunL>" );
	} //lint !e529 not subsequently referenced (amaStartSrv)
