/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Custom interface for Orientation setting.
*
*/


#ifndef ECAMORIENTATIONCUSTOMINTERFACE_H
#define ECAMORIENTATIONCUSTOMINTERFACE_H

#include <e32base.h>

// CONSTANTS

const TUid KCameraOrientationUid = { 0x101F87CD };

// CLASS DECLARATION

/**
*  Custom interface for Orientation setting.
*/
class MCameraOrientation
    {

    public: 

        /**
        * List of Orientation settings
        */
        enum TOrientation
            {
            EOrientation0    = 0x00,
            EOrientation90   = 0x01,
            EOrientation180  = 0x02,
            EOrientation270  = 0x04
            };
 
        /**
        * Gives the current Orientation setting value
        * @return TOrientation
        */
        virtual TOrientation Orientation() const = 0;

        /**
        * Sets Orientation
        * @param aValue new Orientation setting
        * @leave KErrNotSupported if aValue is not supported
        * @leave KErrInUse if camera is not reserved
        * @leave KErrNotReady if camera power is not on
        */
        virtual void SetOrientationL( TOrientation aValue ) = 0;

        /**
        * Gives the bitfield of supported  Orientation settings
        * @return the bitfield that contains supported TOrientation settings
        */
        virtual TUint32 SupportedOrientations() const = 0;

    };

#endif //ECAMORIENTATIONCUSTOMINTERFACE_H
