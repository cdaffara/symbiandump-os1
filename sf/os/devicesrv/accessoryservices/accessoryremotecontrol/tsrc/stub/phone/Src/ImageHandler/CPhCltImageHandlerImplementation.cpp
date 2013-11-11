/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the PhoneClient Messenger interface.
*
*/



// INCLUDE FILES
#include    "CPhCltImageHandlerImplementation.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhCltImageHandlerImplementation::NewL
// 
// Creates Phone Client image handler instance.
// -----------------------------------------------------------------------------
//
CPhCltImageHandlerImplementation* 
    CPhCltImageHandlerImplementation::NewL()
    {
    CPhCltImageHandlerImplementation* self = 
        new (ELeave) CPhCltImageHandlerImplementation;   
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CPhCltImageHandlerImplementation::~CPhCltImageHandlerImplementation
// 
// Destructor.
// -----------------------------------------------------------------------------
//
CPhCltImageHandlerImplementation::~CPhCltImageHandlerImplementation()
    {
    iStillHandler.Close();
    iServer.Close();
    }

// -----------------------------------------------------------------------------
// CPhCltImageHandlerImplementation::SaveImages
// 
// Saves a number of images to PhoneServer.
// -----------------------------------------------------------------------------
//
TInt CPhCltImageHandlerImplementation::
    SaveImages( CPhCltImageParams& aParams )
    {
    return iStillHandler.SaveImages( aParams );
    }
  
// -----------------------------------------------------------------------------
// CPhCltImageHandlerImplementation::LoadImages
// 
// Loads a number of images from PhoneServer.
// -----------------------------------------------------------------------------
//  
TInt CPhCltImageHandlerImplementation::
    LoadImages( CPhCltImageParams*& aParams )
    {
    return iStillHandler.LoadImages( aParams );
    }

// -----------------------------------------------------------------------------
// CPhCltImageHandlerImplementation::CPhCltImageHandlerImplementation
// 
// C++ constructor.
// -----------------------------------------------------------------------------
//
CPhCltImageHandlerImplementation::CPhCltImageHandlerImplementation()
    {
    }
    
    
 // -----------------------------------------------------------------------------
// CPhCltImageHandlerImplementation::ConstructL
// 
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhCltImageHandlerImplementation::ConstructL()
    {
    User::LeaveIfError( iServer.Connect() );    
    User::LeaveIfError ( iStillHandler.Open( iServer ) ); 
    }

// -----------------------------------------------------------------------------
// CPhCltImageHandlerImplementation::CPhCltBaseImageParamsL
// 
// Creates Phone Client image handler parameter class instance.
// -----------------------------------------------------------------------------
//
CPhCltImageParams* CPhCltImageHandlerImplementation::
    CPhCltBaseImageParamsL( const TPhCltImageType aType )
    {
    CPhCltBaseImageParams* param = NULL;
    switch ( aType )
        {
        case EPhCltTypeVTStill:
        case EPhCltTypeVTDefault:
            param = CPhCltExtVTImageParams::NewL( aType );
            break;
        case EPhCltTypeOperatorLogo:
            param = CPhCltExtOperatorLogoParams::NewL();
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }
    return param;
    }

//  End of File
