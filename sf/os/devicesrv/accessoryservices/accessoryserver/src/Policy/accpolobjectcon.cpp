/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container implementation for accessory policy name/value pair objects.
*
*/
 
#include <accpolobjectcon.h>
#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolObjectCon::CAccPolObjectCon
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CAccPolObjectCon::CAccPolObjectCon()
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::CAccPolObjectCon - Enter" ); 
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::CAccPolObjectCon - Return" );      
    }

// -----------------------------------------------------------------------------
// CAccPolObjectCon::CAccPolObjectCon
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CAccPolObjectCon::CAccPolObjectCon( TUint8 aObjectType, CAccPolObjectCon* aObject ) 
                                             : CAccPolObjectBase( aObjectType, aObject )
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::CAccPolObjectCon - Enter" ); 
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::CAccPolObjectCon - Return" );        
    }    

// Destructor
EXPORT_C CAccPolObjectCon::~CAccPolObjectCon()
    {    
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::~CAccPolObjectCon - Enter" );      
    Reset();  
    iObjects.Close();
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::~CAccPolObjectCon - Return" );         
    }

// ---------------------------------------------------------------------------
// CAccPolObjectCon::Reset()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolObjectCon::Reset()
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::Reset() - Enter" );
    iObjects.ResetAndDestroy();
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::Reset() - Return" );
  }
// ---------------------------------------------------------------------------
// CAccPolObjectCon::AddL()
// ---------------------------------------------------------------------------
//             
EXPORT_C void CAccPolObjectCon::AddL( const CAccPolObjectBase& aObject )
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::AddL() - Enter" );    
    
    CAccPolObjectBase* ptr = 
                       const_cast<CAccPolObjectBase&>( aObject ).DuplicateLC( this );    
    iObjects.AppendL( ptr );
    CleanupStack::Pop();    
    
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::AddL() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolObjectCon::AtL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolObjectBase *const &CAccPolObjectCon::AtL( TInt aIndex ) const
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::AtL() - Enter" );      
    API_TRACE_1( "[AccFW:ACCPOLICY] CAccPolObjectCon::AtL() - index=%d", aIndex );    
    TInt ObjectCount( iObjects.Count() );
    if( ObjectCount == 0 )
        {
        API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::AtL - Container is empty!" );
        User::Leave( KErrGeneral );
        }
        else
        {        
        if ( aIndex < 0 )
            {
            API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::AtL() - KErrUnderflow" );  
            User::Leave( KErrUnderflow ); 
            }
        else if( aIndex >= ObjectCount ) 
            {
            API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::AtL() - KErrOverflow" ); 
            User::Leave( KErrOverflow );    
            }
        else
            {
            API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::AtL() - In range" );
            }
        }

    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::AtL() - Return" );
    return (CAccPolObjectBase*&)iObjects.operator[]( aIndex );
    }
    
// ---------------------------------------------------------------------------
// CAccPolObjectCon::Count()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAccPolObjectCon::Count() const
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::Count() - Enter" );        
    TInt count ( iObjects.Count() );
    API_TRACE_1( "[AccFW:ACCPOLICY] CAccPolObjectCon:Count - Count=%d ", count );
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectCon::Count() - Return" );     
    return count;
    }   
            
//  End of File    
