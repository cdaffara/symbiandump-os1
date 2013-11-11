// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cmmtsyobjectlist.h"

// ======== MEMBER FUNCTIONS ========

CMmTsyObjectList::~CMmTsyObjectList()
    {
    
    iMmPhone = NULL;

    if ( iObjectContainer )
        {
        DeleteContainer();
        }
    iObjectContainer = NULL;

    }

// ---------------------------------------------------------------------------
// CMmTsyObjectList::CreateContainerL
// Creates the array of TSY objects.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmTsyObjectList::CreateContainerL()
    {
    //Granularity should be carefully chosen to minimise the number of calls 
    //to the memory allocator and to avoid wasted space within the array 
    //buffer.
    TInt aGranularity( 2 );

    iObjectContainer = new (ELeave) CArrayPtrFlat<CTelObject>( aGranularity );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmTsyObjectList::DeleteContainer
// Deletes the list of TSY objects.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmTsyObjectList::DeleteContainer()
    {
    if ( iObjectContainer )
        {           
		TInt count = iObjectContainer->Count();
        while ( count > 0 )
            {
            count--;
            CTelObject* object = reinterpret_cast<CTelObject*>(
                iObjectContainer->At( count ) );
            object->Close();
            }

        //first reset the container
        iObjectContainer->ResetAndDestroy();
        //then delete it
        delete iObjectContainer;
        iObjectContainer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CMmTsyObjectList::GetObjectByIndex
// Returns TSY object by index. The indexing begins from 0.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CBase* CMmTsyObjectList::GetObjectByIndex(
    const TInt aIndex ) const
    {
    CBase* mmObject = NULL;

    if ( iObjectContainer )
        {
        if ( aIndex >= 0 && aIndex < iObjectContainer->Count() )
            {
            mmObject = iObjectContainer->At( aIndex );
            }
        }

    return mmObject;
    }

// ---------------------------------------------------------------------------
// CMmTsyObjectList::AddObject
// Adds a TSY object to the array.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmTsyObjectList::AddObject(
    CTelObject* aTsyObject )
    {
    TInt ret( KErrGeneral );

    if ( iObjectContainer )
        {
        TInt trapError( 0 );

        //Append the object to the container
        TRAP( trapError, iObjectContainer->AppendL( aTsyObject ) );
        
        if ( KErrNone != trapError )
            {
            //change return value to indicate out of memory error
            ret = KErrNoMemory;
            }
        else
            {
            //success return value
            ret = KErrNone;
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmTsyObjectList::RemoveObject
// Removes a TSY object from the array.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmTsyObjectList::RemoveObject(
    CTelObject* aTsyObject )
    {
    TInt ret( KErrNotFound );

    if ( iObjectContainer )
        {
        for ( TInt i = 0; i < iObjectContainer->Count(); i++ )
            {
            CTelObject* aCurrentObject = iObjectContainer->At( i );     
            if ( aCurrentObject == aTsyObject )
                {           
                iObjectContainer->Delete( i );          
                if ( iObjectContainer->Count() > 1 )
                    {
                    iObjectContainer->Compress();
                    }
                ret = KErrNone;
                break;
                }       
            }   
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmTsyObjectList::RemoveAll
// Removes all objects from the array.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmTsyObjectList::RemoveAll()
    {
    if ( iObjectContainer )
        {
        //Delete all objects from the container
        iObjectContainer->Delete( 0, iObjectContainer->Count() );
        }
    }

// ---------------------------------------------------------------------------
// CMmTsyObjectList::GetNumberOfObjects
// Returns the number of TSY objects in container.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmTsyObjectList::GetNumberOfObjects()
    {
    TInt count( 0 );

    if ( iObjectContainer )
        {
        //return number of object in the container
        count = iObjectContainer->Count();
        }

    return count;
    }

//  End of File 




