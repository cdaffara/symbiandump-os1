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
#include "remconactive.h"
#include "mremconobserver.h"
#include "siftrace.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
 
// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
// 
CRemConActive* CRemConActive::NewL( MRemConObserver& aObserver )
	{
	COMPONENT_TRACE( _L( "[REMCOMTESTCONTROL] CRemConActive::NewL() start" ) );
	CRemConActive* self = new(ELeave) CRemConActive( aObserver );
	COMPONENT_TRACE( _L( "[REMCOMTESTCONTROL] CRemConActive::NewL() end" ) );
	return self;
	}
// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
CRemConActive::~CRemConActive()
	{
	COMPONENT_TRACE( _L( "[REMCOMTESTCONTROL] CRemConActive::~CRemConActive() start" ) );
	
	COMPONENT_TRACE( _L( "[REMCOMTESTCONTROL] CRemConActive::~CRemConActive() end" ) );
	}

// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
CRemConActive::CRemConActive( MRemConObserver& aObserver )
:	CActive(CActive::EPriorityStandard),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}
	
// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
void CRemConActive::SetActive()
	{
	CActive::SetActive();
	}
	
// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
void CRemConActive::RunL()
	{
	TInt error( iStatus.Int() );
	iStatus =  KErrNone;
	iObserver.CompleteResponse( error );
	}
// -----------------------------------------------------------------------------
// 
// 
// -----------------------------------------------------------------------------
//
void CRemConActive::DoCancel()
	{

	}

// End of file
