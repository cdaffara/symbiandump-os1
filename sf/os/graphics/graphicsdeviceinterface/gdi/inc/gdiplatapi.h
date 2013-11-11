// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __GDIPLATAPI_H__
#define __GDIPLATAPI_H__

#include <e32base.h>

/*
 * Please note that RFontTable API is officially supported from SymTB10.1.
 * It is not guaranteed to work in SymTB9.2 or earlier.
 * 
 * The class keeps the parameter 'aFont', and will use it in Close(). 
 * Destructing the CFont object before calling Close() can result in Panic.
 * 
 * @publishedPartner
 */
class RFontTable 
    {
public:
    IMPORT_C RFontTable();
    IMPORT_C TInt Open(CFont& aFont, TUint32 aTag);
    IMPORT_C TInt TableLength() const;
    IMPORT_C const TUint8* TableContent() const;
    IMPORT_C void Close();
private:
    TAny* iTableContent;
    TInt iLength;
    
    // need to remember font and table name, to be used in releasing the table
    const CFont* iFont;
    TUint32 iTag;
    };


/*
 * Please note that RGlyphOutlineIterator API is officially supported 
 * from SymTB10.1. It is not guaranteed to work in SymTB9.2 or earlier.
 * 
 * The class keeps the parameter 'aFont', and will use it in Close(). 
 * Destructing the CFont object before calling Close() can result in Panic.
 * 
 * @publishedPartner
 */
class RGlyphOutlineIterator 
    {
public:
    IMPORT_C RGlyphOutlineIterator();
    IMPORT_C TInt Open(CFont& aFont, const TUint* aCodes, TInt aCount, TBool aHinted = EFalse);
    IMPORT_C TInt Next();
    IMPORT_C const TUint8 *Outline() const;
    IMPORT_C TInt OutlineLength() const;
    IMPORT_C void Close();
private:
    TAny** iOutlines;
    TInt* iLengths;
    TInt iCursor;
    TInt iCount;
    
    // these information is needed in Close().
    const CFont* iFont;
    TUint* iCodes;
    TBool iHinted;
    };


#endif __GDIPLATAPI_H__
