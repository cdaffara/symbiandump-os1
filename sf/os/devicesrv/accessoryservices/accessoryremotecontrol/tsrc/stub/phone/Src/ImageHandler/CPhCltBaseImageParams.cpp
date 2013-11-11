/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Parameter class.
*
*/


#include    "CPhCltBaseImageParams.h"
#include    "PhCltClientServer.h"
// -----------------------------------------------------------------------------
// CPhCltBaseImageParams::CPhCltBaseImageParams
// 
// C++ constructor.
// -----------------------------------------------------------------------------
//
CPhCltBaseImageParams::CPhCltBaseImageParams( 
    const TPhCltImageType aType )
    {
    iImages().iType = aType;    
    }

// -----------------------------------------------------------------------------
// CPhCltBaseImageParams::Type
// 
// Returns type.
// -----------------------------------------------------------------------------
//
TPhCltImageType CPhCltBaseImageParams::Type() const
    {
    return iImages().iType;
    }

// -----------------------------------------------------------------------------
// CPhCltBaseImageParams::AddImage
// 
// Adds an image.
// -----------------------------------------------------------------------------
//
void CPhCltBaseImageParams::AddImageL( TInt aHandle )
    {
    if ( iImages().iImageCount > KPhCltImagesArrayLen - 1 ) 
        {
        User::Leave( KErrOverflow );
        }
    iImages().iImages[ iImages().iImageCount ] = aHandle;
    iImages().iImageCount++;
    }

// -----------------------------------------------------------------------------
// CPhCltBaseImageParams::Clean
// 
// Cleans class content.
// -----------------------------------------------------------------------------
//
void CPhCltBaseImageParams::Clean()
    {
    iImages().iImageCount = 0;
    for ( TInt i = 0; i < KPhCltImagesArrayLen; i++ )
        {
        iImages().iImages[ i ] = 0;
        }
    }

// -----------------------------------------------------------------------------
// CPhCltBaseImageParams::CPhCltBaseImageParams
// 
// Destructor.
// -----------------------------------------------------------------------------
//
CPhCltBaseImageParams::~CPhCltBaseImageParams()
    {
    }

// -----------------------------------------------------------------------------
// CPhCltBaseImageParams::operator=
// 
// Assignment operator.
// -----------------------------------------------------------------------------
//
CPhCltBaseImageParams& CPhCltBaseImageParams::
    operator=( const CPhCltBaseImageParams& aParamClass )
    {
    Clean();
    this->iImages().iType = aParamClass.iImages().iType;
    this->iImages().iImageCount = aParamClass.iImages().iImageCount;
    for ( TInt i = 0; i < KPhCltImagesArrayLen; i++ )
        {
        iImages().iImages[ i ] = aParamClass.iImages().iImages[ i ];
        }
    return *this;
    }

// -----------------------------------------------------------------------------
// CPhCltBaseImageParams::GetImageL
// 
// Gets an image handle.
// -----------------------------------------------------------------------------
//
TInt CPhCltBaseImageParams::GetImageL( const TInt aIndex ) const
    {
    if ( aIndex > iImages().iImageCount - 1 ) 
        {
        User::Leave( KErrArgument );
        }
    return iImages().iImages[ aIndex ];
    }

// -----------------------------------------------------------------------------
// CPhCltBaseImageParams::Count
// 
// Return image count.
// -----------------------------------------------------------------------------
//
TInt CPhCltBaseImageParams::Count() const
    {
    return iImages().iImageCount;
    }

// -----------------------------------------------------------------------------
// CPhCltBaseImageParams::Images
// 
// Returns all images.
// -----------------------------------------------------------------------------
//
TPckgBuf< TPhCltImageArray >& CPhCltBaseImageParams::Images()
    {
    return iImages;
    }

// -----------------------------------------------------------------------------
// CPhCltBaseImageParams::CPhCltBaseImageParams
// 
// C++ constructor.
// -----------------------------------------------------------------------------
//
CPhCltExtVTImageParams::CPhCltExtVTImageParams( const TPhCltImageType aType ) 
    : CPhCltBaseImageParams( aType )
    {
    }

// -----------------------------------------------------------------------------
// CPhCltBaseImageParams::NewL
// 
// Constructor.
// -----------------------------------------------------------------------------
//
CPhCltExtVTImageParams* CPhCltExtVTImageParams::NewL( 
    const TPhCltImageType aType )
    {
    CPhCltExtVTImageParams* self = 
        new( ELeave )CPhCltExtVTImageParams( aType );
    return self;
    }

// -----------------------------------------------------------------------------
// CPhCltExtVTImageParams::~CPhCltExtVTImageParams
// 
// Destructor.
// -----------------------------------------------------------------------------
//
CPhCltExtVTImageParams::~CPhCltExtVTImageParams()
    {
    }

// -----------------------------------------------------------------------------
// CPhCltExtOperatorLogoParams::CPhCltExtOperatorLogoParams
// 
// C++ constructor.
// -----------------------------------------------------------------------------
//
CPhCltExtOperatorLogoParams::CPhCltExtOperatorLogoParams()
: CPhCltBaseImageParams( EPhCltTypeOperatorLogo )
    {
    }

// -----------------------------------------------------------------------------
// CPhCltBaseImageParams::NewL
// 
// Constructor.
// -----------------------------------------------------------------------------
//
CPhCltExtOperatorLogoParams* CPhCltExtOperatorLogoParams::NewL()
    {
    CPhCltExtOperatorLogoParams* self =
        new( ELeave )CPhCltExtOperatorLogoParams();
    return self;
    }

// -----------------------------------------------------------------------------
// CPhCltBaseImageParams::CPhCltBaseImageParams
// 
// Destructor.
// -----------------------------------------------------------------------------
//
CPhCltExtOperatorLogoParams::~CPhCltExtOperatorLogoParams()
    {
    }
    
// -----------------------------------------------------------------------------
// CPhCltExtOperatorLogoParams::AddImage
// 
// Adds an image.
// -----------------------------------------------------------------------------
//
void CPhCltExtOperatorLogoParams::AddImageL( TInt aHandle )
    {
    iImages().iImages[ EPhCltExtOperatorLogoIndex ] = aHandle;
    iImages().iImageCount = 1;
    }    
    
// -----------------------------------------------------------------------------
// CPhCltBaseImageParams::operator=
// 
// Assignment operator.
// -----------------------------------------------------------------------------
//
CPhCltBaseImageParams& CPhCltExtOperatorLogoParams::
    operator=( const CPhCltBaseImageParams& aParamClass )
    {
    Clean();
    this->iImages().iType = aParamClass.iImages().iType;
    this->iImages().iImageCount = aParamClass.iImages().iImageCount;
    this->iImages().iImages[ EPhCltExtOperatorLogoIndex ] = 
        aParamClass.iImages().iImages[ EPhCltExtOperatorLogoIndex ];
    this->iImages().iImages[ EPhCltExtCountryCodeIndex ] = 
        aParamClass.iImages().iImages[ EPhCltExtCountryCodeIndex ];
    this->iImages().iImages[ EPhCltExtNetworkCodeIndex ] = 
        aParamClass.iImages().iImages[ EPhCltExtNetworkCodeIndex ];
    this->iImages().iImages[ EPhCltExtLogoTypeIndex ] = 
        aParamClass.iImages().iImages[ EPhCltExtLogoTypeIndex ];        
    return *this;
    }    
    
// -----------------------------------------------------------------------------
// CPhCltExtOperatorLogoParams::SetCodesL
// 
// Sets country and network code.
// -----------------------------------------------------------------------------
//    
void CPhCltExtOperatorLogoParams::SetCodesL( const TPhCltCountryCode aCountryCode, 
    const TPhCltNetworkCode aNetworkCode,
    const TPhCltExtOperatorLogoType aLogoType )
    {
    iImages().iImages[ EPhCltExtCountryCodeIndex ] = aCountryCode;
    iImages().iImages[ EPhCltExtNetworkCodeIndex ] = aNetworkCode;
    iImages().iImages[ EPhCltExtLogoTypeIndex ] = aLogoType;
    }    

// END OF FILE
