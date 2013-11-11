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
 * Description:  GSServer is responsible for initializing the TV
 *                driver during the boot and always when the cable is connected.
 *
 */


// INCLUDE FILES
#include "tvstandbyfigure.h"
#include "TVOutStandbyFigure.mbg"
#include <e32std.h>
#include <fbs.h>
#include <f32file.h>
#include <bitdev.h>
#include <gdi.h>
//#include <r_display.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTvStandbyFigure::CTvStandbyFigure
// Empty default constructor.
// -----------------------------------------------------------------------------
//
CTvStandbyFigure::CTvStandbyFigure()
    {
    }


// -----------------------------------------------------------------------------
// CTvStandbyFigure::ConstructL
// Open the needed sessions, load the original bitmap and draw over it.
// -----------------------------------------------------------------------------
//
void CTvStandbyFigure::ConstructL(
                            const TDesC& aFilename,
                            TInt aImageNumber )
    {
    //  Setup the font and bitmap server and open session to it
    User::LeaveIfError( FbsStartup() );
    User::LeaveIfError( RFbsSession::Connect() );

    //  Create and load a bitmap from the given file, then draw the text over it
    iBitmap = new (ELeave) CFbsBitmap();
    TInt err = iBitmap->Load( aFilename, aImageNumber );
    User::LeaveIfError( err );
    }


// -----------------------------------------------------------------------------
// CTvStandbyFigure::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTvStandbyFigure* CTvStandbyFigure::NewLC(
                                        const TDesC& aFilename,
                                        TInt aImageNumber )
    {
    CTvStandbyFigure* self = new (ELeave) CTvStandbyFigure();
    CleanupStack::PushL( self );
    self->ConstructL( aFilename, aImageNumber );
    return self;
    }


// -----------------------------------------------------------------------------
// CTvStandbyFigure::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTvStandbyFigure* CTvStandbyFigure::NewL(
                                        const TDesC& aFilename,
                                        TInt aImageNumber )
    {
    CTvStandbyFigure* self = CTvStandbyFigure::NewLC(
                                        aFilename,
                                        aImageNumber );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CTvStandbyFigure::NewL
// Release the bitmap and close the session.
// -----------------------------------------------------------------------------
//
CTvStandbyFigure::~CTvStandbyFigure()
    {
    if ( iBitmap )
        {
        iBitmap->Reset();
        delete iBitmap;
        RFbsSession::Disconnect();
        }    
    }


// -----------------------------------------------------------------------------
// CTvStandbyFigure::RgbConvertL
// Convert bitmap to RGB565.
// -----------------------------------------------------------------------------
//
TUint16* CTvStandbyFigure::RgbConvertL()
    {
    TSize drawingSize( iBitmap->SizeInPixels() );
    TInt pixels = drawingSize.iWidth * drawingSize.iHeight;
    TUint16* RGB565Data = new(ELeave) TUint16[pixels];
    CleanupStack::PushL(RGB565Data);
    
    //This is needed to lock the bitmap object before
    //manipulating the object using TBitmapUtil object
    iBitmap->LockHeapLC();
    TBitmapUtil bitmapUtil( iBitmap );
    bitmapUtil.Begin( TPoint( 0, 0 ) );

    for (TInt y = 0; y < drawingSize.iHeight; ++y)
        {
        bitmapUtil.SetPos( TPoint( 0, y ) );
        for (TInt x = 0; x < drawingSize.iWidth; ++x)
            {
            TUint32 pixelValue = bitmapUtil.GetPixel();
            TUint r = (pixelValue >> 16) & 0xff;
            TUint g = (pixelValue >> 8) & 0xff;
            TUint b = pixelValue & 0xff;
            RGB565Data[x + y * drawingSize.iWidth] =
                            (( r >> 3) << 11) +
                            (( g >> 2) << 5) +
                            (b >> 3);
            bitmapUtil.IncXPos();
            }
        }

    bitmapUtil.End();
    
    CleanupStack::PopAndDestroy(); // bitmapUtil
    CleanupStack::Pop(RGB565Data);
    return RGB565Data;
    }


// -----------------------------------------------------------------------------
// CTvStandbyFigure::GetFigureSize
// -----------------------------------------------------------------------------
//
void CTvStandbyFigure::GetFigureSize( TSize& aSize )
    {
    aSize = iBitmap->SizeInPixels();
    }

// End of File
