// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains utility classes/functions which are
// shared across multiple tests.

#include <gdi.h>
#include <fbs.h>
#include <fntstore.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <graphics/openfontconstants.h>
#include <graphics/openfontrasterizer.h>
#include <test/graphicsfontutils.h>

// The width of 1000 pixels, in twips
const TInt KDefaultWidthInTwips = 11860;
const TInt KDefaultHeightInTwips = 11860;

/* 
Used for cleanup of RImplInfoArray implementationArray 
in CCharCodeConverter::LoadOpenFontLibraries().
*/
void ResetAndDestroyRImplInfoPtrArray(TAny* aPtr)
    {
    RImplInfoPtrArray* array = reinterpret_cast <RImplInfoPtrArray*> (aPtr);
    array->ResetAndDestroy();
    }


CCharCodeConverter::CCharCodeConverter()
    {
    }

EXPORT_C CCharCodeConverter::~CCharCodeConverter()
    {
    if (iFont)
        {
        iFontStore->ReleaseFont(iFont);  
        }
    delete iFontStore;
    REComSession::FinalClose();
    }

EXPORT_C CCharCodeConverter* CCharCodeConverter::NewL()
    {
    CCharCodeConverter* self = new(ELeave) CCharCodeConverter();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(1); // self;
    return self;
    }

EXPORT_C CCharCodeConverter* CCharCodeConverter::NewLC()
    {
    CCharCodeConverter* self = new(ELeave) CCharCodeConverter();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CCharCodeConverter::ConstructL()
    {
    // Setup fontstore...
    iFontStore = CFontStore::NewL(&User::Heap());
    iFontStore->iKPixelWidthInTwips = KDefaultWidthInTwips;
    iFontStore->iKPixelHeightInTwips = KDefaultHeightInTwips;
    //load all ecom implemented rasterizer dlls. installs the rasterizer.
    LoadOpenFontLibraries(iFontStore);
    //add any required font files
    iFontStore->LoadFontsAtStartupL();  
    }

void CCharCodeConverter::LoadOpenFontLibraries(CFontStore* aFontStore)
     {
     RImplInfoPtrArray implementationArray;
     TCleanupItem cleanup(ResetAndDestroyRImplInfoPtrArray, &implementationArray);
     CleanupStack::PushL(cleanup);
     TInt error;
     TInt ecomerror;
     TInt ecomnotready;
     TUid uid = {KUidOpenFontRasterizerPlunginInterface};
 
     // Making sure that no race situation arises
     // If ECom is not ready, give it another chance and try again. if it still doesn't work 
     // after the third try, then it just carries on quietly and fails... 
     for (ecomnotready = 0; ecomnotready < 3; ecomnotready++)
         {
         TRAP(ecomerror,REComSession::ListImplementationsL(uid, implementationArray));
         if (ecomerror == KErrNone)
             {
             break;
             }
         else
             {
             ecomerror = KErrNone;
             User::After(0);
             }
         }
 
     const TInt availCount = implementationArray.Count();
     for (TInt count = 0; count < availCount; ++count)
         {
         const CImplementationInformation* info = implementationArray[count];
         TUid rasterizerUid = info->ImplementationUid();
         // Create a rasterizer
         COpenFontRasterizer* rasterizer = 0;
         TRAP(error,rasterizer = COpenFontRasterizer::NewL(rasterizerUid));
         if (!error)
             {
             // Install it in the font store.
             TRAP(error,aFontStore->InstallRasterizerL(rasterizer));
             if (error)
                 {
                 delete rasterizer;
                 }
             }
         }
     CleanupStack::PopAndDestroy(&implementationArray);  
     }

/**
Tells the converter which font object is to be used when 
converting character codes to glyph codes.
@param aFont The font to use for conversion of codes
 */
EXPORT_C void CCharCodeConverter::UseFontL(CFbsFont* aFont)
    {
    User::LeaveIfNull(iFontStore);
    if (iFont)
        {
        iFontStore->ReleaseFont(iFont);
        iFont = NULL;
        iGlyphIndexExt = NULL;
        }
    // Setup font interface...
    TFontSpec testFontSpec(aFont->FontSpecInTwips().iTypeface.Name(), aFont->HeightInPixels()); 
    CFont* cfont = 0;
    iFontStore->GetNearestFontToDesignHeightInPixels(*&cfont, testFontSpec);    
    iFont = (CBitmapFont*)cfont;
    User::LeaveIfNull(iFont);
    TAny* ext = 0;
    // This is the interface used to get the glyph code index from the rasterizer..
    iFont->OpenFont()->ExtendedInterface(KUidOpenFontShapingExtension, ext);
    iGlyphIndexExt = reinterpret_cast<MOpenFontShapingExtension*>(ext);
    User::LeaveIfNull(ext);
    }

/**
Returns the glyph code for the given character code, for the 
font that this object was constructed with. No shaping of
the characters is performed by this function.
@param aCharCode The character code to request the glyph code for.
@return The glyph code
 */
EXPORT_C TInt CCharCodeConverter::GlyphCodeL(TInt aCharCode) const
    {
    // If a leave occurs, it's because UseFontL() has
    // not been successfully called.
    User::LeaveIfNull(iGlyphIndexExt);
    TInt glyphCode = iGlyphIndexExt->GlyphIndex(aCharCode);
    return glyphCode;
    }


