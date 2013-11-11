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



// INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmmbmscontextlistTraces.h"
#endif

#include "cmmmbmscontextlist.h"

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CMmMBMSContextList::CMmMBMSContextList
// C++ constructor
// ---------------------------------------------------------------------------
//
CMmMBMSContextList::CMmMBMSContextList()
    {
    }

CMmMBMSContextList* CMmMBMSContextList::NewL()
    {

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTLIST_NEWL_1, "TSY: CMmMBMSContextList* CMmMBMSContextList::NewL." );

    CMmMBMSContextList* const contextList =
        new ( ELeave ) CMmMBMSContextList();

    CleanupStack::PushL( contextList );
    contextList->ConstructL();
    CleanupStack::Pop();

    return contextList;

    }

void CMmMBMSContextList::ConstructL()
    {

    //Create container
    User::LeaveIfError( CreateContainerL() );

    }

CMmMBMSContextList::~CMmMBMSContextList()
    {
    //delete object container
    DeleteContainer();    
    }
       
       

// ---------------------------------------------------------------------------
// CMmMBMSContextList::AddContext
// Adds a CMmMBMSContextTsy object to the tsy object and context info lists.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextList::AddContext( CMmMBMSContextTsy* aContext )      
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTLIST_ADDCONTEXT_1, "TSY: CMmMBMSContextList::AddContext.");

    TInt ret( KErrArgument );
    
    ret = CMmTsyObjectList::AddObject( aContext );
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextList::RemoveContext
// Removes a CMmMBMSContextTsy object from the tsy object context info
// lists. Sets used proxy id released in proxy id list.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextList::RemoveContext( CMmMBMSContextTsy* aContext )
    {

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTLIST_REMOVECONTEXT_1, "TSY: CMmMBMSContextList::RemoveContext." );

    TInt ret = RemoveObject(aContext);

    return ret;
    }


// ---------------------------------------------------------------------------
// CMmMBMSContextList::ContextByIndex
// Returns Packet Context by index.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmMBMSContextTsy* CMmMBMSContextList::ContextByIndex( TInt aIndex ) const
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTLIST_CONTEXTBYINDEX_1, "TSY: CMmMBMSContextList::ContextByIndex. aIndex: %d", aIndex );
	return (CMmMBMSContextTsy*)GetObjectByIndex(aIndex);
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextList::ContextByName
// Returns Packet Context by name.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmMBMSContextTsy* CMmMBMSContextList::ContextByName(
        const TInfoName& aName ) //name of the context
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTLIST_CONTEXTBYNAME_1, "TSY: CMmMBMSContextList::ContextByName. Context Name: %S ", aName );
    // Find name from context info list
    TInt size = NumberOfContexts();    
    for ( TInt i = 0; i < size; i++ )
        {
        CMmMBMSContextTsy* current = ContextByIndex(i);
        if ( aName == current->Name() )
            {

            // Name found, return object 
            return current;

            }
        }

    // Object not found, return NULL
    return NULL;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextList::NumberOfContexts
// This function returns the number of created contexts.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextList::NumberOfContexts()
	{
    return GetNumberOfObjects();
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextList::GenerateNewContextName
// This function generates new proxy id and context name.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmMBMSContextList::GenerateNewContextName(
        TDes& aNewName,     //name of the context
        TUint8& aProxyId,     //proxy id of the context
        TInt aMaxContexts )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTLIST_GENERATENEWCONTEXTNAME_1, "TSY: CMmMBMSContextList::GenerateNewContextName." );
    
	if(aMaxContexts <= NumberOfContexts() )
		{
		return KErrOverflow;
		}
	TInt idx( 0 );
	aNewName.Zero();
	
	_LIT( KContextNameInt, "MBMS_Context_" );
	aNewName.Append( KContextNameInt );

	// find last context index
	if(NumberOfContexts() !=0 )
		{
		idx = ContextByIndex(NumberOfContexts() - 1)->ContextIndex() + 1;
		}
	aNewName.AppendNum( idx );
	aProxyId = idx;
	
	return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmMBMSContextList::IsMbmsContextReceiving
// This function boolean whether MBMS is received currently
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmMBMSContextList::IsMbmsContextReceiving()
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMBMSCONTEXTLIST_ISMBMSCONTEXTRECEIVING_1, "TSY: CMmMBMSContextList::IsMbmsContextReceiving." );
	TInt size = NumberOfContexts();
	for( TInt i = 0; i < size; i++ )
        {
       	
       	if( RPacketContext::EStatusReceiving == ContextByIndex( i )->ContextStatus() )
	       	{
	       	return ETrue;
	       	}
        }
            
    return EFalse;	
	}
		

//  End of File 



