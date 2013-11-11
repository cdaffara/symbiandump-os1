/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


// INCLUDE FILES
#include "remconcontrol.h"
#include "remconactive.h"
#include "siftrace.h"
#include "remconcall.h"


// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
CRemConCall::CRemConCall()
    {    
     iError = KErrNone;
     iEvent     = ETFRemConEvent_None;
     iParameter = KErrNone;	
    }

// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
void CRemConCall::ConstructL( CRemConInterfaceSelector* aTargetInterfaceSelector )
    {
    iCall = CRemConCallHandlingTarget::NewL( *aTargetInterfaceSelector, *this );
    iActive = CRemConActive::NewL( *this );
	}

// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
CRemConCall* CRemConCall::NewL( CRemConInterfaceSelector* aTargetInterfaceSelector )
    {
    CRemConCall* self = new( ELeave ) CRemConCall();
    CleanupStack::PushL( self );
    self->ConstructL( aTargetInterfaceSelector );
    CleanupStack::Pop();
    return self;
    }
 
// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
CRemConCall::~CRemConCall()
    {
    delete iActive;
    }

// -----------------------------------------------------------------------------
// 
// 
// 
// -----------------------------------------------------------------------------
void CRemConCall::CompleteResponse( const TInt aError )
    {
		if(aError != KErrNone)
	    {
    	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL -  CRemConCall::CompleteResponse( %d)" ), aError) );
    	}
    	else
    	{
    	MRemConControl* aControl = GetRemConControl();
    	aControl->CompleteKeyEventResponse( iEvent, iParameter, aError );	
    	}	
     
     iEvent     = ETFRemConEvent_None;
     iParameter = KErrNone;	
	}
// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//	
	/**
	* Answer an incoming phone call.
	*/
	void CRemConCall::AnswerCall()
	{
		iEvent = ETFRemConEvent_None;
		iParameter = KErrNone;
	}

	/**
	* End an incoming/ongoing phone call.
	*/
	void CRemConCall::EndCall()
	{
		iEvent = ETFRemConEvent_None;
		iParameter = KErrNone;
	}
	
	/**
	* End an incoming/ongoing phone call.
	*/
	void CRemConCall::AnswerEndCall()
	{
		iEvent = ETFRemConEvent_None;
		iParameter = KErrNone;
	}
	
	/**
	* Make a voice call.
	*/
	void CRemConCall::VoiceDial( const TBool aActivate )
	{
		iEvent = ETFRemConEvent_None;
		iParameter = KErrNone;
	}
	
	/**
	* Redial last phone call.
	*/
	void CRemConCall::LastNumberRedial()
	{
		iEvent = ETFRemConEvent_None;
		iParameter = KErrNone;
	}
	
	/**
	* Dial a phone call.
	*/
	void CRemConCall::DialCall( const TDesC8& aTelNumber )
	{
		iEvent = ETFRemConEvent_None;
		iParameter = KErrNone;
	}
	
	/**
	* Make multiparty call.
	*/
	void CRemConCall::MultipartyCalling( const TDesC8& aData )
	{
		iEvent = ETFRemConEvent_None;
		iParameter = KErrNone;
	}
	
	/**
	* Generates DTMF signal.
	*/
	void CRemConCall::GenerateDTMF( const TChar aChar )
	{
		iEvent = ETFRemConEvent_None;
		iParameter = KErrNone;
	}
	
	/**
	* Make a speed dial call.
	*/
	void CRemConCall::SpeedDial( const TInt aIndex )
	{
		iEvent = ETFRemConEvent_None;
		iParameter = KErrNone;
	}
// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//	     
void CRemConCall::GetCommandSourceInfo(TRemConExtCmdSource& aSource)
    {
    iCall->GetCommandSourceInfo( aSource );
    }
