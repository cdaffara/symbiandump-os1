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
 @internalComponent
*/

#include "amastartasync.h"

#include "ssmdebug.h"

CAmaStartAsync* CAmaStartAsync::NewL( RMessage2 aMessage )
	{
	DEBUGPRINT1A( ">CAmaStartAsync::NewL" );
	
	CAmaStartAsync* self = NewLC( aMessage );
	CleanupStack::Pop();
	
	DEBUGPRINT1A( "CAmaStartAsync::NewL>" );
	return self;
	}
	
	
	
CAmaStartAsync* CAmaStartAsync::NewLC( RMessage2 aMessage )
	{
	DEBUGPRINT1A( ">CAmaStartAsync::NewLC" );
	
	CAmaStartAsync* self = new(ELeave) CAmaStartAsync( aMessage );
	CleanupStack::PushL( self );
	self->ConstructL();
	
	DEBUGPRINT1A( ">CAmaStartAsync::NewLC" );
	return self;
	}



CAmaStartAsync::~CAmaStartAsync()
	{
	DEBUGPRINT1A( ">CAmaStartAsync::~CAmaStartAsync" );
	
	Cancel();

	delete iStartSafe;
	if (iDscStore.IsOpened())
		{
		iDscStore.Close();
		}
	iSysMon.Close();
	
	DEBUGPRINT1A( "CAmaStartAsync::~CAmaStartAsync>" );
	}



/**
 * Initiate AMA processing.
 */	
void CAmaStartAsync::StartL( const TUid& aDscId )
	{
	DEBUGPRINT1A( ">CAmaStartAsync::StartL" );
	
	if( iRunning )
		{
		User::Leave( KErrInUse );
		}

	DEBUGPRINT1( _L("***** Set running") );
	
	iRunning = ETrue;
	iDscStore.EnumOpenLC(aDscId);
	CleanupStack::Pop();
		
	SetActive();
	TRequestStatus* trs = &iStatus;
	User::RequestComplete( trs, KErrNone );
	
	DEBUGPRINT1A( "CAmaStartAsync::StartL>" );
	}



/**
 * Succsessive calls to StartDscItemL, but only one per RunL in order for
 * the scheduler/ server to remain responsive.
 */
void CAmaStartAsync::RunL()
	{
	DEBUGPRINT1A( ">CAmaStartAsync::RunL %d" );
	CDscItem* item = iDscStore.EnumReadNextL();
	if (item)
		{
		//if loading of one AMA from the DSC fails we should move to next AMA - so we trap here 
		TRAP_IGNORE(StartDscItemL(*item));
		
		delete item;
		
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		
		}
	else // No more items in store, so finish here
		{
		iDscStore.EnumClose();
		iDscStore.Close();
		iMessage.Complete(KErrNone);
		}

	DEBUGPRINT1A( "CAmaStartAsync::RunL>" );
	}
	
	

TInt CAmaStartAsync::RunError( TInt aError )
	{
	DEBUGPRINT1A( ">CAmaStartAsync::RunError" );
	
	iDscStore.EnumClose();
	iDscStore.Close();
	iMessage.Complete(aError);
	
	DEBUGPRINT1A( "CAmaStartAsync::RunError>" );
	return KErrNone;
	}



/**
 * Cancel the current command.
 */
void CAmaStartAsync::DoCancel()
	{
	DEBUGPRINT1A( ">CAmaStartAsync::DoCancel" );

	if( iRunning )
		{
		iDscStore.EnumClose();
		iMessage.Complete(KErrCancel);
		iRunning = EFalse;
		}
	
	DEBUGPRINT1A( "CAmaStartAsync::DoCancel>" );
	}
	


void CAmaStartAsync::ConstructL()
	{
	DEBUGPRINT1A( ">CAmaStartAsync::ConstructL" );
	
	iStartSafe = CStartSafe::NewL();
	iDscStore.OpenL();
	CActiveScheduler::Add( this );
	
	DEBUGPRINT1A( "CAmaStartAsync::ConstructL>" );
	}
	
	
	
CAmaStartAsync::CAmaStartAsync( RMessage2 aMessage )
: CActive( EPriorityStandard ), iMessage( aMessage )
	{
	DEBUGPRINT1A( ">CAmaStartAsync::CAmaStartAsync> (Empty constructor)" );
	}
	
	

void CAmaStartAsync::StartDscItemL(const CDscItem& aDscItem)
	{
	DEBUGPRINT1A( ">CAmaStartAsync::StartDscItemL" );
	
	//A process to be created inside iStartSafe->StartL();
	RProcess process;

	//Number of retries made for starting the process.
	TInt tried=0;
	
	// start the process
	const CStartupProperties& properties = aDscItem.StartupProperties();
	iStartSafe->StartL(properties, process, tried);
	
	DEBUGPRINT2(_L("%d times has been retried to start the process successfully"), tried);

	TInt error=KErrNone;
	//monitor the process if indicated
	if(aDscItem.Monitored())
		{
		// first time monitoring, so connect with the SysMon server
		if (iSysMon.Handle() == KNullHandle)
			{
			TRAP(error, iSysMon.OpenL());
			}
		// monitor the process	
		if (KErrNone==error) 
			{
			TRAP(error, iSysMon.MonitorL(properties, process));
			}
		//Connect to SysMon fail or register to monitor the process fail
		//Kill the started process because Start and Monitor should be one atomic function.
		if (KErrNone !=error)
			{
			process.Kill(error);
			process.Close();
			}
		User::LeaveIfError(error);	
		}
		
	DEBUGPRINT1A( "CAmaStartAsync::StartDscItemL>" );
	}
