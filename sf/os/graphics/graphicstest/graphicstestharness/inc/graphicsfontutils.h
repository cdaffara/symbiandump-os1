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
// shared across multiple test steps.

#ifndef GRAPHICSFONTUTILS_H
#define GRAPHICSFONTUTILS_H

#include <e32base.h>
class CFontStore;
class CFbsFont;
class MOpenFontShapingExtension;
class CBitmapFont;

/**
Helper class that converts a character code to a glyph code. 
To do this requires calling into a font rasterizer from this
process, therefore the rasterizer must be loaded in this 
process - to do this it must create its own fontstore and load
its own instance of a font rasterizer. This class performs 
no shaping of characters.

@note This class and/or its implementation should not be 
    copied or used as refernce code - the proper implementation
    would not create a new fontstore, but instead either parse the
    font table of the associated font itself, or use the
    FBS server-side fontstore. 
 */
class CCharCodeConverter : public CBase
    {
public:
    IMPORT_C static CCharCodeConverter* NewL();
    IMPORT_C static CCharCodeConverter* NewLC();
    IMPORT_C ~CCharCodeConverter();    
    IMPORT_C void UseFontL(CFbsFont* aFont);
    IMPORT_C TInt GlyphCodeL(TInt aCharCode) const;
private:
    CCharCodeConverter();
    void ConstructL();
    void LoadOpenFontLibraries(CFontStore* aFontStore);    
private:
    CFontStore* iFontStore;
    CBitmapFont* iFont;
    MOpenFontShapingExtension* iGlyphIndexExt;
    };

#endif // GRAPHICSFONTUTILS_H
