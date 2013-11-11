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
#include "cmmcalllist.h"
#include "cmmcalltsy.h"
#include "cmmphonetsy.h"

// ======== MEMBER FUNCTIONS ========

CMmCallList::CMmCallList()
    {
    }

CMmCallList* CMmCallList::NewL(
    CMmPhoneTsy* aMmPhone )
    {
    
    CMmCallList* aCallList = NULL;
        
    if ( aMmPhone )
        {
        aCallList = new (ELeave) CMmCallList();  
        CleanupStack::PushL( aCallList );
        aCallList->iMmPhone = aMmPhone;
        aCallList->ConstructL();
        CleanupStack::Pop();
        }

    return aCallList;
    }

void CMmCallList::ConstructL()
    {
    //Create call container
    CreateContainerL();
    }

CMmCallList::~CMmCallList()
    {
    }

// ---------------------------------------------------------------------------
// CMmCallList::GetMmCallByIndex
// Returns call object by index. The indexing begins from 0.
// ---------------------------------------------------------------------------
//
CMmCallTsy* CMmCallList::GetMmCallByIndex(
    TInt aIndex )
    {
    CMmCallTsy* mmCall = NULL;

    if ( iObjectContainer )
        {
        if ( aIndex >= 0 && aIndex < iObjectContainer->Count() )
            {
            mmCall = reinterpret_cast<CMmCallTsy*>(
                iObjectContainer->At( aIndex ) );
            }
        }

    return mmCall;
    }

// ---------------------------------------------------------------------------
// CMmCallList::GetMmCallByIndexAndLine
// Returns call object by index and line.
// ---------------------------------------------------------------------------
//
CMmCallTsy* CMmCallList::GetMmCallByIndexAndLine(
    TInt aIndex,
    const TName* aLineName )
    {

    CMmCallTsy* mmCall = NULL;
    CMmCallTsy* mmCallSeek = NULL;

    TInt callCount = iObjectContainer->Count();
    TInt index( 0 );

    //search through call list
    for ( TInt i = 0; i < callCount; i++ )
        {
        //get current call
        mmCallSeek = reinterpret_cast<CMmCallTsy*>(
            iObjectContainer->At( i ) );

        //get the name of the line from which this call has been opened.
        //see class CMmLineTsy to see how the call object naming works.
        TName curLineName = mmCallSeek->
                                CallName().Left( aLineName->Length() );

        //if the name is the same as the line name given as input parameter
        if ( curLineName.Compare( *aLineName ) == 0 )
            {
            if ( index == aIndex )
                {
                //Break out
                mmCall = mmCallSeek;
                i = callCount;
                }
            else
                {
                index++;
                }
            }
        }

    return mmCall;
    }

// ---------------------------------------------------------------------------
// CMmCallList::GetMmCallById
// Returns call object by call id. Call ID has to differ from 0 because
// call list may contain multiple call objects with call ID 0!
// In that case the method will return NULL.
// ---------------------------------------------------------------------------
//
CMmCallTsy* CMmCallList::GetMmCallById(
    const TInt aCallId )
    {
    CMmCallTsy* mmCall = NULL;
    CMmCallTsy* mmCallSeek = NULL;

    if ( ( 0 < aCallId ) && iObjectContainer )
        {
        for ( TInt i = 0; i < iObjectContainer->Count(); i++ )
            {
            mmCallSeek = reinterpret_cast<CMmCallTsy*>(
                iObjectContainer->At( i ) );
            
            if ( aCallId == mmCallSeek->CallId() )
                {
                mmCall = mmCallSeek;
                break;
                }
            }
        }

    return mmCall;
    }

// ---------------------------------------------------------------------------
// CMmCallList::GetMmCallByName
// Returns call object by name.
// ---------------------------------------------------------------------------
//
CMmCallTsy* CMmCallList::GetMmCallByName(
    const TName* aName )
    {
    CMmCallTsy* mmCall = NULL;
    CMmCallTsy* mmCallSeek = NULL;

    if ( iObjectContainer )
        {
        for ( TInt i = 0; i < iObjectContainer->Count(); i++ )
            {
            mmCallSeek = reinterpret_cast<CMmCallTsy*>(
                iObjectContainer->At( i ) );  

            TName aCurrCallName = mmCallSeek->CallName();
            if ( aCurrCallName.Compare( *aName ) == KErrNone )
                {
                mmCall = mmCallSeek;
                break;
                }
            }
        }

    return mmCall;
    }

// ---------------------------------------------------------------------------
// CMmCallList::GetMmCallByMode
// Returns call object by mode.
// ---------------------------------------------------------------------------
//
CMmCallTsy* CMmCallList::GetMmCallByMode(
    RMobilePhone::TMobileService aCallMode )
    {
    CMmCallTsy* mmCall = NULL;
    CMmCallTsy* mmCallSeek = NULL;

    if ( iObjectContainer )
        {
        for ( TInt i = 0; i < iObjectContainer->Count(); i++ )
            {
            mmCallSeek = reinterpret_cast<CMmCallTsy*>(
                iObjectContainer->At( i ) ); 
            if ( mmCallSeek->CallMode() == aCallMode )
                {           
                mmCall = mmCallSeek;
                break;
                }
            }
        }

    return mmCall;
    }

// ---------------------------------------------------------------------------
// CMmCallList::RemoveMmCallById
// Removes a call by id from the list of calls.
// ---------------------------------------------------------------------------
//
TInt CMmCallList::RemoveMmCallById(
    TInt aMmCallId )
    {
    TInt ret( KErrNotFound );

    if ( iObjectContainer )
        {
        for ( TInt i = 0; i < iObjectContainer->Count(); i++ )
            {
            CMmCallTsy* mmCall = 
                reinterpret_cast<CMmCallTsy*>( iObjectContainer->At( i ) );

            if ( aMmCallId == mmCall->CallId() )
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
// CMmCallList::GetMmCallByStatus
// Returns a call by status from the list of calls.
// ---------------------------------------------------------------------------
//
CMmCallTsy* CMmCallList::GetMmCallByStatus(
    RMobileCall::TMobileCallStatus aStatus )
    {
    CMmCallTsy* mmCall = NULL;
    CMmCallTsy* mmCallSeek = NULL;

    if ( iObjectContainer )
        {
        for ( TInt i = 0; i < iObjectContainer->Count(); i++ )
            {
            mmCallSeek = reinterpret_cast<CMmCallTsy*>(
                iObjectContainer->At( i ) );
            if ( mmCallSeek->MobileCallStatus() == aStatus )
                {           
                mmCall = mmCallSeek;
                break;
                }
            }
        }

    return mmCall;
    }

// ---------------------------------------------------------------------------
// CMmCallList::RemoveCallsByLine
// Removes Call objects from Call list that has been opened 
// from the Line (which name is given as a input parameter).
// ---------------------------------------------------------------------------
//
void CMmCallList::RemoveCallsByLine(
    const TName* aLineName )
    {
    //if call list exists
    if ( iObjectContainer )
        {
        TInt callCount = iObjectContainer->Count();
        TInt index( 0 );
        //search through call list
        for ( TInt i = 0; i < callCount; i++ )
            {
            //get current call
            CMmCallTsy* mmCall = 
              reinterpret_cast<CMmCallTsy*>( iObjectContainer->At( index ) );

            //get the name of the line from which this call has been opened.
            //see class CMmLineTsy to see how the call object naming works.
            TName curLineName = mmCall->
                                    CallName().Left( aLineName->Length() );

            //if the name is the same as the line name given as input param.
            if ( curLineName.Compare( *aLineName ) == 0 )
                {
                //close this call
                mmCall->Close();
                index--;
                }
            index++;
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmCallList::AddObject
// Adds a call object to the array and remove unused call objects.
// from the Line (which name is given as a input parameter).
// ---------------------------------------------------------------------------
//
TInt CMmCallList::AddObject(
    CTelObject* aTsyObject )
    {   
    TInt ret( KErrGeneral );

    if ( iObjectContainer )
        {    
        for ( TInt i = 0; i < iObjectContainer->Count(); i++ )
            {
            CMmCallTsy* mmCall = reinterpret_cast<CMmCallTsy*>(
                iObjectContainer->At( i ) );

            //Remove object for incoming call or ghost call that is on idle
            //state and don't have a owner (client)
            if ( ( RCall::EStatusIdle == mmCall->Status() ) &&
                 ( mmCall->IsUnownedCallObject() ) )
                {
                iObjectContainer->Delete( i );
                mmCall->Close();
                }
            }

        if ( iObjectContainer->Count() > 1 )
            {
            iObjectContainer->Compress();
            }

        TInt trapError( KErrNone );

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

//  End of File 
