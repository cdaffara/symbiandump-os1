// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CTsyDelegatesTraces.h"
#endif

#include "ctsydelegates.h"
#include "cmmmessagemanagerbase.h"

// ======== MEMBER FUNCTIONS ========

CTsyDelegates* CTsyDelegates::NewL()
    {
    CTsyDelegates* tsyDelegates = new( ELeave ) CTsyDelegates();
    return tsyDelegates;
    }
    
CTsyDelegates::CTsyDelegates()
    {	
    }
    
 CTsyDelegates::~CTsyDelegates()
    {
    }
// ---------------------------------------------------------------------------
// CTsyDelegates::GetTsyObject
// Returns pointer requested TSY object
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CBase* CTsyDelegates::GetTsyObject( 
    CMmMessageManagerBase::TTsyObjects aObject )
	{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTSYDELEGATES_GETTSYOBJECT_1, "TSY: CTsyDelegates::GetTsyObject object=%d", aObject );
	
	CBase* object = NULL;	
 	object = iTsyObjectArray.At( aObject );	
 	return object ; 	
	}

// ---------------------------------------------------------------------------
// CTsyDelegates::RegisterTsyObject
// Registers a Tsy object for request and notification completions
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CTsyDelegates::RegisterTsyObject(
    CMmMessageManagerBase::TTsyObjects aTsyObjectType,
    CBase* aTsyObject )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTSYDELEGATES_REGISTERTSYOBJECT_1, "TSY: CTsyDelegates::RegisterTsyObject type=%d address=0x%x", TInt( aTsyObjectType ), aTsyObject );

    iTsyObjectArray[ aTsyObjectType ] = aTsyObject;
    }

// ---------------------------------------------------------------------------
// CTsyDelegates::DeregisterTsyObject
// Remove previously registered Tsy object from the array. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CTsyDelegates::DeregisterTsyObject( CBase* aTsyObject )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTSYDELEGATES_DEREGISTERTSYOBJECT_1, "TSY: CTsyDelegates::DeregisterTsyObject address=0x%x", aTsyObject );

    for( TInt i = 0; i < TInt( 
        CMmMessageManagerBase::EMaxNumOfTsyObjects ); i++ )
        {
        if( iTsyObjectArray[ i ] == aTsyObject )
            {
            iTsyObjectArray[ i ] = 0;
            break;
            }
        }
    }


//  End of File
