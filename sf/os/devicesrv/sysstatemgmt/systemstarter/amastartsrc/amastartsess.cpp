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

#include "amastartsess.h"



CAmaStartSession* CAmaStartSession::NewL( CAmaStartServer& aAmaStartServer )
	{
	DEBUGPRINT1A( ">CAmaStartSession::NewL" );
	
	CAmaStartSession* self = new(ELeave) CAmaStartSession( aAmaStartServer );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	
	DEBUGPRINT1A( "CAmaStartSession::NewL>" );
	return self;
	}



CAmaStartSession::~CAmaStartSession()
	{
	DEBUGPRINT1A( ">CAmaStartSession::~CAmaStartSession" );

	iAmaStartSvr.DeregisterSession( iSessionIndex );
	
	DEBUGPRINT1A( "CAmaStartSession::~CAmaStartSession>" );
	}



/**
 * In the case of EAmaStartSrvExecuteCmdList, the RMessage is completed by the 
 * Executor call into the server's ListExecutionComplete method.
 */
void CAmaStartSession::ServiceL( const RMessage2& aMessage )
	{
	DEBUGPRINT1A( ">CAmaStartSession::ServiceL" );
	aMessage.HasCapabilityL( ECapabilityProtServ, __PLATSEC_DIAGNOSTIC_STRING( "CleSrv Platsec violation" ) );

	switch( aMessage.Function() )
		{
		case EAmaStartSrvStartDsc:
			{
			StartDscL( aMessage );
			}
			break;
			
		case EAmaStartSrvStartDscCancel:
			{
			StartDscCancel();
			aMessage.Complete(KErrCancel);
			}
			break;
	
		default:
			aMessage.Complete( KErrUnknown );
			break;			
		}
	DEBUGPRINT1A( "CAmaStartSession::ServiceL>" );
	}
	
	

CAmaStartSession::CAmaStartSession( CAmaStartServer& aAmaStartServer )
: iAmaStartSvr( aAmaStartServer )
	{
	DEBUGPRINT1A( ">CAmaStartSession::CAmaStartSession> (Empty constructor)" );
	}



void CAmaStartSession::ConstructL()
	{
	DEBUGPRINT1A( ">CAmaStartSession::ConstructL" );
	iAmaStartSvr.RegisterSessionL( iSessionIndex );
	DEBUGPRINT1A( "CAmaStartSession::ConstructL>" );
	}



void CAmaStartSession::StartDscL( const RMessage2& aMessage )
	{
	DEBUGPRINT1A( ">CAmaStartSession::StartDscL" );
	
	iAmaStartSvr.StartDscL( TUid::Uid(aMessage.Int0()), aMessage, iSessionIndex );
	
	DEBUGPRINT1A( "CAmaStartSession::StartDscL>" );
	}



void CAmaStartSession::StartDscCancel()
	{
	DEBUGPRINT1A( ">CAmaStartSession::StartDscCancel" );
	
	iAmaStartSvr.StartDscCancel( iSessionIndex );
	
	DEBUGPRINT1A( "CAmaStartSession::StartDscCancel>" );
	}
