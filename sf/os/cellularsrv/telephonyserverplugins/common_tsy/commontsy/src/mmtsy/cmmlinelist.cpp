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



//INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmlinelistTraces.h"
#endif

#include "cmmlinelist.h"
#include "cmmphonetsy.h"
#include "cmmlinetsy.h"
#include <ctsy/mmtsy_names.h>

// ======== MEMBER FUNCTIONS ========

CMmLineList::CMmLineList()
    {
    }
    
CMmLineList* CMmLineList::NewL( 
    CMmPhoneTsy* aMmPhone )
    {   
    CMmLineList* aLineList = NULL;
    
    if ( aMmPhone )
        {
        aLineList = new ( ELeave ) CMmLineList();  
        CleanupStack::PushL( aLineList );
        aLineList->iMmPhone = aMmPhone;
        aLineList->ConstructL();
        CleanupStack::Pop();
        }

    return aLineList;
    }

void CMmLineList::ConstructL()
    {    
    //Create line container
    CreateContainerL();
    
    //Max of 4 lines
    iObjectContainer->ResizeL( KMmTsyNumOfSupportedLines );
    
    iInitialised = EFalse;
    
    CTelObject* tempObject = 
                    reinterpret_cast< CTelObject* >( this );
    
    //Insert know objects to array                
    for ( TInt i = 0; i < KMmTsyNumOfSupportedLines; i++ )
        {
        iObjectContainer->Delete(i);
        iObjectContainer->Compress();
        iObjectContainer->InsertL( i, tempObject );                 
        } 
    
   	if( KMmTsyNumOfSupportedLines < iObjectContainer->Count() )   		
   		{
   		//Delete array objects that are greater three
   		//There should only be four elements in array at this point
        //iObjectContainer->Delete( KMmTsyNumOfSupportedLines, 
        //							iObjectContainer->Count() );
        //Compress Array
        iObjectContainer->Compress();                
   		}

    //no line is used
    for ( TInt i = 0; i < KMmTsyNumOfSupportedLines; i++)
        {
        iIsLineInUse[ i ] = EFalse;                
        }    
       
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINELIST_CONSTRUCTL_1, "TSY: CMmLineList::ConstructL:iObjectContainer->Count()=%d", iObjectContainer->Count());
       
    }

CMmLineList::~CMmLineList()
    {    
    //delete object container
    this->DeleteContainer();    
    }

// ==================== LOCAL FUNCTIONS =====================================

// ---------------------------------------------------------------------------
// CMmLineList::GetMmLineByMode
// Returns a Ptr to a Line object by mode.
//------------------------------------------------------------------------------
//
CMmLineTsy* CMmLineList::GetMmLineByMode(
    const RMobilePhone::TMobileService aLineMode ) const
    {    
    CMmLineTsy* mmLine = NULL;

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINELIST_GETMMLINEBYMODE_1, "TSY: CMmLineList::GetMmLineByMode");
    if ( iObjectContainer  && iInitialised)
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINELIST_GETMMLINEBYMODE_2, "TSY: CMmLineList::GetMmLineByMode::iObjectContainer->Count()=%d", iObjectContainer->Count());
       
        for ( TInt i = 0; i < iObjectContainer->Count(); i++ )
            {
            if ( iIsLineInUse[ i ] != EFalse )
                { 
                CMmLineTsy* mmLineSeek = 
                    reinterpret_cast<CMmLineTsy*>( 
                        iObjectContainer->At( i ) );

                if ( mmLineSeek->LineMode() == aLineMode )
                    {           
                    mmLine = mmLineSeek;
                    break;
                    }
                }
            }
        }

    return mmLine;
    }

// ---------------------------------------------------------------------------
// CMmLineList::GetMmLineByIndex
// Returns a Ptr to a Line by index. The indexing begins from 0.
// ---------------------------------------------------------------------------
//
CMmLineTsy* CMmLineList::GetMmLineByIndex( 
    TInt aIndex )
    {
    CMmLineTsy* mmLine = NULL;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINELIST_GETMMLINEBYINDEX_1, "TSY: CMmLineList::GetMmLineByIndex");
	//This is for checking that return objects is right
   	CTelObject* tempObject = 
                    reinterpret_cast< CTelObject* >( this );
                    
    if ( iObjectContainer && iInitialised )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINELIST_GETMMLINEBYINDEX_2, "TSY: CMmLineList::GetMmLineByIndex:iObjectContainer->Count()=%d", iObjectContainer->Count());

        if ( aIndex >= 0 && aIndex < iObjectContainer->Count() )
            {
            if( iObjectContainer->At( aIndex ) != tempObject )
            	{
                mmLine = reinterpret_cast<CMmLineTsy*>(
                	iObjectContainer->At( aIndex ) );	
            	}
            else
            	{
            	mmLine = NULL;	
            	}

            }
        }

    return mmLine;
    }

// ---------------------------------------------------------------------------
// CMmLineList::AddLineObject
// Adds a TSY to the Line list
// ---------------------------------------------------------------------------
//
TInt CMmLineList::AddLineObject ( 
    CTelObject* aTsyObject, const TName& aLineName )
    {
    TInt ret ( KErrGeneral );
    
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINELIST_ADDLINEOBJECT_1, "TSY: CMmLineList::AddLineObject");
    if ( iObjectContainer )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINELIST_ADDLINEOBJECT_2, "TSY: CMmLineList::AddLineObject:iObjectContainer->Count()=%d", iObjectContainer->Count());

        
        TInt trapError( KErrNone );    

        if ( 0 == aLineName.CompareF( KMmTsyVoice1LineName ) )
            {
            iIsLineInUse[ ELine0Voice1 ] = ETrue;
            iObjectContainer->Delete( ELine0Voice1 );
            iObjectContainer->Compress();
            TRAP( trapError, iObjectContainer->InsertL( ELine0Voice1, 
                                                        aTsyObject ) );
            }
        else if ( 0 == aLineName.CompareF( KMmTsyVoice2LineName ) )
            {
            iIsLineInUse[ ELine1Voice2 ] = ETrue;
            iObjectContainer->Delete( ELine1Voice2 );
            iObjectContainer->Compress();
            TRAP( trapError, iObjectContainer->InsertL( ELine1Voice2, 
                                                        aTsyObject ) );
            }
        else if ( 0 == aLineName.CompareF( KMmTsyDataLineName ) )
            {
            iIsLineInUse[ ELine2Data ] = ETrue;
            iObjectContainer->Delete( ELine2Data );
            iObjectContainer->Compress();
            TRAP( trapError, iObjectContainer->InsertL( ELine2Data,
                                                        aTsyObject ) );
            }
        else if ( 0 == aLineName.CompareF( KMmTsyFaxLineName ) )
            {
            iIsLineInUse[ ELine3Fax ] = ETrue;
            iObjectContainer->Delete( ELine3Fax );
            iObjectContainer->Compress();
            TRAP( trapError, iObjectContainer->InsertL( ELine3Fax,
                                                        aTsyObject ) );
            }
        //Check the trap error
        if ( KErrNone != trapError )
            {
            //change return value to indicate out of memory error
            ret = KErrNoMemory;
            }
        else
            {
            //success return value
            ret = KErrNone;
            iInitialised = ETrue;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmLineList::RemoveObject
// Marks a line object in array as not used
// ---------------------------------------------------------------------------
//
TInt CMmLineList::RemoveLine ( 
    const TName& aLineName )
    {
    TInt ret ( KErrGeneral );

    if ( iObjectContainer )
        {
        ret = KErrNone;
        if ( 0 == aLineName.CompareF( KMmTsyVoice1LineName ) )
            {
            iIsLineInUse[ ELine0Voice1 ] = EFalse;
            }
        else if ( 0 == aLineName.CompareF( KMmTsyVoice2LineName ) )
            {
            iIsLineInUse[ ELine1Voice2 ] = EFalse;
            }
        else if ( 0 == aLineName.CompareF( KMmTsyDataLineName ) )
            {
            iIsLineInUse[ ELine2Data ] = EFalse;
            }
        else if ( 0 == aLineName.CompareF( KMmTsyFaxLineName ) )
            {
            iIsLineInUse[ ELine3Fax ] = EFalse;
            }
        else
            {
            ret = KErrNotFound;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmLineList::DeleteContainer
// Deletes the Line List
// ---------------------------------------------------------------------------
//
void CMmLineList::DeleteContainer()
    {
    if ( iObjectContainer )
        {
        //first reset the container
        iObjectContainer->Reset();

        //then delete it
        delete iObjectContainer;
        iObjectContainer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CMmLineList::GetObjectByIndex
// Returns a Ptr to a Line object by index. The indexing begins from 0.
// ---------------------------------------------------------------------------
//
CBase* CMmLineList::GetObjectByIndex ( 
    TInt aIndex ) const
    {
    CBase* mmObject = NULL;

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINELIST_GETOBJECTBYINDEX_1, "TSY: CMmLineList::GetObjectByIndex. aIndex=%d", aIndex );
    if ( iObjectContainer )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMLINELIST_GETOBJECTBYINDEX_2, "TSY: CMmLineList::GetObjectByIndex. iObjectContainer->Count()=%d", iObjectContainer->Count());

        if ( iIsLineInUse[ aIndex ] )
            {
            mmObject = iObjectContainer->At( static_cast<TInt>( aIndex ) );
            }
        }

    return mmObject;
    }

//  End of File 



