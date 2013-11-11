/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This Custom API is created to fix the synchronization problem
*                between Camera Application and Camera Server about the current
*                screen orientation. CWsScreenDevice is used as a reference and
*                Camera Application should use its NumScreenModes() and
*                GetScreenModeSizeAndRotation() methods to inform it by the mode
*                index, which mode it is currently in. It is most likely the one
*                also returned by CurrentScreenMode() but also other mode can be
*                given. Camera server then returns an error if that mode is not
*                supported and some alternative value should be used if it's
*                wanted to be set.
*
*/


#ifndef ECAMUIORIENTATIONOVERRIDECUSTOMAPI_H
#define ECAMUIORIENTATIONOVERRIDECUSTOMAPI_H

#include <e32base.h>

// CONSTANTS

const TUid KCameraUIOrientationOverrideUid = { 0x2001E2AE };

// CLASS DECLARATION

/**
*  Custom interface for Orientation setting.
*/
class MCameraUIOrientationOverride
    {

    public: 

        /**
        * Sets Orientation mode and is based on wsini.ini that can be fetched
        * from CWsScreenDevice. This method should be called after calling
        * CCamera::New2L(), before CCamera::Reserve() and before
        * CCamera::CameraInfo().
        * @param aMode Mode that specifies the wanted orientation
        * @leave KErrNotSupported if aMode is not supported
        * @leave KErrGeneral if this method is not supported
        * @leave KErrInUse if this method is not called at the right time
        */
        virtual void SetOrientationModeL( TInt aMode ) = 0;

    };

#endif //ECAMUIORIENTATIONOVERRIDECUSTOMAPI_H
