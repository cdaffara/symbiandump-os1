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
 * Description:  Plugged Display Engine is responsible for initializing the TV
 *               driver during the boot and always when the cable is connected.
 *
 */


#ifndef CTVSTANDBYFIGURE_H
#define CTVSTANDBYFIGURE_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CFbsBitmap;

// CLASS DECLARATION

/**
 *  Helper class for handling the stand by figure.
 *  Loads an bitmap from the .mbm file, writes a string over it and
 *  converts the result bitmap to 16-bit RGB565 format.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
class CTvStandbyFigure : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * The bitmap should be in 16-million color (display mode EColor16M).
        * @param aFilename The name of the mbm file which contains the bitmap.
        * @param aImageNumber The number of the image in the mbm file.
        * @param aText The text to be drawn to the bitmap.
        */
        static CTvStandbyFigure* NewLC(
                                            const TDesC& aFilename,
                                            TInt aImageNumber );

        /**
        * Two-phased constructor.
        * The bitmap should be in 16-million color (display mode EColor16M).
        * @param aFilename The name of the mbm file which contains the bitmap.
        * @param aImageNumber The number of the image in the mbm file.
        * @param aText The text to be drawn to the bitmap.
        */
        static CTvStandbyFigure* NewL(
                                            const TDesC& aFilename,
                                            TInt aImageNumber );
        
        /**
        * Destructor.
        */
        virtual ~CTvStandbyFigure();

    public: // New functions

        /**
        * Return the bitmap as 16-bit RGB565 coded bitmap.
        * @since Series60 3.1
        * @return The 16-bit RGB565 coded bitmap, 
        * the caller should dispose when no longer needed.
        */
        TUint16* RgbConvertL();

        /**
        * Returns the size of the bitmap.
        * @since Series60 3.1
        * @param aSize The returned size of the bitmap.
        */
        void GetFigureSize( TSize& aSize );

    private:
        /**
        * C++ default constructor.
        */
        CTvStandbyFigure();

        /**
        * 2nd phase constructor, does everything. After the object has been
        * constructed, the modified bitmap is ready. The bitmap should be in
        * 16-million color (display mode EColor16M).
        * @param aFilename The name of the mbm file which contains the bitmap.
        * @param aImageNumber The number of the image in the mbm file.
        * @param aText The text to be drawn to the bitmap.
        */
        void ConstructL(
                    const TDesC& aFilename,
                    TInt aImageNumber );

    private:    // Data
        // The bitmap to be manipulated
        CFbsBitmap* iBitmap;
    };

#endif      // CTVSTANDBYFIGURE_H
            
// End of File
