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



// INCLUDE FILES
#include "epos_cpossingletonmanager.h"
#include "epos_defaultproxycommon.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosSingletonManager::CPosSingletonManager()
    {
    }

// ---------------------------------------------------------
// CPosSingletonManager::GetInstanceL
// ---------------------------------------------------------
//
CPosSingletonManager* CPosSingletonManager::GetInstanceL()
    {
    CPosSingletonManager* self = reinterpret_cast< CPosSingletonManager* > (
        Dll::Tls() );
        
    if ( !self ) 
        {
        self = new ( ELeave ) CPosSingletonManager;
        Dll::SetTls( self );
        }

    return self;
    }

// ---------------------------------------------------------
// CPosSingletonManager::GetInstanceL
// ---------------------------------------------------------
//
CPosSingletonManager* CPosSingletonManager::GetInstance()
    {
    CPosSingletonManager* self = reinterpret_cast< CPosSingletonManager* > (
        Dll::Tls() );
        
    return self;
    }

// Destructor
CPosSingletonManager::~CPosSingletonManager()
    {
    TRACESTRING( "CPosSingletonManager::destructor" )
    iObjectsArray.Close();
    Dll::SetTls( NULL );
    }

// ---------------------------------------------------------
// CPosSingletonManager::GetObject
// ---------------------------------------------------------
//
CBase* CPosSingletonManager::GetObject( TInt aObjectId )
    {
    CPosSingletonManager* self = GetInstance();
    if ( self )
        {
        return self->GetAObject( aObjectId );
        }
    return NULL;
    }

// ---------------------------------------------------------
// CPosSingletonManager::GetAObject
// ---------------------------------------------------------
//
CBase* CPosSingletonManager::GetAObject( TInt aObjectId )
    {
    TInt count = iObjectsArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iObjectsArray[i].iObjectId == aObjectId )
            {
            return iObjectsArray[i].iObject;
            }
        }
    return NULL;
    }

        
// ---------------------------------------------------------
// CPosSingletonManager::SetObjectL
// ---------------------------------------------------------
//
void CPosSingletonManager::SetObjectL( 
            CBase* aObject,
            TInt aObjectId )
    {
    if( aObject )
        {
        CPosSingletonManager* self = GetInstanceL();
        self->SetAObjectL( aObject, aObjectId );
        }
    else
        {
        CPosSingletonManager* self = GetInstance();
        if ( self )
            {
            self->SetAObjectL( aObject, aObjectId );
            }
        }
    }

// ---------------------------------------------------------
// CPosSingletonManager::SetAObjectL
// ---------------------------------------------------------
//
void CPosSingletonManager::ReleaseObject(
            TInt aObjectId )
    {
    CPosSingletonManager* self = GetInstance();
    if ( self )
        {
        self->ReleaseAObject( aObjectId );
        }
    }

// ---------------------------------------------------------
// CPosSingletonManager::SetAObjectL
// ---------------------------------------------------------
//
void CPosSingletonManager::SetAObjectL( 
            CBase* aObject,
            TInt aObjectId )
    {
    //Check if the object is already in the array. If
    //yes, then delete the old object and store the new one.
    TInt count = iObjectsArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iObjectsArray[i].iObjectId == aObjectId )
            {
            delete iObjectsArray[i].iObject;
            iObjectsArray[i].iObject = aObject;
            ClearIfNeeded();
            return;
            }
        }
        
    //If the object has not be stored, then add this object
    //to the array.
    if( aObject )
        {
        TSingletonOb ob;
        ob.iObject = aObject;
        ob.iObjectId = aObjectId;
        
        User::LeaveIfError( iObjectsArray.Append( ob ) );
        }
    }

// ---------------------------------------------------------
// CPosSingletonManager::ReleaseAObject
// ---------------------------------------------------------
//
void CPosSingletonManager::ReleaseAObject( TInt aObjectId )
    {
    TInt count = iObjectsArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iObjectsArray[i].iObjectId == aObjectId )
            {
            delete iObjectsArray[i].iObject;
            iObjectsArray[i].iObject = NULL;
            ClearIfNeeded();
            return;
            }
        }
        
    }

// ---------------------------------------------------------
// CPosSingletonManager::ClearIfNeeded
// ---------------------------------------------------------
//
void CPosSingletonManager::ClearIfNeeded()
    {
    //Check if there is still singleton objects stored.
    TInt count = iObjectsArray.Count();
    for( TInt i=0; i < count; i++ )
        {
        TSingletonOb& ob = iObjectsArray[i];
        if( ob.iObject )
            {
            //There is still singleton objects. We shall not
            //clear this object
            return;
            }
        }
        
    //No singleton objects is stored, then delete this object
    delete this;
    }


//  End of File
