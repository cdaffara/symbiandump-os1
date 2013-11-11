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
#include "remconpoc.h"
#include "remcontypes.h"

// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
CRemConPoc::CRemConPoc()
    {
     iError     = KErrNone;
     iEvent     = ETFRemConEvent_None;
     iParameter = KErrNone;	
    }
// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
void CRemConPoc::ConstructL( CRemConInterfaceSelector* aTargetInterfaceSelector )
    {
    iPoc = CRemConPocTarget::NewL( *aTargetInterfaceSelector, *this );
    iActive = CRemConActive::NewL( *this );
	}

// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
CRemConPoc* CRemConPoc::NewL( CRemConInterfaceSelector* aTargetInterfaceSelector ) 
    {
    CRemConPoc* self = new( ELeave ) CRemConPoc();
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
CRemConPoc::~CRemConPoc()
    {    
    delete iActive;
    }
// -----------------------------------------------------------------------------
// 
// 
// 
// -----------------------------------------------------------------------------
void CRemConPoc::CompleteResponse( const TInt aError )
    {
    	if( aError != KErrNone )
	    {
    	COMPONENT_TRACE( ( _L( "REMCONTESTCONTROL -  CRemConPoc::CompleteResponse( %d)" ), aError) );
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
void CRemConPoc::APOCButtonDown()
{
    iEvent     = ETFRemConEvent_PocDown;
    iParameter = KErrNone;
    MRemConControl* aControl = GetRemConControl();
	aControl->NotifyKeyEvent( iEvent, iParameter );
}
// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//	
void CRemConPoc::APOCButtonUp()
{
    iEvent     = ETFRemConEvent_PocUp;
    iParameter = KErrNone;
    MRemConControl* aControl = GetRemConControl();
	aControl->NotifyKeyEvent( iEvent, iParameter );
}
		
// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//	
void CRemConPoc::APOCButtonUpResponse()
{	
    iActive->iStatus = KRequestPending;
	iPoc->APOCButtonUpResponse( iActive->iStatus, iError );
	iActive->SetActive(); 
}
   
// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//	
void CRemConPoc::APOCButtonDownResponse()
{	

    iActive->iStatus = KRequestPending;
	iPoc->APOCButtonDownResponse( iActive->iStatus, iError );
	iActive->SetActive();
}
   
void CRemConPoc::GetCommandSourceInfo(TRemConExtCmdSource& aSource)
    {
    iPoc->GetCommandSourceInfo( aSource );
    }
