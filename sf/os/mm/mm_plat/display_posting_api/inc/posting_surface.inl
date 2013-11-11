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
* Description:     Header file for S60 display posting API
*
*/





/**
*   Constructor of TPostingCapab class
*/
inline CPostingSurface::TPostingCapab::TPostingCapab():
        iMaxSourceSize(0),
        iMaxPixelSize( 0, 0 ),
        iSupportedRotations(0),
        iSupportsMirroring(EFalse),
        iSupportsScaling(EFalse),
        iSupportsBrightnessControl(EFalse),
        iSupportsContrastControl(EFalse),
        iSupportedPostingBuffering(EBufferingInvalid),
        iSupportedBufferTypes(EBufferTypeInvalid)
    {
    }

/**
*   Constructor of TPostingSourceParams class
*/
inline CPostingSurface::TPostingSourceParams::TPostingSourceParams():
        iPostingBuffering(EBufferingInvalid),
        iBufferType(EBufferTypeInvalid),
        iPostingUsage(EUsageInvalid),
        iSourceImageSize( 0, 0 ),
        iPostingFormat(EFormatInvalidValue),
        iPixelAspectRatioNum(0),
        iPixelAspectRatioDenom(0),
        iContentCopyRestricted(EFalse)
    {
    }

/**
*   Constructor of TPostingParams class
*/
inline CPostingSurface::TPostingParams::TPostingParams():
        iDisplayedRect( 0, 0, 0, 0 ),
        iScaleToRect( 0, 0, 0, 0 ),
        iInputCrop( 0, 0, 0, 0 ),
        iMirror(EFalse),
        iRotation(ENoRotation),
        iBrightness(0),
        iContrast(0),
        iBackGround(0)//Black
    {
    }

// End of File

