/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of CTFStubModuleInterface inline functions
*
*/


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CTFStubModuleInterface::CTFStubModuleInterface
// -----------------------------------------------------------------------------
inline CTFStubModuleInterface::CTFStubModuleInterface( void )
    {
    }


// -----------------------------------------------------------------------------
// CTFStubModuleInterface::NewL
// -----------------------------------------------------------------------------
inline CTFStubModuleInterface* CTFStubModuleInterface::NewL( const TDesC8& aMatchString )
    {
    TEComResolverParams resolverParams;
    resolverParams.SetDataType( aMatchString );
    resolverParams.SetWildcardMatch( ETrue );

    TInterfaceInitParams initParams;
    initParams.integer        = 0;
    initParams.descriptor    = NULL;

    return REINTERPRET_CAST( CTFStubModuleInterface*, 
        REComSession::CreateImplementationL( 
        KCTFStubModuleInterfaceUid, _FOFF( CTFStubModuleInterface, iDtor_ID_Key ), 
        &initParams,
        resolverParams ) );
    }


// -----------------------------------------------------------------------------
// CTFStubModuleInterface::~CTFStubModuleInterface
// -----------------------------------------------------------------------------
inline CTFStubModuleInterface::~CTFStubModuleInterface( void )
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }


// ---------------------------------------------------------
// CTFStubModuleInterface::ListAllImplementationsL
// ---------------------------------------------------------
inline void CTFStubModuleInterface::ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray )
    {
    REComSession::ListImplementationsL( KCTFStubModuleInterfaceUid, aImplInfoArray );
    }

// End of File
