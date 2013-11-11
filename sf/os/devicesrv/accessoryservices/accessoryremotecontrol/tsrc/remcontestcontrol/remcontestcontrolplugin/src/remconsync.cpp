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
#include "remconsync.h"


// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
CRemConSync::CRemConSync()
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
void CRemConSync::ConstructL( CRemConInterfaceSelector* aTargetInterfaceSelector )
    {
    iSync = CRemConSynchronizationTarget::NewL( *aTargetInterfaceSelector, *this );
    iActive = CRemConActive::NewL( *this );
	}

// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
CRemConSync* CRemConSync::NewL( CRemConInterfaceSelector* aTargetInterfaceSelector )
    {
    CRemConSync* self = new( ELeave ) CRemConSync();
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
CRemConSync::~CRemConSync()
    {
    delete iActive;
    }

// -----------------------------------------------------------------------------
// 
// 
// 
// -----------------------------------------------------------------------------
void CRemConSync::CompleteResponse( const TInt aError )
    {
		if(aError != KErrNone)
	    {
    	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL -  CRemConSync::CompleteResponse( %d)" ), aError) );
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
void CRemConSync::Synchronize()
{
    iEvent = ETFRemConEvent_Sync;
    iParameter = KErrNone;
    MRemConControl* aControl = GetRemConControl();
	aControl->NotifyKeyEvent(iEvent, iParameter);
}

// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//	   
void CRemConSync::SynchronizeResponse()
    {
    iActive->iStatus = KRequestPending;
    iSync->SynchronizeResponse(iActive->iStatus, iError);
    iActive->SetActive();
    COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL -  SynchronizeResponse return" )) );
	}
   
void CRemConSync::GetCommandSourceInfo(TRemConExtCmdSource& aSource)
    {
    iSync->GetCommandSourceInfo( aSource );
    }
