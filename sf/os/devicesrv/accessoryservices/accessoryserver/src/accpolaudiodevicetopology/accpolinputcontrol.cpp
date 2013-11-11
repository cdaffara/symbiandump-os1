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
* Description:  Implementation for mute control object.
*
*/
 
#include <accpolinputcontrol.h>
#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolInputControl::CAccPolInputControl()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolInputControl::CAccPolInputControl( CAccPolObjectCon* aObject )
    : CAccPolAudioControlBase ( EAccPolAudioInputControlObject, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::CAccPolInputControl() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolMuteControl::CAccPolInputControl() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolMuteControl::CAccPolMuteControl()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolInputControl::CAccPolInputControl( const TUint8 aUnitId )
    : CAccPolAudioControlBase ( EAccPolAudioMuteControlObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::CAccPolInputControl() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::CAccPolInputControl() - Return" );
    iUnitId = aUnitId;
    }

// ---------------------------------------------------------------------------
// CAccPolInputControl::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolInputControl* CAccPolInputControl::NewL( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::NewL() - Enter" );    
    CAccPolInputControl* self = CAccPolInputControl::NewLC( aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::NewL() - Return" );	
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolInputControl::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolInputControl* CAccPolInputControl::NewLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::NewLC() - Enter" );    	
    CAccPolInputControl* self = new( ELeave ) CAccPolInputControl( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::NewLC() - Return" );    
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolInputControl::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolInputControl* CAccPolInputControl::NewL( const TUint8 aUnitId )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::NewL() - Enter" );    	
    CAccPolInputControl* self =
                         CAccPolInputControl::NewLC( aUnitId );

    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::NewL()- Return" );    
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolInputControl::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolInputControl* CAccPolInputControl::NewLC( const TUint8 aUnitId )                                                        

    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::NewLC() - Enter" );    		
    CAccPolInputControl* self = new( ELeave ) CAccPolInputControl( aUnitId );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::NewLC() - Return" );    	
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolInputControl::ConstructL()
// ---------------------------------------------------------------------------
//
void CAccPolInputControl::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::ConstructL() - Enter" );    		
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::ConstructL()- Return void" );    			
    }

// Destructor
EXPORT_C CAccPolInputControl::~CAccPolInputControl()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::~CAccPolInputControl() - Enter" );    		
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::~CAccPolInputControl()- Return void" );    				
    }

// ---------------------------------------------------------------------------
// CAccPolInputControl::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolInputControl& CAccPolInputControl::operator=( const CAccPolInputControl& aInputControlObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::operator() - Enter" );    			
	  CAccPolAudioControlBase::operator=( (CAccPolAudioControlBase&)aInputControlObject );	  	  	  
	  iUnitId = aInputControlObject.Input();
	  API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::~CAccPolInputControl() - Return" );    		
	  return ( *this );
    }
    
// ---------------------------------------------------------------------------
// CAccPolInputControl::SetInput()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolInputControl::SetInput( const TUint8 aUnitId )
    {
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::SetInput() - Input=%d", aUnitId );    			
    iUnitId = aUnitId;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::SetInput() - Return" );    		
    }
// ---------------------------------------------------------------------------
// CAccPolInputControl::Input()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CAccPolInputControl::Input() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::input() - Enter" );    			
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::input() - Input=%d", iUnitId );    				
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::input() - Return" );    			
    return iUnitId;
    }

// ---------------------------------------------------------------------------
// CAccPolInputControl::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolInputControl::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::ExternalizeL() - Enter" );
    aStream.WriteUint8L( ObjectType() );
    ExternalizeAudioControlbaseL( aStream );
    aStream.WriteUint8L( iUnitId );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolInputControl::InternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolInputControl::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::InternalizeL() - Enter" );
    InternalizeAudioControlbaseL( aStream );
    iUnitId = aStream.ReadUint8L();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::InternalizeL - return void" );
    }

// ---------------------------------------------------------------------------
// CAccPolMuteControl::DuplicateL()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolInputControl::DuplicateLC( CAccPolObjectCon* aObject )
    {
		API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::DuplicateLC() - Enter" );
		CAccPolInputControl* aInput = CAccPolInputControl::NewLC( aObject );		
    *aInput = (CAccPolInputControl&)*this;            		
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputControl::DuplicateLC() - Return" );            
    return aInput;	
    }
    
//  End of File
