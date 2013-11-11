/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  TPhoneServer interface for image handling.
*
*/


// INCLUDES

#include    "RPhCltImageHandler.h"
#include    "PhCltClientServer.h"
#include    <RPhCltServer.h>
#include    <e32std.h>
#include    <e32base.h>
#include    <fbs.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RPhCltImageHandler::RPhCltImageHandler
// 
// C++ constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
RPhCltImageHandler::RPhCltImageHandler()
:   RSubSessionBase()
    {
    }


// -----------------------------------------------------------------------------
// RPhCltImageHandler::Open
// 
// Open subsession to Phone Server.
// -----------------------------------------------------------------------------
//
TInt RPhCltImageHandler::Open( RPhCltServer& aServer )
    {
    __ASSERT_ALWAYS( aServer.Handle(), User::Panic( 
            KPhClientPanicCategory, 
            EPhCltClientSidePanicNullHandle ) );

    return CreateSubSession( 
        aServer, 
        EPhoneServerImageHandlerSubSessionOpen, 
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltImageHandler::Close
// 
// Close subsession.
// -----------------------------------------------------------------------------
//
void RPhCltImageHandler::Close()
    {
    iVTBitMaps.ResetAndDestroy();
    iOperatorLogos.ResetAndDestroy();
    CloseSubSession( EPhoneServerImageHandlerSubSessionClose );
    }

// -----------------------------------------------------------------------------
// RPhCltImageHandler::SaveImages
// 
// Saves images to Phoneserver.
// -----------------------------------------------------------------------------
//
TInt RPhCltImageHandler::SaveImages( CPhCltImageParams& aParams )
    {
    __ASSERT_ALWAYS( 
        SubSessionHandle(), User::Panic( 
            KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

    return SendReceive( 
        EPhoneServerImageHandlerSubSessionSave, 
        TIpcArgs( 
            &aParams.Images()) );
    }     
    
// -----------------------------------------------------------------------------
// RPhCltImageHandler::LoadImages
// 
// Loads images from Phoneserver.
// -----------------------------------------------------------------------------
//
TInt RPhCltImageHandler::LoadImages( CPhCltImageParams*& aParams )
    {
    __ASSERT_ALWAYS( 
        SubSessionHandle(), User::Panic( 
            KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

    const TInt ret = SendReceive( EPhoneServerImageHandlerSubSessionLoad, 
        TIpcArgs( &aParams->Images() ) );
        
    // No point in commiting if failed.
    if ( ret != KErrNone )
        {
        return ret;
        }
    return FinalizeLoad( aParams->Images() );
    }
 
// -----------------------------------------------------------------------------
// RPhCltImageHandler::FinalizeLoad
// 
// Duplicates loaded images, and commits transaction.
// -----------------------------------------------------------------------------
//    
TInt RPhCltImageHandler::FinalizeLoad( TPckgBuf< TPhCltImageArray >& aImages )
    {
    TRAPD( err, 
        {
        switch( aImages.operator()().iType )
            {
            case EPhCltTypeVTStill:
            case EPhCltTypeVTDefault:
                CopyStillsL( aImages.operator()() );
                break;
            case EPhCltTypeOperatorLogo:
                CopyLogosL( aImages.operator()() );
                break;
            default:
                break;
            }
        } );

    // Must release in any case
    const TInt ret = SendReceive( EPhoneServerImageHandlerSubSessionCommitLoad, 
        TIpcArgs( &aImages ) );

    return err != KErrNone ? err : ret;

    }

// -----------------------------------------------------------------------------
// RPhCltImageHandler::CopyLogos
// 
// Replaces / appends a new logo.
// -----------------------------------------------------------------------------
//    
void RPhCltImageHandler::CopyLogosL( TPhCltImageArray& aImages )
    {
    TBool found( EFalse );
    CFbsBitmap* bitMap = 
        CreateBitMapL( aImages.iImages[ EPhCltExtOperatorLogoIndex ] );

    // Check if there is one stored already, replace handle if so.
    for ( TInt i = 0; i < iOperatorLogos.Count(); i ++ )
        {
        if ( aImages.iImages[ EPhCltExtCountryCodeIndex ] == 
            iOperatorLogos[ i ]->iCountryCode && 
            aImages.iImages[ EPhCltExtNetworkCodeIndex ] == 
            iOperatorLogos[ i ]->iNetworkCode && 
            aImages.iImages[ EPhCltExtLogoTypeIndex ] == 
            iOperatorLogos[ i ]->iLogoType            
             )
            {
            delete iOperatorLogos[ i ]->iBitmap;
            iOperatorLogos[ i ]->iBitmap = bitMap;
            found = ETrue;
            break;
            }
        }

    // Else add a new one.
    if ( !found )
        {
        CPhCltOperatorLogoContainer* logo = 
            new (ELeave)CPhCltOperatorLogoContainer(
            aImages.iImages[ EPhCltExtCountryCodeIndex ], 
            aImages.iImages[ EPhCltExtNetworkCodeIndex ],
            ( TPhCltExtOperatorLogoType )
            aImages.iImages[ EPhCltExtLogoTypeIndex ],
            bitMap );
        iOperatorLogos.Append( logo );
        }
    }

// -----------------------------------------------------------------------------
// RPhCltImageHandler::CopyStillsL
// 
// Creates new duplicates of still images.
// -----------------------------------------------------------------------------
//   
void RPhCltImageHandler::CopyStillsL( TPhCltImageArray& aImages ) 
    {
    // Remove all first.
    iVTBitMaps.ResetAndDestroy();

    for ( TInt i = 0 ; i < aImages.iImageCount; i ++ )
        {
        CFbsBitmap* bitMap = 
            CreateBitMapL( aImages.iImages[ i ] );
        iVTBitMaps.Append( bitMap );
        }
    }

// -----------------------------------------------------------------------------
// RPhCltImageHandler::CreateBitMapL
// 
// Creates new duplicate of still image.
// -----------------------------------------------------------------------------
//   
CFbsBitmap* RPhCltImageHandler::CreateBitMapL( const TInt aHandle )
    {
    CFbsBitmap* bitMap = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bitMap );
    User::LeaveIfError( bitMap->Duplicate( aHandle ) );
    CleanupStack::Pop( bitMap );
    return bitMap;
    }

// End of File
