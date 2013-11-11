/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Base class for accessory objects.
*
*/
 
#include <accpolobjectbase.h>
#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolObjectBase::CAccPolObjectBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CAccPolObjectBase::CAccPolObjectBase()    
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectBase::CAccPolObjectBase - Enter" );	
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectBase::CAccPolObjectBase - Return" );		
    }

// -----------------------------------------------------------------------------
// CAccPolObjectBase::CAccPolObjectBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CAccPolObjectBase::CAccPolObjectBase( TUint8 aObjectType, CAccPolObjectCon* aObject )
    : iObjectContainer( aObject ), 
    iObjectType( aObjectType )
          
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectBase::CAccPolObjectBase - Enter" );	
    API_TRACE_1( "[AccFW:ACCPOLICY] CAccPolObjectBase::CAccPolObjectBase - ObjectType=%d", aObjectType );	
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectBase::CAccPolObjectBase - Return" );			
    }

// Destructor
EXPORT_C CAccPolObjectBase::~CAccPolObjectBase()
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectBase::~CAccPolObjectBase - Enter" );	
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectBase::~CAccPolObjectBase - Return" );			
    }
    
// ---------------------------------------------------------------------------
// CAccPolObjectBase::operator
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolObjectBase& CAccPolObjectBase::operator=( const CAccPolObjectBase &aObjectBase )
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectBase::operator - Enter" );	
    iObjectType =	aObjectBase.ObjectType();	
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectBase::operator - Return" );
    return ( *this );
    }
            
// ---------------------------------------------------------------------------
// CAccPolObjectBase::SetObjectType
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolObjectBase::SetObjectType( TUint8 aObjectType )
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectBase::SetObjectType - Enter" );		
    API_TRACE_1( "[AccFW:ACCPOLICY] CAccPolObjectBase::SetObjectType=%d", aObjectType );
    iObjectType = aObjectType;
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectBase::SetObjectType - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolObjectBase::ObjectId
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CAccPolObjectBase::ObjectType() const
    {
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectBase::ObjectType - Enter" );			
    API_TRACE_1( "[AccFW:ACCPOLICY] CAccPolObjectBase::ObjectType=%d", iObjectType );
    API_TRACE_( "[AccFW:ACCPOLICY] CAccPolObjectBase::ObjectType - Return" );
    return iObjectType;
    }
